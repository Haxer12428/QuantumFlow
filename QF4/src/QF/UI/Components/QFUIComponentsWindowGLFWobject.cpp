#include "QFUIComponentsWindow.h"

/* Constructor & Destructor */
	QF::UI::Components::Window::GLFWobject::GLFWobject() : 
		m_Object{ nullptr }, 
		m_SizeStarting{ __QF_DONT_CARE, __QF_DONT_CARE },
		m_PositionStarting{ __QF_DONT_CARE, __QF_DONT_CARE }
	{
		
	}

	QF::UI::Components::Window::GLFWobject::~GLFWobject() {
		/* Set context's to avoid deleting not wanted backend's & avoid opengl's buffers glitches */
		glfwMakeContextCurrent(m_Object);
		ImGui::SetCurrentContext(m_ImGuiContext);

		/* Shutdown the backend's */
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();

		/* Destroy imgui's content */
		ImGui::DestroyContext(m_ImGuiContext);

		/* Destroy glfw window */
		glfwDestroyWindow(m_Object);
	}
/* Getting params by user */
	const QF::Utils::Vec2 QF::UI::Components::Window::GLFWobject::g_Size() const {
		/* Check for object existance */
		if (m_Object == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"GLFWobject doesn't exist; m_Object == nullptr"
			);
			return {};
		}
		/* Get Object's size */
		int sx, sy; 
		glfwGetWindowSize(m_Object, &sx, &sy);
		/* Return casted size */
		return { static_cast<float>(sx), static_cast<float>(sy) };
	}

	ImGuiContext* QF::UI::Components::Window::GLFWobject::g_ImGuiContext()  {
		return m_ImGuiContext;
	}

	GLFWwindow* QF::UI::Components::Window::GLFWobject::g_Object() {
		return m_Object;
	}
/* Setting params by user */
	void QF::UI::Components::Window::GLFWobject::s_PositionStarting(
		const QF::Utils::Vec2& _Pos
	) {
		m_PositionStarting = _Pos; 
	}

	void QF::UI::Components::Window::GLFWobject::s_SizeStarting(
		const QF::Utils::Vec2& _Size
	) {
		m_SizeStarting = _Size; 
	}

	const bool QF::UI::Components::Window::GLFWobject::s_Position(
		const QF::Utils::Vec2& _New) 
	{
		/* Check for existance */
		if (m_Object == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Cannot set position, GLFWobject == nullptr"
			); return false;
		}

		/* Modify */
		glfwSetWindowPos(m_Object,
			static_cast<int>(_New.x),
			static_cast<int>(_New.y)
		);
		return true; 
	}

	const bool QF::UI::Components::Window::GLFWobject::s_Size(
		const QF::Utils::Vec2& _New)
	{
		/* Check for existance */
		if (m_Object == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Cannot set size, GLFWobject == nullptr"
			); return false;
		}

		glfwSetWindowSize(m_Object,
			static_cast<int>(_New.x),
			static_cast<int>(_New.y)
		);
		return true; 
	}
/* GLFW window creation */
	void QF::UI::Components::Window::GLFWobject::createObject() {
		/* GLFWobject creation check */
		if (m_Object != nullptr) {
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__,
				"GLFWobject is already created."); 
			return; 
		}
		/* Generate positions, incase: set to __QF_DONT_CARE */
		generateDefaultStartingPosition();
		generateDefaultStartingSize();

		/* Make it border less,
			QF will generate titlebar & resize support
		*/
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		/* Create object with positons not set eg: 1, 1 
			glfw is returning nullptr when window is created with negative integeters
		*/
		m_Object = glfwCreateWindow(1, 1, "GLFWwindow", NULL, NULL);
		/* Check for window creation status */
		if (m_Object == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"m_Object creation failed"
				);
			throw std::runtime_error("Window returned nullptr ");
		}
		glfwMakeContextCurrent(m_Object);

		/* Apply starting values -> calculated before hand
			* cannot apply them in constructor of GLFWwindow '-'
			* here those applied by QF api calls of current class 
		*/
		s_Size(m_SizeStarting);
		s_Position(m_PositionStarting);

		/* Create ImGui's context */
		if (!createImGuiContext()) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Failed to create ImGui's context."
			); 
			throw std::runtime_error("ImGui's context creation error");
		}
	}

	const bool QF::UI::Components::Window::GLFWobject::createImGuiContext() {
		/* Check version */
		IMGUI_CHECKVERSION();

		/* Create context */
		m_ImGuiContext = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();


		ImGui::SetCurrentContext(m_ImGuiContext);
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui_ImplGlfw_InitForOpenGL(m_Object, true);

		/* Return result */
		return (ImGui::GetCurrentContext() != nullptr);
	}

	void QF::UI::Components::Window::GLFWobject::generateDefaultStartingPosition() {
		/* Check if needs to be generated */
		if (m_PositionStarting.x != __QF_DONT_CARE && m_PositionStarting.y != __QF_DONT_CARE)
			return;

		/* Window starting position calculating algorithm 
			* working on a grid 
			* ignoring window's taskbar 
			* checking for grided windows 
		*/

		/* Getting windows & taskbar information */
		const std::vector<QF::Utils::System::WindowInfo> windowsOnScreenInfo =
			QF::Utils::System::g_AllWindowsOnMonitor();

		const QF::Utils::System::TaskbarInfo taskbarInfo = QF::Utils::System::g_TaskbarInfo();
		/* Default position if algorithm wasn't successfull */
		QF::Utils::Vec2 defaultWindowPosition = { 50.0f, 50.0f };
		/* Getting primary screen resolution (screen i will display window on)
			* i dont really care about more screens atm
		*/

		/* Get glfw monitor */
		GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();

		/* Check for glfw monitor existance */
		if (!glfwMonitor) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Failed to retrive glfw monitor, default window positions are being set"
				);
			m_PositionStarting = defaultWindowPosition; return; 
		}
		/* Get glfw video mode */
		const GLFWvidmode* glfwVideoMode = glfwGetVideoMode(glfwMonitor);

		/* Check for glfw video mode existance*/
		if (!glfwVideoMode) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Failed to retrive glfw video mode, default window positions are being set"
			);
			m_PositionStarting = defaultWindowPosition; return;
		}
		/* Get vectored screen resolution */
		const QF::Utils::Vec2 screenResolution = {
			static_cast<float>(glfwVideoMode->width), static_cast<float>(glfwVideoMode->height)
		};
		/* Update default window position to account resolution */
		defaultWindowPosition = (screenResolution.x / 2.0f);

		/* Grid settings */
		float gridAmountPerAxis = 100;

		QF::Utils::Vec2 gridSize = (screenResolution / gridAmountPerAxis);
		/* Calculate optimal grid size (very low resolution's support) */
		if (gridSize.x < 1.0f || gridSize.y < 1.0f) 
			__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__,
				"Calculating optimal grid size for current resolution, if your window is creating slowly thats probably the problem"
			);

		while (gridSize.x < 1.0f || gridSize.y < 1.0f) {
			/* Decrement and calculate new */
			gridAmountPerAxis--;

			(gridSize = (
				screenResolution / gridAmountPerAxis)
				);
		}

		gridSize.x = static_cast<float>((int)gridSize.x);
		gridSize.y = static_cast<float>((int)gridSize.y);

		auto gridCheckIsValid = [&](const QF::Utils::Vec2& _ScreenPosition) -> bool {
			/* Check if interfering with taskbar */
			if (_ScreenPosition.is_InBounds(
				taskbarInfo.m_Position, taskbarInfo.m_Size)
				)
				return false; 

			/* Check if interfering with other windows */
			for (const QF::Utils::System::WindowInfo& _WindowInfo : windowsOnScreenInfo
				) {
				/* If interfering with any of window abort with status false 
					* dont even look at this spagetification of conversion :)
				*/
				if ((
					(_WindowInfo.m_Positon.is_InBounds(
						{ 0.0f,_ScreenPosition.y }, { screenResolution.x, gridSize.y }
					) == true ) ||
					(_WindowInfo.m_Positon.is_InBounds(
						{ _ScreenPosition.x, 0.0f }, { gridSize.x, screenResolution.y }
					) == true)))
					return false; 
			}
			/* Return positive value */
			return true; 
		};


		/* Calculating best position based on grid for both axis (x, y) */

		auto gridGetValidPosition = [&]() -> const QF::Utils::Vec2 {
			for (int _x = 0; _x < gridAmountPerAxis;
				_x++)
			{ /* x axis */
				for (int _y = 0; _y < gridAmountPerAxis;
					_y++)
				{ /* y axis */
					const QF::Utils::Vec2 currentScreenPosition = {
						static_cast<float>(_x * gridSize.x), static_cast<float>(_y * gridSize.y)
					};
					/* Check if position if valid */
					if (gridCheckIsValid(
						currentScreenPosition
					)) {
						return currentScreenPosition;
					}
				};
			};
			return defaultWindowPosition;
		};

		/* Getting best window positon based on grid calculation */
		const QF::Utils::Vec2 windowBestPosition = gridGetValidPosition();

		/* Setting best window postion */
		m_PositionStarting = windowBestPosition;

		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__,
			std::format("Generated: {}", windowBestPosition.g_String())
		);
	}

	void QF::UI::Components::Window::GLFWobject::generateDefaultStartingSize() {
		QF::Utils::Vec2 startingSizeNotGenerated = { 
			__QF_DONT_CARE, __QF_DONT_CARE };
		/* Default starting size */
		QF::Utils::Vec2 defaultStartingSize = { 800, 600 };

		/* Check if generated */
		if (m_SizeStarting.x == startingSizeNotGenerated.x) {
			/* Generate for x axis */
			m_SizeStarting.x = defaultStartingSize.x;

			__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__,
				"Generated m_StartingSize.x axis");
		}

		if (m_SizeStarting.y == startingSizeNotGenerated.y) {
			/* Generate for y axis */
			m_SizeStarting.y = defaultStartingSize.y;

			__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__,
				"Generated m_StartingSize.y axis");
		}
	}