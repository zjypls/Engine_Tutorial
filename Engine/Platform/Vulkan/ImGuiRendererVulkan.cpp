//
// Created by z on 2023/9/5.
//

#include "Include/imgui/imgui.h"
#include "Include/imgui/backends/imgui_impl_glfw.h"

#include "Z/Core/Application.h"
#include "Platform/Vulkan/ImGuiRendererVulkan.h"
#include "Platform/Vulkan/ImGuiVulkanRenderDocking.h"

namespace Z {
	static ImGuiIO* zImGuiGlobalIO;

	void ImGuiRendererVulkan::PlatformInit() {
		zImGuiGlobalIO=&ImGui::GetIO();
		zImGuiGlobalIO->BackendPlatformName="imgui_impl_glfw";

		ImGui_ImplGlfw_InitForVulkan((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow(), true);

		ImGui_ImplVulkan_InitInfo initInfo{};
		ImGui_ImplVulkan_Init(&initInfo,{});
	}

	void ImGuiRendererVulkan::Begin() {

	}

	void ImGuiRendererVulkan::End() {

	}


	void ImGuiRendererVulkan::Shutdown() {

	}

}
