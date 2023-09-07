//
// Created by z on 2023/9/5.
//

#include "./ImGuiRendererOpenGL.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "Z/Core/Application.h"
namespace Z {
	static ImGuiIO* zImGuiGlobalIO;


	void ImGuiRendererOpenGL::PlatformInit() {
		zImGuiGlobalIO=&ImGui::GetIO();
		zImGuiGlobalIO->BackendPlatformName = "imgui_impl_glfw";
		//io.SetAppAcceptingEvents(true);

		//set platform and renderer
		//Todo : move to platform
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}
	//call per-frame before use imgui
	void ImGuiRendererOpenGL::Begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	//call per-frame after use imgui
	void ImGuiRendererOpenGL::End() {
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		if (zImGuiGlobalIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}


	void ImGuiRendererOpenGL::Shutdown() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

}