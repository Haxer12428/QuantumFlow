﻿#include "QF/QF.h"


class Application : QF::UI::App
{
public:
	Application()
		: QF::UI::App()
	{};

	const bool onInit() override {
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

		for (int x = 1; x <= 10; x++)
		{
			QF::UI::Components::Window* _Window = new QF::UI::Components::Window(
				{ 400, 400 }, { 800, 600 });



			g_WindowHandler() ->
				im_Child(_Window);



			std::cout << "Created windows: " << x << "\n";
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