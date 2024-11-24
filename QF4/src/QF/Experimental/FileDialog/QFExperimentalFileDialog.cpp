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

		components::Built::TextBox* textBox = new components::Built::TextBox(this, 0.0f, { g_GLFWobject()->g_Size().x, 40.0f }, components::Panel::Flags::m_None, _StartingPath.string(), textboxFont);
		
		components::Built::TextBox::Hints& textBoxHints = textBox->g_Hints();
		textBoxHints.m_BGColor = ImColor(25, 25, 25, 255);
		textBoxHints.m_BGColorActive = ImColor(30, 30, 30, 255);
		textBoxHints.m_TextColor = ImColor(200, 200, 200, 255);
		textBoxHints.m_TextColorActive = ImColor(0, 128, 255, 255);

	}

	self::~FileDialog() = default; 