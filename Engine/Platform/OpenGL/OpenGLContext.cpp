//
// Created by 32725 on 2023/3/14.
//
#include "Include/glad/include/glad/glad.h"

#include "Z/Core/Log.h"
#include "Z/Core/Core.h"
#include "Platform/OpenGL/OpenGLContext.h"
namespace  Z{

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(windowHandle);
		auto state = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		Z_CORE_ASSERT(state, "failed to init glad!!!");
	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(windowHandle);
	}

	OpenGLContext::OpenGLContext(GLFWwindow* window) : windowHandle(window){
		Z_CORE_ASSERT(windowHandle, "window handle is null!!!");
	}

	void OpenGLContext::Destroy() {
		Z_CORE_WARN("OpenGL Context Destroy!");
	}

	void OpenGLContext::PreInit() {

	}

	void OpenGLContext::DeviceSynchronize() {

	}
}