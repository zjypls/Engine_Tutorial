//
// Created by 32725 on 2023/3/17.
//

#include "Z/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"
#include "Platform/Vulkan/VulkanRenderAPI.h"

namespace Z {
	//Todo: Add vulkan support
	#ifdef Z_USE_OPENGL
	Scope<RenderAPI> RenderCommand::s_RenderAPI = CreateScope<OpenGLRenderAPI>();
	#else
	Scope<RenderAPI> RenderCommand::s_RenderAPI = CreateScope<VulkanRenderAPI>();
	#endif
	void RenderCommand::Init() {
		switch(RenderAPI::GetAPI()){
			#define tCase(api) case Z::RenderAPI::API::api: s_RenderAPI = CreateScope< api##RenderAPI>();break
			tCase(OpenGL);
			tCase(Vulkan);
			#undef tCase
		}
		s_RenderAPI->Init();
	}
}