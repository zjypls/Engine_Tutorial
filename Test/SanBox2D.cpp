//
// Created by 32725 on 2023/3/19.
//

#include "SanBox2D.h"
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


SanBox2D::SanBox2D() : Layer("SanBox2D"), controller(1200.f / 800.0f) {

}

SanBox2D::~SanBox2D() {

}

void SanBox2D::OnAttach() {
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
	vertexArray = Z::VertexArray::Create();
	texture[0] = Z::Texture2D::CreateTexture("../Assets/Textures/Colum.png");
	texture[1] = Z::Texture2D::CreateTexture("../Assets/Textures/Layla.jpg");
	texture[2] = Z::Texture2D::CreateTexture("../Assets/Textures/Nahida.png");
	texture[3] = Z::Texture2D::CreateTexture("../Assets/Sprites/rpgSheet.png");
	subTex = Z::SubTex2D::Create(texture[3], glm::vec2{10, 10}, glm::vec2{128, 128}, {3, 3});
	textureMap['W'] = Z::SubTex2D::Create(texture[3], glm::vec2{11, 11}, glm::vec2{128, 128}, {1, 1});
	textureMap['D'] = Z::SubTex2D::Create(texture[3], glm::vec2{6, 11}, glm::vec2{128, 128}, {1, 1});
	auto vertexBuffer = Z::VertexBuffer::Create(vertices, sizeof(vertices));
	{
		auto layout = Z::CreateScope<Z::BufferLayout>(Z::BufferLayout{
				{Z::ShaderDataType::Float3, "position"},
				{Z::ShaderDataType::Float2, "texCoord"}});
		vertexBuffer->SetLayout(*layout);
	}
	vertexArray->AddVertexBuffer(vertexBuffer);
	auto indexBuffer = Z::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	vertexArray->SetIndexBuffer(indexBuffer);
	vertexArray->Unbind();
	shader = Z::Shader::CreateShader("../Shaders/One.glsl");
	shader->Bind();
	shader->UnBind();
	grid = Z::Shader::CreateShader("Grid", "../Shaders/vert.vert", "../Shaders/grid.frag", true);
}

void SanBox2D::OnDetach() {

}

void SanBox2D::OnUpdate() {
	controller.OnUpdate(Z::Time::DeltaTime());
	Z::RenderCommand::SetClearValue(clearValue);
	Z::RenderCommand::Clear();

	Z::Renderer2D::BeginScene(controller.GetCamera());


	for (int i = 0; i < mapWidth; i++) {
		for (int j = 0; j < mapHeight; j++) {
			auto c = maps[j * mapWidth + i];
			if (textureMap.count(c)) {
				Z::Renderer2D::DrawQuad({i - mapWidth / 2, mapHeight / 2 - j}, {1, 1}, textureMap[c]);
			} else {
				Z::Renderer2D::DrawQuad({i - mapWidth / 2, mapHeight / 2 - j}, {1, 1}, subTex);
			}
		}
	}


	if (Z::Input::IsMouseButtonPressed(Z_MOUSE_BUTTON_4)) {
		auto [x, y] = Z::Input::GetMousePosition();
		auto w = Z::Application::Get().GetWindow().GetWidth();
		auto h = Z::Application::Get().GetWindow().GetHeight();
		auto size = controller.GetSize();
		auto pos = controller.GetCamera()->GetPosition();
		x = (x / w * 2 - 1) * size.x, y = ((h - y) / h * 2 - 1) * size.y;

		auto pos2 = glm::vec2(x + pos.x, y + pos.y) + Z::Random::RandVec2() * .2f - .1f;
		Z_Test::Particle::AddToPool({glm::vec3(pos2, 0.f),
		                             {Z::Random::Float() / 3.f, 0.1, 0.}, {0, 0, 0},
		                             {Z::Random::RandVec3() * glm::vec3{1.f, 0.5f, 0.6f}, 1.}, 0.03f,
		                             0., (Z::Random::Float() + 0.5f)});
	}
	Z_Test::Particle::OnUpdate(Z::Time::DeltaTime());
	Z_Test::Particle::OnRender();
	Z::Renderer2D::EndScene();
}

void SanBox2D::OnImGuiRender() {
	ImGui::Begin("Settings");

	auto stats = Z::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %u", stats->DrawCalls);
	ImGui::Text("Quads: %u", stats->QuadCount);
	ImGui::Text("Vertices: %u", stats->GetTotalVertexCount());
	ImGui::Text("Indices: %u", stats->GetTotalIndexCount());
	stats->Reset();

	ImGui::End();
}

void SanBox2D::OnEvent(Z::Event &event) {
	controller.OnEvent(event);
}
