//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_ZEDITOR_H
#define ENGINE_TUTORIAL_SANBOX2D_H

#include "z.h"
#include<unordered_map>

class SanBox2D : public Z::Layer {
	Z::CameraController controller;
	Z::Ref<Z::Shader> shader, grid;
	Z::Ref<Z::VertexArray> vertexArray;
	Z::Ref<Z::Texture2D> texture[4];
	Z::Ref<Z::SubTex2D> subTex;
	glm::vec4 gridColor = {0.2f, 0.3f, 0.8f, 1.f}, clearValue = {0.1f, 0.1f, 0.1f, 1.0f};
	glm::vec3 gridPos = {0.0f, 0.0f, 0.0f};
	glm::vec2 gridScale = {1.0f, 1.0f};
	float gridRotate = 0.f;
	float tillingFactor = 1.f;
	glm::ivec2 index{9, 4}, size{2, 3};
	std::unordered_map<char,Z::Ref<Z::SubTex2D>> textureMap;

public:
	SanBox2D();

	~SanBox2D() override;

	void OnAttach() override;

	void OnDetach() override;

	void OnUpdate() override;

	void OnImGuiRender() override;

	void OnEvent(Z::Event &event) override;
};


#endif //ENGINE_TUTORIAL_ZEDITOR_H
