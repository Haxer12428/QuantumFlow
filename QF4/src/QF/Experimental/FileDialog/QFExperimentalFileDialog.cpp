#include "QFExperimentalFileDialog.h"

namespace utils = QF::Utils;
namespace components = QF::UI::Components;
using app = QF::UI::App;
using self = QF::Experimental::FileDialog;
using panelalignflags = components::Panel::AlignmentFlags;

/* Constructor and destructor */
	self::FileDialog(app* _Application, const std::filesystem::path& _StartingPath) 
		: components::Window(_Application, __QF_DONT_CARE, __QF_DONT_CARE, "QF File Dialog"),
		m_Layout{ {this} }
	{
		/* Assert path */
		__QF_ASSERT(utils::Filesystem::is_ValidPath(_StartingPath), "!= true");

		/* Create layout instance */

		/* Set animations */
		g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);
		
		/* Create global font */
		m_GlobalFont = std::make_unique<utils::Font>(
			utils::Filesystem::g_InCurrentDirectory("menlofont.ttf"), 17.0f
		);

		/* Create path box */
		createPathBox(); 
		/* Create dir frame */
		createDirFrame();
	}

	self::~FileDialog() = default; 
/* Internal's: PathBox */
	void self::createPathBox() {
		m_PathBox = new components::Built::TextBox(this,
			20.0f, { 400.0f, 30.0f },
			components::Panel::Flags::m_None,
			m_CurrentPath.string(),
			m_GlobalFont->g_ImFont()
		);

		components::Built::TextBox::Hints& textBoxHints = m_PathBox->g_Hints();

		textBoxHints.m_BGColor = ImColor(40, 44, 52, 255);           // Dark bluish-gray background (Atom's vibe)
		textBoxHints.m_BGColorActive = ImColor(42, 47, 57, 255);     // Slightly lighter bluish-gray active background
		textBoxHints.m_TextColor = ImColor(150, 150, 150, 255);      // Soft light gray text
		textBoxHints.m_TextColorActive = ImColor(255, 255, 255, 255); // Bright blue for active text
		textBoxHints.m_Rounding = 2.0f;
		textBoxHints.m_CursorSizeExtendY = 2.0f;
		textBoxHints.m_OutlineColor = ImColor(25, 28, 33, 60);      // Soft, bluish-gray outline to match the bg
		textBoxHints.m_CursorColor = ImColor(0, 153, 255, 255);      // Soft cyan cursor
		textBoxHints.m_SelectionColor = ImColor(72, 78, 91, 255);

		/* Aligner's */
		m_PathBox->alignMatchPositionWith([&](components::Panel* _Panel) -> const utils::Vec2 {
			return m_Layout.g_PathBoxPosition();
			}, components::Panel::AlignmentFlags::m_AlignX | components::Panel::AlignmentFlags::m_AlignY);

		m_PathBox->alignMatchSizeWith([&](components::Panel* _Panel) -> const utils::Vec2 {
			return m_Layout.g_PathBoxSize(); 
			}, components::Panel::AlignmentFlags::m_AlignX | components::Panel::AlignmentFlags::m_AlignY);

		/* Hooks : binds */
		m_PathBox->g_EventHandler()->Subscribe<components::Built::TextBox::EnterPressedEvent>(this, &self::handlePathBoxEnter);
	
	}
/* Path box: enter pressed event handelr */
	void self::handlePathBoxEnter(components::Built::TextBox::EnterPressedEvent& _Event) {
		const std::filesystem::path eventPath = _Event.g_Text();

		/* Path not accepted */
		if (!std::filesystem::is_regular_file(eventPath) && !std::filesystem::is_directory(eventPath)) {
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1 
			__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, std::format("Path[{}] is not accepted.", eventPath.string()));
	#endif 
#endif // !NDEBUG
			m_PathBox->s_Value(m_CurrentPath.string());
			return;
		}
		m_CurrentPath = eventPath;
		m_DirFrame->updateCurrentPath(m_CurrentPath);

#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
			__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, std::format("Successfully dispatched, switched to path[{}]", m_CurrentPath.string()));
	#endif 
#endif // !NDEBUG
	}
/* Internals: Dir frame */
	void self::createDirFrame() {
		m_DirFrame = new DirFrame(this);

		/* Align */
		m_DirFrame->alignMatchPositionWith([&](components::Panel*) -> const utils::Vec2 {
			return m_Layout.g_DirFramePosition(); }, panelalignflags::m_AlignBothAxis);

		m_DirFrame->alignMatchSizeWith([&](components::Panel*) -> const utils::Vec2 {
			return m_Layout.g_DirFrameSize(); }, panelalignflags::m_AlignBothAxis);
	}
/* Publics: Sets */
	void self::updateCurrentPath(const std::filesystem::path& _New) {
		m_CurrentPath = _New;
		m_PathBox->s_Value(m_CurrentPath.string());
	}
