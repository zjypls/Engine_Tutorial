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
        //fixme:
        // noticed that imgui call vkDestroyBuffer(vertexBuffer and indexBuffer) when the buffer is used for submitting which cause a validation error
        // on Platform/Vulkan/ImGuiVulkanRenderDocing.cpp line 535 and 537
        // it works well at the 0-maxFlightFrame-1 frames,and happens when the maxFlightFrame frame is drawing
        //mark (Arch) :validationLabelImGui (mark as a symbol for local search)
        //mark again (Windows) :seems only happened on my Arch with AMD integrated GPU
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),((VulkanGraphicInterface*)RenderManager::GetInstance().get())->GetCurrentCommandBuffer());
	}


	void ImGuiRendererVulkan::Shutdown() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiRendererVulkan::InitUIRenderBackend(RenderPassInterface *renderPassInterface,uint32 uiIndex) {
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
		info.MinImageCount=swapchaininfo.minImageCount>=2?swapchaininfo.minImageCount:2;
		info.DescriptorPool=gContext->GetDescriptorPool();
		auto uiPassInterface=((VulkanRenderPass*)renderPassInterface)->Get();

		bool res = ImGui_ImplVulkan_Init(&info,uiPassInterface);
        Z_CORE_ASSERT(res,"failed to init vulkan for imgui !");

		ImGui_ImplVulkan_CreateFontsTexture();
	}

    void ImGuiRendererVulkan::CreateTextureForImGui(Sampler *sampler, ImageView *imageView, ImageLayout layout , ImTextureID &set) {
        set=ImGui_ImplVulkan_AddTexture(((VulkanSampler*)sampler)->Get(),((VulkanImageView*)imageView)->Get(),(VkImageLayout)layout);
    }
}
