//
// Created by 32725 on 2023/3/19.
//
#include "ZEditor.h"
#include "Z/Scene/SceneSerializer.h"
#include "Z/Utils/ZUtils.h"
#include "Z/Script/ScriptEngine.h"
#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "filewatch/filewatch.h"
#include "Z/Project/Project.h"


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
	EditorLayer::EditorLayer() : Layer("EditorLayer") {

	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::OnAttach() {
		Z_CORE_INFO("Layer:{0} Attach!", GetName());
		std::filesystem::path ProjectPath = Z::Utils::FileOpen("*.zPrj");
		if(!Project::Init(ProjectPath)){
			Z_CORE_ASSERT(false,"Project Init Failed!");
			return;
		}
		//Sample Code /Todo:clean
		/*
		//Todo:clean
		float vertices[] = {
				-.5f, -.5f, .0f, 0, 0, .5f, -.5f, .0f, 1, 0,
				-.5f, .5f, .0f, 0, 1, .5f, .5f, .0f, 1, 1};
		uint32_t indices[] = {0, 1, 2, 1, 3, 2};
		vertexArray = VertexArray::Create();
		texture[0] = Texture2D::CreateTexture("Assets/Textures/Colum.png");
		texture[1] = Texture2D::CreateTexture("Assets/Textures/Layla.jpg");
		texture[2] = Texture2D::CreateTexture("Assets/Textures/Nahida.png");
		texture[3] = Texture2D::CreateTexture("Assets/Sprites/rpgSheet.png");
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
		*/

		FrameBufferSpecification spec;
		spec.width = 1200;
		spec.height = 800;
		spec.attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::R32I,
		                    FrameBufferTextureFormat::DEPTH};
		frameBuffer = FrameBuffer::Create(spec);
		spec.width = spec.width / 5.f;
		spec.height = spec.height / 5.f;
		spec.attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::DEPTH};
		previewFrame = FrameBuffer::Create(spec);
		scene = CreateRef<Scene>();
		editorCamera = Z::EditorCamera(45.f, 1.f, 0.1f, 1000.f);
		//Todo:remove this
//		auto nahida = scene->CreateEntity("Nahida");
//		auto &tex = nahida.AddComponent<SpriteRendererComponent>();
//		tex.texture = texture[2];
//		auto _sceneCamera = scene->CreateEntity("SceneCamera");
//		auto &_camera = _sceneCamera.AddComponent<CameraComponent>();
//		_camera.camera.OnViewportResize(1200, 800);
//		_camera.camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
//		auto &_Transform = _sceneCamera.GetComponent<TransformComponent>();
//		_Transform.translation = {0, 0, 3};
		//Todo:optimize with a project system
		playButtonIcon = Texture2D::CreateTexture("Assets/Icons/PlayButton.png");
		stopButtonIcon = Texture2D::CreateTexture("Assets/Icons/StopButton.png");
		simulateButtonIcon = Texture2D::CreateTexture("Assets/Icons/SimulateButton.png");
		pauseButtonIcon = Texture2D::CreateTexture("Assets/Icons/PauseButton.png");
		stepButtonIcon = Texture2D::CreateTexture("Assets/Icons/StepButton.png");
		toolButtons[0] = playButtonIcon;
		toolButtons[1] = simulateButtonIcon;
		toolButtons[2] = stepButtonIcon;
		/*
		//a native C++  script
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

*/

		sceneHierarchyPlane = CreateScope<SceneHierarchyPlane>(scene);
		contentBrowser = CreateScope<ContentBrowser>();
		//ScriptEngine::Init();
		//Todo:change this to a better way
		ScriptEngine::LoadAssembly("Bin-C/MSVC/scripts.dll");
		LoadScene(Project::GetProjectRootDir()/Project::GetStartScene());
		//Todo:remove this test code
//		auto* watch= new filewatch::FileWatch<std::string >("Bin-C/MSVC/scripts.dll", [this](const std::string &str, auto action) {
//			if(action==filewatch::Event::modified){
//				Z_CORE_INFO(str);
//			}
//		});
		//ScriptEngine::ShutDown();

	}

	void EditorLayer::OnDetach() {
		Project::Save();
	}

	void EditorLayer::OnUpdate() {
		//Todo : change this to a better way
		if (ScriptEngine::HasReLoadApp()) {
			ScriptEngine::ReCreateFields(scene);
			ScriptEngine::SetReLoadApp(false);
		}
		frameBuffer->Bind();

		RenderCommand::SetClearValue(clearValue);
		RenderCommand::Clear();

		frameBuffer->ClearAttachment(1, -1);

		{
			SceneState state = sceneState;
			if (state == SceneState::Pause)
				state = BackState;
			switch (state) {
				case SceneState::Edit: {
					if (IsViewportFocused && IsViewportHovered) {
						editorCamera.OnUpdate();
					}
					scene->OnEditorUpdate(Time::DeltaTime(), editorCamera);
					break;
				}
				case SceneState::Play: {
					scene->OnUpdate(Time::DeltaTime());
					break;
				}
				case SceneState::Simulate: {
					if (IsViewportFocused && IsViewportHovered) {
						editorCamera.OnUpdate();
					}
					scene->OnSimulateUpdate(Time::DeltaTime(), editorCamera);
					break;
				}
			}
			OnDebugShow();
		}
		if (selectedEntity = sceneHierarchyPlane->GetSelectedEntity();
				(sceneState == SceneState::Edit && IsViewportFocused && IsViewportHovered && !ImGuizmo::IsOver()) &&
				((selectedEntity && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) ||
				 (!selectedEntity && Input::IsMouseButtonPressed(MouseCode::ButtonLeft)))) {

			auto value = frameBuffer->GetPixel(CursorPos.x, CursorPos.y);
			selectedEntity = sceneHierarchyPlane->SetSelectedEntity(value);
		}
		frameBuffer->UnBind();
		if ((sceneState != SceneState::Play) && selectedEntity && selectedEntity.HasComponent<CameraComponent>()) {
			previewFrame->Bind();
			RenderCommand::SetClearValue(clearValue);
			RenderCommand::Clear();
			scene->OnPreviewUpdate(Time::DeltaTime(), selectedEntity.GetComponent<CameraComponent>().camera,
			                       selectedEntity.GetComponent<TransformComponent>().GetTransform());
			OnDebugShow(true);
			Renderer2D::EndScene();
			previewFrame->UnBind();
		}
	}

	//Todo : To be optimized
	void EditorLayer::OnImGuiRender() {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		{
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
		static bool dockspaceOpen = true;
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);


		ImGui::PopStyleVar(3);


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
			//Todo : change
			if (ImGui::BeginMenu("Scripts")) {
				if (ImGui::MenuItem("Reload Scripts")) {
					if (scene->isRunning()) {
						Z_CORE_ERROR("Try to reload scripts when scene is running!!!");
					} else {
						scriptReload = true;
					}
				}
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
		static float fps = 1.f / Time::DeltaTime();
		static float dt = Time::DeltaTime() * 1000.f;
		if (Time::GetFlushTime() > 1) {
			fps = 1.f / Time::DeltaTime();
			dt = Time::DeltaTime() * 1000.f;
			Time::FlushTime();
		}
		ImGui::Text("FPS: %.0f", fps);
		ImGui::Text("Frame Time: %.2f ms", dt);
		stats->Reset();
		ImGui::End();
		ImGui::Begin("Settings");
		ImGui::Checkbox("Editor Visualize Collider", &EditorVisualizeCollider);
		ImGui::Checkbox("RunTime Visualize Collider", &RunTimeVisualizeCollider);
		ImGui::DragFloat4("Collider ActiveColor", glm::value_ptr(ActiveColor), 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat4("Collider InActiveColor", glm::value_ptr(InactiveColor), 0.01f, 0.0f, 1.0f);
		ImGui::DragInt("StepFrameCount", &stepFrames, 1, 1, 100);

		sceneHierarchyPlane->OnImGuiRender();
		contentBrowser->OnImGuiRender();

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("ViewPort", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar |
		                                  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav |
		                                  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus |
		                                  ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNavInputs);
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

		uint32_t textureID = frameBuffer->GetAttachmentID(0);
		ImGui::Image((void *) textureID, viewSize, ImVec2{0, 1}, ImVec2{1, 0});

		if ((viewportSize != *(glm::vec2 *) &viewSize) && !Input::IsMouseButtonPressed(MouseCode::ButtonLeft)) {
			viewportSize = glm::vec2{viewSize.x, viewSize.y};
			previewFrame->Resize(viewportSize.x / 4.f, viewportSize.y / 4.f);
			frameBuffer->Resize(viewportSize.x, viewportSize.y);
			scene->OnViewportResize(viewportSize.x, viewportSize.y);
			editorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const char *path = (const char *) payload->Data;
				LoadScene(path);
				ImGui::PopStyleVar();
				ImGui::End();
				ImGui::End();
				return;
			}
		}
		if (selectedEntity = sceneHierarchyPlane->GetSelectedEntity();
				(sceneState == SceneState::Edit || sceneState == SceneState::Simulate) && selectedEntity &&
				selectedEntity.HasComponent<CameraComponent>()) {
			ImGui::SetNextWindowPos(ImGui::GetWindowPos() +
			                        ImVec2(ImGui::GetWindowSize().x - ImGui::GetWindowSize().x / 4,
			                               ImGui::GetWindowSize().y - ImGui::GetWindowSize().y / 4));
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x / 4, ImGui::GetWindowSize().y / 4));
			ImGui::Begin("##Camera preview", nullptr,
			             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse |
			             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
			             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
			             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			             ImGuiWindowFlags_NoBackground
			             | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav);
			ImGui::Image((void *) previewFrame->GetAttachmentID(0),
			             ImVec2{ImGui::GetWindowSize().x, ImGui::GetWindowSize().y}, ImVec2{0, 1},
			             ImVec2{1, 0});
			ImGui::End();
		}
		if (selectedEntity && currentGizmoOperation != -1 &&
		    (sceneState == SceneState::Edit || sceneState == SceneState::Simulate)) {
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
		OnButtonUI();

		ImGui::End();
	}

	void EditorLayer::OnButtonUI() {
		//Todo : improve button layout
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 2});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2{0, 0});
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0, 0});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3, 0.3, 0.3, 0.5});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0, 0, 0, 0});
		ImGui::Begin("##tools", nullptr,
		             ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::BeginDisabled(!scene);
		float size = ImGui::GetWindowHeight() - 4;
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x / 2 - ImGui::GetWindowHeight());
		if (ImGui::ImageButton((ImTextureID) toolButtons[0]->GetRendererID(), ImVec2{size, size}, ImVec2{0, 1},
		                       ImVec2{1, 0})) {
			if (sceneState == SceneState::Edit) {
				OnPlay();
			} else {
				OnStop();
			}
		}
		ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID) toolButtons[1]->GetRendererID(), ImVec2{size, size}, ImVec2{0, 1},
		                       ImVec2{1, 0})) {
			if (sceneState == SceneState::Edit) {
				OnSimulate();
			} else if (sceneState != SceneState::Pause) {
				BackState = sceneState;
				sceneState = SceneState::Pause;
				scene->SetPaused(true);
				nextStep = false;
				toolButtons[1] = playButtonIcon;
			} else {
				sceneState = BackState;
				scene->SetPaused(false);
				toolButtons[1] = pauseButtonIcon;
				nextStep = false;
			}
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(sceneState == SceneState::Edit);
		if (ImGui::ImageButton((ImTextureID) toolButtons[2]->GetRendererID(), ImVec2{size, size}, ImVec2{0, 1},
		                       ImVec2{1, 0})) {
			if (sceneState == SceneState::Pause) {
				scene->SetPaused(true);
				scene->SetFrameStepCount(stepFrames);
				nextStep = true;
			} else {
				BackState = sceneState;
				sceneState = SceneState::Pause;
				nextStep = false;
				scene->SetPaused(true);
				scene->SetFrameStepCount(0);
			}
		}
		ImGui::EndDisabled();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
		ImGui::End();

	}

	void EditorLayer::OnEvent(Event &event) {
		if (IsViewportHovered && IsViewportFocused) {
			editorCamera.OnEvent(event);
		}
		EventDispatcher dispatcher(event);
		dispatcher.Handle<KeyPressEvent>(Z_BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
	}

	void EditorLayer::SaveScene() {
		auto path = Z::Utils::FileSave("*.zscene");
		if (!path.empty()) {
			WorkPath = path;
			if (WorkPath.extension() != ".zscene")
				WorkPath.replace_extension(".zscene");
			InnerSave(WorkPath.string());
		}
	}

	void EditorLayer::SaveHotKey() {
		if (sceneState != SceneState::Edit) {
			Z_CORE_WARN("Trying to Save Scene not in Edit Mode!!!");
			return;
		}
		if (!WorkPath.empty()) {
			InnerSave(WorkPath.string());
		} else {
			SaveScene();
		}
	}

	void EditorLayer::InnerSave(const std::string &path) {
		SceneSerializer serializer(scene);
		serializer.Serialize(path);
	}

	void EditorLayer::LoadScene() {
		auto path = Z::Utils::FileOpen("*.zscene");
		if (!path.empty()) {
			LoadScene(path);
		}
	}

	void EditorLayer::LoadScene(const std::filesystem::path&path) {
		if (sceneState != SceneState::Edit) {
			OnStop();
		}
		scene = CreateRef<Scene>();
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetSelectedEntity(-1);
		sceneHierarchyPlane->SetContext(scene);
		SceneSerializer serializer(scene);
		if (serializer.Deserialize(path.string())) {
			WorkPath = path;
		}
	}

	void EditorLayer::NewScene() {
		if (!WorkPath.empty()) {
			if (sceneState != SceneState::Edit)
				OnStop();
			InnerSave(WorkPath.string());
		}
		scene = CreateRef<Scene>();
		sceneHierarchyPlane->SetSelectedEntity(-1);
		sceneHierarchyPlane->SetContext(scene);
		WorkPath = std::filesystem::path();
	}


	void EditorLayer::OnPlay() {
		sceneState = SceneState::Play;
		toolButtons[1] = pauseButtonIcon;
		toolButtons[0] = stopButtonIcon;
		BackScene = scene;
		scene = Scene::Copy(BackScene);
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetContext(scene);
		scene->OnRuntimeStart();
	}

	void EditorLayer::OnStop() {
		auto state = sceneState;
		if(state == SceneState::Pause)
			state=BackState;
		if (state == SceneState::Play) {
			scene->OnRuntimeStop();
		} else if (state == SceneState::Simulate) {
			scene->OnSimulateStop();
		}
		toolButtons[1] = simulateButtonIcon;
		toolButtons[0] = playButtonIcon;
		sceneState = SceneState::Edit;
		scene = BackScene;
		sceneHierarchyPlane->SetContext(scene);
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
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
				if (control)
					if (shift) {
						if (WorkPath.empty()) {
							WorkPath = "./Untitled.zscene";
							Z_CORE_WARN("WorkPath is empty,Save to {0}", WorkPath.string());
						}
						InnerSave(WorkPath.string());
					} else
						SaveHotKey();
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
			case Key::D:
				if (sceneState == SceneState::Edit)
					if (auto et = sceneHierarchyPlane->GetSelectedEntity();shift && et) {
						scene->CopyEntity(et);
					}
				break;
		}
		return false;
	}

	void EditorLayer::OnDebugShow(bool preview) {
		if (preview) {
			if (selectedEntity)
				Renderer2D::BeginScene(selectedEntity.GetComponent<CameraComponent>().camera,
				                       selectedEntity.GetComponent<TransformComponent>().GetTransform());
			else {
				Z_CORE_ERROR("No camera to preview!!!");
				return;
			}
		} else if (sceneState == SceneState::Edit || sceneState == SceneState::Simulate) {
			Renderer2D::BeginScene(editorCamera);
		} else if (sceneState == SceneState::Play) {
			auto camera = scene->GetMainCamera();
			if (!camera) {
				Z_CORE_WARN("No camera in scene");
				return;
			}
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().camera,
			                       camera.GetComponent<TransformComponent>().GetTransform());
		}

		if (((sceneState != SceneState::Play) && EditorVisualizeCollider) ||
		    (sceneState == SceneState::Play && RunTimeVisualizeCollider)) {
			scene->GetComponentView<BoxCollider2DComponent, TransformComponent>().each(
					[&](auto &box2D, auto &transformComponent) {
						//Todo : to be optimized
						auto translation = transformComponent.translation +
						                   glm::mat3(transformComponent.GetTransform()) *
						                   glm::vec3(box2D.offset, 0.f);
						auto size = glm::vec3(box2D.size, 0.f) * transformComponent.scale;
						auto trans = glm::translate(glm::mat4(1.f), translation) *
						             glm::rotate(glm::mat4(1.f), transformComponent.rotation.z,
						                         glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.f), size);
						Renderer2D::DrawRect(trans, box2D.size, *(int *) (box2D.ptr) ? ActiveColor : InactiveColor);

					});
			scene->GetComponentView<CircleCollider2DComponent, TransformComponent>().each(
					[&](auto &circle2D, auto &transformComponent) {
						//Todo : to be optimized
						auto translation = transformComponent.translation +
						                   glm::mat3(transformComponent.GetTransform()) *
						                   glm::vec3(circle2D.offset, 0.f);
						auto size = transformComponent.scale * (circle2D.radius * 2.f);
						auto trans = glm::translate(glm::mat4(1.f), translation) * glm::scale(glm::mat4(1.f), size);
						Renderer2D::DrawCircle(trans, *(int *) (circle2D.ptr) ? ActiveColor : InactiveColor);
					});
		}
		Renderer2D::ChangeDepthTest(RenderAPI::DepthTestState::Always);
		Renderer2D::EndScene();
		Renderer2D::ChangeDepthTest();
	}

	void EditorLayer::OnSimulate() {
		sceneState = SceneState::Simulate;
		toolButtons[1] = pauseButtonIcon;
		toolButtons[0]=stopButtonIcon;
		BackScene = scene;
		scene = Scene::Copy(BackScene);
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetContext(scene);
		scene->OnSimulateStart();
	}

}