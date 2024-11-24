#pragma once
#include "../QF.h"
#include <filesystem>

namespace QF {
	namespace Utils {
		class Filesystem {
		public: 
			static const bool is_ValidPath(const std::filesystem::path& _Path);
			static const std::filesystem::path g_InCurrentDirectory(const std::filesystem::path& _Path);
		};
	}
}