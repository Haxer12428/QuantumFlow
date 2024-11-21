#pragma once
#include <GLFW/glfw3.h>
#include "SOIL/SOIL.h"
#include "imgui.h"
#include <filesystem>

namespace QF
{
	namespace Utils
	{
		class Image
		{
		public:
			Image(const std::filesystem::path& _Path);
			Image(const std::vector<char>& _Data); 

			virtual ~Image();
			
			GLuint g_GLTextureID() const;
			
			void reload();

		private:
			void loadGLTextureFromData(); 
			void loadGLTextureFromFile();

			void clearGLTexture(); 

			bool m_LoadedFromFile = false; 

			std::vector<char> m_TextureData; 
			std::filesystem::path m_TexturePath; 
			GLuint m_GLTextureId = 0;
		};
	}
}
