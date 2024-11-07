#include "../QF.h"

QF::Utils::Image::Image(const std::filesystem::path& _PathToImage, bool _ModifyColor, const ImColor& _ImageColor)
	: m_Path{ _PathToImage }, m_ModifyColor{ _ModifyColor }, m_Color{ _ImageColor } {
	/* Load texture */


	func_LoadTexture();

	if (m_Texture == 0) return;
	QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__, 
		std::format("Successfully loaded OpenGL texture, id: {}", m_Texture));
} 

QF::Utils::Image::~Image()
{
	if (!m_Texture) return; 
	glDeleteTextures(1, &m_Texture);

	QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::IMPORTANT, __FUNCTION__, 
			"Cleaned up, deleted OpenGL textures.");
}

void QF::Utils::Image::func_LoadTexture()
{
	m_Texture = SOIL_load_OGL_texture(
		m_Path.string().c_str(),
		SOIL_LOAD_AUTO, // Automatically detect the format
		SOIL_CREATE_NEW_ID, // Create a new OpenGL texture ID
		0 // Invert the Y axis to match OpenGL's texture coordinates
	);
}

ImTextureID QF::Utils::Image::g_ImGuiTextureure() const
{
	return reinterpret_cast<ImTextureID>(m_Texture);
}