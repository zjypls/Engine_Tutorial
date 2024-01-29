//
// Created by 32725 on 2023/3/12.
//

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "Include/stb/stb_image.h"

#include "Z/Events/ApplicationEvent.h"
#include "Z/Events/KeyEvent.h"
#include "Z/Events/MouseEvent.h"
#include "Z/Core/Random.h"

#include "Z/Core/WindowImpl.h"


namespace Z {
	bool WindowImpl::IsGLFWInit=false;
	WindowImpl::WindowImpl(const WindowProps &props) {
		Random::Init();
		Init(props);
	}

	//create glfw error callback func
	static void GLFWErrorCallback(int error,const char* description){
		if(error==65548)return;
		Z_CORE_ERROR("GLFW Error ({0}):{1}",error,description);
	}

	void WindowImpl::Init(const WindowProps &props) {
        Z_CORE_ASSERT(!IsGLFWInit,"ReInit failed!");
		WinData.title=props.title;
		WinData.width=props.width;
		WinData.height=props.height;
		int res=glfwInit();
        glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
		Z_ASSERT(res,"failed to Init GLFW!!!");
		window= glfwCreateWindow(WinData.width,WinData.height,(WinData.title +"-[Vulkan]").c_str(), nullptr, nullptr);
		GLFWimage Icon{};
		Icon.pixels=stbi_load("Assets/Configs/AppIcon.png",&Icon.width,&Icon.height, nullptr,STBI_rgb_alpha);
		Z_CORE_ASSERT(Icon.pixels!= nullptr,"Failed to load icon");
		glfwSetWindowIcon(window,1,&Icon);
		stbi_image_free(Icon.pixels);
		stbi_set_flip_vertically_on_load(1);
		glfwSetWindowUserPointer(window,&WinData);
		//SetVSync(false);

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
	}

	WindowImpl::~WindowImpl() {
	}

	void WindowImpl::Shutdown() {
		Z_CORE_WARN("Window Shutdown");
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