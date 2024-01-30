//
// Created by z on 2023/9/5.
//

#include "Include/imgui/imgui.h"
#include "Include/imgui/backends/imgui_impl_glfw.h"

#include "Z/Core/Application.h"
#include "Z/Renderer/RenderManager.h"

#include "Platform/Vulkan/ImGuiRendererVulkan.h"
#include "Platform/Vulkan/ImGuiVulkanRenderDocking.h"
#include "Platform/Vulkan/VulkanGraphicInterface.h"

namespace Z {
	static ImGuiIO* zImGuiGlobalIO;

	void ImGuiRendererVulkan::PlatformInit() {
		zImGuiGlobalIO=&ImGui::GetIO();
		zImGuiGlobalIO->BackendPlatformName="imgui_impl_glfw";

		ImGui_ImplGlfw_InitForVulkan((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow(), true);
	}

	void ImGuiRendererVulkan::Begin() {
		ImGui_ImplVulkan_NewFrame();
	}

	void ImGuiRendererVulkan::End() {
		auto data=ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(data,((VulkanGraphicInterface*)RenderManager::GetInstance().get())->GetCurrentCommandBuffer());
	}


	void ImGuiRendererVulkan::Shutdown() {
		ImGui::DestroyContext();
	}

}
