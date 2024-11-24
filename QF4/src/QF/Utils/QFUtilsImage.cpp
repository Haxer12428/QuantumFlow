#include "QFUtilsImage.h"

namespace utils = QF::Utils;
using self = utils::Image;

/* Constructor & Destructor */
	self::Image(const std::filesystem::path& _Path)
		: m_TexturePath{_Path}, m_LoadedFromFile{true}
	{
		loadGLTextureFromFile();
	}

	self::Image(const std::vector<char>& _Data)
		: m_TextureData{ _Data }, m_LoadedFromFile{ false }
	{
		loadGLTextureFromData(); 
	}

	self::Image(const std::vector<unsigned char>& _Data)
		: m_TextureDataUnsignedChar{ _Data }, m_LoadedFromFile{ false }
	{
		loadGLTextureFromFile();
	}

	self::~Image() {
		clearGLTexture();
	}
/* Public user functions */
	GLuint self::g_GLTextureID() const {
		return m_GLTextureId;
	}
/* Private internal functions */
	void self::clearGLTexture() {
		/* Clearing bugged */
		if (true) return; 
		if (m_GLTextureId == 0) return; 
		glDeleteTextures(1, &m_GLTextureId);
	}

	void self::reload() {
		if (m_LoadedFromFile) {
			loadGLTextureFromFile(); return;
		}
		loadGLTextureFromData(); 
	}

	void self::loadGLTextureFromFile() {
		/* Clear */
		clearGLTexture();
		/* Load with soil.h */
		m_GLTextureId = SOIL_load_OGL_texture(
			m_TexturePath.string().c_str(), 
			SOIL_LOAD_AUTO,          
			SOIL_CREATE_NEW_ID,      
			SOIL_FLAG_MIPMAPS     
		);
	}

	void self::loadGLTextureFromData() {
		/* Clear */
		clearGLTexture(); 
		/* Load from data with soil */
		m_GLTextureId = SOIL_load_OGL_texture_from_memory(
			(m_TextureData.size() != 0 ? reinterpret_cast<const unsigned char*>(m_TextureData.data()) : reinterpret_cast<const unsigned char*>(m_TextureDataUnsignedChar.data())),
			m_TextureData.size() == 0 ? m_TextureDataUnsignedChar.size() : m_TextureData.size(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);
	}

