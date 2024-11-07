#include "../QF.h"

std::unordered_map < QF::Utils::Vec2::Hint, QF::Utils::Vec2::HintValue> QF::Utils::Vec2::m_GlobalVec2Hints;

/*========================= Constructor & Destructor  =========================*/
	/* Constructor with both params */
	QF::Utils::Vec2::Vec2(const int _x, const int _y)
		: m_x { static_cast<float>(_x) }, m_y { static_cast<float>(_y) }
	{}

	QF::Utils::Vec2::Vec2(const double _x, const double _y)
		: m_x{ static_cast<float>(_x) }, m_y{ static_cast<float>(_y) }
	{}

	QF::Utils::Vec2::Vec2(
		const float _x, const float _y)
		: m_x{ _x }, m_y{ _y } 
	{}
	/* Empty constructor */
	QF::Utils::Vec2::Vec2() : m_x{ 0 }, m_y{ 0 } {};
	/* Singular constructor 
		1 value for both axis 
	*/
	QF::Utils::Vec2::Vec2(const float _singular) : m_x{ _singular }, m_y{ _singular } {};
	/* Other object */
	QF::Utils::Vec2::Vec2(const ImVec2& _Vec) : m_x{ _Vec.x }, m_y{ _Vec.y } {};

/*========================= Operators =========================*/
	/* Addition */
		QF::Utils::Vec2 QF::Utils::Vec2::operator+(const QF::Utils::Vec2& _Other) const 
		{
			return QF::Utils::Vec2(m_x + _Other.m_x, m_y + _Other.m_y);
		}
	
		QF::Utils::Vec2 QF::Utils::Vec2::operator+(const float _Other) const
		{
			return QF::Utils::Vec2(m_x + _Other, m_y + _Other);
		}

		QF::Utils::Vec2 QF::Utils::operator+(const float _Scalar, const QF::Utils::Vec2& _Vector)
		{
			return QF::Utils::Vec2(_Scalar + _Vector.m_x, _Scalar + _Vector.m_y);
		}
	/* Substraction */
		QF::Utils::Vec2 QF::Utils::Vec2::operator-(const QF::Utils::Vec2& _Other) const
		{
			return QF::Utils::Vec2(m_x - _Other.m_x, m_y - _Other.m_y);
		}

		QF::Utils::Vec2 QF::Utils::Vec2::operator-(const float _Other) const
		{
			return QF::Utils::Vec2(m_x - _Other, m_y - _Other);
		}

		QF::Utils::Vec2 QF::Utils::operator-(const float _Scalar, const QF::Utils::Vec2& _Vector)
		{
			return QF::Utils::Vec2(_Scalar - _Vector.m_x, _Scalar - _Vector.m_y); 
		}
	/* Mulitiplication */
		QF::Utils::Vec2 QF::Utils::Vec2::operator*(const QF::Utils::Vec2& _Other) const
		{
			return QF::Utils::Vec2(m_x * _Other.m_x, m_y * _Other.m_y);
		}

		QF::Utils::Vec2 QF::Utils::Vec2::operator*(const float _Other) const
		{
			return QF::Utils::Vec2(m_x * _Other, m_y * _Other);
		}

		QF::Utils::Vec2 QF::Utils::operator*(const float _Scalar, const QF::Utils::Vec2& _Vector)
		{
			return QF::Utils::Vec2(_Scalar * _Vector.m_x, _Scalar * _Vector.m_y);
		}
	/* Division */
		/*
			If division hint isn't setup then its ALWAYS aborting with throwing an exception
		*/
		const QF::Utils::Vec2 QF::Utils::Vec2::g_Divider(const QF::Utils::Vec2& _Other)
		{
			/* Everything normal */
			if (_Other.m_x != 0 && _Other.m_y != 0) return _Other;

			/* Atleast one axis is 0 */
			HintValue _DivisionByZeroHint = m_GlobalVec2Hints[QF::Utils::Vec2::Hint::DIVIDE_BY_ZERO];

			std::array<float, 2> _OtherArr = { _Other.m_x, _Other.m_y };
			/* Perform operations */
			for (float& _Member : _OtherArr)
			{ /* Check for 0 */
				if (_Member == 0)
				{ /* Handle it */
					switch (_DivisionByZeroHint)
					{
					case HintValue::DIVIDE_BY_ZERO_RETURN_SELF:
						_Member = 1; 

						QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::WARNING, __FUNCTION__,
							"vector tried dividing by 0: set _Member = 0, ps: current vec2 hint is set to return self."
						);

						break; 
					default:
						QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__,
							"vector tried dividing by 0: aborting, ps: current vec2 hint is set to abort."
						);

						throw std::exception(
							(__FUNCTION__, "Hint configuration: abort. Cannot divide by 0.")
						);
					}
				}
			}
			
			return QF::Utils::Vec2(_OtherArr[0], _OtherArr[1]);
		}

		QF::Utils::Vec2 QF::Utils::Vec2::operator/(const QF::Utils::Vec2& _Other) const
		{
			const QF::Utils::Vec2 _Divider = g_Divider(_Other);
				
			return QF::Utils::Vec2(m_x / _Divider.m_x, m_y / _Divider.m_y);
		}

		QF::Utils::Vec2 QF::Utils::Vec2::operator/(const float _Other) const
		{
			const QF::Utils::Vec2 _Divider = g_Divider({ _Other, _Other });
			/* Use first of choice operator */
			return (operator/(_Divider));
		}

		QF::Utils::Vec2 QF::Utils::operator/(const float _Scalar, const QF::Utils::Vec2& _Vector)
		{
			const QF::Utils::Vec2 _Divider = QF::Utils::Vec2::g_Divider(_Vector);

			return QF::Utils::Vec2(_Scalar / _Divider.m_x, _Scalar / _Divider.m_y);
		}
	/* Plus */
		QF::Utils::Vec2& QF::Utils::Vec2::operator+=(const QF::Utils::Vec2& _Other)
		{
			(m_x += _Other.m_x); (m_y += _Other.m_y); return (*this);
		}

		QF::Utils::Vec2& QF::Utils::Vec2::operator+=(const float _Other)
		{
			(m_x += _Other); (m_y += _Other); return (*this);
		}
	/* Minus */
		QF::Utils::Vec2& QF::Utils::Vec2::operator-=(const QF::Utils::Vec2& _Other)
		{
			(m_x -= _Other.m_x); (m_y -= _Other.m_y); return (*this);
		}

		QF::Utils::Vec2& QF::Utils::Vec2::operator-=(const float _Other)
		{
			(m_x -= _Other); (m_y -= _Other); return (*this);
		}
	/* Equal and not equal */
		const bool QF::Utils::Vec2::operator==(const QF::Utils::Vec2& _Other) const 
		{
			return (_Other.m_x == m_x && _Other.m_y == m_y);
		}

		const bool QF::Utils::Vec2::operator!=(const QF::Utils::Vec2& _Other) const
		{
			return !(operator==(_Other));
		}

		QF::Utils::Vec2 QF::Utils::Vec2::operator=(const QF::Utils::Vec2& _Other) 
		{
			m_x = _Other.m_x;
			m_y = _Other.m_y;
			return *this;
		}

		

/*========================= Transformations =========================*/
	const std::string QF::Utils::Vec2::g_String() const
	{
		return std::format("QF::Utils::Vec2({}, {})", m_x, m_y);
	}

	const char* QF::Utils::Vec2::g_CharPtr() const
	{
		return this->g_String().c_str();
	}

	const ImVec2 QF::Utils::Vec2::g_ImVec2() const
	{
		return ImVec2{ m_x, m_y };
	}

	const float QF::Utils::Vec2::g_X() const { return m_x; };
	const float QF::Utils::Vec2::g_Y() const { return m_y; };
/*========================= Global Vec2 Hints =========================*/
	void QF::Utils::Vec2::s_GlobalVec2Hint(
		QF::Utils::Vec2::Hint _Hint, QF::Utils::Vec2::HintValue _HintValue)
	{
		m_GlobalVec2Hints[_Hint] = _HintValue;
	}
