//
// Created by 32725 on 2023/3/11.
//
#include "SanBox2D.h"
#include "Z/Core/Entry.h"
#include "glm/ext/matrix_transform.hpp"

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class ExampleLayer : public Z::Layer {
public:
	ExampleLayer() : Layer("Example"), controller(800.f / 600.0f) {
		Z_TRACE("ExampleLayer Build");

		vertexArray = Z::VertexArray::Create();
		texture=Z::Texture2D::CreateTexture("Assets/Textures/Colum.png");

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

		//Init VAO
		auto vertexBuffer = Z::VertexBuffer::Create(vertices, sizeof(vertices));
		{
			auto layout = Z::CreateScope<Z::BufferLayout>(Z::BufferLayout{
					{Z::ShaderDataType::Float3, "position"},
					{Z::ShaderDataType::Float2, "texCoord"}});

			vertexBuffer->SetLayout(*layout);
		}
		vertexArray->AddVertexBuffer(vertexBuffer);

		auto indexBuffer =Z::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		vertexArray->SetIndexBuffer(indexBuffer);

		vertexArray->Unbind();
		auto shader= shaderLibrary.Load("Shaders/One.glsl");

		shader->Bind();
		texture->Bind();
		shader->UnBind();
		auto grid=shaderLibrary.Add(Z::Shader::CreateShader("Grid","Shaders/vert.vert","Shaders/grid.frag",true));

//		auto Test=Z::Shader::CreateShader("Grid");
	}

	virtual void OnUpdate() override {

		controller.OnUpdate(Z::Time::DeltaTime());
		static auto shader=shaderLibrary.Get("Shaders/One");
		static auto grid=shaderLibrary.Get("Grid");
		Z::RenderCommand::SetClearValue({0.1f, 0.1f, 0.1f, 1.0f});
		Z::RenderCommand::Clear();
		Z::Renderer::BeginScene(controller.GetCamera());
		shader->SetUniform("tex0",0);
		Z::Renderer::Submit(shader, vertexArray);
		grid->SetUniform("col", gridColor);
		auto trans=glm::scale(glm::mat4(1.0f), glm::vec3(.1, .1, .1));
		for (int i = 1; i < 11; ++i) {
			for (int j = 1; j < 11; ++j) {
				auto trs=glm::translate(glm::mat4(1.f),gridPos+glm::vec3(i, j, 0)*0.11f)*trans;
				Z::Renderer::Submit(grid, vertexArray,trs);
			}
		}
		Z::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("Setting");
		ImGui::SliderFloat("lambda", &lambda, 0.0f,10.f);
		ImGui::ColorEdit3("gridColor", &gridColor[0]);
		ImGui::End();
	}

	bool KeyPressedEvent(Z::KeyPressEvent &e) {

		return false;
	}

	virtual void OnEvent(Z::Event &e) override {
		controller.OnEvent(e);
		Z::EventDispatcher dispatcher(e);
		dispatcher.Handle<Z::KeyPressEvent>(Z_BIND_EVENT_FUNC(ExampleLayer::KeyPressedEvent));
	}

private:
	Z::ShaderLibrary shaderLibrary;
	Z::Ref<Z::VertexArray> vertexArray;
	Z::CameraController controller;
	Z::Ref<Z::Texture> texture;
	glm::vec3 gridColor{0.5f, 0.5f, 0.5f};
	glm::vec3 gridPos{0, 0, 0};
	float lambda = 1.f;
};

class App : public Z::Application {
public:
	App() {
		PushLayer(new SanBox2D());
	}

	~App() override = default;
};

Z::Application *Z::GetApplication() {
	return new App{};
}


