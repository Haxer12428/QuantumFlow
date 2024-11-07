#include "../QF.h"

/**
 * @brief Concatenates a vector of strings into a single string with specified line endings.
 *
 * This function takes a vector of strings and combines them into a single string,
 * adding a specified line ending between each element except for the last one.
 *
 * @param _Vec The vector of strings to be concatenated.
 * @param _LineEnding The string to be used as a line ending between vector elements.
 * @return A string containing all elements of the input vector concatenated with the specified line endings.
 *         Returns an empty string if the input vector is empty.
 */
const std::string QF::Utils::Transform::g_StringFromStringVector(
    const std::vector<std::string>& _Vec, const std::string& _LineEnding)
{ 
    const size_t _VecFinal = (_Vec.size() - 1);
    if (_VecFinal == -1) return __QF_EMPTY_STRING;

    std::string _Buffer;

    /* Reserve some memory */
    _Buffer.reserve(_Buffer.size() * 100);

    for (size_t _Iterator = 0; _Iterator <= _VecFinal;
        _Iterator++)
    { /* Add to buffer, if last line dont add ending */
        std::string _Ending = (_Iterator != _VecFinal ? _LineEnding : __QF_EMPTY_STRING);

        (_Buffer += std::format("{}{}", _Vec[_Iterator], _Ending));
    }
    return _Buffer;
}


/**
 * @brief Removes all occurrences of a specified substring from a given string.
 *
 * This function creates a copy of the input string and removes all instances of
 * the specified occurrence from it.
 *
 * @param _Str The original string from which occurrences will be removed.
 * @param _Occurence The substring to be removed from the original string.
 * @return A new string with all occurrences of _Occurence removed from _Str.
 */
const std::string QF::Utils::Transform::g_WithoutOccurences(
    const std::string& _Str, const std::string& _Occurence)
{
    std::string _StrCopy = _Str;

    size_t _Pos; 
    /* Remove all */
    while ((_Pos = _StrCopy.find(_Occurence)) 
        != std::string::npos)
    { /* Erase */
        _StrCopy.erase(_Pos, _Occurence.length());
    }
    /* Return modified copy */
    return _StrCopy;
}


/**
 * @brief Converts an ImColor object to a vector of float values.
 *
 * This function takes an ImColor object and converts its RGBA components
 * to a vector of float values, scaling them from the [0,1] range to [0,255].
 *
 * @param _Color The ImColor object to be converted.
 * @return A vector of four float values representing the RGBA components
 *         of the input color, scaled to the range [0,255].
 */
const std::vector<float> QF::Utils::Transform::g_VectorFloatFromImColor(const ImColor& _Color)
{
    std::vector<float> _Values; 
    _Values.push_back(_Color.Value.x * 255);
    _Values.push_back(_Color.Value.y * 255);
    _Values.push_back(_Color.Value.z * 255);
    _Values.push_back(_Color.Value.w * 255);

    return _Values;
}


/**
 * @brief Converts a vector of float values to an ImColor object.
 *
 * This function takes a vector of float values representing RGBA components
 * and converts them to an ImColor object. The input values are expected to be
 * in the range [0,255] and are cast to integers for the ImColor constructor.
 *
 * @param _Values A vector of four float values representing the RGBA components
 *                of the color. The values should be in the range [0,255].
 * @return An ImColor object created from the input float values.
 */
const ImColor QF::Utils::Transform::g_ImColorFromFloatVector(const std::vector<float>& _Values)
{
    return ImColor(static_cast<int>(_Values[0]), static_cast<int>(_Values[1]), static_cast<int>(_Values[2]), static_cast<int>(_Values[3]));
}
