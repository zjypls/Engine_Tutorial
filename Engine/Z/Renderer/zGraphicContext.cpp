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
				//Todo: Add vulkan support
				return CreateScope<zVulkanContext>((GLFWwindow*)window);
			default:
				return nullptr;
		}
	}

	void zGraphicContext::PreInitForRenderAPI() {
		switch(RenderAPI::GetAPI()){
			case RenderAPI::API::OpenGL:
				zOpenGLContext::PreInit();
				break;
			case RenderAPI::API::Vulkan:
				zVulkanContext::PreInit();
				break;
			default:
				Z_CORE_ERROR("Error RenderAPI");
				Z_CORE_ASSERT(false,"Unknown API");
				break;
		}
		return;
	}
}