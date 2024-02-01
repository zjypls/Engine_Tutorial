//
// Created by 32725 on 2023/3/30.
//

#ifndef ENGINE_TUTORIAL_ZUTILS_H
#define ENGINE_TUTORIAL_ZUTILS_H

#include <string>

#include "Z/Core/Core.h"

namespace Z {
	class Z_API Utils {
	private:
		friend class zGUID;

	public:
		static std::string FileOpen(const char *filter, const char *defaultOpen = "", const char *initialDir =nullptr);

		static std::string FileSave(const char *filter, const char *defaultOpen = "", const char *initialDir = nullptr);
	};
}

#endif //ENGINE_TUTORIAL_ZUTILS_H
