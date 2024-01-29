//
// Created by 32725 on 2023/4/10.
//

#include "Platform/Vulkan/VulkanContext.h"

#ifdef Z_ENABLE_VK_VALIDATION
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif


namespace Z {
	VulkanContext::VulkanContext(GLFWwindow *w) : windowHandle(w) {
		Z_CORE_ASSERT(windowHandle!= nullptr, "window handle is null!!!");

	}

	void VulkanContext::Init() {

	}

	void VulkanContext::SwapBuffers() {

	}

	void VulkanContext::Destroy() {


		vkDestroyDevice(device,allocator);

		vkDestroyInstance(instance,allocator);

	}

	void VulkanContext::PreInit() {
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
	}

	void VulkanContext::DeviceSynchronize() {
		vkDeviceWaitIdle(device);
	}

}
