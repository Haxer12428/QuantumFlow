#pragma once
#include <type_traits>
#include <cstdint>

namespace QF
{
	namespace Utils
	{
		class Compare
		{
		public:
			template<typename __ClassVariable>
			static bool bitFlagPoolMatchEnumClass(__ClassVariable _Pool, __ClassVariable _Var) {
				return ((static_cast<std::underlying_type<__ClassVariable>::type>(_Pool) & static_cast<std::underlying_type<__ClassVariable>::type>(_Var)) != 0);
			}
		};
	}
}