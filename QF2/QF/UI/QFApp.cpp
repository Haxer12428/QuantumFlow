#include "../QF.h"

/*========================= Constructor & Destructor =========================*/
	QF::UI::App::App() : m_LastGivenID{0}
	{
		/* Initialize glew */
	}

	void QF::UI::App::Implement()
	{
		try
		{
			if (!on_Init()) throw std::exception("failed to intialize application");
		}
		catch (...) {
			QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__,
				"Failed to initialize app: virtual bool QF::UI::App::on_Init"
			);
		}
		/* Initialize main loop */
		hk_MainLoop();
	}
/*========================= Initializer  =========================*/
	bool QF::UI::App::on_Init() { return true; }

/*========================= Children declaration =========================*/
	void QF::UI::App::im_Children(Window* _Window) 
	{
		m_Children.push_back(_Window);
	}

	const size_t QF::UI::App::g_GeneratedID()
	{
		m_LastGivenID++;
		return m_LastGivenID;
	}
/*========================= Main loop =========================*/
	void QF::UI::App::func_MainLoopHandleChild(
		Window* _Child, std::vector<Window*>& _WantingForClose
	)
	{
		if (glfwWindowShouldClose(
			_Child->g_GLFWwindow()
		))
		{ /* Closing state */
			_WantingForClose.push_back(_Child);
			/* Return */
			return; 
		}

		_Child->hk_MainLoop();
	}

	void QF::UI::App::func_MainLoopCloseChildren(Window* _Child, const size_t& _Iterator)
	{
		auto _VectorPosition = (m_Children.begin() + _Iterator);
		/* Erase from vector */
		m_Children.erase(_VectorPosition);

		/* Erase ptr */
		delete _Child;

		/* Log deletion */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__,
			"Successfully closed window."
			);
	}

	void QF::UI::App::func_MainLoopHandleClosingChildren(
		std::vector<Window*>& _WaitingForClose)
	{
		for (Window*& _Requested : _WaitingForClose)
		{
			size_t _ID = NULL;

			size_t _Iterator = 0;
			for (Window*& _InVector : m_Children)
			{
				/* Verify id */
				if (_InVector->g_ID() == _Requested->g_ID())
				{
					/* Close child */
					func_MainLoopCloseChildren(_Requested, _Iterator); break; 
				}
				_Iterator++;
			}
		}
	}

	void QF::UI::App::hk_MainLoop()
	{
		/* Log initialization of this function */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__,
			"Initialized");

		/* Actuall main loop
			Exit when children are empty
		*/
		while (
			m_Children.empty() == false
			) {
		 

			std::vector<Window*> _WaitingForClose; 

			glfwPollEvents();


			for (Window* _Child : m_Children)
			{
				/* Add for multi threadning queue */
				func_MainLoopHandleChild(_Child, _WaitingForClose);
			}
			

			/* Finalization */
			func_MainLoopHandleClosingChildren(_WaitingForClose);
			glfwPollEvents();
		}; 

		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::WARNING, __FUNCTION__,
			"App main loop exited.");
	}