#pragma once
#include "../../QF.h"

namespace QF {
	namespace Experimental {
		class FileDialog : public QF::UI::Components::Window {
		public:
			FileDialog(QF::UI::App* _ParentApplication, const std::filesystem::path& _StartingPath);
			virtual ~FileDialog(); 
		private:

		};
	}
}