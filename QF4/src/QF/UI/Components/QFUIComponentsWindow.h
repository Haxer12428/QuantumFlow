#pragma once
#include "../../QF.h"
#include "../App/QFUIApp.h"
#define NOMINMAX
#include <windows.h>
#include <unordered_map>
#include <typeindex>
#include <array>
#include "imgui.h"

#pragma region EventSystem_H
namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class EventSystem
			{
			public:
			
				class Event {
				public:
					virtual ~Event() = default; // Ensure proper cleanup for derived classes
				};

				class Events
				{
				public:

					class Render : public Event {
					public: 
						virtual ~Render() = default; 
						Render() = default;
					};

					class BeforeRender : public Event {
					public:
						virtual ~BeforeRender() = default; 
						BeforeRender() = default;
					};

					class PanelAssignedToWindowStack : public Event {
					public:
						PanelAssignedToWindowStack() = default;
						virtual ~PanelAssignedToWindowStack() = default;
					};

					class MouseEvent : public Event {
					public:
						MouseEvent(const QF::Utils::Vec2& FixedTo, const QF::Utils::Vec2& Regular);

						const QF::Utils::Vec2 g_Position() const;
						const QF::Utils::Vec2 g_PositionFixed() const; 
					private:
						QF::Utils::Vec2 m_PosFixed, m_Pos;
					};

					class MouseMotionEvent : public MouseEvent {
					public:
						MouseMotionEvent(const QF::Utils::Vec2& _Fixed, const QF::Utils::Vec2& _Regular);
					};

					class MouseClickEvent : public MouseEvent {
					public:
						MouseClickEvent(const QF::Utils::Vec2& _Fixed, const QF::Utils::Vec2& _Regular);
					};

					class MousePanelDragEvent : public MouseMotionEvent {
					public:
						MousePanelDragEvent(const QF::Utils::Vec2& _Fixed, const QF::Utils::Vec2& _Regular, const QF::Utils::Vec2& _PosClickedFixed, const QF::Utils::Vec2& _PosClicked);
					
						const QF::Utils::Vec2 g_PositionClicked() const;
						const QF::Utils::Vec2 g_PositionClickedFixed() const;
					private:
						QF::Utils::Vec2 m_PosClickedFixed, m_PosClicked;
					};

					class CharEvent : public Event {
					public:
						CharEvent(const std::string& _Char, int _Key, GLFWwindow* _WinObject);

						const bool is_Held(int _Key) const;
						const std::string g_Char() const;
						const bool has_Char() const;
						const int g_Key() const;
					private:
						GLFWwindow* m_WinObject = nullptr; 
						std::string m_Pressed = ___QF_EMPTY_STRING; 
						int m_Key = __QF_UNDEFINED;
					};
				};

				class EventHandler {
				private:
					/* Event handler funcion */

					using EventHandlerFunc = std::function<void(Event&)>;

					/* Liseners */
					std::unordered_map<std::type_index,
						std::vector<EventHandlerFunc>
					> m_Listeners;

				public:
					~EventHandler() {};
					EventHandler() {};

					/* Subscibe event, _EventType, class instance,& class function definition */
					template <typename __EventType, typename __ClassType>
					void Subscribe(__ClassType* _Instance,
						void (__ClassType::* _MemberFunction)(__EventType&)
					)
					{
						m_Listeners[typeid(__EventType)].emplace_back(
							/* Add to liseners */
							[_Instance, _MemberFunction](Event& _Evt)
							{
								/* Cast event */
								(_Instance->*_MemberFunction)(
									static_cast<__EventType&>(_Evt)
									);
							}

						);
					}

					/* Dispatch event (call event), _EventType */
					template <typename __EventType>
					void Dispatch(__EventType _Evt)
					{
						auto _It = m_Listeners.find(typeid(__EventType));

						/* Check for existance */
						if (_It == m_Listeners.end())
							return;
						for (const auto& handler : _It->second)
						{ /* Dispatch for a listener */
							handler(_Evt);
						}
					}
				};
			};
		}
	}
}
#pragma endregion

#pragma region Element_H

namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class Element
			{
			public:
				Element() = default;
				virtual ~Element() = default; 

				/* Declarations */
				void im_Panel(QF::UI::Components::Panel* _Panel);
				void im_Window(QF::UI::Components::Window* _Window);

				/* Check's */
				const bool ami_Window() const;
				const bool ami_Panel() const;

				const bool has_AbsoluteParent() const;

				/* Get's */
				QF::UI::Components::Panel* g_Panel() const;
				QF::UI::Components::Window* g_Window() const;
				QF::UI::Components::Window* g_AbsoluteParent() const;
			private:
				/* Internal */
				void declarationFail();
			private:
				/* Vars */
				QF::UI::Components::Panel* m_Panel = nullptr;
				QF::UI::Components::Window* m_AbsoluteParent = nullptr;
				QF::UI::Components::Window* m_Window = nullptr;
			};
		}
	}
}


#pragma endregion

#pragma region SimpleDC

namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class SimpleDC
			{
			public:
				enum class DrawingFlags : uint64_t {
					m_None = 0,
					m_AlignCenterX = 1 << 0,
					m_AlignCenterY = 1 << 1,
					m_AlignIgnoreBoundaryLimits = 1 << 2,
					m_putTextureAbortIfTextureUndefined = 1 << 3
				};

			public:
				SimpleDC(QF::UI::Components::Panel* _Panel);
				virtual ~SimpleDC();

				void pushFont(ImFont* _Font);
				void popFont();

				void putRect(const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, ImColor _Clr, float _Rounding = 0.0f, float _Thickness = 1.0f, DrawingFlags _Flags = DrawingFlags::m_None, ImDrawFlags _ImGuiFlags = 0);
				void putRectFilled(const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, ImColor _Clr, float _Rounding = 0.0f, DrawingFlags _Flags = DrawingFlags::m_None, ImDrawFlags _ImGuiFlags = 0);
				
				/* Use push | pop font functions to switch between fonts */
				void putText(const QF::Utils::Vec2& _Pos, const std::string& _Text, ImColor _Clr, DrawingFlags _Flags = DrawingFlags::m_None);
				
				void putTexture(const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, GLuint _GLTextureID, DrawingFlags _Flags = DrawingFlags::m_None, ImU32 col = IM_COL32_WHITE, const QF::Utils::Vec2& uv_min = { 0, 0 }, const QF::Utils::Vec2& uv_max = { 1, 1 });
				
				void putLine(const QF::Utils::Vec2& _Pos1, const QF::Utils::Vec2& _Pos2, ImU32 col, float thickness = 1.0f, DrawingFlags _Flags = DrawingFlags::m_None);
			
				void putCircleFilled(const QF::Utils::Vec2& _Center, float _Radius, ImU32 _Color, int _Segments = 0, DrawingFlags _Flags = DrawingFlags::m_None);

				const QF::Utils::Vec2 g_TextSize(const std::string& _Text) const; 
			private:
				void alignInitFunctions();

				const QF::Utils::Vec2 fixVec2(const QF::Utils::Vec2& _Pos) const;


				const QF::Utils::Rect g_AlignedRect(const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, DrawingFlags _Flags);
				

				std::vector<std::function<const QF::Utils::Rect(const QF::Utils::Vec2&, const QF::Utils::Vec2&, DrawingFlags)>> m_AlignFunctions;
			private:
				QF::UI::Components::Panel* m_Panel;
				ImDrawList* m_DrawList;
				bool m_FontPushed = false;
			};
		}
	}
}

#pragma endregion 


#pragma region Panel_H
namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class SimpleDC;

			class Panel : public QF::UI::Components::Element {
			public:
				enum class Flags : uint64_t {
					m_None = 0,
					m_DontCareAboutFixedPosition = 1 << 0,
					m_DontCareAboutFixedSize = 1 << 1,
					m_DisplayIfSizeIsLessOrEqualToZero = 1 << 2,
					m_DontCareAboutClipRectWhenRendering = 1 << 3,
					m_CatchEventsOutsideOfClientRect = 1 << 4
				};

				enum class AlignmentFlags : uint64_t {
					m_None = 0,
					m_AlignX = 1 << 0, 
					m_AlignY = 1 << 1
				};
			public:
				Panel(Element* _Parent, const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size, const Flags& _Flags = Flags::m_None);
				virtual ~Panel();

				const bool s_ImmutableId(long long _Id);
				Element* g_Parent() const;
				const long long g_ImmutableId() const; 
				void s_Visible(bool _New);
				const bool is_VisibleFixed() const; 
				const QF::Utils::Vec2 g_Size() const; 
				const QF::Utils::Vec2 g_Position() const;
				const QF::Utils::Vec2 g_FixedSize() const;
				const QF::Utils::Vec2 g_FixedPosition() const;
				const QF::Utils::Vec2 g_FinalPositionFixed() const;
				const QF::Utils::Vec2 g_CenterPosition() const;
				const QF::Utils::Vec2 g_Center() const;
				const uint64_t g_Flags() const; 
				const bool is_Focused() const;
				void s_Focus(bool _New); 

				const bool is_Visible() const; 
				const bool is_MouseOnPanel() const; 

				const QF::Utils::Vec2 g_FinalPosition() const; 
				void s_Position(const QF::Utils::Vec2& _New); 
				void s_Size(const QF::Utils::Vec2& _New); 

				void renderingevent(QF::UI::Components::EventSystem::Events::Render& _RenderEvt)
				{
					//QF::UI::Components::SimpleDC canvas{this};

					//canvas.putRect({ 0, 0 }, g_FixedSize(), ImColor(0, 128, 255));
					//canvas.putRectFilled({ 0, 0 }, { 40, 30 }, ImColor(0, 0, 255), 0.0f, QF::UI::Components::SimpleDC::DrawingFlags::m_AlignCenterX | QF::UI::Components::SimpleDC::DrawingFlags::m_AlignCenterY);
				}


				void im_Child(Panel* m_Child);

				std::unique_ptr<EventSystem::EventHandler>& g_EventHandler(); 
			private:
				void assignValuesCapturedFromParent(QF::UI::Components::EventSystem::Events::BeforeRender& _r);
				void alignAndUtilRunCallbacks(QF::UI::Components::EventSystem::Events::Render& _r);

				void assignAsChildrenToAbsoluteParent();
				void assignAsChildrenToParent();
				
				const bool assertParent() const;

				void assignFixedPositionAndSizeFromParent();
				void assignFixedVisibilityFromParent();

				void assignedAsChildrenToAbsoluteParent(EventSystem::Events::PanelAssignedToWindowStack& _Event);
				
			public:
				struct utilCallback {
					uint64_t m_UniqueID;
					std::function<void(Panel*)> m_Callback;
				};
				
				const uint64_t utilAddPreRenderCallback(const std::function<void(Panel* _Panel)>& _Func);

				const uint64_t alignMatchSizeWith(const std::function<QF::Utils::Vec2(Panel* _Panel)> &_Func, AlignmentFlags _Flags);
				const uint64_t alignMatchPositionWith(const std::function<QF::Utils::Vec2(Panel* _Panel)>& _Fucn, AlignmentFlags _Flags);

				bool alignDestroyCallback(uint64_t UniqueId);

				struct allignObject {
					uint64_t m_UniqueID;
					std::function<void(Panel*)> m_Callback;
				};
			private:
				const QF::Utils::Vec2 alignModVec2BasedOnFlags(const QF::Utils::Vec2& _Current, const QF::Utils::Vec2& _Default, AlignmentFlags _Flags);
			
			private:
				Element* m_Parent; 
				std::vector<Panel*> m_Children; 

				/* Immutable id: for more explanation go to widnow header */
				long long m_ImmutableId = static_cast<long long>(__QF_UNDEFINED);

				/* Event handler */
				std::unique_ptr<QF::UI::Components::EventSystem::EventHandler> m_EventHandler;

				/* Visibility */
				bool m_VisibleFixed = true; 
				bool m_Visible = true; 
				bool m_Focused = false; 
				

				/* Position & Size */
				QF::Utils::Vec2 m_Position, m_Size; 
				QF::Utils::Vec2 m_FixedPosition, m_FixedSize; 

				std::underlying_type<Flags>::type m_Flags = std::underlying_type<Flags>::type(Flags::m_None);
				
				/* Allignment */
				std::vector<allignObject> m_AllignCallbacks;
				std::unique_ptr<QF::Utils::IdManager> m_AllignIdManager = std::make_unique<QF::Utils::IdManager>();
				
				/* Utils */
				std::vector<utilCallback> m_UtilCallbacks;
				std::unique_ptr<QF::Utils::IdManager> m_UtilCallbacksIdManager = std::make_unique<QF::Utils::IdManager>();
			};
		}
	}
}

#pragma endregion 

#pragma region Built_H 

namespace QF
{
	namespace UI
	{
		namespace Components
		{
			namespace Built
			{
				class Button : public QF::UI::Components::Panel
				{
				public:
					struct Hints
					{
						ImColor m_BGActiveColor; 
						ImColor m_BGColor;
						Panel::Flags m_PanelFlags = Panel::Flags::m_None;
						QF::Utils::Vec2 m_Pos;
						QF::Utils::Vec2 m_Size; 

						GLuint m_TextureID;
						bool m_ActAsASwitch = false; 
						bool m_SwitchTurnedOn = false; 
					};

				public:
					Hints& g_Hints();

					Button(QF::UI::Components::Element* _Element, Hints& _Hints); 
					virtual ~Button();

					const uint64_t addOnClickCallback(const std::function<void(QF::UI::Components::Built::Button*, QF::UI::Components::EventSystem::Events::MouseClickEvent& _Event)> _Callback);
					const bool destroyOnClickCallback(uint64_t UniqueID);

					Hints m_Hints;
				private:
					struct OnClickCallback {
						uint64_t m_UniqueID;
						std::function<void(Button*, QF::UI::Components::EventSystem::Events::MouseClickEvent& _Event)> m_Callback;
					};

					/* Callbacks stack */
					std::vector<
						OnClickCallback
						> m_OnClickCallbacks;

					std::unique_ptr<QF::Utils::BasicAnim> m_BGTransitionAnim = std::make_unique<QF::Utils::BasicAnim>();
					std::unique_ptr<QF::Utils::IdManager> m_OnClickCallbacksIdManager = std::make_unique<QF::Utils::IdManager>();

					void Render(QF::UI::Components::EventSystem::Events::Render& _r);
					void MouseClickEvent(QF::UI::Components::EventSystem::Events::MouseClickEvent& _r);
				};
			
				class TextBox : public QF::UI::Components::Panel { 
				public:
					struct Hints {
						ImColor m_BGColor;
						ImColor m_BGColorActive; 
						ImColor m_TextColor; 
						ImColor m_TextColorActive; 
						ImColor m_CursorColor = ImColor(255, 255, 255);
						int m_CursorBlinkTimeMs = 500;
						float m_CursorSizeExtendY = 6; 
						float m_CursorSizeX = 2;
						int m_CursorMoveSpeedMs = 35;

						ImColor m_OutlineColor = ImColor(60, 60, 60, 255);
						float m_Rounding = 1;
					};

				public: 
					TextBox(QF::UI::Components::Window* _Parent, const QF::Utils::Vec2& _Pos, const QF::Utils::Vec2& _Size, QF::UI::Components::Panel::Flags _PanelFlags, const std::string& _StartingText, std::unique_ptr<QF::Utils::Font>& _Font);
					virtual ~TextBox();

					const std::string g_Value() const; 
					void s_Value(const std::string& _Val);

					Hints& g_Hints(); 
				private:
					void handleAnimations(QF::UI::Components::EventSystem::Events::BeforeRender&);
					void handleCursorPosition(QF::UI::Components::EventSystem::Events::Render&);
					void hookRender(QF::UI::Components::EventSystem::Events::Render&);
					void handleTyping(QF::UI::Components::EventSystem::Events::CharEvent&);
				private:
					std::unique_ptr<QF::Utils::BasicAnim> m_BGColorAnim;
					std::unique_ptr<QF::Utils::BasicAnim> m_TextColorAnim; 
					std::unique_ptr<QF::Utils::BasicAnim> m_CursorColorAnim; 
					std::unique_ptr<QF::Utils::BasicAnim> m_CursorPositionAnim; 
					std::string m_CurrentText; 
					Hints m_Hints; 
					std::unique_ptr<QF::Utils::Font> m_TextFont; 
					int m_CursorAt = 0; 
					float m_CursorPos = 0; 
				}; 
			}
		}
	}
}

#pragma endregion Built_H

#pragma region Window_H
namespace QF
{

	namespace UI
	{
		
		namespace Components
		{
#pragma region Window_H -> Start
			class Window : public QF::UI::Components::Element {
			public:
				Window(App* Application, 
					const QF::Utils::Vec2& _Position = {__QF_DONT_CARE},
					const QF::Utils::Vec2& _Size = {__QF_DONT_CARE},
					const std::string& _Name = "QFWindow",
					bool _CustomTitleBar = true 
				);

				/* Destructor */
				void destroyObject();
				virtual ~Window();
			public:
				/* Children handling */
				void im_Child(std::unique_ptr<Panel> _Child);
				void i_WantToBeAssigned(Panel* _Child);

				App* g_Application(); 
			public:
				void hook_MainLoop();
			private:
				void mainloopEarly();
				void mainloopPrepareForRender();
				void mainloopFinalizeRender();
				void mainloopRender();

				/* Events */
					/* Mouse motion */
					QF::Utils::Vec2 m_EventMouseMotionLastFramePosition;
					void mainloopEventMouseMotion();
					/* Mouse click */
					QF::UI::Components::Panel* m_EventMouseClickedOnPanel = nullptr; 
					QF::UI::Components::Panel* m_EventMouseClickedOnPanelNoChange = nullptr;
					QF::Utils::Vec2 m_EventMouseClickedOnPanelPos, m_EventMouseClickedOnPanelPosFixed; 
					bool m_EventMouseClickLastFrameHeld = false; 
					void mainloopEventMouseClick(); 
					/* Mouse panel drag */
					void mainloopEventMousePanelDrag();
					/* Char */
					void mainloopEventChar();
					std::array<bool, (GLFW_KEY_LAST + 1)> m_EventCharKeyHeldLastFrame{false};
					int m_EventCharHeld = __QF_UNDEFINED;
					std::chrono::high_resolution_clock::time_point m_EventCharHeldSince = std::chrono::high_resolution_clock::now();

					void mainloopChildrenHandleFocus();

				/* Event handlers */
				template<typename __EventType>
				void childrenEventPropagationBottomToTop(
					const std::function<bool(std::unique_ptr<Panel>&)>& _Condition, const std::function<__EventType(std::unique_ptr<Panel>&)>& _Event, bool _AbortOnPropagation = false
				) {
					if (m_Children.empty()) return;
					/* Iterate throught children from bottom to top */
					for (int _Iterator = 0; _Iterator < m_Children.size(); 
						_Iterator++) {
						std::unique_ptr<Panel>& childObj = m_Children[_Iterator];

						/* If condition returns true propagate event */
						if (_Condition(childObj)) {
							(childObj->g_EventHandler())->Dispatch(_Event(childObj));

							/* Check for abortion flag */
							if (_AbortOnPropagation) return; 
						}
					}
				};

				template<typename __EventType>
				void childrenEventPropagationTopToBottom(
					const std::function<bool(std::unique_ptr<Panel>&)>& _Condition, const std::function<__EventType(std::unique_ptr<Panel>&)>& _Event, bool _AbortOnPropagation = false, bool _ClientRectCheck = true
				) {
					if (m_Children.empty()) return;
					/* Iterate throught children from bottom to top */
					for (int _Iterator = (static_cast<int>(m_Children.size()) - 1); _Iterator >= 0;
						--_Iterator) {
						std::unique_ptr<Panel>& childObj = m_Children[_Iterator];

						bool isInClipRect = true; 

						/* Fix for events to be in client area | can be skipped by flag */
						if (!(childObj->g_Flags() & static_cast<std::underlying_type<Panel::Flags>::type>(
							Panel::Flags::m_CatchEventsOutsideOfClientRect
							)) && _ClientRectCheck) {
							isInClipRect = g_GLFWobject()->is_MouseInClientArea();
						}

						/* If condition returns true propagate event */
						if (isInClipRect &&  _Condition(childObj)) {
							(childObj->g_EventHandler())->Dispatch(_Event(childObj));

							/* Check for abortion flag */
							if (_AbortOnPropagation) return;
						}
					}
				};

			private:

#pragma endregion 
#pragma region Window_H -> GLFWobject_H
				class GLFWobject {
				public:
					struct alpha8Bit {
						alpha8Bit(float _Param);
						
						/* get's */
						const float g() const; 
						const float g_Normalized() const; 

						/* iostream compatibility: operator */
						friend std::ostream& operator<<(std::ostream& _OS, const alpha8Bit& _Self) {
							_OS << std::format("alpha8Bit({})", _Self.g()); return _OS;
						}
					private:
						float m_Value;
					};

#pragma region GLFWobject_H -> CustomTitleBar_H 
					class CustomTitleBar : public QF::UI::Components::Panel {
					public:
						struct Hints
						{
							ImColor m_BGColor = ImColor(54, 54, 54);
							ImColor m_NameColor = ImColor(255, 255, 255);
							ImColor m_ButtonExitActiveColor = ImColor(120, 120, 120, 255);
							ImColor m_ButtonMinimizeActiveColor = ImColor(80, 80, 80, 255);
							ImColor m_ButtonMaximizeActiveColor = ImColor(80, 80, 80, 255);
						};

					public:
						CustomTitleBar(QF::UI::Components::Window* m_Parent);
						virtual ~CustomTitleBar();

						Hints& g_Hints();
					private:
						void Render(EventSystem::Events::Render& _r);
						void alignButtonsPosition();
						void createButtons(int _Count);

						void moveWindow(EventSystem::Events::MousePanelDragEvent& _evt);

						std::vector<std::unique_ptr<QF::Utils::Image>> m_ButtonsImages; 

						Hints m_Hints; 
						std::vector<QF::UI::Components::Built::Button*> m_Buttons;
					};
#pragma endregion
				public:
					void reinitImgui();

					GLFWobject(QF::UI::Components::Window* _Parent, const std::string& _Name);
					virtual ~GLFWobject();

					void createObject(); 
					void hook_MainLoop(); 

					void s_PositionStarting(const QF::Utils::Vec2& _Pos);
					void s_SizeStarting(const QF::Utils::Vec2& _Size);
					void s_CustomTitleBarState(bool _New);

					const bool s_Position(const QF::Utils::Vec2& _Pos);
					const bool s_Size(const QF::Utils::Vec2& _Size);
					void s_VsyncState(bool _New);

					const alpha8Bit g_Alpha() const; 
					void s_Alpha(const alpha8Bit& _New);

					const QF::Utils::Vec2 g_Size() const; 
					const QF::Utils::Vec2 g_Position() const; 
					const QF::Utils::Vec2 g_MousePositionFixed() const; 
					const QF::Utils::Vec2 g_MousePosition() const; 

					const std::string g_Name() const; 

					ImGuiContext* g_ImGuiContext();
					GLFWwindow* g_Object();


					/* Minimalization & Maximalization & Restore 
						* All of them can be custom with setting a hint 
					*/
					void minimalize();
					void maximalize(); 
					void restore(); 

					/* Dont use this to determinate whenever window is minimalized by QF 
						IT SHOULD ONLY BE USED TO CHECK WHENEVER GLFW FUCKED SOMETHING UP AND ICONIFIED THE WINDOW
					*/
					const bool is_GLFWIconified() const;
					const bool is_GLFWMaximized() const; 

					const bool is_Minimalized() const; 
					const bool is_Maximized() const; 
					const bool is_Restored() const; 

					void destroy(); 

					const bool is_Animating() const; 
					const bool is_UsingCustomTitleBar() const;
					const QF::Utils::Rect g_TitleBarRect() const; 
					const QF::Utils::Rect g_ClientAreaRect() const; 
					const QF::Utils::Rect g_FixedClientAreaRect() const; 
					const bool is_MouseInClientArea() const; 

					/* No support for regular titlebar, do override if u want to play with it*/
					virtual void s_GLFWobjectOperationsAnimationState(bool State);
				private:
					const bool createImGuiContext(); 

					void generateDefaultStartingSize();
					void generateDefaultStartingPosition();

					GLFWwindow* m_Object;
					ImGuiContext* m_ImGuiContext;

					QF::Utils::Vec2 m_PositionStarting;
					QF::Utils::Vec2 m_SizeStarting;

					/* Minimalization, maximalization : GLFWobjectOperations */
				private:
					void animateGLFWobjectInit();
					void zeroGLFWobjectOperationStats();
					void handle_GLFWobjectOperations(); 
					void animateGLFWobjectApplyAnim(const QF::Utils::Vec2& _Wanted, const QF::Utils::Vec2& _Size, const alpha8Bit& _Alpha);
					void animateGLFWobjectUpdateRestoreData(); 
				private:
					bool m_AnimateGLFWobjectOperations = false; 
					int m_AnimateGLFWobjectDurationMS = 200; /* Default delay is 300ms; something in the middle */
					
					bool m_LastFrameIconified = false;
					bool m_LastFrameMaximized = false; 

					bool m_Animating = false; 
					bool m_Maximized = false; 
					bool m_Minimalized = false; 
					bool m_Restored = false; 
					bool m_MaximizedBeforeMinimalizing = false; 
					bool m_GLFWIconifiedLastFrame = false; 
					bool m_GLFWMaximizedLastFrame = false; 
					
					bool m_CustomTitleBar = false;

					float m_CustomTitleBarSizeY = 26;

					QF::Utils::Vec2 m_RestoringSize;
					QF::Utils::Vec2 m_RestoringPosition; 
					/* Animation object for GLFWobjectOperations */
					std::unique_ptr<QF::Utils::BasicAnim> m_GLFWobjectOperationsAnim = nullptr;

					/* Custom title bar instance */
					CustomTitleBar* m_CustomTitleBarInstance = nullptr; 
					QF::UI::Components::Window* m_Parent = nullptr;

					std::string m_Name;
				};
#pragma endregion
#pragma region Window_H -> Continue			
private:
				/* Immutable identification of a window 
					* Cannot be changed after set, should be only set by windowing engine
					* Its this way because of engine structure, could i do it without this? yes, did i? no
				*/
				long long m_ImmutableID = static_cast<long long>(__QF_UNDEFINED); 
				bool m_Destructed = false; 
				bool m_DestructionWanted = false; 
				bool m_InMainLoop = false; 

				std::unique_ptr<GLFWobject> m_GLFWobject;
				/* Main application instance, essential for this class to work */
				App* m_Application;
				
				/* Children handling */
				std::vector<std::unique_ptr<Panel>> m_Children; 
				std::vector<std::unique_ptr<Panel>> m_ChildrenAssigmentStack; 

				void assignChildrenFromStack(); 

				const long long g_NewImmutableIdForChild();
				long long m_ChildrenImmutableIdsCount = -1;

			public:
				const bool is_WantingToDestruct() const; 
				const bool is_Destructed() const; 
				const bool is_InMainLoop() const;

				const bool s_ImmutableID(const long long& _ID);
				const long long g_ImmutableID() const;
				std::unique_ptr<GLFWobject>& g_GLFWobject();
			};
#pragma endregion
		}
	}
}

#pragma endregion