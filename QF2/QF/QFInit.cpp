#include "QF.h"

bool QF::Init::m_GlewInitialized = false; 



void QF::Init::Glew()
{
	static std::once_flag initFlag;

	std::call_once(initFlag, [&] {
		glewInit();
		std::cout << "Init glew\n";
		});
}