//
// Created by 32725 on 2023/3/12.
//

#include <filesystem>

#include "Include/stb/stb_image.h"

#include "Z/Events/ApplicationEvent.h"
#include "Z/Events/KeyEvent.h"
#include "Z/Events/MouseEvent.h"
#include "Z/Core/Random.h"
#include "Z/Renderer/Particle.h"

#include "Z/Core/WindowImpl.h"
#include "Platform/OpenGL/OpenGLContext.h"


namespace Z {
	bool WindowImpl::IsGLFWInit=false;
	WindowImpl::WindowImpl(const WindowProps &props) {
		Random::Init();
		Particle::Init();
		Init(props);
	}

	//create glfw error callback func
	static void GLFWErrorCallback(int error,const char* description){
		//todo:glfw always throw an error when using Wayland : "Wayland: The platform does not provide the window position"
		//noticed that , if don't use imgui,this error disappear
		//marked at 2024/1/28 20.

		//skip an error when using wayland that may caused by imgui Docking window
#ifdef Z_USING_WAYLAND
		if(error==65548)return;
#endif
		Z_CORE_ERROR("GLFW Error ({0}):{1}",error,description);
	}

	void WindowImpl::Init(const WindowProps &props) {
		WinData.title=props.title;
		WinData.width=props.width;
		WinData.height=props.height;
		if(!IsGLFWInit){
			int res=glfwInit();
			Z_ASSERT(res,"failed to Init GLFW!!!");
			IsGLFWInit=true;
		}
		GraphicContext::PreInitForRenderAPI();
		window= glfwCreateWindow(WinData.width,WinData.height,(WinData.title +"-[" + RenderAPI::GetApiStr()+"]"
#ifdef Z_DEBUG
        "-[Debug-Build]"
#endif
        ).c_str(), nullptr, nullptr);
		GLFWimage Icon{};
		Icon.pixels=stbi_load("Assets/Configs/AppIcon.png",&Icon.width,&Icon.height, nullptr,STBI_rgb_alpha);
		Z_CORE_ASSERT(Icon.pixels!= nullptr,"Failed to load icon");
		glfwSetWindowIcon(window,1,&Icon);
		stbi_image_free(Icon.pixels);
		stbi_set_flip_vertically_on_load(1);
		Context=GraphicContext::Create(window);
		Context->Init();
		glfwSetWindowUserPointer(window,&WinData);
		SetVSync(false);

		//set window resize call back for glfw
		glfwSetFramebufferSizeCallback(window,[](GLFWwindow* win,int w,int h){
			auto data=(WindowData*) glfwGetWindowUserPointer(win);
			data->width=w;
			data->height=h;
			WindowResizeEvent e{w,h};
			data->eventCall(e);
		});

		//set window close call back for glfw
		glfwSetWindowCloseCallback(window,[](GLFWwindow* win){
			auto data=(WindowData*) glfwGetWindowUserPointer(win);
			WindowCloseEvent e;
			data->eventCall(e);
		});

		glfwSetCursorPosCallback(window,[](GLFWwindow* win,double x,double y){
			auto data=(WindowData*) glfwGetWindowUserPointer(win);
			MouseMovedEvent e{(float)x,(float)y};
			data->eventCall(e);
		});
		//set key press call back for glfw
		glfwSetKeyCallback(window,[](GLFWwindow* win,int key,int scancode,int action,int mods){
			auto data=(WindowData*) glfwGetWindowUserPointer(win);
			switch(action){
				case GLFW_PRESS:{
					KeyPressEvent e{(KeyCode)key,0};
					data->eventCall(e);
					break;
				}
				case GLFW_RELEASE:{
					KeyReleasedEvent e{(KeyCode)key};
					data->eventCall(e);
					break;
				}
				case GLFW_REPEAT:{
					KeyPressEvent e{(KeyCode)key,1};
					data->eventCall(e);
					break;
				}
			}
		});
		//set mouse button press call back for glfw
		glfwSetMouseButtonCallback(window,[](GLFWwindow* win,int button,int action,int mods){
			auto data=(WindowData*) glfwGetWindowUserPointer(win);
			switch(action){
				case GLFW_PRESS:{
					MouseButtonPressedEvent e{(MouseCode)button};
					data->eventCall(e);
					break;
				}
				case GLFW_RELEASE:{
					MouseButtonReleasedEvent e{(MouseCode)button};
					data->eventCall(e);
					break;
				}
			}
		});
		//set mouse scroll call back for glfw
		glfwSetScrollCallback(window,[](GLFWwindow* win,double xoffset,double yoffset){
			auto data=(WindowData*) glfwGetWindowUserPointer(win);
			MouseScrollEvent e{(float)xoffset,(float)yoffset};
			data->eventCall(e);
		});
		//set glfw error call
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	void WindowImpl::Update() {
		glfwPollEvents();
		Context->SwapBuffers();
	}

	WindowImpl::~WindowImpl() {
	}

	void WindowImpl::Shutdown() {
		Z_CORE_WARN("Window Shutdown");
		Context->Destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void WindowImpl::SetVSync(bool enable) {
		glfwSwapInterval(enable);
		WinData.VSync=enable;

	}
	zWindow* zWindow::Create(const WindowProps &props) {
		return new WindowImpl(props);
	}
}