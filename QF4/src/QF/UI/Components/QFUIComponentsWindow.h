#pragma once
#include "../../QF.h"

namespace QF
{

	namespace UI
	{
		namespace Components
		{
			class Window {
			public:
				Window(const QF::Utils::Vec2& _Position, 
					const QF::Utils::Vec2& _Size);

				/* Destructor */
				void destroyObject();
				~Window();
			public:

				void hook_MainLoop() {
					/* Override main loop flag */
					m_InMainLoop = true;

					glfwMakeContextCurrent(g_GLFWobject()->g_Object());
					ImGui::SetCurrentContext(g_GLFWobject()->g_ImGuiContext());

					/* Initialize backend */

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
					ImGui::Begin("asd", nullptr,
						/* Flags */
						ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
					);
					//glfwPollEvents();
					glfwPollEvents();
					/* @hook */
					
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
					GLFWobject();
					~GLFWobject();

					void createObject(); 

					void s_PositionStarting(const QF::Utils::Vec2& _Pos);
					void s_SizeStarting(const QF::Utils::Vec2& _Size);

					const bool s_Position(const QF::Utils::Vec2& _Pos);
					const bool s_Size(const QF::Utils::Vec2& _Size);
					
					const QF::Utils::Vec2 g_Size() const; 

					ImGuiContext* g_ImGuiContext();
					GLFWwindow* g_Object();
				private:
					const bool createImGuiContext(); 

					void generateDefaultStartingSize();
					void generateDefaultStartingPosition();

					GLFWwindow* m_Object;
					ImGuiContext* m_ImGuiContext;

					QF::Utils::Vec2 m_PositionStarting;
					QF::Utils::Vec2 m_SizeStarting;
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