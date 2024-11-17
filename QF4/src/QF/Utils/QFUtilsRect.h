#pragma once
#include "../QF.h"

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

			/* Vars */
			float x, y, sx, sy;
		private:
		};
	}
}