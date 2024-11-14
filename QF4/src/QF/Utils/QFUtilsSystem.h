#pragma once
#include "../QF.h"


namespace QF
{
	namespace Utils
	{
		class System
		{
		public:
			struct WindowInfo {
				QF::Utils::Vec2 m_Positon;
				QF::Utils::Vec2 m_Size; 
				HWND m_HWND;
			};

			static const std::vector<WindowInfo> g_AllWindowsOnMonitor();

			struct TaskbarInfo {
				QF::Utils::Vec2 m_Position; 
				QF::Utils::Vec2 m_Size; 
			};

			static const TaskbarInfo g_TaskbarInfo();
		};
	}
}