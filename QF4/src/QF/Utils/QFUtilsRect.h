#pragma once
#include "../QF.h"
#include <iostream>

namespace QF
{
	namespace Utils
	{
		class Rect
		{
		public:
			/* Constructors */
			Rect();
			Rect(float _x, float _y, float _sx, float _sy);
			Rect(const QF::Utils::Vec2& _Start, const QF::Utils::Vec2& _Size);

			/* Transformations */
			const QF::Utils::Vec2 g_Position() const;
			const QF::Utils::Vec2 g_Size() const;
			const QF::Utils::Vec2 g_FinalPosition() const;
			const std::string g_String() const; 

			/* Operators */
			friend std::ostream& operator<<(std::ostream& _OS, const Rect& _Self) {
				_OS << _Self.g_String(); return _OS;
			}

			/* Vars */
			float x, y, sx, sy;
		private:
		};
	}
}