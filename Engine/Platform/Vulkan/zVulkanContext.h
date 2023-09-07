//
// Created by 32725 on 2023/4/10.
//

#ifndef ENGINE_TUTORIAL_ZVULKANRENDERAPI_H
#define ENGINE_TUTORIAL_ZVULKANRENDERAPI_H
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Z/Renderer/zGraphicContext.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"
namespace Z {
	class zVulkanContext final : public zGraphicContext {
		GLFWwindow* windowHandle;
		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkAllocationCallbacks* allocator= nullptr;
	public:
		zVulkanContext(GLFWwindow*w);
		static void PreInit();
		void Init()override;

		void SwapBuffers()override;
		void Destroy()override;
	};

}

#endif //ENGINE_TUTORIAL_ZVULKANRENDERAPI_H
