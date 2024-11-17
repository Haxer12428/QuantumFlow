#include "QF/QF.h"


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
			QF::UI::Components::Window* _Window = new QF::UI::Components::Window(this,
				{ __QF_DONT_CARE }, { __QF_DONT_CARE });


			std::cout << "Created windows: " << x << "\n";
			_Window->g_GLFWobject()->s_GLFWobjectOperationsAnimationState(true);

			new QF::UI::Components::Panel(_Window, { 10.0f, 10.0f }, { 100.0f, 100.0f });
			
		}

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