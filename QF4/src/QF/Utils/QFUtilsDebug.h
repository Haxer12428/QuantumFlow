#pragma once
// Debug.h
#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <format>

namespace QF
{
  namespace Utils
  {
    class Debug
    {
    public:
      enum class LogType : int
      {
        Message,
        Error,
        Important,
        Warning,
        AssertionFail
      };

      enum class PrintType : int
      {
        Message,
        Error,
        Important,
        Warning,
        AssertionFail,
        Colored,
        Last
      };

      Debug() = default;

      static void Log(
        LogType _LogHint,
        const std::string& _Function,
        const std::string& _Arguments
      );
      static void Init();
      static void s_PrintHint(PrintType _Hint, const bool& _Value);
      static void s_ToAllPrintHints(const bool& _Value);

    private:
      static const std::string g_FormatedLog(LogType _LogHints,
        const std::string& _Function, const std::string& _Arguments);

      static const std::string g_FormatedLogColor(LogType _LogHint);

      static std::once_flag m_InitFlag;
      static std::vector<bool> m_PrintHints;
      static std::vector<std::string> m_LogColors;
    };

  }
}

// Debug macros
#define __QF_DEBUG_LOG QF::Utils::Debug::Log
#define __QF_ERROR QF::Utils::Debug::LogType::Error
#define __QF_WARNING QF::Utils::Debug::LogType::Warning
#define __QF_IMPORTANT QF::Utils::Debug::LogType::Important
#define __QF_MESSAGE QF::Utils::Debug::LogType::Message
#define __QF_ASSERTIONFAIL QF::Utils::Debug::LogType::AssertionFail 