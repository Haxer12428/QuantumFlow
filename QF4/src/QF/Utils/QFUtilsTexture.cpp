#include "QFUtilsTexture.h"

using self = QF::Utils::Texture; 

/* Constructor & Destructor */
	self::Texture(const std::vector<char>& _RawData, const QF::Utils::Vec2& _Size) 
		: m_TextureId{0}, m_Data{_RawData}, m_Size{_Size}
	{
		load();
	}

	self::~Texture()  {
		glDeleteTextures(1, &m_TextureId);
	}
/* Get */ 
	GLuint self::g_GLTextureId() {
		return m_TextureId;
	}
/* Load */ 
	void self::load() {

		glGenTextures(1, &m_TextureId);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Load image data into OpenGL texture
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_Size.x), static_cast<int>(m_Size.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Data.data()
			);
	}