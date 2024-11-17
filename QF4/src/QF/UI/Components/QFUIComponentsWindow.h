#pragma once
#include "../../QF.h"
#include "../App/QFUIApp.h"
#define NOMINMAX
#include <windows.h>

namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class Element
			{
			public:
				Element() {

				};

				~Element() {}

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


				void renderloop() {
					//printf("rendering\n");
				}
			private:
				void assignAsChildrenToAbsoluteParent();

			private:
				Element* m_Parent; 
				std::vector<Panel*> m_Children; 

				/* Immutable id: for more explanation go to widnow header */
				long long m_ImmutableId = static_cast<long long>(__QF_UNDEFINED);
			};
		}
	}
}


namespace QF
{
	namespace UI
	{
		namespace Components
		{
			class EventSystem
			{
			public:
				class Event
				{
				public:
					Event() = default; 

					
				};
			};
		}
	}
}

namespace QF
{

	namespace UI
	{
		
		namespace Components
		{

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
				void hook_MainLoop() {
					/* Push all children from asignment stack */
					assignChildrenFromStack();

					/* Override main loop flag */
					m_InMainLoop = true;

					/* Initialize context's */
					glfwMakeContextCurrent(g_GLFWobject()->g_Object());
					ImGui::SetCurrentContext(g_GLFWobject()->g_ImGuiContext());

					glfwSwapInterval(1);

					ImGuiIO& io1 = ImGui::GetIO();
					io1.WantCaptureMouse = false; /* true if ImGui needs to capture mouse for this window */
					io1.WantCaptureKeyboard = false;

					glfwPollEvents();
					/* Call GLFWobject mainloop's handler first */
					m_GLFWobject->hook_MainLoop();

					/* Initalize context and glclear current buffer */
					glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
					glClear(GL_COLOR_BUFFER_BIT);

					/* Create imgui frame */
					ImGui_ImplOpenGL3_NewFrame();
					ImGui_ImplGlfw_NewFrame();
					ImGui::NewFrame();

					/* Get window size */
					int _sx = 0;
					int _sy = 0;

					glfwGetWindowSize(g_GLFWobject()->g_Object(), &_sx, &_sy);

					/* Create main panel for rendering at given window */
						/* Set panel  size */
					ImGui::SetNextWindowSize(ImVec2(static_cast<float>(_sx), static_cast<float>(_sy)),
						ImGuiCond_Always
					);
					/* Set panel postion: default 0, 0*/
					ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f),
						ImGuiCond_Always
					);


					/* Set panel params */
					

					ImGui::Begin(std::to_string(g_ImmutableID()).c_str(), nullptr,
						/* Flags */
						ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | (g_GLFWobject()->is_Animating() ? ImGuiWindowFlags_NoInputs : 0)
					);
					//glfwPollEvents();



					/* @hook */
					for (std::unique_ptr<Panel>& _Child : m_Children) {
						_Child->renderloop();
					}
					
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					const QF::Utils::Vec2 Position = { 0.0f, 0.0f };
					const QF::Utils::Vec2 Size = { 5.0f * static_cast<float>(g_ImmutableID()) };

					drawList->AddRectFilled(Position, (Position + Size), ImColor(0, 128, 255));


					if (ImGui::Button("Maximize"))
					{
						// This code will be executed when the button is clicked
						// Bind your action here
						g_GLFWobject()->maximalize();

						// You can call any action, function, or event here
						// For example, toggle a boolean, change state, etc.
					}

					if (ImGui::Button("Restore"))
					{
						// This code will be executed when the button is clicked
						// Bind your action here
						g_GLFWobject()->restore();

						// You can call any action, function, or event here
						// For example, toggle a boolean, change state, etc.
					}

					if (ImGui::Button("Minimalize"))
					{
						// This code will be executed when the button is clicked
						// Bind your action here
						g_GLFWobject()->minimalize();

						// You can call any action, function, or event here
						// For example, toggle a boolean, change state, etc.
					}

					/* @endof hook */
					glfwPollEvents();
					//glfwPollEvents();
					/* Finalize draw list*/
					ImGui::End();
					ImGui::EndFrame();
					//ImGui::UpdatePlatformWindows();
					ImGui::UpdatePlatformWindows();
					/* Get render and pass data to open gl */
					ImGui::Render();

					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					/* Swap buffers to finalize */
					glfwSwapBuffers(g_GLFWobject()->g_Object());

					/* Override mainloop flag back to regular */
					m_InMainLoop = false; 
				}
			private:
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
		}
	}
}