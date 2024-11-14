#include "QFUIApp.h"

/* Constructor & Destructor */
	QF::UI::App::WindowHandler::WindowHandler() {
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
			"Initialized.");
	}

	QF::UI::App::WindowHandler::~WindowHandler() {
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
			"Destructed.");
	}
/* Main loop */
	void QF::UI::App::WindowHandler::enterMainLoop() {
		std::chrono::high_resolution_clock::time_point framerateCountStarted;
		size_t childrenIterator = 0;

		/* Get next window that's suitable for processing */
		auto processedAll = [&]() -> void {
			childrenIterator = 0; /* Reset the children iterator */
		};

		auto didProcessAll = [&]() -> const bool {
			return (childrenIterator == (m_Children.size()));
		};

		auto g_NextWindowToProcess = [&]() -> const size_t {
			for (size_t _Iterator = childrenIterator; _Iterator < m_Children.size();
				_Iterator++) {
				/* Find next */
				auto Window = &m_Children[_Iterator];

				childrenIterator = (_Iterator + 1);
				if (!((!Window) || ((*Window)->is_Destructed())))
				{ /* Add window to q if the conditions are right */
					return (_Iterator);
				}
			};
			/* No windows suitable for processing found
				* Return sizeof m_Children to stop processing 
			*/
			return (m_Children.size());
		};
		
		/* Processing window instance 
			* If wanting to be deleted return's bool
		*/
		long long windowsMainLoopFinishedCount = 0; 

		auto processWindowInstance = [&](const size_t& _At) -> const bool {
			auto Window = &m_Children[_At];

			/* Check if glfw want's to close the window */
			if (glfwWindowShouldClose((*Window)
				->g_GLFWobject()->g_Object()
			)) {
				return true;
			};
			/* Check if destruction happened (if we have a invaild ptr in a stack) or wanted*/
			if ((*Window)->is_WantingToDestruct())
				return true;

			/* Process main loop */
			
			(*Window)->hook_MainLoop();
			/* Process window */
			return false; 
		};
		
		auto processWindows = [&]() -> void {
			std::vector<long long> windowsWaitingToCloseIDs;

			while (!didProcessAll()) {
				/* Obtain instance & index */
				const size_t thisWindowIdx = g_NextWindowToProcess();
				auto windowInstance = &m_Children[thisWindowIdx];

				/* Process window */
				if (processWindowInstance(thisWindowIdx)) {
					/* Wants to close, add to queue */
					windowsWaitingToCloseIDs.push_back((*windowInstance)->
						g_ImmutableID()
					);}
			};
			/* Tell that i finished processing */
			processedAll();
			/* Close windows in queue */
			closeWindows(windowsWaitingToCloseIDs);

			/* All windows finished */
			windowsMainLoopFinishedCount = 0;
		};

		/* Fps counter */
		auto framerateStartCounting = [&]() -> void {
			framerateCountStarted = std::chrono::high_resolution_clock::now();
		};
		
		std::vector<float> framerateSum;

		auto g_Framerate = [&]() -> float {
			auto currentTime = std::chrono::high_resolution_clock::now();

			/* Count */
			const long long millisecondsPassed = (std::chrono::duration_cast<std::chrono::milliseconds>(
				currentTime - framerateCountStarted
			).count());

			const float framerateCounted = (1000.0f / static_cast<float>(millisecondsPassed));

			if (framerateSum.size() > 10) {
				framerateSum.erase(framerateSum.begin());
			}
			framerateSum.push_back(framerateCounted);

			float framerateAll = 0;
			for (const float& _Frame : framerateSum) {
				framerateAll += _Frame; 
			}

			/* Return counted */
			return (framerateAll / static_cast<float>(framerateSum.size()));
		};


		while (!m_Children.empty()) {
			framerateStartCounting();
			processWindows();
			std::string framerateStr = std::to_string(g_Framerate());
			SetConsoleTitle(LPCWSTR(std::wstring(framerateStr.begin(), framerateStr.end()).c_str()));
		}
	}
/* Children handling */
	const long long QF::UI::App::WindowHandler::g_NewImmutableIdForChild() {
		m_ChildrenImmutableIdsCount++; return m_ChildrenImmutableIdsCount;
	}

	void QF::UI::App::WindowHandler::im_Child(Components::Window* _Window) {
		/* Check for destruction flag  */
		if (!_Window || _Window->is_Destructed()) {
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Tried pushing destructed window ptr as an argument.");
			return; 
		}

		/* Add to children */
		m_Children.push_back(std::unique_ptr<Components::Window>(_Window));
		/* Assign immutable id 
			Immutable id should be assigned ONLY ONCE and ONLY HERE due to security reasons
		*/
		if (!_Window
			->s_ImmutableID(g_NewImmutableIdForChild())
			) { /* We got a big problem here */
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__,
				(std::string)"Read message before, crash-state might occur anymoment throught QF lifetime. " +
				"Something went wrong very badly. Revise ur code."
			);

			/* Print shutdown message & shutdown */
			__QF_SHUTDOWN_MSG();
			throw std::runtime_error("QF shutdown");
		}
	}


	void QF::UI::App::WindowHandler::closeWindows(
		const std::vector<long long>& _IdsToClose
	) {
		/* Closing singular children func */
		auto closeChild = [&](const long long& _Id) -> bool {
			size_t childrenIterator = 0;
			for (
				auto& _Child : m_Children
				) {
				/* Check for signature id match */
				if ((*_Child).g_ImmutableID() == _Id) {
					/* Erase from vector & return positive */
					m_Children.erase(
						(m_Children.begin() + childrenIterator)
						);
					return true; 
				}
				/* Increment iterator */
				childrenIterator++;
			}
			/* Return negative to suggest that id ain't real */
			return false; 
		};

		for (const long long& _Id : _IdsToClose) {
			/* Erase child from existance */
			if (!closeChild(_Id)) {
				/* Error invaild id provided!
					*Operation aborted, qf shutdown required
				*/
				__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
					"_Id provided doesn't match any window signature. Might cause bad problems."
				); __QF_SHUTDOWN_MSG();
				/* Throw exception */
				throw std::exception("_Id doens't match any window signature");
			}
		}
	}