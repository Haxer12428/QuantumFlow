#include "../../QF.h"

/* Component : Button */
	QF::UI::Components::Button::Button(Hints _Hints)
		: m_Hints{ _Hints }, Panel(_Hints.m_Parent, _Hints.m_Position, _Hints.m_Size, _Hints.m_Special)
	{
		g_EventHandler()->Subscribe<EventSystem::RenderEvent>(this, &Button::hk_Render);
		g_EventHandler()->Subscribe<EventSystem::MouseClickedEvent>(this, &Button::hk_OnMouseClick);
	}

	QF::UI::Components::Button::~Button()
	{
		if (m_Hints.m_Image == nullptr) return ;
		delete m_Hints.m_Image;
	}

	void QF::UI::Components::Button::hk_Render(EventSystem::RenderEvent& _Event)
	{
		/* Get background color */
		bool _MouseOnPanel = is_InBounds(g_AbsoluteParent()->g_MousePosition());

		ImU32 _BGColor = (_MouseOnPanel ? m_Hints.m_ColorActivated : m_Hints.m_ColorDefault);

		_BGColor  = 
			QF::Utils::Transform::g_ImColorFromFloatVector(
				m_BackgroundAnimations->func_SmartLerp(
					QF::Utils::Transform::g_VectorFloatFromImColor(_BGColor), 
					100
				));

		/* Setup Canvas */
		QF::UI::SimpleDC _Canvas{this};

		/* Draw background */
		_Canvas.draw_RectFilled({0,0},g_Size(), _BGColor);

		/* Draw image if available */
		if (m_Hints.m_Image != nullptr) 
		{
		const QF::Utils::Vec2 _ImageSize = (g_Size() * m_Hints.m_ImageSizeFactor);
		const QF::Utils::Vec2 _ImagePosition = ((g_Size() - _ImageSize) / 2.0f);

		_Canvas.draw_Image(m_Hints.m_Image, _ImagePosition, _ImageSize, m_Hints.m_ImageColor);
		}

		/* Draw text if available */
		if (m_Hints.m_DrawText)
		{
			_Canvas.draw_Text(QF::Utils::Math::g_TextCenteredPosition(
				{0, 0}, g_Size(), m_Hints.m_Text, m_Hints.m_Font
			), m_Hints.m_TextColor, m_Hints.m_Text, m_Hints.m_Font);

			
		}
	}

	void QF::UI::Components::Button::func_SetCallback(std::function<void(QF::UI::Components::Button*)> _Function)
	{
		m_Callback = _Function;
	}

	void QF::UI::Components::Button::hk_OnMouseClick(EventSystem::MouseClickedEvent& _Event)
	{
		/* Propagate callback */
		if (m_Callback != nullptr)
		{
			m_Callback(this);
		}

		/* Propagate event further */
		EventSystem::MouseButtonClickEvent _ToPropagate{this};
		g_EventHandler()->Dispatch( _ToPropagate );
	}