//
// Created by z on 2023/9/5.
//
#include "Z/ImGui/ImGuiRendererPlatform.h"
#include "Platform/Vulkan/ImGuiRendererVulkan.h"

namespace Z {
	Ref<ImGuiRendererPlatform> ImGuiRendererPlatform::renderer;
	void ImGuiRendererPlatform::Init() {
        renderer= CreateRef<ImGuiRendererVulkan>();
	}
}