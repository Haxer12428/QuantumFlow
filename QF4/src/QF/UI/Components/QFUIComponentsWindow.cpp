#include "QFUIComponentsWindow.h"

namespace utils = QF::Utils;
namespace components = QF::UI::Components;
using self = QF::UI::Components::Window;

using ccl = std::chrono::high_resolution_clock;
using evts = QF::UI::Components::EventSystem::Events;

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
		if (this == nullptr) return true; 
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
	void QF::UI::Components::Window::i_WantTobeDisassigned(Panel* _Panel) {
		__QF_ASSERT(_Panel, "panel cannot be unassigned, its not existing");

		m_ChildrenIdsDisassignmentStack.push_back(_Panel);
	}
	
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
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Put panel is assignment queue.");
	}

	QF::UI::App* self::g_Application() {
		return m_Application;
	}

/* Children handling -> Private */
	const long long QF::UI::Components::Window::g_NewImmutableIdForChild() {
		m_ChildrenImmutableIdsCount++; return m_ChildrenImmutableIdsCount;
	}

	void QF::UI::Components::Window::disassignChildrenFromStack() {
		//if (!m_ChildrenAssigmentStack.empty()) return; 
		//__QF_ASSERT(!m_ChildrenAssigmentStack.empty() && !m_ChildrenIdsDisassignmentStack.empty(), "cannot dissasign when there are element to be assigned");

		auto disassign = [&](Panel* _Child) -> const bool {
			const long long childImmutableId = _Child->g_ImmutableId();
			__QF_ASSERT(childImmutableId != static_cast<long long>(__QF_UNDEFINED), "children not assigned; engine error\n");
			
			size_t childrenIterator = 0; 
			for (auto& _Obj : m_Children) {
				if (_Obj && _Obj->g_ImmutableId() == childImmutableId) {
					auto erasePos = (m_Children.begin() + childrenIterator);

					m_Children.erase(erasePos);
					return true; 
				}
				childrenIterator++;
			}
			return false; 
		}; 

		while (!m_ChildrenIdsDisassignmentStack.empty()) {
			if (!disassign(m_ChildrenIdsDisassignmentStack.front())) 
			{ /* Fail */
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
				__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Could not disassign child.");
	#endif 
#endif // !NDEBUG

			} /* Success */ else {
				m_ChildrenIdsDisassignmentStack.erase(m_ChildrenIdsDisassignmentStack.begin());
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1 
				__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Successfully disassigned child.");
	#endif 
#endif // !NDEBUG

			}
		}
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

		/* Focus handle */
		mainloopChildrenHandleFocus();

		/* Events */
		mainloopEventMouseMotion();
		mainloopEventMouseClick();
		mainloopEventMousePanelDrag();
		mainloopEventChar();

		/* Prepare for render */
		mainloopPrepareForRender();

		/* Render call */
		mainloopRender();

		/* Finalize render */
		mainloopFinalizeRender();

		disassignChildrenFromStack();
		assignChildrenFromStack();

		/* Set flag */
		m_InMainLoop = false; 
	}
/* Main loop -> private */
	void QF::UI::Components::Window::mainloopRender()
	{
		childrenEventPropagationBottomToTop<QF::UI::Components::EventSystem::Events::Render>(
			[&](std::unique_ptr<Panel>& _Panel) -> bool
			{
				_Panel->g_EventHandler()->Dispatch(EventSystem::Events::BeforeRender{});
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
		//ImGui::UpdatePlatformWindows();

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
	/* Main loop mouse click event 
		Also handles dclick, saves me code, adds complexity :) 
	*/
	void self::mainloopEventMouseClick() {
		/* Event propagate check */
		auto propageteEvent = [&]() -> bool {
			bool currentLMBstate = glfwGetMouseButton(
				g_GLFWobject()->g_Object(), GLFW_MOUSE_BUTTON_LEFT
			) == GLFW_PRESS;

			if (m_EventMouseClickLastFrameHeld == currentLMBstate) return false;

			if (m_EventMouseClickLastFrameHeld && !currentLMBstate) {
				m_EventMouseClickLastFrameHeld = false; return false;
			}

			m_EventMouseClickLastFrameHeld = currentLMBstate; 
			return true; 
		};

		if (!propageteEvent()) return;

		int doubleClickTimeMs = static_cast<int>(GetDoubleClickTime());
		const utils::Vec2 mousePosition = g_GLFWobject()->g_MousePositionFixed();

		/* Mouse dclick event */
		/* TODO: move this to other func(handler)  */
		if (std::chrono::duration_cast<std::chrono::milliseconds>(ccl::now() - m_EventMouseClickedOnPanelWhen).count() <= doubleClickTimeMs) {
			auto propagateDClick = [&]() -> const bool {
				if (!m_EventMouseClickedOnPanelNoChange) return false; 

				if (!mousePosition.is_InBounds(
					m_EventMouseClickedOnPanelNoChange->g_FixedPosition(), 
					m_EventMouseClickedOnPanelNoChange->g_FinalPositionFixed()
					)) return false; 

				utils::Vec2 fixedDClickPos = (mousePosition - m_EventMouseClickedOnPanelNoChange->g_FixedPosition());

				m_EventMouseClickedOnPanelNoChange->g_EventHandler()->Dispatch(evts::MouseDoubleClickEvent{fixedDClickPos, mousePosition});
				
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
				__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Dispatched mouse DClick event");
	#endif 
#endif // !NDEBUG
				return true; 
			};
			
			if (propagateDClick()) return; 
		}

		/* Propagate */
		childrenEventPropagationTopToBottom<QF::UI::Components::EventSystem::Events::MouseClickEvent>(
			[&](std::unique_ptr<Panel>& _Panel) -> bool
			{
				const bool boundsAndVisibilityCheckResult = (_Panel->is_VisibleFixed() &&
					mousePosition.is_InBounds(_Panel->g_FixedPosition(), _Panel->g_FixedSize()
					));
				return boundsAndVisibilityCheckResult;
			},
			[&](std::unique_ptr<Panel>& _Panel) -> QF::UI::Components::EventSystem::Events::MouseClickEvent
			{
				/* Get positions */
				utils::Vec2 pos, fixedPos;

				fixedPos = (mousePosition - _Panel->g_FixedPosition());
				pos = mousePosition;
				/* Override vars */
				m_EventMouseClickedOnPanel = _Panel.get();
				m_EventMouseClickedOnPanelPos = pos;
				m_EventMouseClickedOnPanelPosFixed = fixedPos; 
				m_EventMouseClickedOnPanelNoChange = _Panel.get();
				m_EventMouseClickedOnPanelWhen = ccl::now();

				/* Return created */
				return QF::UI::Components::EventSystem::Events::MouseClickEvent{
					fixedPos, pos 
				};
			},
			true
		);
	}
	/* MainLoop: mouse panel drag event */
	void self::mainloopEventMousePanelDrag() {
		auto propagateEvent = [&]() -> const bool {
			if (!m_EventMouseClickedOnPanel) return false; 

			bool currentLMBstate = glfwGetMouseButton(
				g_GLFWobject()->g_Object(), GLFW_MOUSE_BUTTON_LEFT
			) == GLFW_PRESS;

			if (!currentLMBstate) {
				m_EventMouseClickedOnPanel = nullptr; return false; 
			}
			return true; 
		};

		if (!propagateEvent()) return; 

		const utils::Vec2 mousePosition = g_GLFWobject()
			->g_MousePositionFixed();

		components::Panel* panelObj = m_EventMouseClickedOnPanel;

		utils::Vec2 pos, fixedPos; 
		pos = mousePosition; 
		fixedPos = (pos - panelObj->g_FixedPosition());

		panelObj->g_EventHandler()
			->Dispatch(
				QF::UI::Components::EventSystem::Events::MousePanelDragEvent{
			fixedPos, pos, m_EventMouseClickedOnPanelPosFixed, m_EventMouseClickedOnPanelPos
		});
	}

	/* MainLoop: char event */
	void self::mainloopEventChar() {
		struct keyStat {
			int m_ClickedKey = __QF_UNDEFINED;
			std::vector<int> m_HeldKeys;
		};

		GLFWwindow* glfwObj = g_GLFWobject()->g_Object();

		auto g_KeyData = [&]() -> keyStat {
			keyStat keyData;

			for (int _Key = GLFW_KEY_SPACE; _Key <= GLFW_KEY_LAST; _Key++) {
				int keyState = glfwGetKey(glfwObj, _Key);
				/* Check for position */
				__QF_ASSERT(_Key < m_EventCharKeyHeldLastFrame.size(), "incorrect key assignment");

				if (keyState == GLFW_PRESS && m_EventCharKeyHeldLastFrame[_Key] == false) {
					keyData.m_ClickedKey = _Key;
				}

				if (keyState == GLFW_PRESS) {
					keyData.m_HeldKeys.push_back(_Key);
					m_EventCharKeyHeldLastFrame[_Key] = true;
				}
				else { m_EventCharKeyHeldLastFrame[_Key] = false; }
			}
			return keyData; 
		};
		
		auto getEventKey = [&]() -> const int {
			keyStat keyData = g_KeyData();

			if (keyData.m_ClickedKey != __QF_UNDEFINED) {
				m_EventCharHeld = keyData.m_ClickedKey;
				m_EventCharHeldSince = ccl::now();
				return m_EventCharHeld;
			};

			if (m_EventCharHeld == __QF_UNDEFINED) return __QF_UNDEFINED;

			if (glfwGetKey(g_GLFWobject()->g_Object(), m_EventCharHeld) != GLFW_PRESS) {
				m_EventCharHeld = __QF_UNDEFINED; return __QF_UNDEFINED;
			};

			int repeatDelay = __QF_UNDEFINED, repeatRate = __QF_UNDEFINED;

			SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &repeatDelay, 0);
			SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &repeatRate, 0);

			__QF_ASSERT(((repeatDelay != __QF_UNDEFINED) && (repeatRate != __QF_UNDEFINED)), "couldn't extract system info");

			/* fix repeat delay */
			repeatDelay = repeatDelay * 1000;

			int heldForMs = std::chrono::duration_cast<std::chrono::milliseconds>(ccl::now() - m_EventCharHeldSince).count() - repeatDelay;

			/* Check for system's repeat reate */
			if (heldForMs < repeatRate) return __QF_UNDEFINED;

			/* Apply fixed for  repeat delay */
			m_EventCharHeldSince = (ccl::now() - std::chrono::milliseconds(repeatDelay));

			return m_EventCharHeld;
		};
		
		/* Get key */
		const int eventKey = getEventKey();

		if (eventKey == __QF_UNDEFINED) return;

		/* Get char (keyName) */
		const char* glfwKeyName = glfwGetKeyName(eventKey, 0);
		std::string keyName = (glfwKeyName == nullptr ? ___QF_EMPTY_STRING : glfwKeyName);

		/* Fix for space */
		if (keyName.empty() && eventKey == GLFW_KEY_SPACE) {
			keyName = " ";
		}

		auto transfromKeyNameToSpecialChar = [&](std::string& _keyname) {
			std::unordered_map<char, char> charTransformations = {
				{'1', '!'},
				{'2', '@'},
				{'3', '#'},
				{'4', '$'},
				{'5', '%'},
				{'6', '^'},
				{'7', '&'},
				{'8', '*'},
				{'9', '('},
				{'0', ')'},
				{'-', '_'},
				{'=', '+'},
				{';', ':'},
				{',', '<'},
				{'.', '>'},
				{'\'', '"'},
				{'/', '?'},
				{'[', '{'},
				{']', '}'},
				{'\\', '|'},
				{'`', '~'}
			};

			std::transform(_keyname.begin(), _keyname.end(), _keyname.begin(), [&](char c) {
				auto it = charTransformations.find(c);
				if (it != charTransformations.end()) return it->second;
				return c;
				});
		};

		/* Handle upper str */
		if (!keyName.empty()) {
			bool capslockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
			bool shiftOn = false; 

			if (glfwGetKey(glfwObj, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
				glfwGetKey(glfwObj, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
				shiftOn = true; 
				transfromKeyNameToSpecialChar(keyName);
			};
			
			if (capslockOn && !shiftOn || !capslockOn && shiftOn) {
				std::transform(keyName.begin(), keyName.end(), keyName.begin(), [](unsigned char c) {
					return std::toupper(c);
					});
			};
		}

		/* Dispatch event for focused panel: only this can be focues and only 1 pane can be focused 
			Im to lazy to implement TODO: ->g_FocusedPanel(); <- nullptr | std::unique_ptr<Panel>&
		*/
		if (m_EventMouseClickedOnPanelNoChange != nullptr) {

			m_EventMouseClickedOnPanelNoChange
				->g_EventHandler()->Dispatch<evts::CharEvent>
				(
				evts::CharEvent{keyName, eventKey, g_GLFWobject()->g_Object()}
				);
		};
 
#ifndef NDEBUG 
		#if __QF_DEBUG_LEVEL == 0
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Propagated char event");
		#endif
#endif // !NDEBUG

	}

	void self::mainloopChildrenHandleFocus() {
		const long long focusedId = (
			m_EventMouseClickedOnPanelNoChange == nullptr ? __QF_UNDEFINED : m_EventMouseClickedOnPanelNoChange->g_ImmutableId()
			);

		auto applyFocusForChildren = [&](const long long& _Id) -> void {
			for (auto& _Child : m_Children) {
				const long long idOfChild = _Child->g_ImmutableId();
				__QF_ASSERT(idOfChild != __QF_UNDEFINED, "engine internal error, kindof weird that it dind't crashed before");

				if (idOfChild == focusedId) {
					_Child->s_Focus(true);
				} 
				else { _Child->s_Focus(false); }

#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL == 0 
				__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Set focus for children. Check message after for more info;");
	#endif
#endif // !NDEBUG

			}};
		/* Apply focus */
		applyFocusForChildren(focusedId);

#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL == 0 
			__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Finished");
	#endif 
#endif // NDEBUG

	}
