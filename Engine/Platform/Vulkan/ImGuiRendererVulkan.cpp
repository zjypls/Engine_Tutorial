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

		ImGui_ImplVulkan_InitInfo initInfo{};
        VulkanGraphicInterface* gContext=(VulkanGraphicInterface*)RenderManager::GetInstance().get();
        initInfo.Instance=gContext->GetInstance();
        initInfo.Device=gContext->GetDevice();
        initInfo.PhysicalDevice=gContext->GetPhysicalDevice();
        initInfo.DescriptorPool=gContext->GetDescriptorPool();
        initInfo.Queue=((VulkanQueue*)gContext->GetGraphicQueue())->Get();
        initInfo.QueueFamily=gContext->GetQueueFamily().graphics.value();
        static_assert(false);//not complete yet
		ImGui_ImplVulkan_Init(&initInfo,{});
	}

	void ImGuiRendererVulkan::Begin() {

	}

	void ImGuiRendererVulkan::End() {

	}


	void ImGuiRendererVulkan::Shutdown() {

	}

}
