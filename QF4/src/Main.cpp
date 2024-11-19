#include "QF/QF.h"

namespace utils = QF::Utils;

class Application : QF::UI::App
{
public:
	Application()
		: QF::UI::App()
	{};

	const bool onInit() override {
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

		for (int x = 1; x <= 1; x++)
		{
			//QF::Utils::Debug::s_ToAllPrintHints(true);
			QF::UI::Components::Window* _Window = new QF::UI::Components::Window(this,
				{ __QF_DONT_CARE }, { __QF_DONT_CARE }, "QFexample");

			std::cout << "Created windows: " << x << "\n";
			_Window->g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);

			//QF::UI::Components::Panel* lastpanel = new QF::UI::Components::Panel(_Window, { 10.0f, 10.0f }, { 500.0f, 300.0f });
			
			std::cout << _Window->g_GLFWobject()->g_ClientAreaRect().g_String() << "\n";
			
			for (int x = 0; x < 0; x++) 
			{

				//lastpanel = new QF::UI::Components::Panel(lastpanel, { 100.0f, 100.0f }, { 200.0f, 150.0f });
			}
			//QF::Utils::Debug::s_ToAllPrintHints(true);
		}

		QF::UI::Components::Panel::Flags flagspanel = QF::UI::Components::Panel::Flags::m_None;// = QF::UI::Components::Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero;
		const bool matched = utils::Compare::bitFlagPoolMatchEnumClass(flagspanel, QF::UI::Components::Panel::Flags::m_DisplayIfSizeIsLessOrEqualToZero);
		std::cout << "flag: " << matched << "\n";

		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

		__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
			std::format("Took total of: {} milliseconds", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
		);

		return this->Execute();
	}
};



int main()
{
	__QF_INIT();
	QF::Utils::Debug::s_ToAllPrintHints(true);


	QF::UI::App::implementApplication<Application>();
}