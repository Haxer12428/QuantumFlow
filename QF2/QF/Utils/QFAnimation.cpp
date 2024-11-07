#include "../QF.h"

QF::Utils::Animation::Animation(const std::vector<float>& _ValuesWanted)
	: m_ValuesStarted{ _ValuesWanted } {};

const std::vector<float> QF::Utils::Animation::func_Lerp(
	const std::vector<float>& _ValuesStarted,
	const std::vector<float>& _ValuesWanted, const int& _ReachInMs)
{
	/* Check if values current are the same size as values wanted */
	if (m_ValuesWanted != _ValuesWanted)
	{
		m_LastUpdated = std::chrono::high_resolution_clock::now();
	}

	m_ValuesStarted = _ValuesStarted;
	m_ValuesWanted = _ValuesWanted;

	/* Limited _T {0, 1}f*/
	const double _T = std::max(0.0, std::min(1.0,
		(static_cast<double>
		(std::chrono::duration_cast<std::chrono::milliseconds>(
			/* Timeframe */
			(std::chrono::high_resolution_clock::now() - m_LastUpdated)
		).count()) 
		/* Reach factor */
		/ static_cast<double>(_ReachInMs)
	)));

	std::vector<float> _ValuesCurrent;

	for (size_t _Iterator = 0; _Iterator < m_ValuesWanted.size(); _Iterator++)
	{
		_ValuesCurrent.push_back(method_Lerp(m_ValuesStarted[_Iterator], m_ValuesWanted[_Iterator], _T));
	}
	/* Update last updated */

	return _ValuesCurrent;
}

const std::vector<float> QF::Utils::Animation::func_SmartLerp(const std::vector<float>& _Wanted,
	const int& _Speed)
{
	if (m_ValuesCurrent.size() != _Wanted.size())
		m_ValuesCurrent = _Wanted;

	m_ValuesWanted = _Wanted;

	const double _T = std::max(0.0, std::min(1.0,
		(static_cast<double>
		(std::chrono::duration_cast<std::chrono::milliseconds>(
			/* Timeframe */
			(std::chrono::high_resolution_clock::now() - m_LastUpdated)
		).count()) 
		/* Reach factor */
		/ static_cast<double>(_Speed)
	)));

	for (size_t _Element = 0; _Element < m_ValuesWanted.size(); _Element++)
	{
		m_ValuesCurrent[_Element] = method_Lerp(m_ValuesCurrent[_Element], m_ValuesWanted[_Element], _T);
	}
	m_LastUpdated = std::chrono::high_resolution_clock::now();

	return m_ValuesCurrent;
}

const float QF::Utils::Animation::method_Lerp(const float& _Start, const float& _Final, const float& _T) const
{
	return (_Start + (_Final - _Start) * _T);
}