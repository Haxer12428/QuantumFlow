#include "QFUIComponentsWindow.h"

/* Constructor & Destructor */
	QF::UI::Components::Window::Window(
		const QF::Utils::Vec2& _Position, const QF::Utils::Vec2& _Size
	) {
		try 
		{
			/* Create glfw object */
			m_GLFWobject = std::make_unique<GLFWobject>();
			/* Create glfw window */
			m_GLFWobject->createObject();

		}
		catch (const std::exception& _Exception) { 
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, std::format(
				"Window creation failed: {}", _Exception.what()
			));}

		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Intiialized successfully");
	}

	void QF::UI::Components::Window::destroyObject() {
		/* Check if in main loop */
		if (is_InMainLoop()) {
			/* If in main loop just set the flag
				* Dont to anything except that, destructor call will be handled in main loop 
			*/
			m_DestructionWanted = true; 
			return;
		}
		/* Regular destruction outside of 'events' */
		delete this; 
	}

	QF::UI::Components::Window::~Window() {
		/* Override flag to tell window menager that u cannot push me or use me ;) */
		m_Destructed = true; 

		/* Let the ppl know */
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, 
			std::format("Destroyed: [{}]", g_ImmutableID())
			);
	}
/* Public get's */
	std::unique_ptr<QF::UI::Components::Window::GLFWobject>& QF::UI::Components::Window::g_GLFWobject() {
		return m_GLFWobject;
	}

	/* Retruns legitimate id or __QF_UNDEFINED int->(long long) on error */
	const long long QF::UI::Components::Window::g_ImmutableID() const {
		return m_ImmutableID;
	}

	const bool QF::UI::Components::Window::is_Destructed() const {
		return m_Destructed; 
	}

	const bool QF::UI::Components::Window::is_InMainLoop() const {
		return m_InMainLoop;
	}

	const bool QF::UI::Components::Window::is_WantingToDestruct() const {
		return m_DestructionWanted;
	}
	/* Destruction */
/* Public set's */
	const bool QF::UI::Components::Window::s_ImmutableID(const long long& _ID) {
		/* Cannot be modified more than once,
			* Security reasons 
		*/
		if (m_ImmutableID != __QF_UNDEFINED) {
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__,
				"Cannot modify id for a window instance more than once, if this message was sent by engine it means QF might have undefined behaviour"
			); 
			return false; 
		}
		/* Set */
		m_ImmutableID = _ID;
		return true; 
	}