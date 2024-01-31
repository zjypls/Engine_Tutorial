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

	void ImGuiRendererVulkan::InitUIRenderBackend(RenderPassInterface *renderPassInterface,uint32 uiIndex) {


		//todo:add imgui init for render backend
		ImGui_ImplVulkan_InitInfo info{};
		info.Allocator=nullptr;
		auto gContext=(VulkanGraphicInterface*)RenderManager::GetInstance().get();
		info.Instance=gContext->GetInstance();
		info.Device=gContext->GetDevice();
		info.PhysicalDevice=gContext->GetPhysicalDevice();
		info.Queue=((VulkanQueue*)gContext->GetGraphicQueue())->Get();
		info.Subpass=uiIndex;
		auto&swapchaininfo=gContext->GetSwapChainInfo();
		info.ImageCount=swapchaininfo.imageCount;
		info.MinImageCount=swapchaininfo.minImageCount;
		info.DescriptorPool=gContext->GetDescriptorPool();
		auto tPass=((VulkanRenderPass*)renderPassInterface)->Get();

		ImGui_ImplVulkan_Init(&info,tPass);

		auto buffer = gContext->BeginOnceSubmit();
		ImGui_ImplVulkan_CreateFontsTexture(buffer);
		gContext->EndOnceSubmit(buffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}
