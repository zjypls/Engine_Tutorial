//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_SANBOX2D_H
#define ENGINE_TUTORIAL_ZEDITOR_H

#include "z.h"
#include<unordered_map>

namespace Z {
	class EditorLayer : public Layer {
		CameraController controller;
		Ref<Shader> shader, grid;
		Ref<VertexArray> vertexArray;
		Ref<Texture2D> texture[4];
		Ref<SubTex2D> subTex;
		glm::vec4 gridColor = {0.2f, 0.3f, 0.8f, 1.f}, clearValue = {0.1f, 0.1f, 0.1f, 1.0f};
		glm::vec3 gridPos = {0.0f, 0.0f, 0.0f};
		glm::vec2 gridScale = {1.0f, 1.0f};
		float gridRotate = 0.f;
		float tillingFactor = 1.f;
		bool IsViewportFocused = false, IsViewportHovered = false;
		glm::ivec2 index{9, 4}, size{2, 3};
		std::unordered_map<char, Ref<SubTex2D>> textureMap;
		Ref<FrameBuffer> frameBuffer;
		glm::vec2 viewportSize = {1200, 800};
		glm::vec2 CursorPos{0, 0};

	public:
		EditorLayer();

		~EditorLayer() override;

		void OnAttach() override;

		void OnDetach() override;

		void OnUpdate() override;

		void OnImGuiRender() override;

		void OnEvent(Event &event) override;
	};

}
#endif //ENGINE_TUTORIAL_SANBOX2D_H
