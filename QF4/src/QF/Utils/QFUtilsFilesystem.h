#pragma once
#include "../QF.h"
#include "../QFDefines.h"
#include <filesystem>
#include <fmt/core.h>

namespace QF {
	namespace Utils {
		class Filesystem {
		public: 
			static const bool is_ValidPath(const std::filesystem::path& _Path);
			static const std::filesystem::path g_InCurrentDirectory(const std::filesystem::path& _Path);
			static const std::string g_WithoutNewLines(const std::string& _Buffer);
			static const std::string g_WithoutTabs(const std::string& _Buffer);

			class Path : public std::filesystem::path { 
			public: 
				Path(const std::string& _Path);
				Path(const char* _Path);
				Path(const std::filesystem::path& _Path);  
				virtual ~Path();

				const bool is_SystemReservedDeviceName() const;
				const bool is_Broken() const;
			private:

			};
		};
	}
}