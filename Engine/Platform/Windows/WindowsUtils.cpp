//
// Created by 32725 on 2023/3/30.
//

#define GLFW_EXPOSE_NATIVE_WIN32
#include <Windows.h>
#include "Z/Utils/ZUtils.h"
#include "Z/Core/Application.h"
#include "Z/Project/Project.h"
#include "Include/glfw/include/GLFW/glfw3.h"
#include "Include/glfw/include/GLFW/glfw3native.h"
namespace Z {
	std::string Utils::FileOpen(const char *filter,const char* defaultOpen,const char* initialDir) {
		OPENFILENAMEA ofn{};
		char szFile[256]{};
		strcpy(szFile,defaultOpen);
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow()));
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (initialDir == nullptr) {
			Z_CORE_WARN("No Give Open Path");
			ofn.lpstrInitialDir = Project::GetProject() == nullptr ? ".\\" : Project::GetProjectRootDir().string().c_str();
		}
		else {
			Z_CORE_INFO("Open Path:{}",initialDir);
			ofn.lpstrInitialDir =(std::filesystem::current_path()/initialDir).string().c_str();
		}
		if (GetOpenFileNameA(&ofn)) {
			return ofn.lpstrFile;
		}
		return "";
	}

	std::string Utils::FileSave(const char *filter,const char* defaultOpen,const char* initialDir) {
		OPENFILENAMEA ofn{};
		char szFile[260]{0};
		strcpy(szFile,defaultOpen);
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if(initialDir==nullptr)
			ofn.lpstrInitialDir = Project::GetProject()== nullptr?".\\" : Project::GetProjectRootDir().string().c_str();
		else
			ofn.lpstrInitialDir=initialDir;
		if (GetSaveFileNameA(&ofn)) {
			return ofn.lpstrFile;
		}
		return "";
	}
}
