//
// Created by 32725 on 2023/3/17.
//
#include "RenderAPI.h"

namespace Z{
	//ToDo:optimize
	#ifdef Z_USE_OPENGL
	RenderAPI::API RenderAPI::api= RenderAPI::API::OpenGL;
	#elifdef Z_USE_VULKAN
	RenderAPI::API RenderAPI::api= RenderAPI::API::Vulkan;
	#else
	#warning "Platform Micro Must Should Be Defined"
	RenderAPI::API RenderAPI::api= RenderAPI::API::None;
	#endif
}