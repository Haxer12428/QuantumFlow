#pragma once
#include "../../QF.h"
#include "../Components/QFUIComponentsWindow.h"

namespace QF
{
	namespace UI
	{
		namespace Components {
			class Window; 
		}

		class App
		{
		public:
			App();
			~App();

			/* Initialization */
			virtual const bool onInit();
			const bool Execute();
			std::unique_ptr<QF::Utils::ImageManager>& g_ImageManager();
			/* Handling windows */
			class WindowHandler { public:
				/* Constructor & Destructor */
				WindowHandler();
				~WindowHandler();
				/* Main loop */
				void enterMainLoop();
				/* Children handling */
			public:
				void im_Child(Components::Window* _Window);
				QF::UI::Components::Window* g_Window(const long long _ImmutableID); 
			private:
				const long long g_NewImmutableIdForChild();

				void closeWindows(const std::vector<long long>& _IdsToClose);
			private:
				std::vector<std::unique_ptr<
					Components::Window>> m_Children;

				/* Window immutable id's count */
				long long m_ChildrenImmutableIdsCount = -1;
			};
			
      std::unique_ptr<WindowHandler>& g_WindowHandler();

			/* Static Implementation: Make sure to use class derived from App */
			template <typename __Class>
			static void implementApplication()
			{
				bool appImplementation = false;

				try {
					QF::Utils::Debug::Log(QF::Utils::Debug::LogType::Important, __FUNCTION__,
						"Implementing app: Make sure to use class derived from QF::UI::App!"
						);

					auto appImplemented =
						reinterpret_cast<QF::UI::App*>(new __Class());

					appImplementation = appImplemented->onInit();
				}
				catch (...)
				{
					/* Print error */
					__QF_DEBUG_LOG(QF::Utils::Debug::LogType::Error, __FUNCTION__,
						"Reinterpet cast failed.");
					/* Set implementation status to false */
					appImplementation = false; 
				}

				if (appImplementation == true) {
					/* Finalized this implementation */
					__QF_DEBUG_LOG(__QF_IMPORTANT, __FUNCTION__,
						"Finalized lifetime of the app, implementation was successfull."
					);
					return;
				}

				/* Print error */
				__QF_DEBUG_LOG(QF::Utils::Debug::LogType::Error, __FUNCTION__,
					"Failed to implement");
			}
		


		private:
			/* Window handler */
			std::unique_ptr<WindowHandler> m_WindowHandler;
			std::unique_ptr<QF::Utils::ImageManager> m_ImageManager; 
		};
	}
}

#define __QF_IMPLEMENT_APP QF::UI::App::implementApplication;