//
// Created by 32725 on 2023/4/10.
//

#include "zVulkanContext.h"
#include "vulkan/vulkan.hpp"

namespace Z {
	zVulkanContext::zVulkanContext(GLFWwindow *w):windowHandle(w) {
		Z_CORE_ASSERT(windowHandle, "window handle is null!!!");
	}

}
