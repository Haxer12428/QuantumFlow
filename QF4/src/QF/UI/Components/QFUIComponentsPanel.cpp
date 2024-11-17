#include "QFUIComponentsWindow.h"

QF::UI::Components::Panel::Panel(QF::UI::Components::Element* _Parent, const QF::Utils::Vec2& _Position,
	const QF::Utils::Vec2& _Size, bool _CARE) 
	: m_Parent{_Parent}, Element(), m_EventHandler{std::make_unique<QF::UI::Components::EventSystem::EventHandler>()},
	m_Position{_Position}, m_Size{_Size}
{
	/* Declare that im a panel */
	im_Panel(this);
	
	/* Assign as children to a assignment stack in a parenting window */
	assignAsChildrenToAbsoluteParent();

	/* Subscribe to event -> called after panel is assigned to absolute parent */
	m_EventHandler->Subscribe<EventSystem::Events::PanelAssignedToWindowStack>(this, &Panel::assignedAsChildrenToAbsoluteParent);

	/* Log success */
	__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, 
		"Panel created successfully"
		);

	/* Experimental -> */
		m_EventHandler->Subscribe<EventSystem::Events::Render>(this, &Panel::renderingevent);
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

	void QF::UI::Components::Panel::im_Child(Panel* _Child) {
		m_Children.push_back(_Child);
		/* Log */
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, std::format("Assigned as a child[panel], child request({}) -> panel({}).",
			_Child->g_ImmutableId(), g_ImmutableId()
		));
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

/* Get's public */
	/* If this is not visible, children also won't be */
	const bool QF::UI::Components::Panel::is_Visible() const
	{
		return m_Visible; 
	}

	const QF::Utils::Vec2 QF::UI::Components::Panel::g_FinalPosition() const {
		return (m_Position + m_Size);
	}
/* Parent handling -> private */
	void QF::UI::Components::Panel::assignedAsChildrenToAbsoluteParent(EventSystem::Events::PanelAssignedToWindowStack& _Event) {
		/* Assign to parenting panel(if any) as children */
		assignAsChildrenToParent();
	}

	void QF::UI::Components::Panel::assignAsChildrenToAbsoluteParent() {
		if (!has_AbsoluteParent()) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__,
				"Absolute parent for this panel doesn't exist, requesting shutdown."
			);
			__QF_SHUTDOWN_MSG();
			/* runtime error */
			throw std::runtime_error("Absolute parent == nullptr");
		}
		/* Assign as a child */
		g_AbsoluteParent()->i_WantToBeAssigned(this);
	}

	void QF::UI::Components::Panel::assignAsChildrenToParent() {
		if (!m_Parent->ami_Panel() && !m_Parent->ami_Window()) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Whatami? Im not a parent of a window or a parent of a panel, requesting shutdown");
			__QF_SHUTDOWN_MSG();
			/* runtime error */
			throw std::runtime_error("No parent?");
		}
		/* Check for panel parenting */
		if (!m_Parent->ami_Panel()) return; 

		/* Assign as a child */
		m_Parent->g_Panel()->im_Child(this);
	}
/* Event handling -> public */
	std::unique_ptr<QF::UI::Components::EventSystem::EventHandler>& QF::UI::Components::Panel::g_EventHandler() {
		return m_EventHandler;
	}
