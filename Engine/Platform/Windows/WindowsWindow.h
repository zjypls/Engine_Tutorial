//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_WINDOWSWINDOW_H
#define ENGINE_TUTORIAL_WINDOWSWINDOW_H
#include "Z/Core/zWindow.h"
#include "Z/Core/Log.h"
#include "GLFW/glfw3.h"
#include "Z/Renderer/GraphicContext.h"

namespace Z {
	class Z_API WindowsWindow final : public zWindow{
	public:
		WindowsWindow(const WindowProps&);

		inline virtual unsigned int GetWidth()const override{return WinData.width;}
		inline virtual unsigned int GetHeight()const override{return WinData.height;}

		inline virtual bool IsVSync() override{return WinData.VSync;}
		inline virtual void SetVSync(bool enable) override;
		inline virtual void SetEventCallFunc(const EventCallFunc&callFunc) override{WinData.eventCall=callFunc;}
		~WindowsWindow();
		virtual void Update() override;
		inline virtual void* GetNativeWindow() override{return window;}
		virtual Scope<GraphicContext>& GetContext(){return Context;}
		virtual void Shutdown();
	protected:
		virtual void Init(const WindowProps&);
	private:
		using WData = struct WindowData {
			std::string title;
			unsigned int width;
			unsigned int height;
			bool VSync;
			EventCallFunc eventCall;
		};
		WData WinData;
		static bool IsGLFWInit;
		GLFWwindow* window;
		Scope<GraphicContext> Context;
	};

}


#endif //ENGINE_TUTORIAL_WINDOWSWINDOW_H
