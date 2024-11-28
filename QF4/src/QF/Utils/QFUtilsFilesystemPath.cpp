#include "QFUtilsFilesystem.h"


namespace utils = QF::Utils;
namespace fs = std::filesystem;
using self = utils::Filesystem::Path;

/* Constructor & Destructor */
	self::Path(const fs::path& _Path) : fs::path(_Path) {
	};

	self::Path(const std::string& _Path) : fs::path(_Path) {};

	self::Path(const char* _Path) : fs::path(_Path) {};

	self::~Path() = default; 
/* Validaton */
	const bool self::is_Broken() const {
		/* For paths over 260 len */
		HANDLE hFile = CreateFileA(
			this->string().c_str(),
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL
		);

		/* Useless but cool ig */
		struct HandleLock {
			HandleLock(HANDLE _Handle) : hFile{ _Handle } {} ~HandleLock() {
				if (hFile != INVALID_HANDLE_VALUE)
					CloseHandle(this->hFile);
			}; HANDLE hFile;
		} hLock(hFile);

		if (hFile == INVALID_HANDLE_VALUE) {
#ifndef NDEBUG
#if __QF_DEBUG_LEVEL <= 1
			__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, std::format("Path[{}] was broken", this->string()));
#endif 
#endif // !NDEBUG
			return true;
		}
		return false;
	}

	/* Not heavy in specific but might be not efficient in rendering loops */
	const bool self::is_SystemReservedDeviceName() const {
		std::string thisStr = this->string();
		const size_t thisStrLen = thisStr.length();

		/* Convert this to uppercase to match devicenames (fs dont care if those are lower or upper, treats them as the same)*/
		std::transform(thisStr.begin(), thisStr.end(), thisStr.begin(), [](unsigned char c) {
			return std::toupper(c);
			});

		/* add device names with sizes going up if u mess up this part gl debugging */
		constexpr static const std::array<const char*, 22> reservedDeviceNames = {
		"CON",  "PRN",  "AUX",  "NUL",
		"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
		"LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
		};

		constexpr static const std::array<const char*, 2> formats = {
			".\\", "./"
		};

		/* Silly but cool af: 
			I will look at this someday and think 'what an idiot' but thats cool ngl, yesterday learnt i can do that 
		*/
		struct WhenReservedDebugGuard {
			WhenReservedDebugGuard(const std::string& _Str) : m_Str{ _Str } {}; ~WhenReservedDebugGuard() {
				if (!m_Reserved) return; 
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
					__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, std::format("path[{}] is system reserved device name.", m_Str));
	#endif 
#endif // !NDEBUG
			}; bool m_Reserved = true; std::string m_Str; 
		};

		WhenReservedDebugGuard reservedDebugGuard{ thisStr };

		for (const char* _Reserved : reservedDeviceNames) {
			const size_t reservedLen = std::string(_Reserved).length();

			/* Agressive optimisation: after this point there cannot be a match! */
			if (reservedLen > thisStrLen) {
				reservedDebugGuard.m_Reserved = false; return false; 
			}

			const std::string thisStrFront = thisStr.substr(0, reservedLen);
			const std::string thisStrEnd = thisStr.substr(thisStrLen - reservedLen);
			
			/* Early check */
			if (thisStrEnd == (std::string)_Reserved || thisStrFront == (std::string)_Reserved) return true;

			/* Check for all possible formats */
			for (const char* _Format : formats) {
				/* FUCK STD::FORMAT I SWEAR */
				std::string formatted = _Format;

				/* Format */
				const size_t findDot = formatted.find(".");
				formatted.replace(findDot, 1, _Reserved);

				if (!(thisStrLen < reservedLen) && thisStr.find(
					formatted
				) != std::string::npos
					)
					return true;
				
			}};
		reservedDebugGuard.m_Reserved = false; 
		return false; 
	}