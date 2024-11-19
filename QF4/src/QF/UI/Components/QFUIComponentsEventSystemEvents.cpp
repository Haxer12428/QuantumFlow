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
