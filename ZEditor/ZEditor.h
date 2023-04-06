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
		struct BackData {
			int gizmoOperation = -1;
		};
		BackData backData;
		//Todo: clear the code
		enum class SceneState {
			Edit, Play, Simulate
		};
		SceneState sceneState = SceneState::Edit;
		CameraController controller;
		Ref<VertexArray> vertexArray;
		Ref<Texture2D> texture[4];
		Ref<SubTex2D> subTex;
		glm::vec4 clearValue{0.1f, 0.1f, 0.1f, 1.0f};
		glm::vec4 ActiveColor{0.3f, 0.8f, 0.1f, 1.0f},InactiveColor{0.4f, 0.4f, 0.1f, 1.0f};
		bool IsViewportFocused = false, IsViewportHovered = false, RunTimeVisualizeCollider = true, EditorVisualizeCollider = true;
		glm::ivec2 index{9, 4}, size{2, 3};
		std::unordered_map<char, Ref<SubTex2D>> textureMap;
		Ref<FrameBuffer> frameBuffer, previewFrame;
		glm::vec2 viewportSize{1200, 800};
		glm::vec2 CursorPos{0, 0};
		Ref<Scene> scene, BackScene;
		Scope<SceneHierarchyPlane> sceneHierarchyPlane;
		int currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		Z::EditorCamera editorCamera;
		Scope<ContentBrowser> contentBrowser;
		Ref<Z::Texture2D> playButtonIcon, stopButtonIcon, simulateButtonIcon, currentButtonIcon0, currentButtonIcon1;
		std::stringstream data;
		std::filesystem::path WorkPath{};
		Entity selectedEntity;

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

		void LoadScene(const std::string &path);

		void NewScene();

		void OnPlay();
		void OnSimulate();

		void OnStop();
	};

}
#endif //ENGINE_TUTORIAL_SANBOX2D_H
