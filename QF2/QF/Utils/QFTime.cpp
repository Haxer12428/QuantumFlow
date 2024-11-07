#include "../QF.h"
/**
 * @brief Generates a formatted timestamp string based on the current system time.
 *
 * This function creates a timestamp string using the current system time and
 * formats it according to the specified format string.
 *
 * @param _Format A string specifying the desired format of the timestamp.
 *                Must follow the format specifiers as defined in std::put_time.
 *                Cannot be empty.
 *
 * @return A constant string containing the formatted timestamp.
 *
 * @throws std::invalid_argument If the _Format string is empty or invalid.
 * @throws std::runtime_error If the resulting timestamp string is empty.
 */
const std::string QF::Utils::Time::g_CurrentTimestamp(const std::string& _Format)
{
    if (_Format.empty()
        == true) throw std::invalid_argument(std::format(
            "[{}] - {}", __FUNCTION__, "_Format cannot be empty."
        ));
								

    const std::chrono::system_clock::time_point _Now = std::chrono::system_clock::now(); 
    const time_t _NowTimeT = std::chrono::system_clock::to_time_t(_Now);
    std::tm _NowTM = {};
    /* Get local time -> swapped because old function was unsafe */
    localtime_s(&_NowTM, &_NowTimeT);

    try 
    {
        /* Format */
        std::ostringstream _Timestamp;
        _Timestamp << std::put_time(&_NowTM, _Format.c_str());

        const std::string _TimestampStr = _Timestamp.str();
        if (_TimestampStr.empty()
            == true) throw std::runtime_error(std::format(
                "[{}] - {}", __FUNCTION__, "Got empty timestamp."
            ));
        return _TimestampStr;
    }
    catch (...) {
        throw std::invalid_argument(
            std::format("[{}] - {}", __FUNCTION__, "Invaild format string provided.")
    ); }
};
