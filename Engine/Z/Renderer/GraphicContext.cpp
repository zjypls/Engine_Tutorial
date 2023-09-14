//
// Created by 32725 on 2023/3/14.
//
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

#include "Z/Renderer/RenderAPI.h"
#include "Z/Renderer/GraphicContext.h"
namespace  Z{

	Scope<GraphicContext> GraphicContext::Create(void *window) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return CreateScope<OpenGLContext>((GLFWwindow*)window);
			case RenderAPI::API::Vulkan:
				//Todo: Add vulkan support
				return CreateScope<VulkanContext>((GLFWwindow*)window);
			default:
				return nullptr;
		}
	}

	void GraphicContext::PreInitForRenderAPI() {
		switch(RenderAPI::GetAPI()){
			case RenderAPI::API::OpenGL:
				OpenGLContext::PreInit();
				break;
			case RenderAPI::API::Vulkan:
				VulkanContext::PreInit();
				break;
			default:
				Z_CORE_ERROR("Error RenderAPI");
				Z_CORE_ASSERT(false,"Unknown API");
				break;
		}
		return;
	}
}