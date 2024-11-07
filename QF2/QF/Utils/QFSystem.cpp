#include "../QF.h"

/* System taskbar */
	const QF::Utils::Vec2 QF::Utils::System::g_TaskbarPosition() 
	{
		const RECT _TaskbarRect = g_TaskbarRect();
		return QF::Utils::Vec2(
			static_cast<float>(_TaskbarRect.left), 
			static_cast<float>(_TaskbarRect.top)
			);
	}

	const QF::Utils::Vec2 QF::Utils::System::g_TaskbarSize()
	{
		const RECT _TaskbarRect = g_TaskbarRect();
		return QF::Utils::Vec2(
			static_cast<float>(_TaskbarRect.right - _TaskbarRect.left), 
			static_cast<float>(_TaskbarRect.bottom - _TaskbarRect.top)
			);
	}

	const RECT QF::Utils::System::g_TaskbarRect()
	{
		APPBARDATA _AppBarData = {};
		_AppBarData.cbSize = sizeof(APPBARDATA);

		/* Retrive information */
		if (SHAppBarMessage(ABM_GETTASKBARPOS, &_AppBarData))
		{
			/* Return rectangle of taskbar */
			return _AppBarData.rc;
		}
		/* Log crit error */
		QF::Utils::Debug().Insert(QF::Utils::Debug::LogHint::CRITICAL_ERROR, __FUNCTION__,
			"Failed to retrive Windows taskbar information - returned empty struct in hope this info will be found later");

		return RECT();
	}


