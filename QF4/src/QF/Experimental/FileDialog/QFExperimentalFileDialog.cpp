#include "QFExperimentalFileDialog.h"

namespace utils = QF::Utils;
namespace components = QF::UI::Components;
using app = QF::UI::App;
using self = QF::Experimental::FileDialog;

/* Constructor and destructor */
	self::FileDialog(app* _Application, const std::filesystem::path& _StartingPath) 
		: components::Window(_Application, __QF_DONT_CARE, __QF_DONT_CARE, "QF File Dialog") 
	{
		/* Assert path */
		__QF_ASSERT(utils::Filesystem::is_ValidPath(_StartingPath), "!= true");

		/* Set animations */
		g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);
		
		std::unique_ptr<utils::Font> textboxFont = std::make_unique<utils::Font>(
			utils::Filesystem::g_InCurrentDirectory("menlofont.ttf"), 17.0f
		);

		components::Built::TextBox* textBox = new components::Built::TextBox(this, 20.0f, { 400.0f, 30.0f }, components::Panel::Flags::m_None, _StartingPath.string(), textboxFont);
		
		components::Built::TextBox::Hints& textBoxHints = textBox->g_Hints();
		
		textBoxHints.m_BGColor = ImColor(40, 44, 52, 255);           // Dark bluish-gray background (Atom's vibe)
		textBoxHints.m_BGColorActive = ImColor(42, 47, 57, 255);     // Slightly lighter bluish-gray active background
		textBoxHints.m_TextColor = ImColor(150, 150, 150, 255);      // Soft light gray text
		textBoxHints.m_TextColorActive = ImColor(255, 255, 255, 255); // Bright blue for active text
		textBoxHints.m_Rounding = 2.0f;
		textBoxHints.m_CursorSizeExtendY = 2.0f;
		textBoxHints.m_OutlineColor = ImColor(20, 20, 33, 25);      // Soft, bluish-gray outline to match the bg
		textBoxHints.m_CursorColor = ImColor(0, 153, 255, 255);      // Soft cyan cursor
		textBoxHints.m_SelectionColor = ImColor(72, 78, 91, 255);

	}

	self::~FileDialog() = default; 