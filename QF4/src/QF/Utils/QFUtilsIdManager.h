#pragma once
#include <cstdint>

namespace QF {
	namespace Utils
	{
		class IdManager
		{
		public:
			const uint64_t g_New();
		private:
			uint64_t m_LastGeneratedId = 0;
		};
	}
};