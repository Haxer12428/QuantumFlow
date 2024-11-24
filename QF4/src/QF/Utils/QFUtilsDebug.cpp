#include "QFUtilsDebug.h"
#include "../QF.h"

std::vector<bool> QF::Utils::Debug::m_PrintHints = {};
std::once_flag QF::Utils::Debug::m_InitFlag;

std::vector<std::string> QF::Utils::Debug::m_LogColors =
{
	"\033[1;36m", "\033[1;31m", "\033[1;31m", "\033[1;33m", "\033[1;33m"
};

void QF::Utils::Debug::Log(QF::Utils::Debug::LogType _LogType,
		const std::string& _Function, const std::string& _Arguments)
{
		const bool isInitialized = (
				m_PrintHints.size() > static_cast<int>(_LogType)
		);

		/* Check for not initialized status */
		if (!isInitialized)
		{
				printf("critical error: Debug not initialized properly.\n"); return; 
		}

		/* Check for printing status */
		if (!m_PrintHints[static_cast<size_t>(_LogType)]) return; 

		/* Printout formated log */
		printf(g_FormatedLog(_LogType, _Function, _Arguments).c_str());
}

const std::string QF::Utils::Debug::g_FormatedLog(QF::Utils::Debug::LogType _LogType, 
		const std::string& _Function, const std::string& _Arguments)
{
	return std::format("{}[{}] => {}{}\n",
		g_FormatedLogColor(_LogType), _Function, "\033[0m", _Arguments
		);
}

const std::string QF::Utils::Debug::g_FormatedLogColor(QF::Utils::Debug::LogType _LogType)
{
	const bool hasColor = (m_LogColors.size() > static_cast<size_t>(_LogType));
	
	/* Check if has color to begin with */
	if (!hasColor) return ___QF_EMPTY_STRING;

	const std::string colorValue = m_LogColors[static_cast<size_t>(_LogType)];

	/* Return correct color */
	return colorValue;
}


void QF::Utils::Debug::Init()
{
		std::call_once(m_InitFlag, []() {
				const bool defaultValue = false; 

				/* Override for default value */
				m_PrintHints = {};

				for (int _Iterator = 0; _Iterator <= static_cast<int>
						(PrintType::Last); _Iterator++)
				{
						m_PrintHints.push_back(defaultValue);
				}
				/* Print log */
				printf(std::format("Debug initialized => defaultValue: {};\n",
						defaultValue ? "true" : "false"
				).c_str());
			});
}


void QF::Utils::Debug::s_PrintHint(QF::Utils::Debug::PrintType _Hint,
	const bool& _Value)
{
	const bool hintsAvailable = (m_PrintHints.size() > static_cast<size_t>(_Hint));

	/* Check if hints are available */
	if (!hintsAvailable){
		printf("critical error: Debug hints aren't available"); return; 
	}

	m_PrintHints[static_cast<size_t>(_Hint)] = _Value;
}

void QF::Utils::Debug::s_ToAllPrintHints(const bool& _Value)
{
	const bool hintsAvailable = (m_PrintHints.size() != 0);
	
	if (!hintsAvailable) {
		printf("critical error: Debug hints aren't available"); return;
	}

	/* Set for all of the hints avaialble */
	for (size_t _Iterator = 0; _Iterator < m_PrintHints.size(); 
		_Iterator++) 
	{
		m_PrintHints[_Iterator] = _Value;
	}
}

/* Quick logging */