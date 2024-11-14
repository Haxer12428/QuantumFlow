#include "QFUtilsVec2.h"

/* Constructors */
 QF::Utils::Vec2::Vec2() 
	 : x{0.0f}, y{0.0f}
 {};

 QF::Utils::Vec2::Vec2(float _x)
	 : x{ _x }, y{ _x }
 {};

 QF::Utils::Vec2::Vec2(float _x, float _y)
	 : x{ _x }, y{ _y }
 {};
/* Operators */
 QF::Utils::Vec2 QF::Utils::Vec2::operator-(Vec2& _Other)
 {
	 return { x - _Other.x, y - _Other.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator+(Vec2& _Other)
 {
	 return { x + _Other.x, x + _Other.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator*(Vec2& _Other)
 {
	 return { x * _Other.x, y * _Other.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator*(const float& _Other)
 {
	 return { x * _Other, y * _Other };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator/(Vec2& _Other)
 {
	 QF::Utils::Vec2 otherCopy = _Other;
	 /* Handle division by 0 */
	 if (otherCopy.x == 0) otherCopy.x = 1; 
	 if (otherCopy.y == 0) otherCopy.y = 1;
	 
	 return { x / otherCopy.x, y / otherCopy.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator/(const float& _Other)
 {
	 float otherCopy = (_Other == 0 ? 1 : _Other);

	 return { x / otherCopy, y / otherCopy };
 }
/* Transformations */
 const std::string QF::Utils::Vec2::g_String() const 
 { return std::format("Vec2(x: {},y: {})", x, y); }

 const QF::Utils::Vec2 QF::Utils::Vec2::g_PositionFromRECT(const RECT& _Rect) {
	 return { static_cast<float>(_Rect.left), static_cast<float>(_Rect.top) };
 }

 const QF::Utils::Vec2 QF::Utils::Vec2::g_SizeFromRECT(const RECT& _Rect) {
	 return { static_cast<float>(_Rect.right - _Rect.left), static_cast<float>(_Rect.bottom - _Rect.top) };
 }
/* Chekcs */
 const bool QF::Utils::Vec2::is_InBounds(const Vec2& _First, const Vec2& _Size) const {
	 return (
		 (_First.x <= this->x && this->x <= (_First.x + _Size.x)) &&
		 (_First.y <= this->y && this->y <= (_First.y + _Size.y))
		 );
 }