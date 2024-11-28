
#include <shlobj.h>
#include "QFExperimentalFileDialog.h"


namespace utils = QF::Utils;
namespace components = QF::UI::Components;
namespace experimental = QF::Experimental;
using self = experimental::FileDialog::DirFrame;
using evts = components::EventSystem::Events; 
using panelalignflags = components::Panel::AlignmentFlags;

/* Constructor & Destructor */
	self::DirFrame(FileDialog* _Parent) : components::Panel(_Parent, 0.0f, 1.0f), m_Parent{_Parent} {
		__QF_ASSERT(_Parent, "must have parent");

		createSliders();

		/* Load folder image */
		m_FolderImage = std::make_unique<utils::Image>(utils::Filesystem::g_InCurrentDirectory("folderimage.png"));
		m_FileImage = std::make_unique<utils::Image>(utils::Filesystem::g_InCurrentDirectory("fileimage.png"));

		/* Hook events */
		auto& eventHandler = g_EventHandler();

		eventHandler->Subscribe<evts::Render>(this, &self::hookRender);
	}
	
	self::~DirFrame() = default; 

/* Public set's */
	void self::updateCurrentPath(const utils::Filesystem::Path& _New) {
		if (_New.is_Broken() || _New.is_SystemReservedDeviceName()) {
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
			__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, std::format("Path[{}] is not accepted", _New.string()));
	#endif	
#endif // !NDEBUG
			return; 
		}

		m_CurrentPath = _New;

		/* Clear current objects */
		for (auto& _obj : m_Children) {
			__QF_ASSERT(_obj, "object cannot be nullptr");
			_obj->destroy();
		}
		m_Children = {}; 

		/* Update parent's path */
		m_Parent->updateCurrentPath(m_CurrentPath);
		

		/* Create new one's */
		for (const std::filesystem::directory_entry& _Entry :
				std::filesystem::directory_iterator(m_CurrentPath)) {
			try {
				const utils::Vec2 filePanelPosition = {
					m_Params.m_PathPanelFreeSpaceX / 2.0f, static_cast<float>(m_Children.size()) * m_Params.m_PathPanelSizeY
				};
				
				m_Children.push_back(new Element(this, _Entry.path().string(), m_FolderImage->g_GLTextureID(), m_FileImage->g_GLTextureID()));

				auto& filePanelHints = m_Children.back()->g_Hints();

				filePanelHints.m_TextureID = 0;
				filePanelHints.m_BGActiveColor = ImColor(40, 44, 52, 255); // Atom's dark gray
				filePanelHints.m_BGColor = m_Params.m_BGColor;       // Atom's medium gray
				filePanelHints.m_ActAsASwitch = true;

				m_Children.back()->alignMatchPositionWith([=](components::Panel*) -> const utils::Vec2 {
					return filePanelPosition;
					}, panelalignflags::m_AlignBothAxis);

				m_Children.back()->alignMatchSizeWith([=](components::Panel*) -> const utils::Vec2 {
					return { g_Size().x - m_Params.m_PathPanelFreeSpaceX, m_Params.m_PathPanelSizeY };
					}, panelalignflags::m_AlignBothAxis);

				m_Children.back()->addOnClickCallback([=](components::Built::Button* bt, evts::MouseClickEvent&) {
					selectedByHand(bt->g_ImmutableId());
					});
			}
			catch (const std::exception& Ex) {};
		}
		
	}
/* Render hook */
	void self::hookRender(evts::Render& _Render) {
		components::SimpleDC canvas{this};

		canvas.putRectFilled(0.0f, g_Size(), m_Params.m_BGColor);
	}
/* Public handlers */
	void self::selectedByHand(const long long _ImmutableID) {
		for (auto& _Child : m_Children) {
			if (_Child->g_ImmutableId() != _ImmutableID) {
				_Child->g_Hints().m_SwitchTurnedOn = false; 
			}}
#ifndef NDEBUG
	#if __QF_DEBUG_LEVEL <= 1
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Clicked by hand,redone selection for elements.");
	#endif
#endif // !NDEBUG
	}
/* Creation */
	void self::createSliders() {
		m_VerticalSlider = new components::Built::Slider(this, 0.0f, { 250.0f, 200.0f }, components::Built::Slider::Flags::m_Horizontal);
	
		auto& sliderHints = m_VerticalSlider->g_Hints();
		sliderHints.m_ElementActiveColor = ImColor(40, 123, 222, 255); // Vibrant Blue
		sliderHints.m_ElementDefaultColor = ImColor(171, 178, 191, 255); // Light Gray
		sliderHints.m_BGColor = ImColor(40, 44, 52, 255); // Dark Background



	}