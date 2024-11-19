#include "QFUIComponentsWindow.h"

namespace utils = QF::Utils;
namespace components = QF::UI::Components;
using self = QF::UI::Components::Window;

/* Constructor & Destructor */
	QF::UI::Components::Window::Window(QF::UI::App* _Application,
		const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size, const std::string& _Name, bool _CustomTitleBar
	) 
		: m_Application{ _Application }, Element()
	{
		try 
		{
			im_Window(this);

			/* Create glfw object */
			m_GLFWobject = std::make_unique<GLFWobject>(this, _Name);

			/* Set starting position & size assigned to window */
			m_GLFWobject->s_PositionStarting(_Position);
			m_GLFWobject->s_SizeStarting(_Size);

			/* Set custom title bar state */
			m_GLFWobject->s_CustomTitleBarState(_CustomTitleBar);

			/* Create glfw window */
			m_GLFWobject->createObject();

			/* Declare as a child of application event handler */
			m_Application->g_WindowHandler()
				->im_Child(this);
			
		}
		catch (const std::exception& _Exception) { 
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, std::format(
				"Window creation failed: {}", _Exception.what()
			));}

		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Intiialized successfully");
	}

	void QF::UI::Components::Window::destroyObject() {
		/* Check if in main loop */
		if (is_InMainLoop()) {
			/* If in main loop just set the flag
				* Dont to anything except that, destructor call will be handled in main loop 
			*/
			m_DestructionWanted = true; 
			return;
		}
		/* Regular destruction outside of 'events' */
		delete this; 
	}

	QF::UI::Components::Window::~Window() {
		/* Override flag to tell window Manager that u cannot push me or use me ;) */
		m_Destructed = true; 

		/* Let the ppl know */
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, 
			std::format("Destroyed: [{}]", g_ImmutableID())
			);
	}
/* Public get's */
	std::unique_ptr<QF::UI::Components::Window::GLFWobject>& QF::UI::Components::Window::g_GLFWobject() {
		return m_GLFWobject;
	}

	/* Retruns legitimate id or __QF_UNDEFINED int->(long long) on error */
	const long long QF::UI::Components::Window::g_ImmutableID() const {
		return m_ImmutableID;
	}

	const bool QF::UI::Components::Window::is_Destructed() const {
		return m_Destructed; 
	}

	const bool QF::UI::Components::Window::is_InMainLoop() const {
		return m_InMainLoop;
	}

	const bool QF::UI::Components::Window::is_WantingToDestruct() const {
		return m_DestructionWanted;
	}
	/* Destruction */
/* Public set's */
	const bool QF::UI::Components::Window::s_ImmutableID(const long long& _ID) {
		/* Cannot be modified more than once,
			* Security reasons 
		*/
		if (m_ImmutableID != __QF_UNDEFINED) {
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__,
				"Cannot modify id for a window instance more than once, if this message was sent by engine it means QF might have undefined behaviour"
			); 
			return false; 
		}
		/* Set */
		m_ImmutableID = _ID;
		return true; 
	}
/* Children handling -> Public */
	void QF::UI::Components::Window::im_Child(std::unique_ptr<Panel> _Child) {
		if (_Child == nullptr) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Panel cannot become a children of Window, its nullptr"
			); return; 
		}
		/* Set immutable id */
		if (!_Child->s_ImmutableId(g_NewImmutableIdForChild())) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Immutable id for this panel is already generated. This might cause real problems, shutting downm qf."
				);
			__QF_SHUTDOWN_MSG();
			/* Runtime error */
			throw std::runtime_error("Cannot define immutable id");
		}
		/* Push back to children stack */
		m_Children.push_back(std::move(_Child));
		/* Propagate event that tell's panel that it can link with other panels as its linked to window */
		m_Children.back()->g_EventHandler()->Dispatch(
			EventSystem::Events::PanelAssignedToWindowStack{}
			);

		/* Log success */
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, std::format(
			"Assigned panel to children stack, immutable_id: {}", m_Children.back()->g_ImmutableId()
			));
	}

	void QF::UI::Components::Window::i_WantToBeAssigned(Panel* _Child) {
		m_ChildrenAssigmentStack.push_back(std::unique_ptr<Panel>(_Child));
	}

/* Children handling -> Private */
	const long long QF::UI::Components::Window::g_NewImmutableIdForChild() {
		m_ChildrenImmutableIdsCount++; return m_ChildrenImmutableIdsCount;
	}

	void QF::UI::Components::Window::assignChildrenFromStack() {
		/* Assign 1 per loop */
		while (!m_ChildrenAssigmentStack.empty()) {
			/*
				Check for incorrect push:
					i still dont know unique_ptr's or chat gpt is a retard
			*/
			if (m_ChildrenAssigmentStack.front() != nullptr) {
				im_Child(std::move(m_ChildrenAssigmentStack.front()));
				/* Log success */
				__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
					"Successfully pushed panel; assign stack -> children stack"
				);
			}

			/* Deletion of leftover nullptr */
			if (m_ChildrenAssigmentStack.front() == nullptr)
			{
				m_ChildrenAssigmentStack.erase(m_ChildrenAssigmentStack.begin());
			}
		}
	}
/* Main loop -> public */
	void QF::UI::Components::Window::hook_MainLoop() {
		/* Main loop early call */
		mainloopEarly();

		/* Set flag */
		m_InMainLoop = true; 

		/* Events */
		mainloopEventMouseMotion();

		/* Prepare for render */
		mainloopPrepareForRender();

		/* Render call */
		mainloopRender();

		/* Finalize render */
		mainloopFinalizeRender();

		/* Set flag */
		m_InMainLoop = false; 
	}
/* Main loop -> private */
	void QF::UI::Components::Window::mainloopRender()
	{
		childrenEventPropagationBottomToTop<QF::UI::Components::EventSystem::Events::Render>(
			[&](std::unique_ptr<Panel>& _Panel) -> bool
			{
				return (_Panel->is_VisibleFixed());
			},
			[&](std::unique_ptr<Panel>& _Panel) -> QF::UI::Components::EventSystem::Events::Render
			{
				return QF::UI::Components::EventSystem::Events::Render{};
			}
		);
	}

	void QF::UI::Components::Window::mainloopEarly() {
		/* Assign children from stack */
		assignChildrenFromStack();
	}

	void QF::UI::Components::Window::mainloopPrepareForRender() {
		/* Get glfw object */
		GLFWwindow* glfwWindow = m_GLFWobject->g_Object();

		/* Set context's */
		glfwMakeContextCurrent(glfwWindow);
		ImGui::SetCurrentContext(m_GLFWobject->g_ImGuiContext());

		/* Pool events */
		glfwPollEvents();

		/* Call glfwobject's main loop */
		m_GLFWobject->hook_MainLoop();

		/* Handle gl buffer */
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Create imgui's frame */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* Create main panel for rendering */
		ImGui::SetNextWindowSize(m_GLFWobject->g_Size(), ImGuiCond_Always);

		/* Set panel position as 0, 0 */
		ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_Always);

		/* Set panel params */
		ImGui::Begin(std::to_string(g_ImmutableID()).c_str(), nullptr,
			/* Flags */
			ImGuiWindowFlags_NoTitleBar | 
			ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoScrollbar | 
			(m_GLFWobject->is_Animating() ? ImGuiWindowFlags_NoInputs : 0)
			);
		
	}

	void QF::UI::Components::Window::mainloopFinalizeRender() {
		/* Get glfw object */
		GLFWwindow* glfwWindow = m_GLFWobject->g_Object();

		/* Set context's */
		glfwMakeContextCurrent(glfwWindow);
		ImGui::SetCurrentContext(m_GLFWobject->g_ImGuiContext());

		/* Pool events */
		glfwPollEvents();

		/* Finalize draw list*/
		ImGui::End();
		ImGui::EndFrame();

		//ImGui::UpdatePlatformWindows();
		ImGui::UpdatePlatformWindows();

		/* Get render  */
		ImGui::Render();

		/* Pass to opengl  */
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap buffers to finalize */
		glfwSwapBuffers(m_GLFWobject->g_Object());
	}

	/* Main loop mouse motion event */
	void self::mainloopEventMouseMotion() {
		const utils::Vec2 mousePositionThisFrame = m_GLFWobject->g_MousePositionFixed();
		/* Check for change */
		if (m_EventMouseMotionLastFramePosition == mousePositionThisFrame) return; 

		std::cout << mousePositionThisFrame << "\n";
		/* Override */
		m_EventMouseMotionLastFramePosition = mousePositionThisFrame;

		/* Propagate */
		childrenEventPropagationTopToBottom<QF::UI::Components::EventSystem::Events::MouseMotionEvent>(
			[&](std::unique_ptr<Panel>& _Panel) -> bool
			{
				return (_Panel->is_VisibleFixed() && 
					mousePositionThisFrame.is_InBounds(_Panel->g_FixedPosition(), _Panel->g_FinalPositionFixed()
					));
			},
			[&](std::unique_ptr<Panel>& _Panel) -> QF::UI::Components::EventSystem::Events::MouseMotionEvent
			{
				return QF::UI::Components::EventSystem::Events::MouseMotionEvent{
					(mousePositionThisFrame - _Panel->g_FixedPosition()), mousePositionThisFrame
				};
			},
			true
		);
	}

