#include "QFUtilsFont.h"

namespace utils = QF::Utils;
using self = utils::Font;

/* Constructor & Destructor */
	self::Font(const std::filesystem::path& _Path, float _Size) 
	{
		/* Checks for needed things */
		__QF_ASSERT(std::filesystem::is_regular_file(_Path), "font file doesn't exist");
		__QF_ASSERT(ImGui::GetCurrentContext() != nullptr, "Set imgui's context before you create font");

		buildFont(_Path, _Size);
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, std::format("Successfully created font[{}][{}]",
			_Path.string(), _Size
			));
	#endif
#endif // !NDEBUG

	}

	self::~Font() = default; 
/* Internals */
	void self::buildFont(const std::filesystem::path& _Path, float _Size) {
		ImGuiIO& io = ImGui::GetIO();

		m_ImFont = io.Fonts->AddFontFromFileTTF(
			_Path.string().c_str(), _Size, nullptr, io.Fonts->GetGlyphRangesDefault()
			);
		
		__QF_ASSERT(m_ImFont, "font couldn't build, probably corrupted file");
		__QF_ASSERT(io.Fonts->Build(), "font alias couldn't build correctly");
	}

	ImFont* self::g_ImFont() { return m_ImFont; }