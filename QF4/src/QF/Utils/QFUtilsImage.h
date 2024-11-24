#pragma once
#include <GLFW/glfw3.h>
#include "SOIL/SOIL.h"
#include "imgui.h"
#include <filesystem>
#include <unordered_map>
#include "QFUtilsDebug.h"
#include <memory>

namespace QF
{
	namespace Utils
	{
		class Image
		{
		public:
			Image(const std::filesystem::path& _Path);
			Image(const std::vector<char>& _Data); 
			Image(const std::vector<unsigned char>& _Data);


			virtual ~Image();
			
			GLuint g_GLTextureID() const;
			
			void reload();

		private:
			void loadGLTextureFromData(); 
			void loadGLTextureFromFile();

			void clearGLTexture(); 

			bool m_LoadedFromFile = false; 

			std::vector<unsigned char> m_TextureDataUnsignedChar; 
			std::vector<char> m_TextureData; 
			std::filesystem::path m_TexturePath; 
			GLuint m_GLTextureId = 0;
		};

		class ImageManager {
		public:
			ImageManager();
			~ImageManager();

			void putImage(std::unique_ptr<Image> _Image, const std::string& _Name);
			Image* g_Image(const std::string& _Name) const;
			const bool is_There(const std::string& _Name) const; 
			void eraseImage(const std::string& _Name);
		private:
			std::unordered_map<std::string, std::unique_ptr<Image>> m_ImageCollection; 
		};
	}
}
