//
// Created by 32725 on 2023/3/12.
//

#include "WindowsWindow.h"
#include "Z/Events/ApplicationEvent.h"
#include "Z/Events/KeyEvent.h"
#include "Z/Events/MouseEvent.h"
#include "glad/glad.h"
#include "Platform/OpenGL/zOpenGLContext.h"
#include "Z/Core/Random.h"
#include "Z/Renderer/Particle.h"

namespace Z {
	bool WindowsWindow::IsGLFWInit=false;
	WindowsWindow::WindowsWindow(const WindowProps &props) {
		//ToDo:init zOpenGLContext
		Random::Init();
		Z::Particle::Init();
		Init(props);
	}

	//create glfw error callback func
	static void GLFWErrorCallback(int error,const char* description){
		Z_CORE_ERROR("GLFW Error ({0}):{1}",error,description);
	}

	void WindowsWindow::Init(const WindowProps &props) {
		WinData.title=props.title;
		WinData.width=props.width;
		WinData.height=props.height;
		if(!IsGLFWInit){
			int res=glfwInit();
			Z_ASSERT(res,"failed to Init GLFW!!!");
			IsGLFWInit=true;
		}
		window= glfwCreateWindow(WinData.width,WinData.height,WinData.title.c_str(), nullptr, nullptr);
		Context=new zOpenGLContext(window);
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
					KeyPressEvent e{key,0};
					data->eventCall(e);
					break;
				}
				case GLFW_RELEASE:{
					KeyReleasedEvent e{key};
					data->eventCall(e);
					break;
				}
				case GLFW_REPEAT:{
					KeyPressEvent e{key,1};
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
					MouseButtonPressedEvent e{button};
					data->eventCall(e);
					break;
				}
				case GLFW_RELEASE:{
					MouseButtonReleasedEvent e{button};
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

	void WindowsWindow::Update() {
		glfwPollEvents();
		//ToDo: change to zGraphicContext.swapBuffers();have do
		Context->SwapBuffers();
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void WindowsWindow::SetVSync(bool enable) {
		if(enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		WinData.VSync=enable;

	}
	zWindow* zWindow::Create(const WindowProps &props) {
		return new WindowsWindow(props);
	}
}