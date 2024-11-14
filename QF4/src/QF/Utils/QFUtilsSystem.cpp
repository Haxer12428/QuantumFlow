#include "QFUtilsSystem.h"

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

    std::cout << taskbarInfo.m_Size.g_String() << " <- taskbar size\n";
    /* Return valid */
    return taskbarInfo;
  }