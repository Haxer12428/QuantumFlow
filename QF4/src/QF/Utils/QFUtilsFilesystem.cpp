#include "QFUtilsFilesystem.h"

namespace utils = QF::Utils;
using self = utils::Filesystem;
namespace fs = std::filesystem;

/* Checks */
	const bool self::is_ValidPath(const fs::path& _Path) {
		return (fs::is_regular_file(_Path) || fs::is_directory(_Path));
	}

	const fs::path self::g_InCurrentDirectory(const fs::path& _Path) {
		return static_cast<fs::path>(fs::current_path().string() + "\\" + _Path.string());
	}