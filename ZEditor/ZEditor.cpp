//
// Created by 32725 on 2023/3/19.
//

#include "ZEditor.h"
#include"glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <random>

constexpr int mapWidth = 24;

constexpr const char *maps = {
		"WWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWDWWWWWWWWWWWWWWWWWW"
		"WWWWDWDWWWWWWWWDWWWWWWWW"
		"WWWWWDWWWWWWDDWDWWWWWWWW"
		"WWWWWWWWWWWWWDDWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWWWWWWWWWWWWWWWWWWW"
};
int mapHeight = strlen(maps) / mapWidth;

ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs) {
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}
ImVec2 operator/(const ImVec2 &lhs, const ImVec2 &rhs) {
	return {lhs.x / rhs.x, lhs.y / rhs.y};
}

namespace Z {
	EditorLayer::EditorLayer() : Layer("EditorLayer"), controller(1200.f / 800.0f) {

	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::OnAttach() {
		Z_CORE_INFO("Layer:{0} Attach!", GetName());
		float vertices[] = {
				-.5f, -.5f, .0f, 0, 0,
				.5f, -.5f, .0f, 1, 0,
				-.5f, .5f, .0f, 0, 1,
				.5f, .5f, .0f, 1, 1
		};

		uint32_t indices[] = {
				0, 1, 2,
				1, 3, 2
		};
		vertexArray = VertexArray::Create();
		texture[0] = Texture2D::CreateTexture("../Assets/Textures/Colum.png");
		texture[1] = Texture2D::CreateTexture("../Assets/Textures/Layla.jpg");
		texture[2] = Texture2D::CreateTexture("../Assets/Textures/Nahida.png");
		texture[3] = Texture2D::CreateTexture("../Assets/Sprites/rpgSheet.png");
		subTex = SubTex2D::Create(texture[3], glm::vec2{10, 10}, glm::vec2{128, 128}, {3, 3});
		textureMap['W'] = SubTex2D::Create(texture[3], glm::vec2{11, 11}, glm::vec2{128, 128}, {1, 1});
		textureMap['D'] = SubTex2D::Create(texture[3], glm::vec2{6, 11}, glm::vec2{128, 128}, {1, 1});
		auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		{
			auto layout = CreateScope<BufferLayout>(BufferLayout{
					{ShaderDataType::Float3, "position"},
					{ShaderDataType::Float2, "texCoord"}});
			vertexBuffer->SetLayout(*layout);
		}
		vertexArray->AddVertexBuffer(vertexBuffer);
		auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		vertexArray->SetIndexBuffer(indexBuffer);
		vertexArray->Unbind();
		shader = Shader::CreateShader("../Shaders/One.glsl");
		shader->Bind();
		shader->UnBind();
		grid = Shader::CreateShader("Grid", "../Shaders/vert.vert", "../Shaders/grid.frag", true);
		frameBuffer = FrameBuffer::Create({1200, 800});
	}

	void EditorLayer::OnDetach() {
	}

	void EditorLayer::OnUpdate() {
		frameBuffer->Bind();
		if (IsViewportFocused)
			controller.OnUpdate(Time::DeltaTime());
		RenderCommand::SetClearValue(clearValue);
		RenderCommand::Clear();

		Renderer2D::BeginScene(controller.GetCamera());


		for (int i = 0; i < mapWidth; i++) {
			for (int j = 0; j < mapHeight; j++) {
				auto c = maps[j * mapWidth + i];
				if (textureMap.count(c)) {
					Renderer2D::DrawQuad({i - mapWidth / 2, mapHeight / 2 - j}, {1, 1}, textureMap[c]);
				} else {
					Renderer2D::DrawQuad({i - mapWidth / 2, mapHeight / 2 - j}, {1, 1}, subTex);
				}
			}
		}


		if (IsViewportHovered&&IsViewportFocused&&Input::IsMouseButtonPressed(Z_MOUSE_BUTTON_4)) {
			auto size = controller.GetSize();
			auto pos = controller.GetCamera()->GetPosition();
			auto pos2 =(glm::vec2{CursorPos.x,CursorPos.y} + (Random::RandVec2() * .2f - .1f))*size;
			Z_Test::Particle::AddToPool({glm::vec3(pos2, 0.f)+pos,
			                             glm::vec3{Random::Float() / 3.f, 0.1, 0.}*glm::vec3{ size,0.f }, {0, 0, 0},
			                             {Random::RandVec3() * glm::vec3{1.f, 0.5f, 0.6f}, 1.}, 0.03f*controller.GetSize().y,
			                             0., (Random::Float() + 0.5f)});
		}
		Z_Test::Particle::OnUpdate(Time::DeltaTime());
		Z_Test::Particle::OnRender();
		Renderer2D::EndScene();
		frameBuffer->UnBind();
	}

	void EditorLayer::OnImGuiRender() {
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			                ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);


		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Camera Pos: %f,%f", controller.GetCamera()->GetPosition().x, controller.GetCamera()->GetPosition().y);
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %u", stats->DrawCalls);
		ImGui::Text("Quads: %u", stats->QuadCount);
		ImGui::Text("Vertices: %u", stats->GetTotalVertexCount());
		ImGui::Text("Indices: %u", stats->GetTotalIndexCount());
		stats->Reset();
		ImGui::End();
		ImGui::Begin("ViewPort");
		IsViewportFocused = ImGui::IsWindowFocused();
		IsViewportHovered = ImGui::IsWindowHovered();
		auto viewSize = ImGui::GetContentRegionAvail();
		if(IsViewportFocused&& IsViewportHovered&&viewportSize.y!=0&&viewportSize.x!=0){
			auto cursorPos = ImGui::GetMousePosOnOpeningCurrentPopup();
			auto TextPos=ImGui::GetWindowPos();
			cursorPos=cursorPos-TextPos;
			cursorPos=cursorPos/ImGui::GetWindowSize();
			CursorPos = {cursorPos.x-0.5f, 0.5f-cursorPos.y};
			CursorPos*=2.f;
		}
		Application::Get().GetImGuiLayer()->SetBlockEvents(!IsViewportFocused || !IsViewportHovered);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		if (viewportSize != *(glm::vec2 *) &viewSize) {
			viewportSize = glm::vec2{viewSize.x, viewSize.y};
			frameBuffer->Resize(viewportSize.x, viewportSize.y);
			controller.OnResize(viewportSize.x, viewportSize.y);
		}
		uint32_t textureID = frameBuffer->GetColorID();
		ImGui::Image((void *) textureID, viewSize, ImVec2{0, 1}, ImVec2{1, 0});
		ImGui::PopStyleVar();
		ImGui::End();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event &event) {
		controller.OnEvent(event);
	}

}