//
// Created by 32725 on 2023/3/14.
//
#include "Z/Core/Log.h"
#include "Z/Core/Core.h"
#include "zOpenGLContext.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
namespace  Z{

	void zOpenGLContext::Init() {
		glfwMakeContextCurrent(windowHandle);
		auto state = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		Z_CORE_ASSERT(state, "failed to init glad!!!");
	}

	void zOpenGLContext::SwapBuffers() {
		glfwSwapBuffers(windowHandle);
	}

	zOpenGLContext::zOpenGLContext(GLFWwindow* window) :windowHandle(window){
		Z_CORE_ASSERT(windowHandle, "window handle is null!!!");
	}

	void zOpenGLContext::Destroy() {

	}

	void zOpenGLContext::PreInit() {

	}
}