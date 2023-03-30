//
// Created by 32725 on 2023/3/30.
//
#include <Windows.h>
#include "Z/Utils/ZUtils.h"
#include "Z/Core/Application.h"
#include<GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
namespace Z {
	std::string Utils::FileOpen(const char *filter){
		OPENFILENAMEA ofn{};
		char szFile[260]{0};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;
		if(GetOpenFileNameA(&ofn)){
			return ofn.lpstrFile;
		}
		return "";
	}

	std::string Utils::FileSave(const char *filter){
		OPENFILENAMEA ofn{};
		char szFile[260]{0};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;
		if(GetSaveFileNameA(&ofn)){
			return ofn.lpstrFile;
		}
		return "";
	}

}