#include "QFUIApp.h"

/* Constructo & Destructor */
	QF::UI::App::App() 
	{
		/* Initialize glfw */
		glfwInit();
		/* Create window handler */
		m_WindowHandler = std::make_unique<WindowHandler>();
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