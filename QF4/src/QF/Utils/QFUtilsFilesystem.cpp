#include "QFUtilsFilesystem.h"

namespace utils = QF::Utils;
using self = utils::Filesystem;
namespace fs = std::filesystem;

/* Checks */
	const bool self::is_ValidPath(const fs::path& _Path) {
		return (fs::is_regular_file(_Path) || fs::is_directory(_Path));
	}

/* Transforms */
	const fs::path self::g_InCurrentDirectory(const fs::path& _Path) {
		return static_cast<fs::path>(fs::current_path().string() + "\\" + _Path.string());
	}
	const std::string self::g_WithoutNewLines(const std::string& _Buffer) {
		if (_Buffer.empty()) return _Buffer; 

		std::string resultBuffer = _Buffer;	

		auto rmOccurence = [&](std::string& _Str, char _What) {
			_Str.erase(std::remove(_Str.begin(), _Str.end(), _What), _Str.end());
		};
		/* Remove */
		rmOccurence(resultBuffer, '\n');
		rmOccurence(resultBuffer, '\r');

		return resultBuffer;
	}

	const std::string self::g_WithoutTabs(const std::string& _Buffer) {
		if (_Buffer.empty()) return _Buffer;
		std::string resultBuffer = _Buffer;

		auto rmOccurence = [&](std::string& _Str, char _What) {
			_Str.erase(std::remove(_Str.begin(), _Str.end(), _What), _Str.end());
		};
		rmOccurence(resultBuffer, '\t'); return resultBuffer;
	}