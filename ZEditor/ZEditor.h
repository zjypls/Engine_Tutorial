//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_SANBOX2D_H
#define ENGINE_TUTORIAL_ZEDITOR_H

#include <unordered_map>
#include "z.h"
#include "Include/ImGuizmo/ImGuizmo.h"
#include "Z/Renderer/EditorCamera.h"
#include "SceneHierarchy/SceneHierarchyPlane.h"
#include "SceneHierarchy/ContentBrowser.h"

namespace Z {
	class EditorLayer final : public Layer ,ImGuiContent{
		enum class SceneState {
			Edit, Play, Simulate,Pause
		};
		SceneState sceneState = SceneState::Edit,BackState=SceneState::Edit;
		glm::vec4 clearValue{0.1f, 0.1f, 0.1f, 1.0f};
		glm::vec4 ActiveColor{0.3f, 0.8f, 0.1f, 1.0f}, InactiveColor{0.4f, 0.4f, 0.1f, 1.0f};
		bool IsViewportFocused = false, IsViewportHovered = false, RunTimeVisualizeCollider = true, EditorVisualizeCollider = true;
		glm::vec2 viewportSize{1200, 800};
		glm::vec2 CursorPos{0, 0};
		Ref<Scene> scene, BackScene;
		Scope<SceneHierarchyPlane> sceneHierarchyPlane;
		int currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		EditorCamera editorCamera;
		Scope<ContentBrowser> contentBrowser;
		std::filesystem::path WorkPath{};
		Entity selectedEntity;
		bool scriptReload = false,nextStep=false,selfDefLayout=false;
		std::string selfDefLayoutFilePath;
		int stepFrames=1;

		Entity testModel;

		void InnerSave(const std::string &path);

		void SaveHotKey();

	public:
		EditorLayer();

		~EditorLayer() override;

		void OnAttach() override;

		void OnDetach() override;

		void OnUpdate() override;

		void OnButtonUI();

		void OnDebugShow(bool circle2D = false);

		void OnImGuiRender() override;

		void OnEvent(Event &event) override;

	private:
		bool OnKeyPressed(KeyPressEvent &event);

		void SaveScene();

		void LoadScene();

        void LoadProjects();

		void LoadScene(const std::filesystem::path &path);

		void NewScene();

		void OnPlay();

		void OnSimulate();

		void OnStop();
	};

}
#endif //ENGINE_TUTORIAL_SANBOX2D_H
