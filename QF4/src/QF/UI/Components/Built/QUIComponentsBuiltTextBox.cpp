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

			ImFont* _Font 
	)
		: components::Panel(_Parent, _Pos, _Size, _PanelFlags)
	{
		/* Move font */
		m_TextFont = _Font;

		/* Create anim instances */
		m_BGColorAnim = std::make_unique<utils::BasicAnim>();
		m_TextColorAnim = std::make_unique<utils::BasicAnim>(); 
		m_CursorColorAnim = std::make_unique<utils::BasicAnim>();
		m_CursorPositionAnim = std::make_unique<utils::BasicAnim>();
		m_ScrollAnim = std::make_unique<utils::BasicAnim>();

		/* Apply text */
		m_CurrentText = _StartingText;
		m_LastEnteredText = m_CurrentText;
		m_CursorAt = _StartingText.length();

		/* Bind evt's */
		auto& evtHandler = g_EventHandler();

		evtHandler->Subscribe<evts::Render>(this, &self::handleScroll);
		evtHandler->Subscribe<evts::Render>(this, &self::handleCursorPosition);
		evtHandler->Subscribe<evts::BeforeRender>(this, &self::handleSelection);
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

		auto handleScrollAnim = [&]() -> void {
			if (!m_ScrollAnim->is_Finished()) return;
			
			if (m_ScrollAnim->g_Last().size() == 0) {
				m_ScrollAnim->s_Starting(0.0f);
			}
			else { m_ScrollAnim->s_Starting(m_ScrollAnim->g_Last()[0]); }

			m_ScrollAnim->s_Wanted(m_Scroll);
			m_ScrollAnim->startAnimation();
		};

		/* call handlers */
		handleAnim(m_TextColorAnim, m_Hints.m_TextColorActive, m_Hints.m_TextColor);
		handleAnim(m_BGColorAnim, m_Hints.m_BGColorActive, m_Hints.m_BGColor);
		handleCursorColorAnim();
		handleCursorPositionAnim();
		handleScrollAnim();
	}
/* Scroll handle */
	void self::handleScroll(evts::Render&) {
		if (!is_Focused()) m_Scroll = 0; 
		
		components::SimpleDC canvas{this};
		canvas.pushFont(m_TextFont);

		const int currentTextSizeX = canvas.g_TextSize(m_CurrentText).x; 
		const int totalSizeFixedX = (g_Size().x - m_Hints.m_PositionCutFromBothSidesX * 2);
		const int charSizeX = canvas.g_TextSize("g").x;
		const int totalCharsX = static_cast<float>(totalSizeFixedX) / static_cast<float>(canvas.g_TextSize("g").x);
		const int scrolledChars = (float)m_Scroll / (float)charSizeX; 

		int	cursorAtPosition = (canvas.g_TextSize(m_CurrentText.substr(0, m_CursorAt)).x - m_Scroll);

		if (cursorAtPosition > 0 && cursorAtPosition <= totalSizeFixedX) {
			canvas.popFont();  return;
		}

		bool scrollCalcDirection = false; 
		if (cursorAtPosition > totalSizeFixedX)
			scrollCalcDirection = true; 


		auto checkPositionForOptimalScroll = [&](int _at) -> float {
			const int atPositionX = canvas.g_TextSize(m_CurrentText.substr(0, _at)).x;

			if (_at <= m_CursorAt && _at > m_CursorAt - (totalCharsX)) {
				return  atPositionX; 
			}
			return __QF_UNDEFINED; 
		};

		for (
			/* Start */
			int _at = (scrollCalcDirection ? 0 : (m_CurrentText.size() - 1));
			/* Condition */
			(scrollCalcDirection ? _at < m_CurrentText.size() : _at > 0); 
			/* After */
			(scrollCalcDirection ? _at++ : _at--)) 
		{
			float scrollCalc = checkPositionForOptimalScroll(_at);
			if (scrollCalc != __QF_UNDEFINED) { 
				m_Scroll = scrollCalc - canvas.g_TextSize("a").x; break;
			}
		}
		
		canvas.popFont();
	}
/* Cursor pos handle */
	void self::handleCursorPosition(evts::Render&) {
		/* Handle max text size 
			This is easy|quick solution
			If i want to handle non-monospaced font's without limits in text size i will need other system of calculating scroll
			Current solution in my head: pre-calc every char in given font that can be used in 'type' event
				and with this i can calculate scroll etc with a shit load of performance, the shit i have done here is 
				python type shit. ngl im marking this as a TODO: thing
				*btw skateboarding was kinda fun today (cannot rememeber last time)
					might go out more
		*/
		if (m_CurrentText.length() > 250) {
			m_CurrentText = m_CurrentText.substr(0, 250);
			m_CursorAt = m_CurrentText.length();
			m_Selection = Selection(); 
		}

		/* Cursor returns to start when when focus is lost */
		if (!is_Focused()) {
			m_CursorAt = m_CurrentText.length();
		}

		components::SimpleDC canvas{this};

		canvas.pushFont(m_TextFont);
		m_CursorPos = components::SimpleDC(this).g_TextSize(m_CurrentText.substr(0, m_CursorAt)).x;
		canvas.popFont();
	}
/* Render */
	void self::hookRender(evts::Render&) {
		components::SimpleDC canvas{this};

		const ImColor bgColor = utils::BasicAnim::g_ImColor(m_BGColorAnim->g_Animated(100));
		const ImColor textColor = utils::BasicAnim::g_ImColor(m_TextColorAnim->g_Animated(100));
		const ImColor cursorColor = utils::BasicAnim::g_ImColor(m_CursorColorAnim->g_Animated(m_Hints.m_CursorBlinkTimeMs));

		/* Bg draw call */
		canvas.putRectFilled(0.0f, g_Size(), bgColor, m_Hints.m_Rounding);

		float m_ScrollX = m_ScrollAnim->g_Animated(100)[0];

		const utils::Vec2 currentTextPosition = {
			m_Hints.m_PositionCutFromBothSidesX - m_ScrollX, 0.0
		};

		Selection fixedSelection = g_FixedSelection(m_Selection);

		canvas.pushFont(m_TextFont);

		float currentTextHeigth = canvas.g_TextSize(m_CurrentText).y;

		float selectionStartPosX = canvas.g_TextSize(m_CurrentText.substr(0, fixedSelection.m_Start)).x;
		float selectionEndPosX = canvas.g_TextSize(m_CurrentText.substr(0, fixedSelection.m_End)).x;

		const utils::Vec2 selectionPos = {
			selectionStartPosX + m_Hints.m_PositionCutFromBothSidesX - m_ScrollX, 0.0f
		};

		const utils::Vec2 selectionSize = {
			(selectionEndPosX - selectionStartPosX), currentTextHeigth
		};

		/* Selection draw call */
		if (is_AnythingSelected()) {
			canvas.putRectFilled(selectionPos, selectionSize, 
				m_Hints.m_SelectionColor, m_Hints.m_Rounding, 
				components::SimpleDC::DrawingFlags::m_AlignCenterY
				);
			}

		/* Text draw call */
		canvas.putText(currentTextPosition, m_CurrentText, textColor, components::SimpleDC::DrawingFlags::m_AlignCenterY);
		canvas.popFont();

		if (is_Focused()) {

			const utils::Vec2 cursorSize = {
				m_Hints.m_CursorSizeX, currentTextHeigth + m_Hints.m_CursorSizeExtendY
			};

			const utils::Vec2 cursorPos = {
				(m_CursorPositionAnim->g_Animated(m_Hints.m_CursorMoveSpeedMs)[0] + m_Hints.m_PositionCutFromBothSidesX - m_ScrollX), 0.0f
			};

			/* Cursor draw call */
			canvas.putRectFilled(
				cursorPos,
				cursorSize,
				cursorColor,
				0.5f,
				components::SimpleDC::DrawingFlags::m_AlignCenterY
			);
		};

		/* Outline draw call */
		canvas.putRect(0.0f, g_Size(), m_Hints.m_OutlineColor, m_Hints.m_Rounding);
	}
/* Typing handle */
	void self::handleTyping(evts::CharEvent& _Event) {
		const int activeKey = _Event.g_Key();
		std::string activeChar = _Event.g_Char();

		auto handleSelection = [&]() -> const bool {
			GLFWwindow* GLFWobj = g_AbsoluteParent()->g_GLFWobject()->g_Object();

			if (glfwGetKey(GLFWobj, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS
				&& glfwGetKey(GLFWobj, GLFW_KEY_RIGHT_SHIFT) != GLFW_PRESS || !activeChar.empty()
				) {
				m_Selection.m_Selecting = false; return false;
			}

			if (!m_Selection.m_Selecting) {
				m_Selection.m_Start = m_CursorAt;
				m_Selection.m_End = m_CursorAt;
			}
			else { m_Selection.m_End = m_CursorAt; };

			m_Selection.m_Selecting = true;
			return true; 
		};

		auto selectHandler = [&]() -> const bool {
			Selection fixedSelection = g_FixedSelection(m_Selection);

			if (fixedSelection.m_Start != 0 || fixedSelection.m_End != 0) {
				m_CurrentText = (
					m_CurrentText.substr(0, fixedSelection.m_Start) +
					m_CurrentText.substr(fixedSelection.m_End)
					);
				m_CursorAt = fixedSelection.m_Start;

				m_Selection.m_Start = 0;
				m_Selection.m_End = 0;
				m_Selection.m_Selecting = false;
				return true; 
			}
			return false; 
		};

		auto handleClear = [&]() -> const bool {
			if (activeKey != GLFW_KEY_BACKSPACE
				|| m_CurrentText.length() == 0 || m_CursorAt == 0
				) return false;

			if (selectHandler()) return true; 

			m_CurrentText = m_CurrentText.substr(0, m_CursorAt - 1) + m_CurrentText.substr(m_CursorAt); 
			m_CursorAt--;
			return true; 
		};

		auto handleType = [&]() -> const bool {
			if (activeChar.empty()) return false; 

			/* ctrl + */
			if (
				(_Event.is_Held(GLFW_KEY_LEFT_CONTROL) || _Event.is_Held(GLFW_KEY_RIGHT_CONTROL))
				) {
				if (activeKey == GLFW_KEY_C) {
					const std::string textSelected = g_SelectedText();

					const bool setStatus = QF::Utils::System::s_ClipboardData(textSelected);
					if (!setStatus) {
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
						__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Failed to set clipboard data. setStatus != true");
	#endif 
#endif // !NDEBUG
					}

#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
						__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, std::format("Copied str[len: {}]", textSelected.length()));
	#endif 
#endif // !NDEBUG
					return true;
				}
				if (activeKey == GLFW_KEY_V) {
					const std::string clipboardStrFixed = utils::Filesystem::g_WithoutTabs(
						utils::Filesystem::g_WithoutNewLines(
							utils::System::g_ClipboardData()
						));

					activeChar = clipboardStrFixed;
#ifndef NDEBUG
		#if __QF_DEBUG_LEVEL <= 1
					__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, std::format("Pasted str[len: {}]", clipboardStrFixed.length()));
		#endif 
#endif // !NDEBUG
				}
			}

			selectHandler();

			std::string textWithChar = (m_CurrentText + activeChar); 
			
			if (m_CursorAt != m_CurrentText.length() + 1)
			{
				textWithChar = m_CurrentText.substr(0, m_CursorAt) + activeChar + m_CurrentText.substr(m_CursorAt);
			}

			m_CursorAt += activeChar.length();

			m_CurrentText = textWithChar; return true; 
		};

		auto handleCursorMove = [&]() -> const bool {
			int wantedCursorAt = m_CursorAt;

			if (activeKey != GLFW_KEY_LEFT && activeKey != GLFW_KEY_RIGHT) return false; 

			wantedCursorAt += (activeKey == GLFW_KEY_LEFT ? -1 : 1);
			
			m_CursorAt = std::max(0, std::min(
				wantedCursorAt, static_cast<int>(m_CurrentText.length())
				));

			return true; 
		};

		auto handleEnter = [&]() -> const bool {
			if (activeKey != GLFW_KEY_ENTER) return false;
			m_LastEnteredText = m_CurrentText;
			/* Propagate event */
			g_EventHandler()->Dispatch<self::EnterPressedEvent>(self::EnterPressedEvent{ m_CurrentText });
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
			__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Dispatched enter key");
	#endif
#endif // !NDEBUG

			return true; 
		};

		if (handleEnter()) return; 
		if (handleType()) return;
		if (handleClear()) return;

		const bool cursorHandlerOutput = handleCursorMove();
		handleSelection();
		if (cursorHandlerOutput) return; 
		
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Handled typing, no result.");
	#endif 
#endif // !NDEBUG

	}
/* Selection */
	void self::handleSelection(evts::BeforeRender&) {
		/* Need to fix this, when typing on empty path it fuck's up selection and its selecting 1 char */
		/* Total focus loss */
		if (!is_Focused() || m_CurrentText.empty()) {
			m_Selection = Selection(); return;
		}
	};

/* Public: Get's */
	self::Hints& self::g_Hints() {
		return m_Hints;
	}

	const bool self::is_AnythingSelected() const {
		return (m_Selection.m_End != 0 || m_Selection.m_Start != 0);
	}

	const std::string self::g_SelectedText() {
		Selection selectionFixed = g_FixedSelection(m_Selection);

		return m_CurrentText.substr(selectionFixed.m_Start, selectionFixed.m_End - selectionFixed.m_Start);
	}
/* Private: Get's */
	const self::Selection self::g_FixedSelection(Selection& _Current) const {
		/* Selection range must be in a specific order */
		Selection Copy = _Current; 
		if (Copy.m_Start > Copy.m_End) {
			Copy.m_End = _Current.m_Start;
			Copy.m_Start = _Current.m_End;
		} 
		return Copy;
	}
/* Public: Set's */
	void self::s_Value(const std::string& _New) {
		m_CurrentText = _New; 
		m_Selection = Selection(); 
		m_CursorAt = m_CurrentText.length();
	}

	const std::string self::g_Value() const {
		return m_CurrentText; 
	}