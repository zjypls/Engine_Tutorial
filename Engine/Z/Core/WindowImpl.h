//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_WINDOWSWINDOW_H
#define ENGINE_TUTORIAL_WINDOWSWINDOW_H
#pragma once
#include "Include/glfw/include/GLFW/glfw3.h"

#include "Z/Core/zWindow.h"
#include "Z/Core/Log.h"

namespace Z {
	class Z_API WindowImpl final : public zWindow{
	public:
		WindowImpl(const WindowProps&);

		[[nodiscard]] uint32 GetWidth()const override{return WinData.width;}
		[[nodiscard]] uint32 GetHeight()const override{return WinData.height;}

		void SetEventCallFunc(const EventCallFunc&callFunc) override{WinData.eventCall=callFunc;}
		~WindowImpl() override;
		void Update() override;
		void* GetNativeWindow() override{return window;}
		void Shutdown() override;
	protected:
		void Init(const WindowProps&);
	private:
		using WData = struct WindowData {
			std::string title;
			uint32 width;
			uint32 height;
			bool VSync;
			EventCallFunc eventCall;
		};
		WData WinData;
		static bool IsGLFWInit;
		GLFWwindow* window;
	};

}


#endif //ENGINE_TUTORIAL_WINDOWSWINDOW_H
