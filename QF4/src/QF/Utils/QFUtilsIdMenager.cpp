#include "QFUtilsIdManager.h"

const uint64_t QF::Utils::IdManager::g_New() {
	const uint64_t currentIdCopy = m_LastGeneratedId;
	m_LastGeneratedId++;
	return currentIdCopy;
}