#include "QFUIComponentsWindow.h"

void QF::UI::Components::Element::im_Panel(QF::UI::Components::Panel* _Panel) {
	if (ami_Window()) {
		declarationFail(); return; 
	}
	m_Panel = _Panel;
	m_AbsoluteParent = _Panel->g_Parent()->g_AbsoluteParent();
}

void QF::UI::Components::Element::im_Window(QF::UI::Components::Window* _Window) {
	if (ami_Panel()) {
		declarationFail(); return; 
	}	
	m_Window = _Window;
	m_AbsoluteParent = m_Window;
}

const bool QF::UI::Components::Element::ami_Panel() const {
	return (m_Panel != nullptr);
}

const bool QF::UI::Components::Element::ami_Window() const {
	return (m_Window != nullptr); 
}

const bool QF::UI::Components::Element::has_AbsoluteParent() const {
	return (m_AbsoluteParent != nullptr);
}

QF::UI::Components::Window* QF::UI::Components::Element::g_AbsoluteParent() const {
	return m_AbsoluteParent;
}

QF::UI::Components::Window* QF::UI::Components::Element::g_Window() const {
	return m_Window;
}

QF::UI::Components::Panel* QF::UI::Components::Element::g_Panel() const {
	return m_Panel;
}

void QF::UI::Components::Element::declarationFail() {
	__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, 
		"Declaration failed, declared as other object already. Cannot be declared as 2 objects at the same time"
		);
	__QF_SHUTDOWN_MSG();
	throw std::runtime_error("Declaration failed, read debug");
}