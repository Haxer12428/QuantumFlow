#include "../../QFUIComponentsWindow.h"


namespace components = QF::UI::Components;
namespace utils = QF::Utils;
namespace comp_built = components::Built;
using evts = components::EventSystem::Events;


/* Constructor & Destructor */

	components::Window::GLFWobject::CustomTitleBar::CustomTitleBar(Window* _Parent) 
		: Panel(_Parent, {0.0f}, {0.0f}, 
				Panel::Flags::m_DontCareAboutFixedPosition |
				Panel::Flags::m_DontCareAboutFixedSize |
				Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero | 
				Panel::Flags::m_DontCareAboutClipRectWhenRendering | 
				Panel::Flags::m_CatchEventsOutsideOfClientRect 
			) {
		


		/* Create buttons */
		createButtons(3);


		/* Set aligner */
		uint64_t alignmentId = this->alignMatchSizeWith(
			[=](Panel* _Panel) -> utils::Vec2 {
				const utils::Vec2 titleBarSize = _Parent->g_GLFWobject()->g_TitleBarRect().g_Size();

				/* Set buttons sizes */
				for (auto _Button : m_Buttons) {
					_Button->s_Size({ titleBarSize.y, titleBarSize.y - 2.0f });
				}

				return titleBarSize;
			}, Panel::AlignmentFlags::m_AlignX | Panel::AlignmentFlags::m_AlignY);

		/* Bind events */
		g_EventHandler()->Subscribe<EventSystem::Events::MousePanelDragEvent>(this, &CustomTitleBar::moveWindow);
		g_EventHandler()->Subscribe<components::EventSystem::Events::Render>(this, &CustomTitleBar::Render);

		/* Set aligner for buttons */
		alignButtonsPosition();

		/* Log creation */
		__QF_DEBUG_LOG(__QF_MESSAGE, __FUNCTION__, "Created.");
	}

	components::Window::GLFWobject::CustomTitleBar::~CustomTitleBar() {
		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__, "Destroyed.");
	}

/* Get's -> Public */
	components::Window::GLFWobject::CustomTitleBar::Hints& components::Window::GLFWobject::CustomTitleBar::g_Hints() {
		return m_Hints;
	}
/* Render */
	void components::Window::GLFWobject::CustomTitleBar::Render(components::EventSystem::Events::Render&) {
		components::SimpleDC canvas{this};
		/* Draw bg */
		canvas.putRectFilled({0.0f}, g_Size(), m_Hints.m_BGColor);
		/* Draw name */
		std::string windowName = g_AbsoluteParent()->g_GLFWobject()->g_Name();

		canvas.putText({ 0.0f }, windowName, m_Hints.m_NameColor, 
			components::SimpleDC::DrawingFlags::m_AlignCenterX | components::SimpleDC::DrawingFlags::m_AlignCenterY
			);
	}
/* Mouse panel drag event: move window */
	void components::Window::GLFWobject::CustomTitleBar::moveWindow(evts::MousePanelDragEvent& _Event) {
		GLFWobject* glfwObject = g_AbsoluteParent()->g_GLFWobject().get();
		
		const utils::Vec2 globalMousePosition = glfwObject->g_MousePosition();
		const utils::Vec2 newWindowPosition = (globalMousePosition - _Event.g_PositionClickedFixed());
		
		glfwObject->s_Position(newWindowPosition);
	}
/* Internals */
	void components::Window::GLFWobject::CustomTitleBar::alignButtonsPosition()
	{
		/* Allign from left */
		for (size_t _Iterator = 0; _Iterator < m_Buttons.size(); 
			_Iterator++) {
			comp_built::Button* buttonInstance = m_Buttons[_Iterator];

			buttonInstance->alignMatchPositionWith([=](Panel* _Panel) -> utils::Vec2 {
				utils::Vec2 positionNew = { g_Size().x - (_Panel->g_Size().x * (_Iterator + 1)), 1.0f };

				return positionNew;
			}, Panel::AlignmentFlags::m_AlignX | Panel::AlignmentFlags::m_AlignY);
		}
	}

	void components::Window::GLFWobject::CustomTitleBar::createButtons(int _Count) {

		/* Button default hints */
			comp_built::Button::Hints buttonHints;
			buttonHints.m_BGColor = m_Hints.m_BGColor;

			buttonHints.m_PanelFlags = 
				(
						Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero 
					| Panel::Flags::m_DontCareAboutFixedPosition 
					| Panel::Flags::m_DontCareAboutFixedSize
					| Panel::Flags::m_DontCareAboutClipRectWhenRendering
					| Panel::Flags::m_CatchEventsOutsideOfClientRect
				);

		/* Button creation */
		for (int _ = 0; _ < _Count; _++) { 
			m_Buttons.push_back(new comp_built::Button(this, buttonHints));
		}

		/* Self button appliance -> pre render callbacks in case of user value switch */
			/* Exit button */
			m_Buttons[0]->utilAddPreRenderCallback([&](components::Panel*
				) {
				m_Buttons[0]->g_Hints().m_BGActiveColor = m_Hints.m_ButtonExitActiveColor;
				});

			/* Maximize button */
			m_Buttons[1]->utilAddPreRenderCallback([&](components::Panel*
				) {
				m_Buttons[1]->m_Hints.m_BGActiveColor = m_Hints.m_ButtonMaximizeActiveColor;
				});

			/* Minimalize button */
			m_Buttons[2]->utilAddPreRenderCallback([&](components::Panel*
				) {
				m_Buttons[2]->m_Hints.m_BGActiveColor = m_Hints.m_ButtonMinimizeActiveColor;
				});

	
		/* Add functionality to buttons */
			components::Window::GLFWobject* GLFWobject = g_AbsoluteParent()->g_GLFWobject().get();

			/* Exit button */
			m_Buttons[0]->addOnClickCallback([=](comp_built::Button*, evts::MouseClickEvent&
				) {
				GLFWobject->destroy();
				});

			/* Maximize Button */
			m_Buttons[1]->addOnClickCallback([=](comp_built::Button*, evts::MouseClickEvent&
				) {
				if (GLFWobject->is_Maximized()) {
					GLFWobject->restore(); return; 
				}
				GLFWobject->maximalize();
				});

			/* Minimalize Button */
			m_Buttons[2]->addOnClickCallback([=](comp_built::Button*, evts::MouseClickEvent&
				) {
				GLFWobject->minimalize();
				});

		/* Create image's for the buttons & apply the textures  */
			const std::vector<std::string> resourcesNames = {
				"ICON_CLOSE_PNG", "ICON_MAX_PNG", "ICON_MIN_PNG"
			};
			
			/* Create image instances for each button & load resource into it 
				resources for titlebar are embedded 
			*/
			for (size_t _Iterator = 0; _Iterator < resourcesNames.size(); _Iterator++) {
				m_ButtonsImages.push_back(std::make_unique<utils::Image>(
					utils::System::g_PNGdataFromEmbeddedResource(resourcesNames[_Iterator])
				));
				m_Buttons[_Iterator]->g_Hints().m_TextureID = m_ButtonsImages.back()->g_GLTextureID();
			}		
	}
	