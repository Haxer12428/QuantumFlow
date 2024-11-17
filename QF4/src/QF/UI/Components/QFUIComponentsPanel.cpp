#include "QFUIComponentsWindow.h"

QF::UI::Components::Panel::Panel(QF::UI::Components::Element* _Parent, const QF::Utils::Vec2& _Position,
	const QF::Utils::Vec2& _Size, bool _CARE) 
	: m_Parent{_Parent}, Element()
{
	/* Declare that im a panel */
	im_Panel(this);
	
	/* Assign as children to a assignment stack in a parenting window */
	assignAsChildrenToAbsoluteParent();

	/* Log success */
	__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, 
		"Panel created successfully"
		);

	QF::UI::Components::EventSystem::Event();
}

QF::UI::Components::Panel::~Panel() {
	/* Log success */
	__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
		std::format("Panel destructed successfully, immutable_id: {}", m_ImmutableId)
	);
}

/* Parent handling -> public */
	QF::UI::Components::Element* QF::UI::Components::Panel::g_Parent() const {
		return m_Parent;	
	}

	const bool QF::UI::Components::Panel::s_ImmutableId(long long _Id) {
		if (m_ImmutableId != static_cast<long long>(__QF_UNDEFINED)) {
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Cannot set immutable id, its already defined."); return false; 
		}
		/* Assign and return success */
		m_ImmutableId = _Id; return true; 
 	}

	const long long QF::UI::Components::Panel::g_ImmutableId() const {
		return m_ImmutableId;
	}
/* Parent handling -> private */
	void QF::UI::Components::Panel::assignAsChildrenToAbsoluteParent() {
		if (!has_AbsoluteParent()) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, 
				"Absolute parent for this panel doesn't exist, requesting shutdown "
				);
			__QF_SHUTDOWN_MSG();
			/* runtime error */
			throw std::runtime_error("Absolute parent == nullptr");
		}
		/* Assign as a child */
		g_AbsoluteParent()->i_WantToBeAssigned(this);
	}
