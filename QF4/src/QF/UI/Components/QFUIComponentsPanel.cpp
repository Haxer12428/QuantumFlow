#include "QFUIComponentsWindow.h"

QF::UI::Components::Panel::Panel(QF::UI::Components::Element* _Parent, const QF::Utils::Vec2& _Position,
	const QF::Utils::Vec2& _Size, const Flags& _Flags) 
	: m_Parent{_Parent}, Element(), m_EventHandler{std::make_unique<QF::UI::Components::EventSystem::EventHandler>()},
	m_Position{_Position}, m_Size{_Size}, m_Flags{static_cast<std::underlying_type<Flags>::type>(_Flags)}
{
	/* Assert parent */
	assertParent();

	/* Declare that im a panel */
	im_Panel(this);
	
	/* Assign as children to a assignment stack in a parenting window */
	assignAsChildrenToAbsoluteParent();

	/* Subscribe to event -> called after panel is assigned to absolute parent */
	m_EventHandler->Subscribe<EventSystem::Events::PanelAssignedToWindowStack>(this, &Panel::assignedAsChildrenToAbsoluteParent);
	m_EventHandler->Subscribe<EventSystem::Events::Render>(this, &Panel::assignValuesCapturedFromParent);
	m_EventHandler->Subscribe<EventSystem::Events::Render>(this, &Panel::alignRunCallback);

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
	const bool QF::UI::Components::Panel::is_VisibleFixed() const
	{
		/* Check if size offset & position offset is alight */
		if (!(m_Flags & static_cast<std::underlying_type<Flags>::type>(Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero))
				&& m_FixedSize <= QF::Utils::Vec2{ 0.0f, 0.0f }) {
			return false; 
		}
		/* Check for class visibility */
		if (is_Visible() == false)
			return false; 

		/* Return class var */
		return m_VisibleFixed; 
	}

	const bool QF::UI::Components::Panel::is_Visible() const {
		return m_Visible; 
	}

	const QF::Utils::Vec2 QF::UI::Components::Panel::g_Position() const
	{
		return m_Position;
	}

	const QF::Utils::Vec2 QF::UI::Components::Panel::g_FixedSize() const
	{
		return m_FixedSize;	
	}

	const QF::Utils::Vec2 QF::UI::Components::Panel::g_FixedPosition() const
	{
		return m_FixedPosition;
	}

	const QF::Utils::Vec2 QF::UI::Components::Panel::g_FinalPosition() const {
		return (m_Position + m_Size);
	}
/* Parent handling -> private */
	const bool QF::UI::Components::Panel::assertParent() const {
		if (!m_Parent || !m_Parent->ami_Panel() && !m_Parent->ami_Window()) {
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, "Whatami? Im not a parent of a window or a parent of a panel, requesting shutdown");
			__QF_SHUTDOWN_MSG();
			/* runtime error */
			throw std::runtime_error("No parent?");
		}
		return true; 
	}

	void QF::UI::Components::Panel::assignedAsChildrenToAbsoluteParent(EventSystem::Events::PanelAssignedToWindowStack& _Event) {
		/* Assign to parenting panel(if any) as children */
		assignAsChildrenToParent();

		/* Assign fixed position and fixed size based on parent */
		assignFixedPositionAndSizeFromParent();
	}

	void QF::UI::Components::Panel::assignValuesCapturedFromParent(EventSystem::Events::Render& r) {
		assignFixedPositionAndSizeFromParent();
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
		/* Assert parent */
		assertParent();

		/* Check for panel parenting */
		if (!m_Parent->ami_Panel()) return; 

		/* Assign as a child */
		m_Parent->g_Panel()->im_Child(this);
	}

	void QF::UI::Components::Panel::assignFixedVisibilityFromParent() {
		/* Check if parent */
		if (!m_Parent->ami_Panel()) return;
		/* Assign from panel */
		m_VisibleFixed = m_Parent->g_Panel()->is_VisibleFixed();
	}

	void QF::UI::Components::Panel::assignFixedPositionAndSizeFromParent() {
		/* Assert parent */
		assertParent();

		auto calculateFixedPositionAndSize = [&](const std::array<QF::Utils::Vec2, 2>& _Captured) 
			-> const std::array<QF::Utils::Vec2, 2>
		{
			QF::Utils::Vec2 fixedPosition = m_Position; 
			QF::Utils::Vec2 fixedSize = m_Size; 

			/* Get pos */
			if (!(m_Flags & static_cast<uint64_t>(Flags::m_DontCareAboutFixedPosition))) {
				fixedPosition = (fixedPosition + _Captured.at(0));
			}
			/* Get size */
			if (!(m_Flags & static_cast<uint64_t>(Flags::m_DontCareAboutFixedSize))) {
				fixedSize = (fixedSize);
				fixedSize.limit(_Captured.at(1) - m_Position);
			}

			return std::array<QF::Utils::Vec2, 2>{fixedPosition, fixedSize};
		};

		/* Capture from a parent if good */
		/* Window */
		if (m_Parent->ami_Window()) {
			/* Capture */
			std::array<QF::Utils::Vec2, 2> capturedFromWindow;

			capturedFromWindow[0] = { 0, g_Parent()->g_Window()->g_GLFWobject()->g_FixedClientAreaRect().g_Position().y };
			capturedFromWindow[1] = m_Size + m_Position;

			/* Calculate */
			std::array<QF::Utils::Vec2, 2> fixedPosAndSize = calculateFixedPositionAndSize(capturedFromWindow);
			
			/* Apply & Abort */
			m_FixedPosition = fixedPosAndSize[0];
			m_FixedSize = fixedPosAndSize[1];

			return; 
		}
		/* Panel */
		/* Capture */
		std::array<QF::Utils::Vec2, 2> capturedFromPanel; 

		Panel* parentingPanel = m_Parent->g_Panel();

		capturedFromPanel[0] = parentingPanel->g_FixedPosition();
		capturedFromPanel[1] = parentingPanel->g_FixedSize();

		/* Calculate */
		std::array<QF::Utils::Vec2, 2> fixedPosAndSize = calculateFixedPositionAndSize(capturedFromPanel);

		/* Apply */
		m_FixedPosition = fixedPosAndSize[0];
		m_FixedSize = fixedPosAndSize[1];
	}
/* Event handling -> public */
	std::unique_ptr<QF::UI::Components::EventSystem::EventHandler>& QF::UI::Components::Panel::g_EventHandler() {
		return m_EventHandler;
	}
/* Set's -> public */
	void QF::UI::Components::Panel::s_Visible(bool _New) {
		m_Visible = _New;
	}

	void QF::UI::Components::Panel::s_Size(const QF::Utils::Vec2& _New) {
		m_Size = _New;
	}

	void QF::UI::Components::Panel::s_Position(const QF::Utils::Vec2& _New) {
		m_Position = _New; 
	}
/* Alignment */
	void QF::UI::Components::Panel::alignRunCallback(EventSystem::Events::Render&) {
		for (auto _Obj : m_AllignCallbacks) {
			_Obj.m_Callback(this);
		}
	}

	bool QF::UI::Components::Panel::alignDestroyCallback(uint64_t _UniqueId) {
		for (size_t _Iterator = 0; _Iterator < m_AllignCallbacks.size();
			_Iterator++) {
			/* Erase from stack if match  */
			if (m_AllignCallbacks[_Iterator].m_UniqueID == _Iterator) 
			{
				auto erasePosition = (m_AllignCallbacks.begin() + _Iterator);

				m_AllignCallbacks.erase(erasePosition);
				/* Abort */
				return true;
			}}
		/* Destroy failed: no match */
		__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, "Failed to find desired UniqueId, couldn't destroy.");

		return false; 
	}

	const QF::Utils::Vec2 QF::UI::Components::Panel::alignModVec2BasedOnFlags(const QF::Utils::Vec2& _Current, const QF::Utils::Vec2& _Default, AlignmentFlags _Flags) {
		QF::Utils::Vec2 modifiedVec2 = _Default;

		auto flagsFixed = static_cast<std::underlying_type<AlignmentFlags>::type>(_Flags);

		/* y axis */
		if (static_cast<std::underlying_type<AlignmentFlags>::type>(
			AlignmentFlags::m_AlignX) & flagsFixed
			) {
			modifiedVec2.x = _Current.x;
		}
		/* x axis */
		if (static_cast<std::underlying_type<AlignmentFlags>::type>
			(AlignmentFlags::m_AlignX) & flagsFixed
			) {
			modifiedVec2.y = _Current.y;
		}

		return modifiedVec2;
	}


	const uint64_t QF::UI::Components::Panel::alignMatchSizeWith(const std::function<QF::Utils::Vec2(QF::UI::Components::Panel* _Panel)>& _Func, AlignmentFlags _Flags) {
		m_AllignCallbacks.push_back({m_AllignIdManager->g_New(), [=](Panel* _Panel) -> void {
			const QF::Utils::Vec2 newSize = _Func(_Panel);

			s_Size(alignModVec2BasedOnFlags(newSize, m_Size, _Flags));
		}});
		/* Return unique id */
		return m_AllignCallbacks.back().m_UniqueID;
	}
	