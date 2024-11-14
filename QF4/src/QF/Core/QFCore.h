#pragma once
#include "../QF.h"

namespace QF
{
	class Core
	{
	public:
		static void initQF();
	};
}

#define __QF_INIT QF::Core::initQF