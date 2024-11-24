#include "QFExamplePaint.h"


namespace ui = QF::UI;
using app = QF::UI::App;
namespace components = ui::Components;
using self = QFExamplePaint::DrawingFrame;
namespace utils = QF::Utils;
using evts = components::EventSystem::Events;

/* Constructor & Destructor */
self::DrawingFrame(components::Window* _Parent)
	: components::Panel(_Parent, { 20.0f, 0.0f }, 1.0f, components::Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero
	)
{
	
	/* Apply sizer */
	setSizer();

	/* Create screen data */
	createScreenData();
	
	s_Position({ m_ControlsSizeX, 0.0f });

	std::cout << g_Position() << " <= pos \n";
	/* Hook */
	auto& eventHandler = g_EventHandler();

	eventHandler->Subscribe<evts::Render>(this, &self::renderScreenData);
	eventHandler->Subscribe<evts::MousePanelDragEvent>(this, &self::drawMouseWithPen);
}

self::~DrawingFrame() = default;
/* Align */
void self::setSizer() {
	alignMatchSizeWith([&](components::Panel* _Panel) -> utils::Vec2 {
		components::Window* _Window = _Panel->g_AbsoluteParent();

		const utils::Rect windowClientRect = _Window->g_GLFWobject()->g_ClientAreaRect();
		/* Full size */
		return windowClientRect.g_Size() - utils::Vec2(m_ControlsSizeX, 0.0f);
		}, components::Panel::AlignmentFlags::m_AlignX | components::Panel::AlignmentFlags::m_AlignY
	);
}
/* Render */
void self::renderScreenData(evts::Render& _R) {
	/* Create texture */

	if (m_DrawTexture != nullptr) {
		delete m_DrawTexture;
	}
	m_DrawTexture = new utils::Texture(m_ScreenData, m_ScreenSize);

	components::SimpleDC canvas{this};

	//std::cout <<  m_DrawImage->g_GLTextureID() << "\n";
	canvas.putTexture(0.0f, m_ScreenSize, m_DrawTexture->g_GLTextureId());
}

/* Screen data */
void self::createScreenData() {
	/* Override screen size */
	m_ScreenSize = g_AbsoluteParent()->g_GLFWobject()->g_ClientAreaRect().g_Size() - utils::Vec2(m_ControlsSizeX, 0.0f);

	/* Get screen params */
	int width = m_ScreenSize.x;
	int height = m_ScreenSize.y;

	std::vector<char> dataToWrite(width * height * 4, 255);



	/* Override screen data */
	m_ScreenData = dataToWrite;
}
/* Drawing */
void self::drawMouseWithPen(evts::MousePanelDragEvent& _Events) {
	if (glfwGetMouseButton(g_AbsoluteParent()->g_GLFWobject()->g_Object(), GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		return;
	}

	if (!_Events.g_PositionFixed().is_InBounds(0.0f, m_ScreenSize)) return; 

	const utils::Vec2 mousePosition = _Events.g_PositionFixed();
	int circleRadius = 4; // Set your desired radius here

	// Iterate through a square bounding box around the circle
	for (int y = -circleRadius; y <= circleRadius; y++) {
		for (int x = -circleRadius; x <= circleRadius; x++) {
			int px = static_cast<int>(mousePosition.x) + x;
			int py = static_cast<int>(mousePosition.y) + y;

			// Check if the pixel is within the circle
			if (x * x + y * y <= circleRadius * circleRadius) {
				// Calculate the buffer position of the current pixel
				int atPosition = (py * m_ScreenSize.x + px) * 4;

				// Ensure we are within bounds of the screen buffer
				if (atPosition + 3 < m_ScreenData.size() && atPosition >= 0) {
					const int _y = (atPosition / m_ScreenSize.x) / 4;
					const int _x = (atPosition - (_y * m_ScreenSize.x * 4)) / 4;
					
					if ((mousePosition + utils::Vec2(x + 1, y + 1)).is_InBounds(0.0f, m_ScreenSize) && (mousePosition + utils::Vec2(x - 1, y - 1)).is_InBounds(0.0f, m_ScreenSize)) {
						// Set color (e.g., light blue with full alpha)
						m_ScreenData[atPosition] = 0;          // Red
						m_ScreenData[atPosition + 1] = 128;    // Green
						m_ScreenData[atPosition + 2] = 255;    // Blue
						m_ScreenData[atPosition + 3] = 255;    // Alpha
					}
				}
			}
		}
	}

	printf("Drawing Circle!\n");
	} 