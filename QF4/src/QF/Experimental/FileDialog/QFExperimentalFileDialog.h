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

			private:
				QF::UI::Components::Window* m_Parent; 
				Params m_Params = Params(); 
			};

		public:
			FileDialog(QF::UI::App* _ParentApplication, const std::filesystem::path& _StartingPath);
			virtual ~FileDialog(); 
		private:

			void createPathBox(); 
			void handlePathBoxEnter(QF::UI::Components::Built::TextBox::EnterPressedEvent&);
		private:
			std::unique_ptr<QF::Utils::Font> m_GlobalFont; 
			std::filesystem::path m_CurrentPath;
			QF::UI::Components::Built::TextBox* m_PathBox;

			Layout m_Layout; 
		};
	}
}