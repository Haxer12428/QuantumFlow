#include "../../QF.h"

/*========================= Constructor & Destructor =========================*/
	QF::UI::Window::Window(QF::UI::App* _App, const std::string& _Name, const QF::Utils::Vec2& _Size) : 
		Element{}, m_Size{_Size}, m_Name{_Name}, m_EventHandler{ new EventSystem::EventHandler() }
	{
		/* Declare that im window */
		im_Window(this);
		/* Declare that im children of app */
		_App->im_Children(this);
		/* Declare that i want new id */
		m_ID = _App->g_GeneratedID();
		/* Create GLFW Window */
		if (!func_GLFWWindowCreate()) throw 
			std::exception(
				"glfw window creation fail"
			);;

		/* Create ImGui Context */
		if (!func_ImGuiCreateContext()) throw
			std::exception(
				"imgui context creation fail"
			);
		/* Initialize event propagation */
		func_SubscribeEventsForPropagation();
		/* Create titlebar & resize instances */ 
		m_TitleBar = new TitleBar(this);
		m_Resize = new Resize(this);
		
		/* Log success */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__,
			std::format("Successfully created window: {}", m_Name)
			);
	}

	QF::UI::Window::~Window()
	{
		/* Destroy children */
		func_ChildrenDestroy();

		/* Destroy glfw object */
		glfwDestroyWindow(m_Window);

		/* Destroy class event handler */
		delete m_EventHandler;

		/* Log success */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__, 
			std::format("Successfully deleted window: {}", m_Name)
			);
	}
/*========================= Default params return's =========================*/
	const std::string& QF::UI::Window::g_Name() const { return m_Name; }

/*========================= GLFW Window =========================*/
	const QF::Utils::Vec2 QF::UI::Window::g_AbsoluteMousePosition() const
	{
		const QF::Utils::Vec2 _MousePos = g_MousePosition();
		const QF::Utils::Vec2 _WindowPos = g_WindowPosition();

		return (_MousePos + _WindowPos);
	}

	const QF::Utils::Vec2 QF::UI::Window::g_WindowPosition() const
	{
		int _x, _y; 
		glfwGetWindowPos(m_Window, &_x, &_y);

		return { _x, _y };
	}

	void QF::UI::Window::s_Position(const QF::Utils::Vec2& _New)
	{
		glfwSetWindowPos(m_Window, static_cast<int>(_New.x), static_cast<int>(_New.y));
	}

	QF::UI::Window::TitleBar* QF::UI::Window::g_TitleBarInstance() 
	{ return m_TitleBar; }

	/* Returns event handler of the class */
	QF::UI::EventSystem::EventHandler* QF::UI::Window::g_EventHandler() 
	{
		return m_EventHandler;
	}

	/* Pass values lower than 0 to get NO_RESTRICT mode */
	void QF::UI::Window::s_MaximalSize(const QF::Utils::Vec2& _New)
	{
		m_MaximalSize = _New; 
	}

	/* Still cannot be less than 1.0f, you can pass lower values but it will still equal to 1.0f*/
	void QF::UI::Window::s_MinimalSize(const QF::Utils::Vec2& _New)
	{
		float _x, _y; 
		/* Still cannot be less than 1 */
		_x = _New.g_X();
		_y = _New.g_Y();

		_x = std::max(1.0f, _x);
		_y = std::max(1.0f, _y);
		/* Apply */
		m_MinimalSize = {_x, _y};
	}

	/* Class limitations are applied */
	void QF::UI::Window::s_Size(const QF::Utils::Vec2& _New) 
	{
		float _x, _y; 
		_x = _New.g_X();
		_y = _New.g_Y();

		/* Limit */
		float _mx, _my;
		_mx = m_MaximalSize.g_X();
		_my = m_MaximalSize.g_Y();

		/* Finally, limit the final offsets */
		_x = std::max(m_MinimalSize.g_X(), _x);
		_y = std::max(m_MinimalSize.g_Y(), _y);


		if (_mx > 0) _x = std::min(_mx, _y);
		if (_my > 0) _y = std::min(_my, _y);

		/* Apply */
		const QF::Utils::Vec2 _NewSize = {_x, _y};

		glfwSetWindowSize(m_Window, static_cast<int>(_NewSize.g_X()), static_cast<int>(_NewSize.g_Y()));
	}

	const QF::Utils::Vec2 QF::UI::Window::g_MousePosition() const
	{
		double _x, _y; 
		glfwGetCursorPos(m_Window, &_x, &_y);
		/* Return with static cast to not lose data */
		return { static_cast<float>(_x), static_cast<float>(_y) };
	}

	ImGuiContext* QF::UI::Window::g_ImGuiContext() const 
	{
		return m_Context;
	}

	const QF::Utils::Vec2 QF::UI::Window::g_Size() const
	{
		/* Obtain window size from GLFWwindow*/
		int _sx, _sy;
		glfwGetWindowSize(m_Window, &_sx, &_sy);
		/* Return valid info */
		return { _sx, _sy };
	}

	GLFWwindow* QF::UI::Window::g_GLFWwindow() const 
	{
		return m_Window;
	}

	const size_t QF::UI::Window::g_ID() const
	{
		return m_ID;
	}

	const bool QF::UI::Window::func_GLFWWindowCreate()
	{
		if (!glfwInit())
		{ /* Glfw window creation CRITICAL_ERROR */
			QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__,
				"Failed to initialize glfw.");
			return false; 
		}
		/* Apply hints */
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		/* Create object */
		m_Window = glfwCreateWindow(
			m_Size.x, m_Size.y, m_Name.c_str(), NULL, NULL
		);
		/* Check if creation succeed */
		if (!m_Window)
		{ /* Termnate & log */
			glfwTerminate();

			QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__,
				"Failed to create glfw window object."
				);
			return false; 
		}

		/* Make context current */
		glfwMakeContextCurrent(m_Window);
		
		/* Set window pointer */
		glfwSetWindowUserPointer(m_Window, this);

		/* Apply minimal size for a GLFWwindow */
		glfwSetWindowSizeLimits(m_Window, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

		return true; 
	}

	const bool QF::UI::Window::func_ImGuiCreateContext()
	{
		/* Check version */
		IMGUI_CHECKVERSION();

		/* Create context */
		m_Context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();

		ImGui::SetCurrentContext(m_Context);
		/* Return result */
		return ImGui::GetCurrentContext() != nullptr;
	}
/*========================= Main Loop =========================*/
	void QF::UI::Window::hk_MainLoop()
	{
		/* Handle nullptrs after children manual deletions */
		func_EraseNullChildren();

		/* Assign children */
		func_ChildrenAssign();

		/* Check for resizing and block any other event than render */
		if (m_Resize->hk_MainLoop() == false) 
		{
				/* Call events */
			 hk_MouseClickEvent();
			 hk_MouseMotionEvent();
			 hk_MousePanelDragEvent();
		}

		/* Render for last */
		hk_RenderEvent();
	}
/*========================= Children handling =========================*/
	void QF::UI::Window::func_ChildrenAssign()
	{
		for (Panel* _Child : m_ChildrenWaitingForAssignment)
		{
			_Child->func_Assign();

			QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__, 
				std::format("Assigned panel to stack, id: {}", _Child->g_Id()));
		}
		m_ChildrenWaitingForAssignment.clear();
	}

	void QF::UI::Window::im_NoLongerAChildren(Panel* _Panel)
	{
		size_t _Iterator = 0; 
		for (Panel* _Child : m_Children)
		{
			if (_Child->g_Id() == _Panel->g_Id())
			{
				m_Children.erase((m_Children.begin() + _Iterator)); 
				
				QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__,
					std::format("Erased panel from stack, id: {}", _Panel->g_Id())
					);

				return;
			}
			_Iterator++;
		}
	}

	const size_t QF::UI::Window::g_GeneratedID()
	{
		m_LastId++;
		return m_LastId;
	}

	void QF::UI::Window::i_WantToBeAssigned(Panel* _Panel) 
	{
		/* Add to assign queue */
		m_ChildrenWaitingForAssignment.push_back(_Panel);
	}

	void QF::UI::Window::im_Children(Panel* _Panel)
	{
		m_Children.push_back(_Panel);
	}

	void QF::UI::Window::func_EraseNullChildren()
	{
		/* Erase all occurences of nullptrs */
		m_Children.erase(
			std::remove_if(m_Children.begin(), m_Children.end(), [](Panel* ptr) 
				{ return ptr == nullptr; }
		), m_Children.end());
	}

	void QF::UI::Window::func_ChildrenDestroy()
	{
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::MESSAGE, __FUNCTION__,
			std::format("Attempting to destroy all children for window: {}", m_Name)
			);

		/* Children needs to be destroyed like this because of children recursive 
			destroy of other linked panels 
			*/
		while (m_Children.size() != 0)
		{
			m_Children[0]->Destroy();
			func_EraseNullChildren();
		}
	}
/*========================= Events =========================*/
/* Propagation */
	void QF::UI::Window::func_SubscribeEventsForPropagation()
	{
		/* Render event */
		m_EventHandler->Subscribe<EventSystem::RenderEvent>(this, &Window::hk_RenderEventPropagation);

		/* Log Success */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__, 
			std::format("Event propagation initalized successfully for window: {}", m_Name)
			);		
	}

/* Render event */
	void QF::UI::Window::hk_RenderEventPropagation(EventSystem::RenderEvent& _Event)
	{
		/* Propagate from bottom to top */
		for (Panel* _Child : m_Children)
		{
			try {
				if (_Child != nullptr)
				{
					if (_Child->g_Visibility()) { _Child->g_EventHandler()->Dispatch(EventSystem::RenderEvent{}); }
				}
			} catch (...) {};
			/* Dispatch only if visible*/
		}
		/* Call resize icon rendering hook */
		m_Resize->hk_Render();
	}

	void QF::UI::Window::hk_RenderEvent()
	{
		
		/* Prepare backend & frame */
		func_RenderEventPrepare();
		/* Call things */
		
		m_EventHandler->Dispatch(EventSystem::RenderEvent{});

		/* Finalize -> Cleanup */
		func_RenderEventFinalize();
	}



	void QF::UI::Window::func_RenderEventPrepare()
	{
		ImGui::SetCurrentContext(m_Context);
		/* Initialize backend */
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		/* Initalize context and glclear current buffer */
		glfwMakeContextCurrent(m_Window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		/* Create imgui frame */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* Get window size */
		int _sx = 0;
		int _sy = 0;

		glfwGetWindowSize(m_Window, &_sx, &_sy);

		/* Create main panel for rendering at given window */
			/* Set panel  size */
		ImGui::SetNextWindowSize(ImVec2(_sx, _sy),
			ImGuiCond_Always
		);
		/* Set panel postion: default 0, 0*/
		ImGui::SetNextWindowPos(ImVec2(0, 0),
			ImGuiCond_Always
		);
		/* Set panel params */
		ImGui::Begin(m_Name.c_str(), nullptr,
			/* Flags */
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
		);
		glfwPollEvents();
	}

	void QF::UI::Window::func_RenderEventFinalize()
	{
		glfwPollEvents();
		/* Finalize draw list*/
		ImGui::End();
		/* Get render and pass data to open gl */
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		/* Swap buffers to finalize */
		glfwSwapBuffers(m_Window);
		/* Destroy backend */
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
	/* Mouse motion event */
	void QF::UI::Window::hk_MouseMotionEvent()
	{
		const QF::Utils::Vec2 _MousePosition = g_MousePosition();

		/* Check if in the same place */
		if (m_MouseMotionEventLastPlaced == _MousePosition)
			return;
		/* Override mouse position */
		m_MouseMotionEventLastPlaced = _MousePosition;

		/* Propagate further */
		hk_TopToBottomPanelBasedEventPropagation<EventSystem::MouseMotionEvent>(
			[&](Panel* _Panel) -> bool {
				/* Check if its suitable for current panel */
				if (!QF::Utils::Math::is_InBounds(
					_MousePosition, _Panel->g_PositionOffset(), _Panel->g_SizeOffset()
				)) return false; 

				/* Propagate event */
				return true; 
			},
			[&](Panel* _Panel) -> EventSystem::MouseMotionEvent {
				EventSystem::MouseMotionEvent _Event(_Panel); return _Event;
			}
		);
	}

	/* Mouse click event */
	void QF::UI::Window::hk_MouseClickEvent()
	{
		const bool _LeftButtonHeld = (
			glfwGetMouseButton(m_Window , GLFW_MOUSE_BUTTON_1) == GLFW_PRESS
			);

		/* Check if there is anything to process */
		if (_LeftButtonHeld == true && m_MouseClickEventLastFrameHeld)
			return; 
		/* Check if last frame was held to avoid detecting drag */
		if (m_MouseClickEventLastFrameHeld == true)
		{
			/* Override last frame held in order to process this event next frame */
			m_MouseClickEventLastFrameHeld = false; return;
		}
		/* Check if should process further */
		if (not _LeftButtonHeld) return; 
		/* Override to avoid processing same event next frame */
		m_MouseClickEventLastFrameHeld = true;
		/* Propagate */
				
		hk_TopToBottomPanelBasedEventPropagation<EventSystem::MouseClickedEvent>(
			[&](Panel* _Panel) -> bool {
				const QF::Utils::Vec2 _MousePosition = g_MousePosition();
				/* Check if its suitable for current panel */
				if (!_Panel->is_InBounds(_MousePosition)) return false;
				/* Setting up things for other event handlers that require a panel click */
				m_MousePanelDragEventClickedAtPanel = _Panel;
				m_MousePanelDragEventClickedAtPos = _MousePosition;
				/* Propagate event */
				return true;
			},
			[&](Panel* _Panel) -> EventSystem::MouseClickedEvent {
				EventSystem::MouseClickedEvent _Event(_Panel, g_MousePosition()); return _Event;
			}
		);
		/* Call resize for action */
		m_Resize->hk_MouseClick();
	}
	/* Mouse Panel Drag Event */
	void QF::UI::Window::hk_MousePanelDragEvent()
	{
		/* Check if clicked at any panel */
		if (m_MousePanelDragEventClickedAtPanel == nullptr) return;

		/* Get glfw leftmouse button state */
		const bool _LeftButtonHeld = (
			glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS
			);
		/* Abort if not held */
		if (!_LeftButtonHeld)
		{ /* Set ClickedAtPanel to nullptr to avoid breaking this event in next frame */
			m_MousePanelDragEventClickedAtPanel = nullptr;	
			return;
		}
		/* Create event */
		EventSystem::MousePanelDragEvent _Event{
			m_MousePanelDragEventClickedAtPanel,
			m_MousePanelDragEventClickedAtPos
		};
		/* Propagate event */
		m_MousePanelDragEventClickedAtPanel->g_EventHandler()->Dispatch(_Event);
	}
