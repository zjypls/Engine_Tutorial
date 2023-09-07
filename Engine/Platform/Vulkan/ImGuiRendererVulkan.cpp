//
// Created by z on 2023/9/5.
//

#include "./ImGuiRendererVulkan.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_vulkan.h"

#include "Z/Core/Application.h"

namespace Z {
	static ImGuiIO* zImGuiGlobalIO;

	void ImGuiRendererVulkan::PlatformInit() {
		zImGuiGlobalIO=&ImGui::GetIO();
		zImGuiGlobalIO->BackendPlatformName="imgui_impl_glfw";

		ImGui_ImplGlfw_InitForVulkan((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow(), true);

	}

	void ImGuiRendererVulkan::Begin() {

	}

	void ImGuiRendererVulkan::End() {

	}


	void ImGuiRendererVulkan::Shutdown() {

	}

}
