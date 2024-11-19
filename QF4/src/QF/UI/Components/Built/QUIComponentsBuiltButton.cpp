#include "../QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace comp_built = components::Built;
namespace utils = QF::Utils;
using self = comp_built::Button;

/* Constructor & Destructor */
	comp_built::Button::Button(components::Element* _Parent, Hints& _Hints) 
		: m_Hints{_Hints},
		
		components::Panel(_Parent, _Hints.m_Pos, _Hints.m_Size, _Hints.m_PanelFlags)
	{
		/* Bind render event */
		g_EventHandler()->Subscribe<components::EventSystem::Events::Render>(this, &comp_built::Button::Render);
		g_EventHandler()->Subscribe<components::EventSystem::Events::MouseMotionEvent>(this, &self::MouseMotionEvent);
	}

	comp_built::Button::~Button() {
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Destructed.");
	}

	comp_built::Button::Hints& comp_built::Button::g_Hints() {
		return m_Hints;
	}

	void comp_built::Button::Render(components::EventSystem::Events::Render& _r) {
		components::SimpleDC canvas{this};
		/* Render bg */
		//printf("not on panel\n");
		canvas.putRectFilled({ 0.0f }, g_Size(), m_Hints.m_BGColor);
	}

	void self::Button::MouseMotionEvent(components::EventSystem::Events::MouseMotionEvent& _r) {
		printf("on panel\n");
	}