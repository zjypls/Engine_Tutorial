//
// Created by z on 2023/9/5.
//
#include "Z/Core/Log.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"

#include "Platform/Vulkan/ImGuiRendererVulkan.h"
namespace Z {
	Ref<ImGuiRendererPlatform> ImGuiRendererPlatform::renderer=nullptr;
	void ImGuiRendererPlatform::Init() {
#ifdef Z_USE_VULKAN
        renderer= CreateRef<ImGuiRendererVulkan>();
#endif


		Z_CORE_ASSERT(renderer!=nullptr,"ImGui Renderer Need Init !!");
	}
}
