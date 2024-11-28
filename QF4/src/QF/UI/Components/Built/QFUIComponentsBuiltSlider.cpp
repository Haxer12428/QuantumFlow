#include "../QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace utils = QF::Utils;
using self = components::Built::Slider;
using evts = components::EventSystem::Events;

/* Constructor & Destructor */
	self::Slider(Element* _Parent,
		const utils::Vec2& _Position, const utils::Vec2& _Size,
		Flags _Flags, Panel::Flags _PanelFlags
	) 
		: components::Panel(_Parent, _Position, _Size, _PanelFlags)
		,m_Flags{_Flags}
	{
		/* Assert flags to determinate if the slider is eaither vertical or horizontal */
		__QF_ASSERT(
				 utils::Compare::bitFlagPoolMatchEnumClass(m_Flags, Flags::m_Horizontal)
			|| utils::Compare::bitFlagPoolMatchEnumClass(m_Flags, Flags::m_Vertical),
			"any of this flags must be set"
		);

		/* Anim */
		m_PositionAnim = std::make_unique<utils::BasicAnim>();
		m_ColorAnim = std::make_unique<utils::BasicAnim>();
		setupAnimations();

		/* Events */
		auto& eventHandler = g_EventHandler();

		eventHandler->Subscribe<evts::BeforeRender>(this, &self::hookAnimations);
		eventHandler->Subscribe<evts::Render>(this, &self::hookRender);
		eventHandler->Subscribe<evts::MousePanelDragEvent>(this, &self::hookPanelDrag);
		eventHandler->Subscribe<evts::BeforeRender>(this, &self::hookDragFlagDisable);
	}

/* Public: get's */
	self::Hints& self::g_Hints() { return m_Hints; }

	const float self::g_Value() const { return m_Value; }

	void self::s_Value(const float& _Value) { m_Value = _Value; }

	const bool self::is_FlagVertical() {
		return utils::Compare::bitFlagPoolMatchEnumClass(m_Flags, Flags::m_Vertical);
	}
/* Hooks */
	void self::hookRender(evts::Render&) {
		components::SimpleDC canvas{this};


		/* Rendering callas */
			/* Bg */
			canvas.putRectFilled(0.0f, g_Size(), m_Hints.m_BGColor, m_Hints.m_Rounding);
			/* Element */
			canvas.putRectFilled(g_ElementRenderPosition(), g_ElementSize(), 
				utils::BasicAnim::g_ImColor(m_ColorAnim->g_Animated(m_Hints.m_CompleteAnimationMs)),
				m_Hints.m_Rounding
				);
	}

	void self::hookPanelDrag(evts::MousePanelDragEvent& _Event) {
		const bool is_Vertical = is_FlagVertical();

		if (!m_Drag) {
			m_DragPositionClicked = (_Event.g_PositionClickedFixed() - g_ElementRenderPosition());
		}

		const utils::Vec2 positionFixed = (_Event.g_PositionFixed() - m_DragPositionClicked);

		const float scrollNew = (is_Vertical ? positionFixed.y : positionFixed.x);
		static const float scrollMin = 0.0f; 
		const float scrollMax = g_MaximalScroll();
	
		const float scrollInBounds = std::clamp(scrollNew, scrollMin, scrollMax);

		m_Drag = true; 
		m_DragLastFrame = false; 
		m_Scroll = scrollInBounds;
	}

	void self::hookAnimations(evts::BeforeRender&) {
		auto handleAnim = [&](std::unique_ptr<utils::BasicAnim>& _Anim, const std::vector<float>& _Wanted) -> void {
			if (!_Anim->is_Finished()) return; 

			_Anim->s_Starting(_Anim->g_Last());
			_Anim->s_Wanted(_Wanted);
			_Anim->startAnimation();
		};

		const bool colorAnimState = m_Drag;

		handleAnim(m_ColorAnim, (colorAnimState ?
			utils::BasicAnim::g_VectorFloatFromImColor(
				m_Hints.m_ElementActiveColor
				)
			: 
				utils::BasicAnim::g_VectorFloatFromImColor(
					m_Hints.m_ElementDefaultColor
				)
			));

		handleAnim(m_PositionAnim, { g_ValueForPositionAnim() });
	}

	void self::hookDragFlagDisable(evts::BeforeRender&) {
		if (m_Drag == true && m_DragLastFrame == false) {
			m_DragLastFrame = true; return;
		}

		if (m_Drag && m_DragLastFrame) {
			m_Drag = false; return; 
		}

		m_DragLastFrame = false; 
	}
/* Internals: Get's */
	const utils::Vec2 self::g_ScrollWithDirection() {
		const bool is_Vertical = is_FlagVertical();

		return (is_Vertical ? utils::Vec2{0.0f, m_Scroll} 
			: utils::Vec2{ m_Scroll, 0.0f });
	}

	const float self::g_MaximalScroll() {
		const bool is_Vertical = is_FlagVertical();

		const utils::Vec2 panelSize = g_Size();
		const utils::Vec2 elementSize = g_ElementSize();
		/* This is fixed */
		const utils::Vec2 elementPos = (g_ElementPosition() - g_ScrollWithDirection());
		
		const utils::Vec2 sizeDif = (panelSize - elementSize - (2.0f * elementPos));
		
		return (is_Vertical ? sizeDif.y : sizeDif.x);
	}

	const float self::g_ValueForPositionAnim() {
		const bool is_Vertical = is_FlagVertical(); 
		const utils::Vec2 elementPosition = g_ElementPosition();

		return (is_Vertical ? elementPosition.y : elementPosition.x);
	}

	const utils::Vec2 self::g_ElementPosition() {
		const bool is_Vertical = is_FlagVertical();
		const utils::Vec2 elementSize = g_ElementSize();
		const utils::Vec2 panelSize = g_Size();

		const utils::Vec2 positionNoOffset = (is_Vertical 
			? ((panelSize.x - elementSize.x) / 2.0f) 
			: ((panelSize.y - elementSize.y) / 2.0f)
			);

		/* With scroll */
		return (g_ScrollWithDirection() + positionNoOffset);
	}

	const utils::Vec2 self::g_ElementSize() {
		const bool is_Vertical = is_FlagVertical();
		const utils::Vec2 panelSize = g_Size();

		float elementCut = (is_Vertical ? panelSize.x : panelSize.y);
		elementCut *= (1.0f - m_Hints.m_ElementSidesSizeFactor);

		float elementLen = (is_Vertical ? panelSize.y : panelSize.x);
		elementLen *= (1.0f - m_Hints.m_ElementLenFactor);

		utils::Vec2 elementSize = panelSize;

		elementSize = (is_Vertical
			? panelSize - utils::Vec2(elementCut, elementLen) : panelSize - utils::Vec2(elementLen, elementCut)
			);

		return elementSize;
	}

	const utils::Vec2 self::g_ElementRenderPosition() {
		const bool is_Vertical = is_FlagVertical();
		const utils::Vec2 elementPosition = g_ElementPosition();
		const float positionAnimValue = m_PositionAnim->g_Animated(m_Hints.m_CompleteAnimationMs)[0];

		return (is_Vertical ? utils::Vec2{elementPosition.x, positionAnimValue} 
			: utils::Vec2{positionAnimValue, elementPosition.y});
	}
/* Internals : setup */
	void self::setupAnimations() {
		/* Pos */
			const bool is_Vertical = is_FlagVertical();
			const utils::Vec2 elementPosition = g_ElementPosition();

			const float val = (is_Vertical ? elementPosition.y : elementPosition.x);

			m_PositionAnim->s_Starting(val);
			m_PositionAnim->s_Wanted(val);
			m_PositionAnim->startAnimation();
		/* Color */
			m_ColorAnim->s_Starting(m_Hints.m_ElementDefaultColor);
			m_ColorAnim->s_Wanted(m_Hints.m_ElementDefaultColor);
			m_ColorAnim->startAnimation();
	}


