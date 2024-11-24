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
/* Key event: Char */
	self::CharEvent::CharEvent(const std::string& _Pressed, int _Key, GLFWwindow* _WinObject)
		: m_Pressed{_Pressed}, m_WinObject{_WinObject}, m_Key(_Key)
	{
		__QF_ASSERT(_WinObject != nullptr, "Win object required");
		__QF_ASSERT(_Key != __QF_UNDEFINED, "Key cannot be undefined");
	}

	const bool self::CharEvent::is_Held(int _Key) const {
		return glfwGetKey(m_WinObject, _Key) == GLFW_PRESS;
	}

	const std::string self::CharEvent::g_Char() const { return m_Pressed; }
	const bool self::CharEvent::has_Char() const { return !(m_Pressed.size() == 0); }
	const int self::CharEvent::g_Key() const { return m_Key; }

