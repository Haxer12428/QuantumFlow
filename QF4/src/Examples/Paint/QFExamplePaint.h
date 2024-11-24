#include "../../QF/QF.h"

class QFExamplePaint : QF::UI::Components::Window
{
public:
	class DrawingFrame : public QF::UI::Components::Panel
	{
	public:
		DrawingFrame(QF::UI::Components::Window* _Window); 
		virtual ~DrawingFrame();
	
	private:
		void setSizer(); 

		void createScreenData(); 
		void renderScreenData(QF::UI::Components::EventSystem::Events::Render& _Render);
		void drawMouseWithPen(QF::UI::Components::EventSystem::Events::MousePanelDragEvent& _Render);
	private:

		QF::Utils::Vec2 m_ScreenSize = { 100.0f, 100.0f };
		std::vector<char> m_ScreenData;
		QF::Utils::Texture* m_DrawTexture = nullptr;

		float m_ControlsSizeX = 120;
	};


public:
	QFExamplePaint(QF::UI::App* _Application);
	virtual ~QFExamplePaint(); 

private:
	void createDrawingFrame();

	DrawingFrame* m_DrawingFrame; 
	
};