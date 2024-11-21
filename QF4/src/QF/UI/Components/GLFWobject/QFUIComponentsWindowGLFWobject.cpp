#include "../QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
using comp_window = components::Window;
namespace utils = QF::Utils;

/* Constructor & Destructor */
	QF::UI::Components::Window::GLFWobject::GLFWobject(QF::UI::Components::Window* _Parent, const std::string& _Name) : 
		m_Object{ nullptr }, 
		m_SizeStarting{ __QF_DONT_CARE, __QF_DONT_CARE },
		m_PositionStarting{ __QF_DONT_CARE, __QF_DONT_CARE },
		m_Parent{_Parent},
		m_Name{_Name}
	{
		/* Initialize animation class */
		animateGLFWobjectInit();
	}

	QF::UI::Components::Window::GLFWobject::~GLFWobject() {
		/* Set context's to avoid deleting not wanted backend's & avoid opengl's buffers glitches */
		if (m_Object == nullptr) return; 
		/* This is essential as fuck 
			U retarded fuck better read this before doing anythign here: 
				*or prepare for day-long seesion of shit vs 'local debugger'
			the order of this operations is i think only one that works 
			to future me: be carefull when u modify window's code
				it needs to be in this exact order + 
				it needs to reinit imgui for each widnow after 
				one is deleted, i know its stupid!
				But what we can do? Nothing!
				I mean we can, raw opengl
				But then what will be the issue? 
				Kernel? GPU driver? 
				keep going man

						*/

		/* Destroy imgui's content */
		ImGui::SetCurrentContext(m_ImGuiContext);
		glfwDestroyWindow(m_Object);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_ImGuiContext);

	}
/* Getting params by user */
	const QF::Utils::Vec2	QF::UI::Components::Window::GLFWobject::g_Position() const {
		/* Check for object existance */
		if (m_Object == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"GLFWobject doesn't exist; m_Object == nullptr"
			);
			return {};
		}
		/* Get */
		int x, y; 
		glfwGetWindowPos(m_Object, &x, &y);
		/* Return casted */
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	const QF::Utils::Vec2 QF::UI::Components::Window::GLFWobject::g_MousePositionFixed() const {
		double x, y; 
		glfwGetCursorPos(m_Object, &x, &y);

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	const utils::Vec2 comp_window::GLFWobject::g_MousePosition() const {
		const utils::Vec2 windowPosition = g_Position();
		const utils::Vec2 fixedMousePosition = g_MousePositionFixed();

		return (windowPosition + fixedMousePosition);
	}


	const std::string QF::UI::Components::Window::GLFWobject::g_Name() const {
		return glfwGetWindowTitle(m_Object);
	}


	const bool QF::UI::Components::Window::GLFWobject::is_UsingCustomTitleBar() const {
		return m_CustomTitleBar;
	}
	
	const QF::Utils::Rect QF::UI::Components::Window::GLFWobject::g_TitleBarRect() const {
		/* Custom title bar */
		if (m_CustomTitleBar) {
			return QF::Utils::Rect{{0.0f, 0.0f}, { g_Size().x, m_CustomTitleBarSizeY }};
		}
		/* Check for m object existance */
		if (m_Object == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "GLFWobject doesn't exist;"); return {};
		};

		/* Window's title bar */
		RECT windowsTitleBarRect;
		HWND windowHWND = glfwGetWin32Window(m_Object);

		if (GetWindowRect(windowHWND, &windowsTitleBarRect)) {
			/* Get window area */
			int windowWidth = (windowsTitleBarRect.right - windowsTitleBarRect.left);
			int windowHeight = (windowsTitleBarRect.bottom - windowsTitleBarRect.top);

			RECT windowsClientRect;
			GetClientRect(windowHWND, &windowsClientRect);

			/* Get client area */
			int windowClientWidth = (windowsClientRect.right - windowsClientRect.left);
			int windowClientHeight = (windowsClientRect.bottom - windowsClientRect.top);

			/* Return rect of title bar (windows)*/
			return QF::Utils::Rect{
				{0.0f, 0.0f}, { static_cast<float>(windowWidth), static_cast<float>(windowHeight - windowClientHeight) }
				};
		}
		__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Failed to retrive window rect;"); return {};
	}

	const QF::Utils::Rect QF::UI::Components::Window::GLFWobject::g_ClientAreaRect() const {
		const QF::Utils::Vec2 windowSize = g_Size();
		const QF::Utils::Vec2 titleBarSize = g_TitleBarRect().g_Size();

		/* Calculate */
		const QF::Utils::Vec2 clientAreaStart = { 0.0f, titleBarSize.y };
		const QF::Utils::Vec2 clientAreaSize = (windowSize - QF::Utils::Vec2{0.0f, titleBarSize.y});

		return { clientAreaStart, clientAreaSize };
	}

	/* Based on mouse position fixed (to window) */
	const bool components::Window::GLFWobject::is_MouseInClientArea() const {
		const utils::Rect clientRect = g_ClientAreaRect();
		return g_MousePositionFixed().is_InBounds(clientRect.g_Position(), clientRect.g_Size());
	}

	const QF::Utils::Rect QF::UI::Components::Window::GLFWobject::g_FixedClientAreaRect() const
	{
		/* Check for custom title bar */
		if (m_CustomTitleBar) {
			return g_ClientAreaRect();
		}
		/* Fixed so on normal (windows) title bar it will start from 0,0 to avoid rendering issues */
		QF::Utils::Rect clientAreaRect = g_ClientAreaRect();
		return { {0.0f, 0.0f}, clientAreaRect.g_FinalPosition() - clientAreaRect.g_Position() };
	}

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

	void QF::UI::Components::Window::GLFWobject::s_CustomTitleBarState(bool _New) 
	{
		m_CustomTitleBar = _New;	
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

	const bool QF::UI::Components::Window::GLFWobject::is_GLFWIconified() const {
		return (glfwGetWindowAttrib(m_Object, GLFW_ICONIFIED) == GLFW_TRUE);
	}

	const bool QF::UI::Components::Window::GLFWobject::is_GLFWMaximized() const {
		return (glfwGetWindowAttrib(m_Object, GLFW_MAXIMIZED) == GLFW_TRUE);
	}

	const QF::UI::Components::Window::GLFWobject::alpha8Bit QF::UI::Components::Window::GLFWobject::g_Alpha() const {
		HWND windowHWND = glfwGetWin32Window(m_Object);
		BYTE alphaCurrent = 255; 
		DWORD flags;

		/* Check for style */
		LONG_PTR exWindowStyle = GetWindowLongPtr(windowHWND, GWL_EXSTYLE);

		if (!(exWindowStyle & WS_EX_LAYERED)) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Failed to retrive layered window attributes"
			); return 
				alpha8Bit{ static_cast<float>(alphaCurrent) };
		}

		/* Getting alpha */
		if (!GetLayeredWindowAttributes(windowHWND, nullptr, &alphaCurrent, &flags)) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Failed to retrive alpha channel."
			); return
				alpha8Bit{ static_cast<float>(alphaCurrent) };
		}

		/* Flags don't match */
		if (!(flags & LWA_ALPHA)) {
			alphaCurrent = 255;
		}
		/* Return struct */
		return alpha8Bit{ static_cast<float>(alphaCurrent) };
	}

	/* Range from 0 - 255 (float), sets alpha for entire window object  */
	void QF::UI::Components::Window::GLFWobject::s_Alpha(const alpha8Bit& _New)
	{
		/* Get hwnd & Style */
		HWND hwnd = glfwGetWin32Window(m_Object);
		LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);

		/* Set attribs */
		SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, 0, (BYTE)(_New.g()), LWA_ALPHA);
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

		/* Check for titlebar */
		const bool isUsingCustomTitleBar = is_UsingCustomTitleBar();

		/* Make it border less, -> if set to 
			QF will generate titlebar & resize support
		*/
		if (isUsingCustomTitleBar) {
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		}

		/* Create object with positons not set eg: 1, 1 
			glfw is returning nullptr when window is created with negative integeters
		*/
		m_Object = glfwCreateWindow(1, 1, m_Name.c_str(), NULL, NULL);
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

		/* SET ONLY IF USING CUSTOM TITLE BAR -> WINDOWS WILL GENERATE THIS BY ITSELF, fucked system */
		if (isUsingCustomTitleBar)
		{
			s_Position(m_PositionStarting);
		}

		/* Create ImGui's context */
		if (!createImGuiContext()) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Failed to create ImGui's context."
			); 
			throw std::runtime_error("ImGui's context creation error");
		}

		/* Update animation restore data */
		animateGLFWobjectUpdateRestoreData();

		/* Initialize custom title bar */
		if (m_CustomTitleBar) {
			m_CustomTitleBar = new CustomTitleBar(m_Parent);
		}
	}

	void QF::UI::Components::Window::GLFWobject::reinitImgui() {
		ImGui::SetCurrentContext(m_ImGuiContext);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(m_ImGuiContext);
		
		/* Initialize */
		createImGuiContext();
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
		/* Custom titler bar isn't used -> generated by windows  */
		if (!is_UsingCustomTitleBar()) {
			return; 
		}

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
/* Private */
	/* Window's main loop handler for GLFWobject instance */
	void QF::UI::Components::Window::GLFWobject::hook_MainLoop() {
		/* Call GLFWoperations handler (animation purposes, not heavy) */
		handle_GLFWobjectOperations();
	}

	/* Window operations 
		minimalize, maximize, resore

		modes:
			animated (custom logic)
			default (glfw)
	*/

/* Public */
	/* Sets the logic of operations like minimalize of current glfs window, can be animated || regular (glfw's)*/
	void QF::UI::Components::Window::GLFWobject::s_GLFWobjectOperationsAnimationState(bool _New) {
		if (!is_UsingCustomTitleBar()) {
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, 
				"Animations aren't supported on regular taskbar"
				); return;
		}
		/* Set as regular */
		m_AnimateGLFWobjectOperations = _New;	
	}
/* Private */
	void QF::UI::Components::Window::GLFWobject::animateGLFWobjectInit()
	{
		/* Create animation instances */
		m_GLFWobjectOperationsAnim = std::make_unique<QF::Utils::BasicAnim>();
	}


	/* Main handler */
	void QF::UI::Components::Window::GLFWobject::handle_GLFWobjectOperations() {
		/* Iconification by glfw -> handle  */
		auto handleGLFWiconification = [&]() -> void {
			const bool glfwIconified = is_GLFWIconified();

			/* Case: iconfied now, wasn't before && not minimalized -> Minimalize & uniconify  */
			if (glfwIconified && not m_GLFWIconifiedLastFrame && not m_Minimalized) {
				glfwRestoreWindow(m_Object);
				minimalize();
				return; 
			}

			/* Case: iconified before, not iconified now. Not restored || maximized -> Restore to original form*/
			if (m_GLFWIconifiedLastFrame && not glfwIconified) {
				if (m_MaximizedBeforeMinimalizing == true) {
					maximalize();
				} else {
					restore();
				}
				return; 
			}
		};

		/* Call iconfication handler & update flag afterwards */
		handleGLFWiconification();
		m_GLFWIconifiedLastFrame = is_GLFWIconified();

		if (m_GLFWobjectOperationsAnim->is_Finished()) return; 
		
		/* Get animated values */
		std::vector<float> valuesAnimated = m_GLFWobjectOperationsAnim
			->g_Animated(m_AnimateGLFWobjectDurationMS);
		
		/* Set for object*/
			/* Format
			Position [0 - 1]
			Size [2 - 3]
			Alpha [4]
		*/

		s_Position({ valuesAnimated, 0, 1 });
		s_Size({ valuesAnimated, 2, 3 });
		s_Alpha(valuesAnimated[4]);

		/* If minimalized & finished -> iconify the window & set the flag  */
		if (m_Minimalized && m_GLFWobjectOperationsAnim->is_Finished()) {
			glfwIconifyWindow(m_Object);
			m_GLFWIconifiedLastFrame = true; 
		}

		/* Check for maximization without custom taskbar */
		if (m_Maximized && m_GLFWobjectOperationsAnim->is_Finished()) {
			glfwMaximizeWindow(m_Object); 
		}
	}

	/* Animation apply -> used by user functions */
	void QF::UI::Components::Window::GLFWobject::animateGLFWobjectApplyAnim(
		const QF::Utils::Vec2& _WantedPosition, const QF::Utils::Vec2& _WantedSize, const alpha8Bit& _WantedAlpha)
	{
		/* Format
			Position [0 - 1] 
			Size [2 - 3]
			Alpha [4]
		*/

		/* Window current params */
		QF::Utils::Vec2 windowPos, windowSize; 

		windowPos = g_Position();
		windowSize = g_Size();
		const float windowAlpha = g_Alpha().g();

		/* Create anim params */
			std::vector<float> valuesStartingFormatedSum =
			{
				windowPos.x, windowPos.y, windowSize.x, windowSize.y, windowAlpha
			};
		
			std::vector<float> valuesFinalFormatedSum =
			{
				_WantedPosition.x, _WantedPosition.y, _WantedSize.x, _WantedSize.y, _WantedAlpha.g()
			};
			
			/* Check for maximizing from minimalized */
			if (m_MaximizedBeforeMinimalizing && m_Maximized) {
				/* Override params to avoid jitterin animation (probalby caused by glfw, no clue tho)*/
				valuesStartingFormatedSum[0] = valuesFinalFormatedSum[0];
				valuesStartingFormatedSum[1] = valuesFinalFormatedSum[1];
			}


		/* Set anim params */
		m_GLFWobjectOperationsAnim->s_Starting(valuesStartingFormatedSum);
		m_GLFWobjectOperationsAnim->s_Wanted(valuesFinalFormatedSum);
		/* Start animating */
		m_GLFWobjectOperationsAnim->startAnimation();
	}

	void QF::UI::Components::Window::GLFWobject::zeroGLFWobjectOperationStats() {
		m_Maximized = false; 
		m_Minimalized = false; 
		m_Restored = false; 
	}

	void QF::UI::Components::Window::GLFWobject::animateGLFWobjectUpdateRestoreData() {
		/* Check if its maximalized or minimalized */
		

		/* Apply current get's */
		m_RestoringPosition = g_Position();
		m_RestoringSize = g_Size();
	}
/* Public functions (user) */
	void comp_window::GLFWobject::destroy() {
		glfwSetWindowShouldClose(m_Object, true);
	}

	void QF::UI::Components::Window::GLFWobject::minimalize() {
		/* Check if anim is enabled */
		if (!m_AnimateGLFWobjectOperations) {
			glfwIconifyWindow(m_Object); return;
		}

		if (m_Minimalized) return;

		/* Update restore data ->& check if maximized */
			if (m_Maximized) {
				/* Set the flag */
				m_MaximizedBeforeMinimalizing = true; 
			} else {
				animateGLFWobjectUpdateRestoreData();
				m_MaximizedBeforeMinimalizing = false; 
			}

		/* Clear flags */
		zeroGLFWobjectOperationStats();

		/* Get minimalized position */
			const QF::Utils::Vec2 minimalizedPos = QF::Utils::System::g_TaskbarInfo().m_Position;
		
		/* Set animations & set the flag */
			animateGLFWobjectApplyAnim(minimalizedPos, { 0.0f }, alpha8Bit{ 0.0f });
		m_Minimalized = true;
	}

	void QF::UI::Components::Window::GLFWobject::maximalize() {
		/* Check if anim is enabled */
		if (!m_AnimateGLFWobjectOperations) {
			glfwMaximizeWindow(m_Object); return;
		}

		if (m_Maximized) return; 
		/* Update restore data & avoid going back to minimalized when restoring */
		if (!m_MaximizedBeforeMinimalizing) animateGLFWobjectUpdateRestoreData();

		/* Clear flags */
		zeroGLFWobjectOperationStats();

		/* Get maximized position & size */
			glfwMaximizeWindow(m_Object);
			const QF::Utils::Vec2 maximizedPos = g_Position();
			const QF::Utils::Vec2 maximizedSize = g_Size();
			glfwRestoreWindow(m_Object);

		/* Set animations & set the flag */
			m_Maximized = true;
			animateGLFWobjectApplyAnim(maximizedPos, maximizedSize, alpha8Bit{ 255.0f });
	}

	void QF::UI::Components::Window::GLFWobject::restore() {
		/* Check if anim is enabled */
		if (!m_AnimateGLFWobjectOperations) {
			glfwRestoreWindow(m_Object); return;
		}

		if (m_Restored) return; 
		/* Clear flags */
		zeroGLFWobjectOperationStats();
		glfwRestoreWindow(m_Object);

		/* Set animations & set the flag */
			animateGLFWobjectApplyAnim(m_RestoringPosition, m_RestoringSize, alpha8Bit{ 255.0f });
		m_Restored = true;
	}
	/* Get's */
	const bool QF::UI::Components::Window::GLFWobject::is_Maximized() const {
		return (m_AnimateGLFWobjectOperations ? m_Maximized : glfwGetWindowAttrib(m_Object, GLFW_MAXIMIZED) == GLFW_TRUE);
	}

	const bool QF::UI::Components::Window::GLFWobject::is_Minimalized() const {
		return (m_AnimateGLFWobjectOperations ? m_Minimalized : is_GLFWIconified());
	}

	const bool QF::UI::Components::Window::GLFWobject::is_Restored() const {
		return m_Restored;
	}

	const bool QF::UI::Components::Window::GLFWobject::is_Animating() const {
		return (m_AnimateGLFWobjectOperations ? (m_GLFWobjectOperationsAnim != nullptr ? !m_GLFWobjectOperationsAnim->is_Finished() : false) : false);
	}
/* 8 Bit alpha struct */
	QF::UI::Components::Window::GLFWobject::alpha8Bit::alpha8Bit(float _Value
	) : m_Value{ std::clamp(_Value, 0.0f, 255.0f) } 
	{}

	const float QF::UI::Components::Window::GLFWobject::alpha8Bit::g() const { return m_Value; };

	const float QF::UI::Components::Window::GLFWobject::alpha8Bit::g_Normalized() const { return (m_Value / 255.0f); };