//
// Created by 32725 on 2023/3/19.
//

#include "ZEditor.h"
#include"glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <chrono>


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
				-.5f, -.5f, .0f, 0, 0, .5f, -.5f, .0f, 1, 0,
				-.5f, .5f, .0f, 0, 1, .5f, .5f, .0f, 1, 1};
		uint32_t indices[] = {0, 1, 2, 1, 3, 2};
		vertexArray = VertexArray::Create();
		texture[0] = Texture2D::CreateTexture(std::string(Z_SOURCE_DIR) + "/Assets/Textures/Colum.png");
		texture[1] = Texture2D::CreateTexture(std::string(Z_SOURCE_DIR) + "/Assets/Textures/Layla.jpg");
		texture[2] = Texture2D::CreateTexture(std::string(Z_SOURCE_DIR) + "/Assets/Textures/Nahida.png");
		texture[3] = Texture2D::CreateTexture(std::string(Z_SOURCE_DIR) + "/Assets/Sprites/rpgSheet.png");
		subTex = SubTex2D::Create(texture[3], glm::vec2{10, 10}, glm::vec2{128, 128}, glm::vec2{3, 3});
		textureMap['W'] = SubTex2D::Create(texture[3], glm::vec2{11, 11}, glm::vec2{128, 128}, glm::vec2{1, 1});
		textureMap['D'] = SubTex2D::Create(texture[3], glm::vec2{6, 11}, glm::vec2{128, 128}, glm::vec2{1, 1});
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
		shader = Shader::CreateShader(std::string(Z_SOURCE_DIR) + "/Shaders/One.glsl");
		shader->Bind();
		shader->UnBind();
		grid = Shader::CreateShader("Grid", std::string(Z_SOURCE_DIR) + "/Shaders/vert.vert",
		                            std::string(Z_SOURCE_DIR) + "/Shaders/grid.frag", true);
		frameBuffer = FrameBuffer::Create({1200, 800});
		scene = CreateRef<Scene>();
		cameraEntity = scene->CreateEntity("Camera");
		cameraEntity.AddComponent<CameraComponent>(glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f));
		SecondCamera = scene->CreateEntity("SecondCamera");
		SecondCamera.AddComponent<CameraComponent>(glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f));
		SecondCamera.GetComponent<CameraComponent>().primary = false;
		entity = scene->CreateEntity("Square");
		entity.AddComponent<SpriteRendererComponent>(glm::vec4{1, 0, 0, 1});
		auto newSquare = scene->CreateEntity("NewSquare");
		newSquare.AddComponent<SpriteRendererComponent>(glm::vec4{0, 1, 0, 1});


		class CameraCtrl : public ScriptEntity {
		public:
			void OnCreate() {
				Z_CORE_INFO("{0}:CameraCtrl OnCreate", entity.GetComponent<TagComponent>().tag.c_str());
			}

			void OnDestroy() {
				Z_CORE_INFO("CameraCtrl OnDestroy");
			}

			void OnUpdate(float deltaTime) {
				auto pri = entity.GetComponent<CameraComponent>().primary;
				if (!pri)
					return;
				auto &transform = entity.GetComponent<TransformComponent>().translation;
				constexpr float speed = 6.f;
				if (Input::IsKeyPressed(KeyCode::W))
					transform[1] += speed * deltaTime;
				else if (Input::IsKeyPressed(KeyCode::S))
					transform[1] -= speed * deltaTime;
				if (Input::IsKeyPressed(KeyCode::A))
					transform[0] -= speed * deltaTime;
				else if (Input::IsKeyPressed(KeyCode::D))
					transform[0] += speed * deltaTime;
			}
		};

		cameraEntity.AddComponent<ScriptComponent>().Bind<CameraCtrl>();
		SecondCamera.AddComponent<ScriptComponent>().Bind<CameraCtrl>();

		sceneHierarchyPlane = CreateRef<SceneHierarchyPlane>(scene);

	}

	void EditorLayer::OnDetach() {
	}

	void EditorLayer::OnUpdate() {
		frameBuffer->Bind();
		if (IsViewportFocused)
			controller.OnUpdate(Time::DeltaTime());
		RenderCommand::SetClearValue(clearValue);
		RenderCommand::Clear();

		scene->OnUpdate(Time::DeltaTime());

		Renderer2D::BeginScene(controller.GetCamera());
		if (IsViewportHovered && IsViewportFocused && Input::IsMouseButtonPressed(Z_MOUSE_BUTTON_4)) {
			auto size = controller.GetSize();
			auto pos = controller.GetCamera()->GetPosition();
			auto pos2 = (glm::vec2{CursorPos.x, CursorPos.y} + (Random::RandVec2() * .2f - .1f)) * size;
			Z::Particle::AddToPool({glm::vec3(pos2, 0.f) + pos,
			                        glm::vec3{Random::Float() / 3.f, 0.1, 0.} * glm::vec3{size, 0.f},
			                        glm::vec3{0, 0, 0},
			                        glm::vec4{Random::RandVec3() * glm::vec3{1.f, 0.5f, 0.6f}, 1.},
			                        0.03f * controller.GetSize().y,
			                        0., (Random::Float() + 0.5f)});
		}
		Z::Particle::OnRender();
		if (Z::Particle::GetCurrentNum() > 0)
			Renderer2D::EndScene();
		Z::Particle::OnUpdate(Time::DeltaTime());
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
		auto& style = ImGui::GetStyle();
		auto miniSize=style.WindowMinSize.x;
		style.WindowMinSize.x=350;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x=miniSize;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Statics");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %u", stats->DrawCalls);
		ImGui::Text("Quads: %u", stats->QuadCount);
		ImGui::Text("Vertices: %u", stats->GetTotalVertexCount());
		ImGui::Text("Indices: %u", stats->GetTotalIndexCount());
		stats->Reset();

		sceneHierarchyPlane->OnImGuiRender();

		ImGui::End();
		ImGui::Begin("ViewPort");
		IsViewportFocused = ImGui::IsWindowFocused();
		IsViewportHovered = ImGui::IsWindowHovered();
		auto viewSize = ImGui::GetContentRegionAvail();
		if (IsViewportFocused && IsViewportHovered && viewportSize.y != 0 && viewportSize.x != 0) {
			auto cursorPos = ImGui::GetMousePosOnOpeningCurrentPopup();
			auto TextPos = ImGui::GetWindowPos();
			cursorPos = cursorPos - TextPos;
			cursorPos = cursorPos / ImGui::GetWindowSize();
			CursorPos = glm::vec2{cursorPos.x - 0.5f, 0.5f - cursorPos.y};
			CursorPos *= 2.f;
		}
		Application::Get().GetImGuiLayer()->SetBlockEvents(!IsViewportFocused || !IsViewportHovered);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		if (viewportSize != *(glm::vec2 *) &viewSize) {
			viewportSize = glm::vec2{viewSize.x, viewSize.y};
			frameBuffer->Resize(viewportSize.x, viewportSize.y);
			scene->OnViewportResize(viewportSize.x, viewportSize.y);
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
		EventDispatcher dispatcher(event);
		dispatcher.Handle<KeyPressEvent>([&](KeyPressEvent &e) {
			if (KeyCode(e.GetKey()) == Key::Escape)
				Application::Get().Close();
			return false;
		});
	}

}