#include "QFCore.h"

void QF::Core::initQF()
{

	/* Init glfw */
	if (!glfwInit()) {
		printf(
			std::format("{} => Failed to initalize glfw", __FUNCTION__).c_str()
	); }
	glewInit();

	/* Init debug */
	QF::Utils::Debug::Init();
}