#include "QFExamplePaint.h"

namespace ui = QF::UI;
using app = QF::UI::App;
namespace components = ui::Components;
using self = QFExamplePaint;


/* Constructor & Destructor */
	self::QFExamplePaint(
		app* _Application 
	) 
		/* Create QF's window */
		: components::Window(_Application, __QF_DONT_CARE, __QF_DONT_CARE, "QFExamplePaint")
	{
		g_GLFWobject()->s_VsyncState(false);

		/* Set animations to true */
		g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);

		/* Create drawing frame */
		createDrawingFrame();

	}

	self::~QFExamplePaint() = default; 
/* Drawing frame setup */
	void self::createDrawingFrame() {
		m_DrawingFrame = new DrawingFrame(this);
	}