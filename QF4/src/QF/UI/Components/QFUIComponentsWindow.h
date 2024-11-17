#pragma once
#include "../../QF.h"
#include "../App/QFUIApp.h"
#define NOMINMAX
#include <windows.h>
#include <unordered_map>
#include <typeindex>

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

#pragma region Panel_H
namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class Panel : public QF::UI::Components::Element {
			public:
				Panel(Element* _Parent, const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size, bool _Care = true);
				~Panel();

				const bool s_ImmutableId(long long _Id);
				Element* g_Parent() const;
				const long long g_ImmutableId() const; 

				std::unique_ptr<EventSystem::EventHandler>& g_EventHandler(); 

				void renderer(QF::UI::Components::EventSystem::Event& _Event) {
					printf("renderer called\n");
				}
			private:
				void assignAsChildrenToAbsoluteParent();

			private:
				Element* m_Parent; 
				std::vector<Panel*> m_Children; 

				/* Immutable id: for more explanation go to widnow header */
				long long m_ImmutableId = static_cast<long long>(__QF_UNDEFINED);

				/* Event handler */
				std::unique_ptr<QF::UI::Components::EventSystem::EventHandler> m_EventHandler;
			};
		}
	}
}

#pragma endregion 

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
					bool _CustomTitleBar = true 
				);

				/* Destructor */
				void destroyObject();
				~Window();
			public:
				/* Children handling */
				void im_Child(std::unique_ptr<Panel> _Child);
				void i_WantToBeAssigned(Panel* _Child);
			public:
				void hook_MainLoop();
			private:
				void mainloopEarly();
				void mainloopPrepareForRender();
				void mainloopFinalizeRender();

				/* Event handlers */
				template<typename __EventType>
				void childrenEventPropagationBottomToTop(
					const std::function<bool(std::unique_ptr<Panel>&)>& _Condition, const std::function<__EventType(std::unique_ptr<Panel>&)>& _Event, bool _AbortOnPropagation = false
				) {
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

				public:
					GLFWobject();
					~GLFWobject();

					void createObject(); 
					void hook_MainLoop(); 

					void s_PositionStarting(const QF::Utils::Vec2& _Pos);
					void s_SizeStarting(const QF::Utils::Vec2& _Size);
					void s_CustomTitleBarState(bool _New);

					const bool s_Position(const QF::Utils::Vec2& _Pos);
					const bool s_Size(const QF::Utils::Vec2& _Size);

					const alpha8Bit g_Alpha() const; 
					void s_Alpha(const alpha8Bit& _New);

					const QF::Utils::Vec2 g_Size() const; 
					const QF::Utils::Vec2 g_Position() const; 

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

					const bool is_Animating() const; 
					const bool is_UsingCustomTitleBar() const; 

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

					QF::Utils::Vec2 m_RestoringSize;
					QF::Utils::Vec2 m_RestoringPosition; 
					/* Animation object for GLFWobjectOperations */
					std::unique_ptr<QF::Utils::BasicAnim> m_GLFWobjectOperationsAnim = nullptr;
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