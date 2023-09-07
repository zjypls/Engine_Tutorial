//
// Created by z on 2023/9/5.
//
#include "ImGuiRendererPlatform.h"
#include "Z/Renderer/RenderAPI.h"
#include "Platform/OpenGL/ImGuiRendererOpenGL.h"
#include "Platform/Vulkan/ImGuiRendererVulkan.h"

namespace Z {
	Ref<ImGuiRendererPlatform> ImGuiRendererPlatform::renderer;
	void ImGuiRendererPlatform::Init() {
		switch (RenderAPI::GetAPI()) {
			#define zCASE_T(str) case RenderAPI::API::str : renderer = CreateRef<ImGuiRenderer##str>();break
			zCASE_T(OpenGL);
			zCASE_T(Vulkan);
			#undef zCASE_T
			default:
				Z_CORE_ERROR(RenderAPI::GetApiStr()+":API Unsupported Now!!!");
				Z_CORE_ASSERT(false, "Unsupported API");
				break;
		}
	}
}