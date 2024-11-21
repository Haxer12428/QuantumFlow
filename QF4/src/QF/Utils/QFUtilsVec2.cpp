#include "QFUtilsVec2.h"

using self = QF::Utils::Vec2; 

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
 /* Importing from ImVec2 */
 QF::Utils::Vec2::Vec2(ImVec2 _Vec)
	 : x{ _Vec.x }, y{ _Vec.y }
 {};

 /* Importing from vector */ 	 /* Experimental implementations, might cause errors */
 QF::Utils::Vec2::Vec2(const std::vector<float>& _Vec) {
	 /* It's empty */
	 if (_Vec.empty()) {
		 x = 0; y = 0; return; 
	 }
	 /* Only one is correct */
	 if (_Vec.size() == 1) {
		 x = _Vec[0]; y = _Vec[1]; return;
	 }
	 /* Both correct */
	 x = _Vec[0];
	 y = _Vec[1];
 }

 QF::Utils::Vec2::Vec2(const std::vector<float>& _Vec, size_t _IndexX, size_t _IndexY) {
	 /* Vector's empty */
	 if (_Vec.empty()) {
		 x = 0; y = 0; return; 
	 }
	 /* Vector doesn't have those indexes */
	 const size_t vecMaxPos = (_Vec.size() - 1);

	 /* Tried accessing variables that aren't assigned */
	 if (vecMaxPos < _IndexX || vecMaxPos < _IndexY) {
		 x = 0; y = 0; return; 
	 }
	 /* Assign correctly */
	 x = _Vec[_IndexX];
	 y = _Vec[_IndexY];
 }
/* Operators */
 QF::Utils::Vec2 QF::Utils::Vec2::operator-(const Vec2& _Other)
 {
	 return { x - _Other.x, y - _Other.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator+(const Vec2& _Other)
 {
	 return { x + _Other.x, y + _Other.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator*(const Vec2& _Other)
 {
	 return { x * _Other.x, y * _Other.y };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator*(const float& _Other)
 {
	 return { x * _Other, y * _Other };
 }

 QF::Utils::Vec2 QF::Utils::Vec2::operator/(const Vec2& _Other)
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

 QF::Utils::Vec2& QF::Utils::Vec2::operator=(const Vec2& _Other) {
	 if (this == &_Other) return *this;
	 /* Copy */
	 this->x = _Other.x; 
	 this->y = _Other.y;

	 return *this;
 }

 const bool QF::Utils::Vec2::operator<=(const QF::Utils::Vec2& _Other) const {
	 return ((this->x <= _Other.x) && (this->y <= _Other.y));
 }

 const bool QF::Utils::Vec2::operator==(const QF::Utils::Vec2& _Other) {
	 return ((this->x == _Other.x) && (this->y == _Other.y));
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

 const std::vector<float> QF::Utils::Vec2::g_DataVector() const {
	 return { x, y };
 }

 QF::Utils::Vec2& QF::Utils::Vec2::limit(const Vec2& _Limit) {
	 x = std::min(x, _Limit.x);
	 y = std::min(y, _Limit.y);

	 return *this;
 }

 self QF::Utils::Vec2::clamp(const self& _First, const self& _Last) const {
	 float clampedX = std::clamp(x, _First.x, _Last.x);
	 float clampedY = std::clamp(y, _First.y, _Last.y);

	 return { clampedX, clampedY };
 }
/* Chekcs */
 const bool QF::Utils::Vec2::is_InBounds(const Vec2& _First, const Vec2& _Size) const {
	 return (
		 (_First.x <= this->x && this->x <= (_First.x + _Size.x)) &&
		 (_First.y <= this->y && this->y <= (_First.y + _Size.y))
		 );
 }