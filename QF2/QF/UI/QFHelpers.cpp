#include "../QF.h"

ImFont* QF::UI::Helpers::g_FontFromTTF(
	const std::filesystem::path& _Path, const float& _PixelSize, 
		const ImFontConfig* _FontConfig, const ImWchar* _GlyphRanges) 
{
	/* Its not a file */
	if (!std::filesystem::is_regular_file(_Path) || _Path.extension() != ".ttf")
	{ /* Log CRITICAL_ERROR */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__,
			"Building font from .ttf file failed, there is not such file!");
		/* Return nullptr hoping something will handle */
		return nullptr;
	}
	/* Get io */
	ImGuiIO& io = ImGui::GetIO();

	/* Add font to atlas */
	ImFont* _Font = io.Fonts->AddFontFromFileTTF(
		_Path.string().c_str(), _PixelSize, _FontConfig, _GlyphRanges
	);
	/* Build fonts */
	io.Fonts->Build();
	/* Return built font */
	return _Font;
}