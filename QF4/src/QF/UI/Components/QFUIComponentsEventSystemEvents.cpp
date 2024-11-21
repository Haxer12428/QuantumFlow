#include "QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace utils = QF::Utils;
using self = QF::UI::Components::EventSystem::Events;

/* Mouse event: base */
	self::MouseEvent::MouseEvent(const utils::Vec2& _PosFixed, const utils::Vec2& _Pos) 
		: m_Pos{_Pos}, m_PosFixed{_PosFixed}
	{}

	const utils::Vec2 self::MouseEvent::g_Position() const { return m_Pos; }
	const utils::Vec2 self::MouseEvent::g_PositionFixed() const { return m_PosFixed; }
/* Mouse event: motion */
	self::MouseMotionEvent::MouseMotionEvent(const utils::Vec2& _PosFixed, const utils::Vec2& _Pos)
		: MouseEvent{ _PosFixed, _Pos } {};
/* Mouse event: click */
	self::MouseClickEvent::MouseClickEvent(const utils::Vec2& _PosFixed, const utils::Vec2& _Pos)
		: MouseEvent{ _PosFixed, _Pos } {}; 
/* Mouse event: panel drag */
	self::MousePanelDragEvent::MousePanelDragEvent(const utils::Vec2& _PosFixed, const utils::Vec2& _Pos,
		const utils::Vec2& _PosClickedFixed, const utils::Vec2& _PosClicked
	)
		: m_PosClicked{_PosClicked}, m_PosClickedFixed{_PosClickedFixed},
		MouseMotionEvent{ _PosFixed, _Pos } 
	{};

	const utils::Vec2 self::MousePanelDragEvent::g_PositionClicked() const { return m_PosClicked; }
	const utils::Vec2 self::MousePanelDragEvent::g_PositionClickedFixed() const { return m_PosClickedFixed;  }
