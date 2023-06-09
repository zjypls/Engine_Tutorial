//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_SANBOX2D_H
#define ENGINE_TUTORIAL_ZEDITOR_H

#include "z.h"
#include"SceneHierarchy/SceneHierarchyPlane.h"
#include"SceneHierarchy/ContentBrowser.h"
#include "ImGuizmo.h"
#include "Z/Renderer/EditorCamera.h"
#include<unordered_map>

namespace Z {
	class EditorLayer : public Layer {
		enum class SceneState {
			Edit, Play, Simulate,Pause
		};
		SceneState sceneState = SceneState::Edit,BackState=SceneState::Edit;
		Ref<VertexArray> vertexArray;
		Ref<Texture2D> texture[4];
		Ref<SubTex2D> subTex;
		glm::vec4 clearValue{0.1f, 0.1f, 0.1f, 1.0f};
		glm::vec4 ActiveColor{0.3f, 0.8f, 0.1f, 1.0f}, InactiveColor{0.4f, 0.4f, 0.1f, 1.0f};
		bool IsViewportFocused = false, IsViewportHovered = false, RunTimeVisualizeCollider = true, EditorVisualizeCollider = true;
		std::unordered_map<char, Ref<SubTex2D>> textureMap;
		Ref<FrameBuffer> frameBuffer, previewFrame;
		glm::vec2 viewportSize{1200, 800};
		glm::vec2 CursorPos{0, 0};
		Ref<Scene> scene, BackScene;
		Scope<SceneHierarchyPlane> sceneHierarchyPlane;
		int currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		Z::EditorCamera editorCamera;
		Scope<ContentBrowser> contentBrowser;
		Ref<Z::Texture2D> playButtonIcon, stopButtonIcon, simulateButtonIcon,pauseButtonIcon,stepButtonIcon;
		Ref<Z::Texture2D> toolButtons[3];
		std::filesystem::path WorkPath{};
		Entity selectedEntity;
		bool scriptReload = false,nextStep=false;
		int stepFrames=1;

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

		void LoadScene(const std::filesystem::path &path);

		void NewScene();

		void OnPlay();

		void OnSimulate();

		void OnStop();
	};

}
#endif //ENGINE_TUTORIAL_SANBOX2D_H
