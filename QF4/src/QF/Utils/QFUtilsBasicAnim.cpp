#include "QFUtilsBasicAnim.h"

/* Constructor & Destructor */
	QF::Utils::BasicAnim::BasicAnim()  {
	};

	QF::Utils::BasicAnim::~BasicAnim() {
		/* Log destruction */
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
			"Destructed."
			);
	}
/* Setting values */
	/* Float */
	void QF::Utils::BasicAnim::s_Starting(float _New) {
		m_ValuesStarting = { _New };
	}

	void QF::Utils::BasicAnim::s_Wanted(float _New) {
		m_ValuesWanted = { _New };
	}

	/* Vec2 */
	void QF::Utils::BasicAnim::s_Starting(QF::Utils::Vec2 _New) {
		m_ValuesStarting = _New.g_DataVector();
	}

	void QF::Utils::BasicAnim::s_Wanted(QF::Utils::Vec2 _New) {
		m_ValuesWanted = _New.g_DataVector();
	}

	/* std::vector */
	void QF::Utils::BasicAnim::s_Starting(std::vector<float> _New) {
		m_ValuesStarting = _New;
	}

	void QF::Utils::BasicAnim::s_Wanted(std::vector<float> _New) {
		m_ValuesWanted = _New;
	}

	const std::vector<float> QF::Utils::BasicAnim::g_Wanted() const {
		return m_ValuesWanted;
	}

	const bool QF::Utils::BasicAnim::is_Finished() const {
		return (m_ValuesLast == m_ValuesWanted);
	}
/* Class internals */

	void QF::Utils::BasicAnim::startAnimation() {
		/* Check for size match */
		if (m_ValuesStarting.size() != m_ValuesWanted.size()) {
			/* Shutdown 
				Vector WILL be out of range 
			*/

			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, 
				"Did u call s_Starting first?"
				);
			__QF_SHUTDOWN_MSG();

			throw std::runtime_error("expected outcome: Vector out-of-bounds, read debug");
		}
		/* Update clock */
		m_StartedAnimation = std::chrono::high_resolution_clock::now();
	}

	const std::vector<float> QF::Utils::BasicAnim::g_Animated(int _MsToFinish) {
		/* Animation method, todo: make this switchable  */
		auto lerpMethod = [&](double start, double end, double t)
			-> const float
		{
			return start + (end - start) * t * t;
		};
		/* Get t -> factor */
		auto currentTime = std::chrono::high_resolution_clock::now();

		double animationFactor = (static_cast<double>(
			/* Time difference: */
			std::chrono::duration_cast<std::chrono::microseconds>(
				currentTime - m_StartedAnimation
			).count()) 
			/* Animation completion: */
			/ (static_cast<double>(_MsToFinish) * (double)1000)
		);

		/* Limit factor to range: (0, 1) */
		animationFactor = std::min((double)1.0f, std::max((double)0.0f, animationFactor));

		/* Animate */
		std::vector<float> valuesCurrent; 

		for (size_t _Iterator = 0; _Iterator < m_ValuesWanted.size(); 
			_Iterator++) 
		{
			const float valueWanted = m_ValuesWanted[_Iterator];
			const float valueStarting = m_ValuesStarting[_Iterator];

			valuesCurrent.push_back(lerpMethod(
				valueStarting, valueWanted, animationFactor
			));
		}
		/* Update last recorded values */
		m_ValuesLast = valuesCurrent;
		/* Return current result */
		return valuesCurrent;
	}

