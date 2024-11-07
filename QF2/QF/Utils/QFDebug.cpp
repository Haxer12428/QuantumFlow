#include "../QF.h"

std::unordered_map<QF::Utils::Debug::PrintHint, bool> QF::Utils::Debug::m_GlobalDebugPrintHints;
bool QF::Utils::Debug::m_GlobalDebugHintsInitialized = false; 
std::filesystem::path QF::Utils::Debug::m_GlobalLogFilePath = "C:\\QF\\debug.log";
std::once_flag QF::Utils::Debug::m_InitializerInitOnceFlag;
std::string QF::Utils::Debug::m_LastException = __QF_EMPTY_STRING;

/*========================= Constructor  =========================*/
	QF::Utils::Debug::Debug() = default;

/*========================= Hint System =========================*/
	void QF::Utils::Debug::s_GlobalDebugPrintHint(QF::Utils::Debug::PrintHint _Hint, const bool& _Value)
	{
		m_GlobalDebugPrintHints[_Hint] = _Value;
	}

	/* Works same as s_GlobalDebugPrintHint but supports multiple cases
		Value will be set to all of given hints
	*/
	void QF::Utils::Debug::s_MultipleGlobalDebugPrintHints(
		const std::vector<PrintHint>& _Hints, const bool& _Value)
	{ 
		for (const PrintHint& _Hint : _Hints)
		{
			s_GlobalDebugPrintHint(_Hint, _Value);
		}
	}

	/* Dont call more than once, it will cause all the hints settings to reset */
	void QF::Utils::Debug::global_InitializeHints()
	{
		if (m_GlobalDebugHintsInitialized
			== true)
		{ /* Already initialized */
			QF::Utils::Debug().Insert(LogHint::WARNING, __FUNCTION__,
				"Skipped initialization, already initalized");
			return;
		}
		
	
		/* Call once flag to avoid multiple calls */
		std::call_once(m_InitializerInitOnceFlag, []() {
			/* Clear debug file */
			func_ClearDebugFile();

			/* Search for not defined and define them */
			for (int _Iterator = 0; _Iterator < static_cast<int>(PrintHint::COUNT); 
				++_Iterator) {

				if (m_GlobalDebugPrintHints.find(
					static_cast<PrintHint>(_Iterator)
				) == m_GlobalDebugPrintHints.end()
					) 
				{ /* Define to default: false */
					m_GlobalDebugPrintHints[static_cast<PrintHint>(_Iterator)] = false;
				}
			}
			m_GlobalDebugHintsInitialized = true;
			/* Log that initialization was successfull */
			QF::Utils::Debug().Insert(LogHint::MESSAGE, __FUNCTION__, "Initialized successfully");
		});
	}


/*========================= Log =========================*/
	void QF::Utils::Debug::func_ClearDebugFile()
	{
		std::fstream _File; _File.open(m_GlobalLogFilePath, std::ios::out);
		/* Check if can proceed */
		if (!_File.good()) return; 

		/* Clear */
		_File << "";
	}

	void QF::Utils::Debug::s_GlobalLogFilePath(const std::filesystem::path& _Path)
	{
		m_GlobalLogFilePath = _Path;
	}

	void QF::Utils::Debug::hdl_InsertHintsInitialize()
	{
		if (m_GlobalDebugHintsInitialized
			== false)
		{ /* Initialize */
			global_InitializeHints();
			/* Log action */
			Insert(LogHint::WARNING, __FUNCTION__,
				"Initialized hints automaticly due to debug insertion request"
			);
		}
	}

	const std::string QF::Utils::Debug::g_InsertDebugData(
		const std::string& _HintStr, const std::string& _Name
	) const
	{
		const std::string _Timestamp = QF::Utils::Time::g_CurrentTimestamp(
			"%Y-%m-%d %H:%M:%S");
		
		return std::format("[{}] [{}] [{}]", _HintStr, _Timestamp, _Name);
	}

	const std::string QF::Utils::Debug::g_InsertDataToLog(
		const std::string& _DebugData, const std::string& _Arguments
	) const
	{
		return std::format("{} - {}", _DebugData, _Arguments);
	}

	void QF::Utils::Debug::hdl_InsertLogDataToFileCannotBeOpened() const
	{
		LogHint _CurrentHint = LogHint::CRITICAL_ERROR;

		/* Log CRITICAL_ERROR */
		func_InsertPrint(g_InsertDataToPrint(
			g_InsertDebugData(
				m_LogHintMap.find(_CurrentHint)->second, __FUNCTION__
				),
			"Failed to open debug file.", 
			g_InsertPrintColor(_CurrentHint)
			));
	}

	void QF::Utils::Debug::func_InsertLogDataToFile(const std::string& _Data)
	{
		/* Check if we need to print based on hint */
		const bool _PrintToFile = m_GlobalDebugPrintHints.find(
			PrintHint::PRINT_TO_FILE
		)->second;

		if (!_PrintToFile) return;

		/* Create directories */
		if (!std::filesystem::is_regular_file(m_GlobalLogFilePath)) 
		{
			std::filesystem::create_directories(m_GlobalLogFilePath.parent_path());
		}

		/* Log to file */
		std::ofstream _File(m_GlobalLogFilePath, std::ios::app);

		try {
			/* Write */
			_File << std::format("{}\n", _Data);
		} catch (...) { hdl_InsertLogDataToFileCannotBeOpened(); }
	}

	const std::string QF::Utils::Debug::g_InsertPrintColor(LogHint _Hint) const 
	{
		const bool _PrintWithColor = m_GlobalDebugPrintHints.find(PrintHint::PRINT_WITH_COLORS)->second;
		
		if (_PrintWithColor == false) return __QF_EMPTY_STRING; 

		switch (_Hint)
		{
		case QF::Utils::Debug::LogHint::CRITICAL_ERROR:
			return "\033[31m";
		case QF::Utils::Debug::LogHint::WARNING:
			return "\033[93m";
		case QF::Utils::Debug::LogHint::IMPORTANT:
			return "\033[1;33m";
		case QF::Utils::Debug::LogHint::EXCEPTION:
			return "\033[1;31m";
		default:
			return __QF_EMPTY_STRING;
		}
	}

	const std::string QF::Utils::Debug::g_InsertDataToPrint(
		const std::string& _DebugData, const std::string& _Arguments, const std::string& _Color
	) const
	{
		return std::format("{}{}{} - {}\n",
			_Color, _DebugData, "\033[0m", _Arguments
			);
	}

	void QF::Utils::Debug::func_InsertPrint(const std::string& _Data) const
	{
		printf(_Data.c_str());
	}

	void QF::Utils::Debug::Insert(QF::Utils::Debug::LogHint _Hint,
		const std::string& _Name, const std::string& _Arguments)
	{
		/* Handle hints initialization */
		hdl_InsertHintsInitialize();

		/* Get Params */
		const std::string _HintStr = m_LogHintMap.find(_Hint)->second;
		const std::string _DebugData = g_InsertDebugData(_HintStr, _Name);

		/* Log data into file if wanted */
		func_InsertLogDataToFile(g_InsertDataToLog(_DebugData, _Arguments));
			
		const bool _Print = m_GlobalDebugPrintHints.find(
			m_PrintHintMap.find(_Hint)->second
		)->second;

		/* Check if its exception */
		if (_Hint == LogHint::EXCEPTION)
		{
			/* Log as last */
			m_LastException = _Arguments;
		}

		/* Check if should print */
		if (_Print == false) return; 
		
		func_InsertPrint(g_InsertDataToPrint(_DebugData, _Arguments, g_InsertPrintColor(_Hint)));
	}