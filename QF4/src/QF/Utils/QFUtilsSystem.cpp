#include "QFUtilsSystem.h"

namespace utils = QF::Utils;
using self = utils::System;

/* Getting all windows on monitor */
	const std::vector<QF::Utils::System::WindowInfo> QF::Utils::System::g_AllWindowsOnMonitor() {
		std::vector<WindowInfo> windowsCollected;

    auto enumWindowsCallback = [](HWND _HWND, LPARAM _LParam) -> BOOL {
      std::vector<WindowInfo>& windowsCollectedPtr = *(std::vector<WindowInfo>*)_LParam;

      // Only process visible windows
      if (IsWindowVisible(_HWND)) {
        RECT windowRect;
        if (GetWindowRect(_HWND, &windowRect)) 
        {
          QF::Utils::Vec2 windowPosition;
          QF::Utils::Vec2 windowSize; 

          windowPosition.x = static_cast<float>(windowRect.left);
          windowPosition.y = static_cast<float>(windowRect.top); 

          windowSize.x = (static_cast<float>(windowRect.right) - windowPosition.x);
          windowSize.y = (static_cast<float>(windowRect.bottom) - windowPosition.y);

          WindowInfo windowInfo;
          windowInfo.m_HWND = _HWND;
          windowInfo.m_Positon = windowPosition;
          windowInfo.m_Size = windowSize;

          windowsCollectedPtr.push_back(windowInfo);
        }
      }
      return TRUE;  // Continue enumerating 
    };

    EnumWindows(enumWindowsCallback, (LPARAM)&windowsCollected);

    return windowsCollected;
	}

/* Getting taskbar position */
  const QF::Utils::System::TaskbarInfo QF::Utils::System::g_TaskbarInfo() {
    APPBARDATA appbarData; 
    appbarData.cbSize = sizeof(APPBARDATA);

    TaskbarInfo taskbarInfo;
    /* Extraction of taskbar data */
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &appbarData)) {
      /* Error: failed to retrive */
      __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
        "Failed to retrive window's taskbar position data"
        );
      return taskbarInfo;
    }
    /* Write to taskbarInfo */
    taskbarInfo.m_Position = QF::Utils::Vec2::g_PositionFromRECT
      (appbarData.rc);
    
    taskbarInfo.m_Size = QF::Utils::Vec2::g_SizeFromRECT
      (appbarData.rc);

    /* Return valid */
    return taskbarInfo;
  }

  std::string LPWSTRToLPCSTR(LPWSTR wideStr) {
    // Get the size of the wide string when converted to a multi-byte string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    std::string convertedStr(size_needed, 0);

    // Perform the conversion
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &convertedStr[0], size_needed, NULL, NULL);
    return convertedStr;
  }

/* Getting png file from embedded resource */
  std::vector<char> self::g_PNGdataFromEmbeddedResource(const std::string& _ResourceName) {

    HRSRC hResInfo = FindResourceW(NULL, 
      std::wstring(_ResourceName.begin(), _ResourceName.end()).c_str(), 
        RT_RCDATA
      ); /* Binary type */

    if (hResInfo == NULL) {
      __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Resource not found");
      return {};
    }

    DWORD resourceSize = SizeofResource(NULL, hResInfo);
    if (resourceSize == 0) {
      __QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Resource is empty.");
      return {};
    }

    /* Load resource into memory */
    HGLOBAL resourceHandler = LoadResource(NULL, hResInfo);
    if (resourceHandler == NULL) {
      __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Failed to load resource");
      return {};
    }

    /* Lock resource and extract data from it */
    const char* resourceData = static_cast<const char*>(LockResource(resourceHandler));

    /* Get buffer into a vector */
    std::vector<char> resourceBuffer(resourceData, resourceData + resourceSize);

    return resourceBuffer;
  }
 /* Clipboard */
  const std::string self::g_ClipboardData() {
    __QF_ASSERT(OpenClipboard(nullptr), "Clipboard cannot be opened");

    struct ClipboardGuard { ~ClipboardGuard() { CloseClipboard(); } } clipboardGuard;

    /* Get clipboard data */
    HANDLE handleData = GetClipboardData(CF_TEXT);
    if (handleData == nullptr) {
#ifndef  NDEBUG
  #if __QF_DEBUG_LEVEL <= 1
        __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Could not get clipboard data");
  #endif // __QF_DEBUG_LEVEL <= 1
#endif // ! NDEBUG
      
      return ___QF_EMPTY_STRING;
    }

    /* Lock data & get text ptr */
    char* pszText = static_cast<char*>(GlobalLock(handleData));

    /* Cannot lock memory */
    if (pszText == nullptr) {
#ifndef  NDEBUG
  #if __QF_DEBUG_LEVEL <= 1
      __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Could not lock memory");
  #endif // __QF_DEBUG_LEVEL <= 1
#endif // ! NDEBUG

      return ___QF_EMPTY_STRING;
    }
    /* Copy text from clipboard & release locks */
    const std::string clipboardText = pszText;
    GlobalUnlock(handleData);

    return clipboardText; 
  }

  const bool self::s_ClipboardData(const std::string& _Buffer) {
    /* Open & Empty */
    __QF_ASSERT(OpenClipboard(nullptr), "Clipboard cannot be opened");

    struct ClipboardGuard { ~ClipboardGuard() { CloseClipboard(); } } clipboardGuard;

    const size_t bufferSizeFixed = (_Buffer.size() + 1);

    if (!EmptyClipboard()) {
#ifndef  NDEBUG
  #if __QF_DEBUG_LEVEL <= 1
        __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Could not empty clipboard");
  #endif // __QF_DEBUG_LEVEL <= 1
#endif // ! NDEBUG
      return false; 
    }

    /* Allocate memory for buffer */
    HGLOBAL handleGlobal = GlobalAlloc(GMEM_MOVEABLE, bufferSizeFixed);

    if (handleGlobal == nullptr) {
#ifndef  NDEBUG
  #if __QF_DEBUG_LEVEL <= 1
        __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Could not allocate memory");
  #endif // __QF_DEBUG_LEVEL <= 1
#endif // ! NDEBUG
      return false; 
    }

    char* pGlobal = static_cast<char*>(GlobalLock(handleGlobal));

    if (pGlobal == nullptr) {
#ifndef  NDEBUG
  #if __QF_DEBUG_LEVEL <= 1
        __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Could not lock memory");
  #endif // __QF_DEBUG_LEVEL <= 1
#endif // ! NDEBUG
        return false; 
    }

    /* Write data */
    memcpy(pGlobal, _Buffer.c_str(), bufferSizeFixed); 
    /* Unlock */
    GlobalUnlock(handleGlobal);

    /* Place the handle on the clipboard for CF_TEXT format */
    if (SetClipboardData(CF_TEXT, handleGlobal) == nullptr) {
#ifndef  NDEBUG
  #if __QF_DEBUG_LEVEL <= 1
        __QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Could not place handle on clipboard");
  #endif // __QF_DEBUG_LEVEL <= 1
#endif // ! NDEBUG
      GlobalFree(handleGlobal);
      return false;
    }
    /* Close */

    return true; 
  }
