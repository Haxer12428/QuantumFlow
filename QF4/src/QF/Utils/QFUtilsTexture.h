#pragma once
#include "GL/glew.h"
#include <vector>
#include "GLFW/glfw3.h"
#include "QFUtilsVec2.h"

namespace QF {
	namespace Utils {
		class Texture {
		public:
			Texture(const std::vector<char>& _RawData, const QF::Utils::Vec2& _Size);
			virtual ~Texture();

			GLuint g_GLTextureId();
		private:
			void load();

			std::vector<char> m_Data;
			GLuint m_TextureId;
			QF::Utils::Vec2 m_Size; 
		};
	}
}