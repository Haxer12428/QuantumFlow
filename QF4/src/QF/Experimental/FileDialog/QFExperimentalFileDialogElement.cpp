#include "QFExperimentalFileDialog.h"

namespace utils = QF::Utils;
namespace components = QF::UI::Components;
namespace experimental = QF::Experimental;
using self = experimental::FileDialog::DirFrame::Element;
using evts = components::EventSystem::Events;
using panelalignflags = components::Panel::AlignmentFlags;

/* Constructor & Destructor */
	self::Element(DirFrame* _Parent, const std::filesystem::path& _Path, GLuint _FolderTexture, GLuint _FileTexture)
		: components::Built::Button(_Parent, components::Built::Button::Hints{}), m_FolderTexture{_FolderTexture}, m_FileTexture{_FileTexture},
		m_Path{_Path}, m_Parent{_Parent}
	{

		printf("tried\n");

		m_Name = m_Path.filename().string();

		m_IsDirectory = std::filesystem::is_directory(m_Path);

		s_Position(1.0f);
		s_Size(10.0f);
		__QF_ASSERT(_Parent, "cannot proceed without a valid parent");

		auto& eventHandler = g_EventHandler();

		eventHandler->Subscribe<evts::Render>(this, &self::hookRender);
		eventHandler->Subscribe<evts::MouseDoubleClickEvent>(this, &self::hookDClickHandle);

#ifndef NDEBUG
	#if __QF_DEBUG_LEVE <= 1
			__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Created");
	#endif 
#endif // !NDEBUG

	}

	self::~Element() = default;
/* Render hooks */
	void self::hookRender(evts::Render&) {
		components::SimpleDC canvas{this};

		const QF::Utils::Vec2	texturePosition = { m_TexturePushX, 0.0f };
		const QF::Utils::Vec2 textureSize = g_Size().y * 0.75f;

		const QF::Utils::Vec2 namePosition = { texturePosition.x + textureSize.x + texturePosition.x + m_StatsPushX, 0.0f};

		const std::string name = m_Path.filename().string();

		/* Render calls */
			/* Name */

		canvas.putText(namePosition, name, ImColor(255, 255, 255, 255), components::SimpleDC::DrawingFlags::m_AlignCenterY | components::SimpleDC::DrawingFlags::m_AlignIgnoreBoundaryLimits);
		
		GLuint currentTexture = (m_IsDirectory == true ? m_FolderTexture : m_FileTexture);

		canvas.putTexture(texturePosition, textureSize, currentTexture, components::SimpleDC::DrawingFlags::m_AlignCenterY | components::SimpleDC::DrawingFlags::m_AlignIgnoreBoundaryLimits);
	}
/* Dclick hook */
	void self::hookDClickHandle(evts::MouseDoubleClickEvent&) {
		if (m_IsDirectory) {
			m_Parent->updateCurrentPath(m_Path); return;
		}
		system(std::format("start {}", m_Path.string()).c_str());
	}