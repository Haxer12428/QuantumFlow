#pragma once

#include "../../QF.h"


namespace QF {
	namespace Experimental {
		class FileDialog : public QF::UI::Components::Window {
		public:
			class Layout {
			public:
				Layout(QF::UI::Components::Window* _Window) : m_Parent{ _Window } {}; 

				struct Params {
					float m_DirSelectorX = 150.0f; 
					float m_PathBoxY = 28.0f;
				};

				const QF::Utils::Vec2 g_PathBoxPosition() const {
					return { m_Params.m_DirSelectorX, 0.0f };
				}

				const QF::Utils::Vec2 g_PathBoxSize() const {
					const QF::Utils::Rect parentClientRect = m_Parent->g_GLFWobject()->g_ClientAreaRect();
					const QF::Utils::Vec2 parentClientRectSize = parentClientRect.g_Size();

					return { parentClientRectSize.x - m_Params.m_DirSelectorX, m_Params.m_PathBoxY };
				}

				const QF::Utils::Vec2 g_DirFramePosition() const { return {m_Params.m_DirSelectorX, m_Params.m_PathBoxY}; }
				
				const QF::Utils::Vec2 g_DirFrameSize() const {
					const QF::Utils::Vec2 parentClientRectSize = m_Parent->g_GLFWobject()->g_ClientAreaRect().g_Size();
					return { parentClientRectSize.x - m_Params.m_DirSelectorX, parentClientRectSize.y - m_Params.m_PathBoxY };
				}

			private:
				QF::UI::Components::Window* m_Parent; 
				Params m_Params = Params(); 
			};

			class DirFrame : public QF::UI::Components::Panel { 
				struct Params {
					float m_PathPanelSizeY = 26.0f;
					float m_PathPanelFreeSpaceX = 30.0f; 
					ImColor m_BGColor = ImColor(33, 37, 43, 255);
				};

			public:
				class Element : public QF::UI::Components::Built::Button 
				{
				public:
					Element(DirFrame* _Parent, const std::filesystem::path& _Path, GLuint _FolderTexture, GLuint _FileTexture);
					virtual ~Element(); 

				private:
					void hookRender(QF::UI::Components::EventSystem::Events::Render& _Render);
					void hookDClickHandle(QF::UI::Components::EventSystem::Events::MouseDoubleClickEvent& _Dclick);

					const float m_TexturePushX = 5.0f;
					const float m_StatsPushX = 10.0f;
					const std::filesystem::path m_Path; 
					std::string m_Name;
					DirFrame* m_Parent; 
					bool m_IsDirectory;
					GLuint m_FolderTexture;
					GLuint m_FileTexture;
				};


				/* Layout is handled by parent: only reasonable thing to do i guess */
				DirFrame(FileDialog* _Parent); 
				virtual ~DirFrame(); 
				

				void hookRender(QF::UI::Components::EventSystem::Events::Render& _Event); 
				void updateCurrentPath(const std::filesystem::path &_Path);
				void selectedByHand(const long long _ImmutableId);
			private:
				std::vector<Element*> m_Children; 

				Params m_Params = Params();
				std::filesystem::path m_CurrentPath = ___QF_EMPTY_STRING; 
				std::unique_ptr<QF::Utils::Image> m_FolderImage; 
				std::unique_ptr<QF::Utils::Image> m_FileImage; 

				FileDialog* m_Parent; 
			};
		public:	
			FileDialog(QF::UI::App* _ParentApplication, const std::filesystem::path& _StartingPath);
			virtual ~FileDialog(); 

			void updateCurrentPath(const std::filesystem::path& _New);
		private:
			void createPathBox(); 
			void createDirFrame();
			void handlePathBoxEnter(QF::UI::Components::Built::TextBox::EnterPressedEvent&);
		private:
			std::unique_ptr<QF::Utils::Font> m_GlobalFont; 
			std::filesystem::path m_CurrentPath;

			QF::UI::Components::Built::TextBox* m_PathBox;
			DirFrame* m_DirFrame;

			Layout m_Layout; 
		};
	}
}