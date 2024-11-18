#include "../../QFUIComponentsWindow.h"

/* Constructor & Destructor */
	QF::UI::Components::Window::GLFWobject::CustomTitleBar::CustomTitleBar(Window* _Parent) 
		: Panel(_Parent, {0.0f}, {0.0f}, 
				Panel::Flags::m_DontCareAboutFixedPosition |
				Panel::Flags::m_DontCareAboutFixedSize |
				Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero
			) {
		uint64_t alignmentId = this->alignMatchSizeWith(
			[=](Panel* _Panel) -> QF::Utils::Vec2 {
				const QF::Utils::Vec2 titleBarSize = _Parent->g_GLFWobject()->g_TitleBarRect().g_Size();

				return titleBarSize;
			}, Panel::AlignmentFlags::m_AlignX | Panel::AlignmentFlags::m_AlignY);

		alignDestroyCallback(alignmentId);

		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Created.");
	}

	QF::UI::Components::Window::GLFWobject::CustomTitleBar::~CustomTitleBar() {
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Destroyed.");
	}