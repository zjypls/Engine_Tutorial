//
// Created by 32725 on 2023/4/2.
//

#ifndef ENGINE_TUTORIAL_CONTENTBROWSER_H
#define ENGINE_TUTORIAL_CONTENTBROWSER_H
#include "z.h"



namespace Z {
	class ContentBrowser {
	public:
		ContentBrowser();

		~ContentBrowser() = default;
		void loadIcons(const std::initializer_list<std::string>& paths);

		void OnImGuiRender();

		void SetWorkPath(const std::string& path){currentPath=path;}

	private:
		std::filesystem::path currentPath;
	};

}


#endif //ENGINE_TUTORIAL_CONTENTBROWSER_H
