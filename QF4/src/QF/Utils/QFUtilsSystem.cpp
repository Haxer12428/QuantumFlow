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
