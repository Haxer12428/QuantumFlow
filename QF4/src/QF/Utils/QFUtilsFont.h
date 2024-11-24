#pragma once
#include "../QFDefines.h"
#include <filesystem>
#include "imgui.h"
#include "QFUtilsDebug.h"

namespace QF {
	namespace Utils {
		class Font { 
		public: 
			/* Remember to set imgui's context */
			Font(const std::filesystem::path& _Path, float _Size);
			virtual ~Font(); 

			ImFont* g_ImFont(); 
		private:
			void buildFont(const std::filesystem::path& _Path, float _Size); 

			ImFont* m_ImFont;
		};
	};
}