//
// Created by z on 2023/9/5.
//

#include "ImGuiRendererOpenGL.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "Z/Core/Application.h"
namespace Z {
	static ImGuiIO* zImGuiGlobalIO;
	void ImGuiRendererOpenGL::End() {
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		if (zImGuiGlobalIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiRendererOpenGL::Begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiRendererOpenGL::PlatformInit() {
		zImGuiGlobalIO=&ImGui::GetIO();
		zImGuiGlobalIO->BackendPlatformName = "imgui_impl_glfw";
		//io.SetAppAcceptingEvents(true);

		//set platform and renderer
		//Todo : move to platform
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void ImGuiRendererOpenGL::Shutdown() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

}