//
// Created by 32725 on 2023/3/13.
//

#include "ImGuiLayer.h"
#include "Z/Core/Application.h"
#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"


#include "Platform/OpenGL/ImGuiOpenGLRenderDocking.h"


namespace Z {
	void ImGuiLayer::OnAttach() {

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO &io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      //Necessary for docking

		io.Fonts->AddFontFromFileTTF((std::string(Z_SOURCE_DIR)+"/Assets/fonts/Oxygen/Oxygen-Bold.ttf").c_str(), 18.0f);
		io.FontDefault=io.Fonts->AddFontFromFileTTF((std::string(Z_SOURCE_DIR)+"/Assets/fonts/Oxygen/Oxygen-Regular.ttf").c_str(), 18.0f);

		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendPlatformName = "imgui_impl_glfw";
		//io.SetAppAcceptingEvents(true);

		//set style
		//set platform and renderer
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *) Application::Get().GetWindow().GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach() {

	}



	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {

	}

	ImGuiLayer::~ImGuiLayer() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnImGuiRender() {

	}

	void ImGuiLayer::End() {
		auto& io=ImGui::GetIO();


		auto &app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}

	void ImGuiLayer::OnEvent(Event &event) {
		if (BlockEvents) {
			ImGuiIO &io = ImGui::GetIO();
			event.Handled |= event.isCategory(EventMouse) & io.WantCaptureMouse;
			event.Handled |= event.isCategory(EventKeyBoard) & io.WantCaptureKeyboard;
		}
	}
}