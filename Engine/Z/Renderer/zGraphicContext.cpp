//
// Created by 32725 on 2023/3/14.
//
#include "RenderAPI.h"
#include "zGraphicContext.h"
#include "Platform/OpenGL/zOpenGLContext.h"
#include "Platform/Vulkan/zVulkanContext.h"
namespace  Z{

	Scope<zGraphicContext> zGraphicContext::Create(void *window) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return CreateScope<zOpenGLContext>((GLFWwindow*)window);
			case RenderAPI::API::Vulkan:
				return CreateScope<zVulkanContext>((GLFWwindow*)window);
			default:
				return nullptr;
		}
	}
}