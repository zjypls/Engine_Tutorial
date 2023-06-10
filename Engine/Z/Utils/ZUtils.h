//
// Created by 32725 on 2023/3/30.
//

#ifndef ENGINE_TUTORIAL_ZUTILS_H
#define ENGINE_TUTORIAL_ZUTILS_H

#include <string>

namespace Z{
	class Utils{
	private:
		friend class GUID;
	public:
		static std::string FileOpen(const char* filter);
		static std::string FileSave(const char* filter);
	};

}

#endif //ENGINE_TUTORIAL_ZUTILS_H
