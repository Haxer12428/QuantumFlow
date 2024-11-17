#include "QFUtilsRect.h"

/* Constructors */
	QF::Utils::Rect::Rect() 
		: x{0.0f}, y{0.0f}, sx{0.0f}, sy{0.0f} 
	{}

	QF::Utils::Rect::Rect(float _x, float _y, float _sx, float _sy) 
		: x{_x}, y{_y}, sx{_sy}, sy{_sy}
	{}	

	QF::Utils::Rect::Rect(const QF::Utils::Vec2& _Start, const QF::Utils::Vec2& _Size) 
		: x{_Start.x}, y{_Start.y}, sx{_Size.x}, sy{_Size.y}
	{}
/* Transformations */
	const QF::Utils::Vec2 QF::Utils::Rect::g_Position() const {
		return { x, y };
	}

	const QF::Utils::Vec2 QF::Utils::Rect::g_Size() const {
		return { sx, sy };
	}

	const QF::Utils::Vec2 QF::Utils::Rect::g_FinalPosition() const {
		return (g_Position() + g_Size());
	}

