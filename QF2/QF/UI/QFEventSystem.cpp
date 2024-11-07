#include "../QF.h"

/* Mouse motion event */
	QF::UI::EventSystem::MouseMotionEvent::MouseMotionEvent(QF::UI::Panel* _Panel)
		: m_Panel{ _Panel }, m_Position{ _Panel->g_AbsoluteParent()->g_MousePosition() }
	{

	}

	const QF::Utils::Vec2 QF::UI::EventSystem::MouseMotionEvent::g_GlobalPosition() const 
	{ return m_Position; } 

	const QF::Utils::Vec2 QF::UI::EventSystem::MouseMotionEvent::g_Position() const 
	{ /* Formula: m_position -  panel_offseted_position = fixed position to given panel */
		return (m_Position - m_Panel->g_PositionOffset());
	}

	QF::UI::Panel* QF::UI::EventSystem::MouseMotionEvent::g_Panel() const 
	{ return m_Panel; }
/* Mouse Clicked event */
	QF::UI::EventSystem::MouseClickedEvent::MouseClickedEvent(QF::UI::Panel* _Panel, const QF::Utils::Vec2& _Position)
		: m_Panel{ _Panel }, m_Position{_Position} {
	};

	const QF::Utils::Vec2& QF::UI::EventSystem::MouseClickedEvent::g_GlobalPosition() const
	{
		return m_Position;
	}

	const QF::Utils::Vec2& QF::UI::EventSystem::MouseClickedEvent::g_Position() const
	{ /* Formula: m_position -  panel_offseted_position = fixed position to given panel */
		return (m_Position - m_Panel->g_PositionOffset());
	}
/* Mouse Panel Drag Event */
	QF::UI::EventSystem::MousePanelDragEvent::MousePanelDragEvent(QF::UI::Panel* _Panel, const QF::Utils::Vec2& _WhereClicked)
		: m_ClickPosition{ _WhereClicked }, MouseMotionEvent(_Panel) {}

	const QF::Utils::Vec2 QF::UI::EventSystem::MousePanelDragEvent::g_ClickPosition() const 
	{ return (m_ClickPosition - g_Panel()->g_PositionOffset()); }

	const QF::Utils::Vec2 QF::UI::EventSystem::MousePanelDragEvent::g_ClickPositionGlobal() const 
	{ return m_ClickPosition; }
/* Mouse Button Click */
	QF::UI::EventSystem::MouseButtonClickEvent::MouseButtonClickEvent(Panel* _Panel)
		: MouseClickedEvent( _Panel, _Panel->g_AbsoluteParent()->g_MousePosition())
	{};

/* Timer class */
	void	QF::UI::EventSystem::Timer::func_Start(const std::chrono::milliseconds& _Delay, const bool& _Once)
	{
		m_Started = true; 
		m_Delay = _Delay;
		m_Once = _Once;
		m_StartedTime = std::chrono::high_resolution_clock::now();
	}

	void QF::UI::EventSystem::Timer::func_Stop()
	{
		m_Started = false; 
	}

	void QF::UI::EventSystem::Timer::func_Dispatch()
	{
		for (const auto& _Func : m_Listeners)
		{
			QF::UI::EventSystem::TimerEvent evt{};
				_Func(evt);
		}
	}

	void QF::UI::EventSystem::Timer::Link(QF::UI::Element* _Element)
	{
		QF::UI::EventSystem::EventHandler* _EventHandler;

		/* Panel link */
		if (_Element->g_Panel())
		{ _EventHandler = _Element->g_Panel()->g_EventHandler(); }

		/* Window link */
		if (_Element->g_Window()) 
		{ _EventHandler = _Element->g_Window()->g_EventHandler(); }

		/* Case: event handler is nullptr */
		if (_EventHandler == nullptr)
		{
			/* Log debug & abort */
			QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__, 
				"Unable to link timer, event handler == nullptr;");
			return; 
		}
		/* Link to render event of given event handler */
		_EventHandler->Subscribe<QF::UI::EventSystem::RenderEvent>(
				this, &QF::UI::EventSystem::Timer::hk_LinkedLoop
				);
	}

	void QF::UI::EventSystem::Timer::hk_LinkedLoop(QF::UI::EventSystem::RenderEvent& _Evt) 
	{
		/* Check if started */
		if (!m_Started) return;

		std::chrono::high_resolution_clock::time_point _Now = 
			std::chrono::high_resolution_clock::now();

		std::chrono::milliseconds _WentBySinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(
			_Now - m_StartedTime
		);

		/* Check if can call dispatch */
		if (!(m_Delay <= _WentBySinceStart)) return; 

		/* Dispatch */
		func_Dispatch();

		/* Override params */
		m_StartedTime = std::chrono::high_resolution_clock::now();

		/* Override m_Started if only one call */
		if (!m_Once) return;
		m_Started = false; 
	}