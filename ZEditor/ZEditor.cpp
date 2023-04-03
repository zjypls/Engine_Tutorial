//
// Created by 32725 on 2023/3/19.
//
#include "Z/Core/Core.h"
#include "ZEditor.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "Z/Scene/SceneSerializer.h"
#include "Z/Utils/ZUtils.h"
#include "ImGuizmo.h"


ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs) {
	return {lhs.x - rhs.x, lhs.y - rhs.y};
}

ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs) {
	return {lhs.x + rhs.x, lhs.y + rhs.y};
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
//		shader = Shader::CreateShader(std::string(Z_SOURCE_DIR) + "/Shaders/One.glsl");
//		shader->Bind();
//		shader->UnBind();
//		grid = Shader::CreateShader("Grid", std::string(Z_SOURCE_DIR) + "/Shaders/vert.vert",
//		                            std::string(Z_SOURCE_DIR) + "/Shaders/grid.frag", true);
		FrameBufferSpecification spec;
		spec.width = 1200;
		spec.height = 800;
		spec.attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::R32I,
		                    FrameBufferTextureFormat::DEPTH};
		frameBuffer = FrameBuffer::Create(spec);
		scene = CreateRef<Scene>();
		editorCamera = Z::EditorCamera(45.f, 1.f, 0.1f, 1000.f);
		auto nahida = scene->CreateEntity("Nahida");
		auto &tex = nahida.AddComponent<SpriteRendererComponent>();
		tex.texture = texture[2];
		auto _sceneCamera = scene->CreateEntity("SceneCamera");
		auto &_camera = _sceneCamera.AddComponent<CameraComponent>();
		_camera.camera.OnViewportResize(1200, 800);
		_camera.camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
		auto&_Transform=_sceneCamera.GetComponent<TransformComponent>();
		_Transform.translation={0,0,3};
		playButtonIcon = Texture2D::CreateTexture(std::string(Z_SOURCE_DIR) + "/Assets/Icons/PlayButton.png");
		stopButtonIcon = Texture2D::CreateTexture(std::string(Z_SOURCE_DIR) + "/Assets/Icons/StopButton.png");
		currentButtonIcon = playButtonIcon;
/*
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
*/

		sceneHierarchyPlane = CreateRef<SceneHierarchyPlane>(scene);
		contentBrowser = CreateRef<ContentBrowser>();

	}

	void EditorLayer::OnDetach() {
	}

	void EditorLayer::OnUpdate() {
		frameBuffer->Bind();

		RenderCommand::SetClearValue(clearValue);
		RenderCommand::Clear();

		frameBuffer->ClearAttachment(1, -1);

		switch (sceneState) {
			case SceneState::Edit:{
				if (IsViewportFocused) {
					controller.OnUpdate(Time::DeltaTime());
					editorCamera.OnUpdate();
				}
				scene->OnEditorUpdate(Time::DeltaTime(), editorCamera);
				break;
			}
			case SceneState::Play:{
				scene->OnUpdate(Time::DeltaTime());
				break;
			}
		}


		if (IsViewportFocused && IsViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyCode::LeftAlt) &&
		    Input::IsMouseButtonPressed(MouseCode::ButtonLeft)) {
			auto value = frameBuffer->GetPixel(CursorPos.x, CursorPos.y);
			sceneHierarchyPlane->SetSelectedEntity(value);
		}
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
		auto &style = ImGui::GetStyle();
		auto miniSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 350;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = miniSize;


		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("New", "Ctrl+N")) {
					NewScene();
				}

				if (ImGui::MenuItem("Save", "Ctrl+Shift+S")) {
					SaveScene();
				}
				if (ImGui::MenuItem("Load", "Ctrl+O")) {
					LoadScene();
				}
				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		static bool showID = false;
		ImGui::Begin("Statics");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %u", stats->DrawCalls);
		ImGui::Text("Quads: %u", stats->QuadCount);
		ImGui::Text("Vertices: %u", stats->GetTotalVertexCount());
		ImGui::Text("Indices: %u", stats->GetTotalIndexCount());
		stats->Reset();
		ImGui::Checkbox("Show ID", &showID);

		sceneHierarchyPlane->OnImGuiRender();
		contentBrowser->OnImGuiRender();

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("ViewPort");
		IsViewportFocused = ImGui::IsWindowFocused();
		IsViewportHovered = ImGui::IsWindowHovered();
		auto viewSize = ImGui::GetContentRegionAvail();
		if (IsViewportFocused && IsViewportHovered && viewportSize.y != 0 && viewportSize.x != 0 &&
		    Z::Input::IsMouseButtonPressed(Z::MouseCode::ButtonLeft)) {

			auto cursorPos = ImGui::GetMousePos();
			auto offset = ImGui::GetWindowPos();
			auto WinSize = ImGui::GetWindowSize();
			auto MaxSize = WinSize + offset;
			auto McursorPos = MaxSize - cursorPos;
			CursorPos = glm::vec2{cursorPos.x - offset.x, McursorPos.y};
		}
		Application::Get().GetImGuiLayer()->SetBlockEvents(!IsViewportFocused && !IsViewportHovered);

		uint32_t textureID = showID ? frameBuffer->GetAttachmentID(1) : frameBuffer->GetAttachmentID(0);
		ImGui::Image((void *) textureID, viewSize, ImVec2{0, 1}, ImVec2{1, 0});

		if ((viewportSize != *(glm::vec2 *) &viewSize)&&!Input::IsMouseButtonPressed(MouseCode::ButtonLeft)) {
			viewportSize = glm::vec2{viewSize.x, viewSize.y};
			frameBuffer->Resize(viewportSize.x, viewportSize.y);
			scene->OnViewportResize(viewportSize.x, viewportSize.y);
			editorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
			controller.OnResize(viewportSize.x, viewportSize.y);
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const char *path = (const char *) payload->Data;
				LoadScene(path);
			}
		}

		auto selectedEntity = sceneHierarchyPlane->GetSelectedEntity();
		if (selectedEntity && currentGizmoOperation != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x,
			                  ImGui::GetWindowSize().y);
			auto cameraProjection = editorCamera.GetViewMatrix();

			auto &selectTransform = selectedEntity.GetComponent<TransformComponent>();
			auto Transform = selectTransform.GetTransform();
			ImGuizmo::Manipulate(glm::value_ptr(cameraProjection), glm::value_ptr(editorCamera.GetProjectionMatrix()),
			                     (ImGuizmo::OPERATION) currentGizmoOperation, ImGuizmo::MODE::LOCAL,
			                     glm::value_ptr(Transform));
			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, scale, skew;
				glm::quat rotation;
				glm::vec4 perspective;
				glm::decompose(Transform, scale, rotation, translation, skew, perspective);
				selectTransform.translation = translation;
				selectTransform.rotation = (glm::eulerAngles(rotation));
				selectTransform.scale = scale;
			}
		}

		ImGui::PopStyleVar();
		ImGui::End();
		On_UI();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event &event) {
		controller.OnEvent(event);
		if (IsViewportHovered) {
			editorCamera.OnEvent(event);
		}
		EventDispatcher dispatcher(event);
		dispatcher.Handle<KeyPressEvent>(Z_BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressEvent &event) {
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (event.GetKey()) {
			case Key::N:
				if (control) NewScene();
				break;
			case Key::O:
				if (control) LoadScene();
				break;
			case Key::S:
				if (control && shift) SaveScene();
				break;
			case Key::Q:
				currentGizmoOperation = -1;
				break;
			case Key::W:
				currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				break;
			case Key::R:
				currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
				break;
		}
		return false;
	}

	void EditorLayer::SaveScene() {
		auto path = Z::Utils::FileSave("*.zscene");
		if (!path.empty()) {
			SceneSerializer serializer(scene);
			serializer.Serialize(Z::Utils::FileSave("*.zscene"));
		}
	}

	void EditorLayer::LoadScene() {
		auto path = Z::Utils::FileOpen("*.zscene");
		if (!path.empty()) {
			LoadScene(path);
		}
	}

	void EditorLayer::LoadScene(const std::string &path) {
		scene = CreateRef<Scene>();
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetContext(scene);
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
	}

	void EditorLayer::NewScene() {
		scene = CreateRef<Scene>();
		sceneHierarchyPlane->SetContext(scene);
	}

	void EditorLayer::On_UI() {
		ImGui::Begin("##tools", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
		                                   ImGuiWindowFlags_NoScrollWithMouse);
		float size=ImGui::GetWindowHeight()-4;
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x/2-ImGui::GetWindowHeight()/2);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{0, 0});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 2});
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0, 0});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3, 0.3, 0.3, 0.5});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0, 0, 0, 0});
		if (ImGui::ImageButton((ImTextureID) currentButtonIcon->GetRendererID(), ImVec2{size, size}, ImVec2{0, 1},
		                       ImVec2{1, 0})) {
			if (sceneState == SceneState::Edit) {
				OnPlay();
			}else if(sceneState == SceneState::Play){
				OnStop();
			}
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
		ImGui::End();
	}

	void EditorLayer::OnPlay() {
		sceneState = SceneState::Play;
		currentButtonIcon = stopButtonIcon;
	}

	void EditorLayer::OnStop() {
		sceneState = SceneState::Edit;
		currentButtonIcon = playButtonIcon;
	}


}