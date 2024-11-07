#include "../QF.h"

/*========================= Ownership handling: what am i? =========================*/
	void QF::UI::Element::im_Panel(Panel* _Panel, Element* _Parent)
	{
		if (_Panel == nullptr || _Parent == nullptr) return; 

		m_Panel = _Panel;
		/* Set absolute parent of this window based on _Parent passed as argument */
		m_AbsoluteParent = _Parent->g_AbsoluteParent();
	}

	void QF::UI::Element::im_Window(Window* _Window)
	{
		m_Window = _Window;
		/* Also set absolute parent */
		m_AbsoluteParent = _Window;
	}
/*========================= Ownership getting: am i this? =========================*/
	QF::UI::Window* QF::UI::Element::g_Window() const
	{
		return m_Window;
	}

	QF::UI::Panel* QF::UI::Element::g_Panel() const
	{
		return m_Panel;
	}

	QF::UI::Window* QF::UI::Element::g_AbsoluteParent() const
	{
		return m_AbsoluteParent;
	}