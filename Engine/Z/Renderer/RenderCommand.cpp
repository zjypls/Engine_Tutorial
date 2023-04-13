//
// Created by 32725 on 2023/3/17.
//

#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"

namespace Z {
	//Todo: Add vulkan support
	RenderAPI *RenderCommand::s_RenderAPI = new OpenGLRenderAPI();

	void RenderCommand::Init() {
		s_RenderAPI->Init();
	}
}