#include "../QFUIComponentsWindow.h"

namespace components = QF::UI::Components;
namespace utils = QF::Utils;
using self = QF::UI::Components::Built::TextBox; 
using evts = components::EventSystem::Events; 

/* Constructor & Destructor */
	self::TextBox(components::Window* _Parent, 
		const utils::Vec2& _Pos, const utils::Vec2& _Size, 
			
			components::Panel::Flags _PanelFlags, 
			const std::string& _StartingText,

			std::unique_ptr<utils::Font>& _Font 
	)
		: components::Panel(_Parent, _Pos, _Size, _PanelFlags)
	{
		/* Move font */
		m_TextFont = std::move(_Font);

		/* Create anim instances */
		m_BGColorAnim = std::make_unique<utils::BasicAnim>();
		m_TextColorAnim = std::make_unique<utils::BasicAnim>(); 
		m_CursorColorAnim = std::make_unique<utils::BasicAnim>();
		m_CursorPositionAnim = std::make_unique<utils::BasicAnim>();

		/* Apply text */
		m_CurrentText = _StartingText;
		m_CursorAt = _StartingText.length();

		/* Bind evt's */
		auto& evtHandler = g_EventHandler();

		evtHandler->Subscribe<evts::Render>(this, &self::handleCursorPosition);
		evtHandler->Subscribe<evts::BeforeRender>(this, &self::handleAnimations);
		evtHandler->Subscribe<evts::CharEvent>(this, &self::handleTyping);
		evtHandler->Subscribe<evts::Render>(this, &self::hookRender);

#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Created textbox");
	#endif 
#endif // !NDEBUG

	}

	self::~TextBox() = default; 
/* Animation handle */
	void self::handleAnimations(evts::BeforeRender&) {
		auto handleAnim = [&](std::unique_ptr<utils::BasicAnim>& _Anim, const ImColor& _Active, const ImColor& _Default) -> void {
			if (!_Anim->is_Finished()) return; 
			
			ImColor _Wanted = _Active;
			ImColor _Starting = utils::BasicAnim::g_ImColor(_Anim->g_Last());
			
			if (!is_Focused()) {
				_Wanted = _Default;
			}

			_Anim->s_Starting(_Starting);
			_Anim->s_Wanted(_Wanted);
			_Anim->startAnimation();
		};

		auto handleCursorColorAnim = [&]() -> void {
			if (!m_CursorColorAnim->is_Finished()) return;

			ImColor _Wanted = m_Hints.m_CursorColor;
			ImColor _Starting = utils::BasicAnim::g_ImColor(m_CursorColorAnim->g_Last());

			if (m_CursorColorAnim->g_Last() == std::vector<float>{
				_Wanted.Value.x, _Wanted.Value.y, _Wanted.Value.z, _Wanted.Value.w
			}) {
				_Wanted = ImColor(_Wanted.Value.x, _Wanted.Value.y, _Wanted.Value.z, 0.0f);
			}

			m_CursorColorAnim->s_Starting(_Starting);
			m_CursorColorAnim->s_Wanted(_Wanted);
			m_CursorColorAnim->startAnimation();
		};

		auto handleCursorPositionAnim = [&]() -> void {
			if (!m_CursorPositionAnim->is_Finished()) return; 

			float starting = m_CursorPos;
			if (m_CursorPositionAnim->g_Last().size() != 0) {
				starting = m_CursorPositionAnim->g_Last()[0];
			}

			m_CursorPositionAnim->s_Starting(starting);
			m_CursorPositionAnim->s_Wanted(m_CursorPos);
			m_CursorPositionAnim->startAnimation();
		};

		/* call handlers */
		handleAnim(m_BGColorAnim, m_Hints.m_BGColorActive, m_Hints.m_BGColor);
		handleAnim(m_TextColorAnim, m_Hints.m_TextColorActive, m_Hints.m_TextColor);
		handleCursorColorAnim();
		handleCursorPositionAnim();
	}
/* Cursor pos handle */
	void self::handleCursorPosition(evts::Render&) {
		components::SimpleDC canvas{this};

		canvas.pushFont(m_TextFont->g_ImFont());
		m_CursorPos = components::SimpleDC(this).g_TextSize(m_CurrentText.substr(0, m_CursorAt)).x;
		canvas.popFont();
	}
/* Render */
	void self::hookRender(evts::Render&) {
		components::SimpleDC canvas{this};

		const ImColor bgColor = utils::BasicAnim::g_ImColor(m_BGColorAnim->g_Animated(100));
		const ImColor textColor = utils::BasicAnim::g_ImColor(m_TextColorAnim->g_Animated(100));
		const ImColor cursorColor = utils::BasicAnim::g_ImColor(m_CursorColorAnim->g_Animated(m_Hints.m_CursorBlinkTimeMs));

		canvas.putRectFilled(0.0f, g_Size(), bgColor);

		canvas.pushFont(m_TextFont->g_ImFont());
		canvas.putText(0.0f, m_CurrentText, textColor, components::SimpleDC::DrawingFlags::m_AlignCenterY);
		canvas.popFont();

		if (!is_Focused()) return; 

		const utils::Vec2 cursorSize = { 
			m_Hints.m_CursorSizeX, canvas.g_TextSize(m_CurrentText).y + m_Hints.m_CursorSizeExtendY 
			};

		const utils::Vec2 cursorPos = {
			m_CursorPositionAnim->g_Animated(m_Hints.m_CursorMoveSpeedMs)[0], 0.0f
			};

		canvas.putRectFilled(
			cursorPos,
			cursorSize,
			cursorColor, 
			0.5f,
			components::SimpleDC::DrawingFlags::m_AlignCenterY
		);
	}
/* Typing handle */
	void self::handleTyping(evts::CharEvent& _Event) {
		const int activeKey = _Event.g_Key();
		const std::string activeChar = _Event.g_Char();

		auto handleClear = [&]() -> const bool {
			if (activeKey != GLFW_KEY_BACKSPACE
				|| m_CurrentText.length() == 0
				) return false;

			m_CurrentText = m_CurrentText.substr(0, (m_CurrentText.length() - 1)); 
			m_CursorAt--;
			return true; 
		};

		auto handleType = [&]() -> const bool {
			if (activeChar.empty()) return false; 

			m_CursorAt += activeChar.length();
			m_CurrentText += activeChar; return true; 
		};

		if (handleType()) return;
		if (handleClear()) return;
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Handled typing, no result.");
	#endif 
#endif // !NDEBUG

	}
/* Public: Get's */
	self::Hints& self::g_Hints() {
		return m_Hints;
	}

