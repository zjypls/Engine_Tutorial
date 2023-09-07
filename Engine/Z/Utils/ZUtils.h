//
// Created by 32725 on 2023/3/30.
//

#ifndef ENGINE_TUTORIAL_ZUTILS_H
#define ENGINE_TUTORIAL_ZUTILS_H

#include <string>
#include <commdlg.h>

namespace Z {
	class Utils {
	private:
		friend class GUID;

	public:
		static std::string FileOpen(const char *filter, const char *defaultOpen = "", const char *initialDir = "");

		static std::string FileSave(const char *filter, const char *defaultOpen = "", const char *initialDir = "");
	};

}

#endif //ENGINE_TUTORIAL_ZUTILS_H
