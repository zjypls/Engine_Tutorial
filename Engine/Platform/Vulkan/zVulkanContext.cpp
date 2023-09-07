//
// Created by 32725 on 2023/4/10.
//

#include "zVulkanContext.h"

#ifdef Z_ENABLE_VK_VALIDATION
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif


namespace Z {
	zVulkanContext::zVulkanContext(GLFWwindow *w) : windowHandle(w) {
		Z_CORE_ASSERT(windowHandle, "window handle is null!!!");

	}

	void zVulkanContext::Init() {

	}

	void zVulkanContext::SwapBuffers() {

	}

	void zVulkanContext::Destroy() {


		vkDestroyDevice(device,allocator);

		vkDestroyInstance(instance,allocator);

	}

	void zVulkanContext::PreInit() {
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
	}

}
