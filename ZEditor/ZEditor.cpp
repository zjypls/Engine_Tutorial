//
// Created by 32725 on 2023/3/19.
//
#include "Include/imgui/imgui.h"
#include "Include/ImGuizmo/ImGuizmo.h"
#include "Include/glm/glm/gtc/type_ptr.hpp"
#include "Include/glm/glm/gtx/matrix_decompose.hpp"
#include "Include/filewatch/filewatch.h"

#include "Z/Utils/Model.h"

#include "./ZEditor.h"


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
        scene = CreateRef<Scene>();
        editorCamera = EditorCamera(45.f, 1.f, 0.1f, 1000.f);


		sceneHierarchyPlane = CreateScope<SceneHierarchyPlane>();
		contentBrowser = CreateScope<ContentBrowser>();

		//Todo:change this to a better way
		ScriptEngine::LoadAssembly("bin/scripts.dll");
		ScriptEngine::RegisterFileWatch();

		//a Test for mesh Renderer
        /*
		testModel = scene->CreateEntity("Model");
		testModel.AddComponent<MeshRendererComponent>(AssetsSystem::Load<Mesh>(
				(Project::GetProjectRootDir() / "Assets/Models/TinyRoom.obj").string()));
         */

		RenderManager::PushUIContents(this);
	}

	void EditorLayer::OnDetach() {
		if(selfDefLayout) {
			ImGui::SaveIniSettingsToDisk(selfDefLayoutFilePath.c_str());
			Z_CORE_WARN("Layout config file save at : {0}",selfDefLayoutFilePath);
		}
	}

	void EditorLayer::OnUpdate() {

		ScriptEngine::CheckLoad(scene);

		{
			SceneState state = sceneState;
			if (state == SceneState::Pause)
				state = BackState;
			switch (state) {
				case SceneState::Edit: {
					//TODO:optimize operation logic
					if (/*IsViewportFocused &&*/ IsViewportHovered) {
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
				((selectedEntity && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) ||
				 (!selectedEntity && Input::IsMouseButtonPressed(MouseCode::ButtonLeft)))) {
            int value;
			selectedEntity = sceneHierarchyPlane->SetSelectedEntity(value);
		}
		if ((sceneState != SceneState::Play) && selectedEntity && selectedEntity.HasComponent<CameraComponent>()) {
			scene->OnPreviewUpdate(Time::DeltaTime(), selectedEntity.GetComponent<CameraComponent>().camera,
			                       selectedEntity.GetComponent<TransformComponent>().GetTransform());
			OnDebugShow(true);
		}
	}

	//Todo : To be optimized
	void EditorLayer::OnImGuiRender() {
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking|
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		{
			ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		static bool dockspaceOpen = true;
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);


		ImGui::PopStyleVar(3);

		auto &style = ImGui::GetStyle();
		auto miniSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 250;
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
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
					//LoadScene();
                    LoadProjects();
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

		ImGui::Text("Renderer2D Stats:");
		static float fps = 1.f / Time::DeltaTime();
		static float dt = Time::DeltaTime() * 1000.f;
		static uint32 frameCount = 0;
		if (++frameCount >= 500) {
			fps = frameCount / Time::GetFlushTime();
			frameCount = 0;
			dt = Time::DeltaTime() * 1000.f;
			Time::FlushTime();
		}
		ImGui::Text("FPS: %.0f", fps);
		ImGui::Text("Current Frame Time: %.2f ms", dt);
		ImGui::End();
		ImGui::Begin("Settings");
		ImGui::Checkbox("Editor Visualize Collider", &EditorVisualizeCollider);
		ImGui::Checkbox("RunTime Visualize Collider", &RunTimeVisualizeCollider);
		ImGui::Text("Collider ActiveColor:");
		ImGui::DragFloat4("##Collider ActiveColor", glm::value_ptr(ActiveColor), 0.01f, 0.0f, 1.0f);
		ImGui::Text("Collider InActiveColor:");
		ImGui::DragFloat4("##Collider InActiveColor", glm::value_ptr(InactiveColor), 0.01f, 0.0f, 1.0f);
		ImGui::Text("StepFrameCount:");
		ImGui::DragInt("##StepFrameCount", &stepFrames, 1, 1, 100);
        ImGui::Text("Build Version : %s",BUILD_VERSION);
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
		    Input::IsMouseButtonPressed(MouseCode::ButtonLeft)) {

			auto cursorPos = ImGui::GetMousePos();
			auto offset = ImGui::GetWindowPos();
			auto WinSize = ImGui::GetWindowSize();
			auto MaxSize = WinSize + offset;
			auto McursorPos = MaxSize - cursorPos;
			CursorPos = glm::vec2{cursorPos.x - offset.x, McursorPos.y};
		}
		Application::Get().GetImGuiLayer()->SetBlockEvents(!IsViewportFocused && !IsViewportHovered);


		if ((viewportSize != *(glm::vec2 *) &viewSize)) {
            //avoid resize when frame haven't show to viewport yet
			Application::Get().SubmitFunc([this, viewSize]() {
				this->viewportSize = glm::vec2{viewSize.x, viewSize.y};
				this->scene->OnViewportResize(viewportSize.x, viewportSize.y);
				this->editorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
			});
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				std::filesystem::path path = (const char *) payload->Data;
				if (path.extension() == ".zscene") {
					LoadScene(path);
					ImGui::PopStyleVar();
					ImGui::End();
					ImGui::End();
					return;
				}
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
			             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMouseInputs |
                         ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav);
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
			//TODO:Optimize
			static int8_t SavePreTrans = 1;
			if (ImGuizmo::IsUsing()) {
				static glm::quat BefRotation;
				static glm::vec3 BefTranslation, BefScale;
				if (SavePreTrans == 1) {
					//Save transform
					BefRotation = selectTransform.rotation;
					BefTranslation = selectTransform.translation;
					BefScale = selectTransform.scale;
					SavePreTrans = 0;
				}else if (Input::IsMouseButtonPressed(MouseCode::ButtonRight)) {
					//Set transform back
					selectTransform.translation = BefTranslation;
					selectTransform.rotation = glm::eulerAngles(BefRotation);
					selectTransform.scale = BefScale;
					SavePreTrans = -1;
				} else if (SavePreTrans == 0) {
					//decompose and apply transform with transform gizmos
					glm::vec3 translation, scale, skew;
					glm::quat rotation;
					glm::vec4 perspective;
					glm::decompose(Transform, scale, rotation, translation, skew, perspective);
					selectTransform.translation = translation;
					selectTransform.rotation = (glm::eulerAngles(rotation));
					selectTransform.scale = scale;
				}
			}else if (SavePreTrans!=1) {
				//Set gizmos enable
				SavePreTrans = 1;
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
		ImGui::SameLine();
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(sceneState == SceneState::Edit);
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
		auto path = Utils::FileSave("*.zscene");
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
		auto path = Utils::FileOpen("*.zscene");
		if (!path.empty()) {
			LoadScene(path);
		}
	}

	void EditorLayer::LoadScene(const std::filesystem::path &path) {
		if (sceneState != SceneState::Edit) {
			OnStop();
		}
		scene = CreateRef<Scene>();
        SceneSerializer serializer(scene);
        if (serializer.Deserialize(path.string())) {
            WorkPath = path;
        }
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
        sceneHierarchyPlane->SetContext(scene);
		sceneHierarchyPlane->SetSelectedEntity(-1);
	}

	void EditorLayer::NewScene() {
		if (!WorkPath.empty()) {
			if (sceneState != SceneState::Edit)
				OnStop();
			InnerSave(WorkPath.string());
		}
		scene = CreateRef<Scene>();
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetSelectedEntity(-1);
		sceneHierarchyPlane->SetContext(scene);
		WorkPath = std::filesystem::path();
	}


	void EditorLayer::OnPlay() {
		sceneState = SceneState::Play;
		BackScene = scene;
		scene = Scene::Copy(BackScene);
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetContext(scene);
		scene->OnRuntimeStart();
	}

	void EditorLayer::OnStop() {
		auto state = sceneState;
		if (state == SceneState::Pause)
			state = BackState;
		if (state == SceneState::Play) {
			scene->OnRuntimeStop();
		} else if (state == SceneState::Simulate) {
			scene->OnSimulateStop();
		}
		sceneState = SceneState::Edit;
		scene = BackScene;
		sceneHierarchyPlane->SetContext(scene);
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
	}


	bool EditorLayer::OnKeyPressed(KeyPressEvent &event) {
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool RightMouseClicked = Input::IsMouseButtonPressed(MouseCode::ButtonRight);
		switch (event.GetKey()) {
			case Key::N:
				if (control) NewScene();
				break;
			case Key::O:
				if (control) LoadProjects();
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
				if (IsViewportFocused && IsViewportHovered && !RightMouseClicked)
					currentGizmoOperation = -1;
				break;
			case Key::W:
				if (IsViewportFocused && IsViewportHovered && !RightMouseClicked)
					currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				if (IsViewportFocused && IsViewportHovered && !RightMouseClicked)
					currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				break;
			case Key::R:
				if (IsViewportFocused && IsViewportHovered && !RightMouseClicked)
					currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::D:
				if (sceneState == SceneState::Edit)
					if (auto et = sceneHierarchyPlane->GetSelectedEntity();shift && et) {
						scene->InstantiateEntity(et);
					}
				break;
		}
		return false;
	}

	void EditorLayer::OnDebugShow(bool preview) {
		if (preview) {
			if (selectedEntity){}
			else {
				Z_CORE_ERROR("No camera to preview!!!");
				return;
			}
		} else if (sceneState == SceneState::Edit || sceneState == SceneState::Simulate) {
		} else if (sceneState == SceneState::Play) {
			auto camera = scene->GetMainCamera();
			if (!camera) {
				Z_CORE_WARN("No camera in scene");
				return;
			}
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
					});
			scene->GetComponentView<CircleCollider2DComponent, TransformComponent>().each(
					[&](auto &circle2D, auto &transformComponent) {
						//Todo : to be optimized
						auto translation = transformComponent.translation +
						                   glm::mat3(transformComponent.GetTransform()) *
						                   glm::vec3(circle2D.offset, 0.f);
						auto size = transformComponent.scale * (circle2D.radius * 2.f);
						auto trans = glm::translate(glm::mat4(1.f), translation) * glm::scale(glm::mat4(1.f), size);
					});
		}
	}

	void EditorLayer::OnSimulate() {
		sceneState = SceneState::Simulate;
		BackScene = scene;
		scene = Scene::Copy(BackScene);
		scene->OnViewportResize(viewportSize.x, viewportSize.y);
		sceneHierarchyPlane->SetContext(scene);
		scene->OnSimulateStart();
	}

    void EditorLayer::LoadProjects() {
		if(selfDefLayout) {
			ImGui::SaveIniSettingsToDisk(selfDefLayoutFilePath.c_str());
			selfDefLayout=false;
		}
        std::filesystem::path path=Utils::FileOpen("*.zPrj\0", "Test001.zPrj", (ROOT_PATH + "Projects/Test001").c_str());
        if(Project::Init(path)){
            AssetsSystem::InitWithProject(Project::GetProjectRootDir());
            LoadScene(Project::GetProjectRootDir()/Project::GetStartScene());
            contentBrowser->SetWorkPath(Project::GetProjectRootDir().string());
        	if(const auto&configuration=Project::GetEditorLayoutConfiguration();!configuration.empty()) {
        		this->selfDefLayout=true;
        		selfDefLayoutFilePath=configuration.string();
				//avoid reload ini file when imgui recording command
				Application::Get().SubmitFunc([this] {
					ImGui::LoadIniSettingsFromDisk("Assets/Configs/editorLayout.ini");
				});
        		Z_CORE_WARN("Ini config file find : {0}",configuration.string());
        	}
        }else{
            Z_CORE_ERROR("illegal project file : {0}",path);
        }
    }

}