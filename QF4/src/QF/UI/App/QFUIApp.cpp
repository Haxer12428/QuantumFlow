#include "QFUIApp.h"

namespace utils = QF::Utils;
using self = QF::UI::App;

/* Constructo & Destructor */
	QF::UI::App::App() 
	{
		/* Initialize glfw */
		glfwInit();
		/* Create window handler */
		m_WindowHandler = std::make_unique<WindowHandler>();
		m_ImageManager = std::make_unique<utils::ImageManager>();
	}

	QF::UI::App::~App()
	{

		/* Terminate glfw */
		glfwTerminate();
		/* Tell that i termianated successfully */
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
			"Destructed application instance."
			);
	}
/* Initialization */
	const bool QF::UI::App::Execute()
	{
		/* Enter main loop of window handler */
		m_WindowHandler->enterMainLoop();
		/* Execution finalized -> Delete class (outside of main loop)*/
		delete this;
		return true; 
	}

	const bool QF::UI::App::onInit() {
		return true; 
	}

	std::unique_ptr<QF::UI::App::WindowHandler>& QF::UI::App::g_WindowHandler() {
		return m_WindowHandler;
	}
/* Image manager */
	std::unique_ptr<utils::ImageManager>& self::g_ImageManager() {
		return m_ImageManager;
	}
