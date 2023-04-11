//
// Created by 32725 on 2023/4/10.
//

#ifndef ENGINE_TUTORIAL_ZVULKANRENDERAPI_H
#define ENGINE_TUTORIAL_ZVULKANRENDERAPI_H
#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Z/Renderer/zGraphicContext.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.hpp"
namespace Z {
	class zVulkanContext : public zGraphicContext {
		GLFWwindow* windowHandle;
		vk::Instance instance;
		vk::PhysicalDevice physicalDevice;
	public:
		virtual void Init()override;
		zVulkanContext(GLFWwindow*w);

		virtual void SwapBuffers()override;
	};

}

#endif //ENGINE_TUTORIAL_ZVULKANRENDERAPI_H
