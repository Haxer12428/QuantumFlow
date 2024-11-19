#include "../../QFUIComponentsWindow.h"


namespace components = QF::UI::Components;
namespace utils = QF::Utils;
namespace comp_built = components::Built;


/* Constructor & Destructor */

	components::Window::GLFWobject::CustomTitleBar::CustomTitleBar(Window* _Parent) 
		: Panel(_Parent, {0.0f}, {0.0f}, 
				Panel::Flags::m_DontCareAboutFixedPosition |
				Panel::Flags::m_DontCareAboutFixedSize |
				Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero
			) {
		/* Create layout buttons */


		/* Create buttons */
		createButtons(3);


		/* Set aligner */
		uint64_t alignmentId = this->alignMatchSizeWith(
			[=](Panel* _Panel) -> utils::Vec2 {
				const utils::Vec2 titleBarSize = _Parent->g_GLFWobject()->g_TitleBarRect().g_Size();

				/* Set buttons sizes */
				for (auto _Button : m_Buttons) {
					_Button->s_Size(titleBarSize.y);
				}

				return titleBarSize;
			}, Panel::AlignmentFlags::m_AlignX | Panel::AlignmentFlags::m_AlignY);

		/* Bind render */
		g_EventHandler()->Subscribe<components::EventSystem::Events::Render>(this, &CustomTitleBar::Render);

		/* Set aligner for buttons */
		alignButtonsPosition();

		/* Log creation */
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Created.");
	}

	components::Window::GLFWobject::CustomTitleBar::~CustomTitleBar() {
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Destroyed.");
	}

/* Get's -> Public */
	components::Window::GLFWobject::CustomTitleBar::Hints& components::Window::GLFWobject::CustomTitleBar::g_Hints() {
		return m_Hints;
	}
/* Render */
	void components::Window::GLFWobject::CustomTitleBar::Render(components::EventSystem::Events::Render&) {
		components::SimpleDC canvas{this};
		/* Draw bg */
		canvas.putRectFilled({0.0f}, g_Size(), m_Hints.m_BGColor);
		/* Draw name */
		std::string windowName = g_AbsoluteParent()->g_GLFWobject()->g_Name();

		canvas.putText({ 0.0f }, windowName, m_Hints.m_NameColor, 
			components::SimpleDC::DrawingFlags::m_AlignCenterX | components::SimpleDC::DrawingFlags::m_AlignCenterY
			);
	}
/* Internals */
	void components::Window::GLFWobject::CustomTitleBar::alignButtonsPosition()
	{
		/* Allign from left */
		for (size_t _Iterator = 0; _Iterator < m_Buttons.size(); 
			_Iterator++) {
			comp_built::Button* buttonInstance = m_Buttons[_Iterator];

			buttonInstance->alignMatchPositionWith([=](Panel* _Panel) -> utils::Vec2 {
				utils::Vec2 positionNew = { g_Size().x - (_Panel->g_Size().x * (_Iterator + 1)), 0.0f };

				return positionNew;
			}, Panel::AlignmentFlags::m_AlignX);
		}
	}

	void components::Window::GLFWobject::CustomTitleBar::createButtons(int _Count) {
		/* Button default hints */
		comp_built::Button::Hints buttonHints;
		buttonHints.m_BGColor = m_Hints.m_BGColor;
		buttonHints.m_PanelFlags = Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero | Panel::Flags::m_DontCareAboutFixedPosition | Panel::Flags::m_DontCareAboutFixedSize;

		/* Button creation */
		for (int _ = 0; _ < _Count; _++) { m_Buttons.push_back(new comp_built::Button(this, buttonHints)); }


	}
	