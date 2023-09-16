//
// Created by 32725 on 2023/3/17.
//

#include "Z/Core/AssetsSystem.h"
#include "Z/Renderer/Renderer.h"
#include "Z/Renderer/RenderCommand.h"
#include "Z/Renderer/Renderer2D.h"
#include "Z/Renderer/Renderer3D.h"

namespace Z {
	Renderer::SceneData *Renderer::sceneData;
	Renderer::RenderData *Renderer::renderData;
	Ref<Shader> Renderer::skyBoxShader;
	Ref<Texture> Renderer::skyBox;
	Ref<VertexArray> Renderer::cube;

	void Renderer::BeginScene(const Ref<OrithGraphicCamera> &camera) {
		sceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
	}


	void Renderer::EndScene() {

	}

	void Renderer::Submit(Z::Ref<Shader> &shader, const Z::Ref<VertexArray> &vertexArray, const glm::mat4 &transform) {
		vertexArray->Bind();
		shader->Bind();
		shader->SetUniform("ViewPerspective", sceneData->ViewProjectionMatrix);
		shader->SetUniform("Model", transform);
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Init() {
		RenderCommand::Init();
		Renderer3D::Init();
		Renderer2D::Init();
		sceneData = new SceneData{};
		sceneData->ViewProjectionMatrix = glm::mat4{1};
		renderData = new RenderData{};
		renderData->ubo = UniformBuffer::Create(sizeof(SceneData), 1);
		skyBoxShader = Shader::CreateShader("Shaders/SkyBox.glsl");
		cube = VertexArray::Create();

		float verts[]{
				-1.f, -1.f, -1.f,
				1.f, -1.f, -1.f,
				-1.f, 1.f, -1.f,
				1.f, 1.f, -1.f,
				-1.f, -1.f, 1.f,
				1.f, -1.f, 1.f,
				-1.f, 1.f, 1.f,
				1.f, 1.f, 1.f,
		};
		uint32_t indexes[]{
				0, 2, 3, 0, 3, 1,
				0, 4, 6, 0, 6, 2,
				0, 1, 5, 0, 5, 4,
				4, 5, 7, 4, 7, 6,
				1, 3, 7, 1, 7, 5,
				2, 6, 7, 2, 7, 3,
		};
		auto vertexBuffer = VertexBuffer::Create(verts, 24 * sizeof(float));
		vertexBuffer->SetLayout({{ShaderDataType::Float3, "pos"}});
		auto indexBuffer = IndexBuffer::Create(indexes, 36);
		cube->AddVertexBuffer(vertexBuffer);
		cube->SetIndexBuffer(indexBuffer);
		//Fixme:Bug
		//skyBox=AssetsSystem::Load<Texture>(ZSTRCAT(Z_SOURCE_DIR,"Projects/Test001/Textures/skybox/skybox.zConf"));
		skyBox = CubeMap::CreateTexture({
				                                "Projects/Test001/Assets/Textures/skybox/right.jpg",
				                                "Projects/Test001/Assets/Textures/skybox/left.jpg",
				                                "Projects/Test001/Assets/Textures/skybox/top.jpg",
				                                "Projects/Test001/Assets/Textures/skybox/bottom.jpg",
				                                "Projects/Test001/Assets/Textures/skybox/front.jpg",
				                                "Projects/Test001/Assets/Textures/skybox/back.jpg"
		                                });
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::ShutDown() {
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::BeginScene(const Camera &camera, const glm::mat4 &transform) {
		//TODO:optimize
		auto viewMatWithPos=camera()*glm::mat4(glm::inverse(glm::mat3(transform)));
		sceneData->ViewProjectionMatrix=viewMatWithPos;
		renderData->ubo->SetData(sceneData, sizeof(SceneData));
		RenderSkyBox();
		sceneData->ViewProjectionMatrix = camera()*glm::inverse(transform);
		renderData->ubo->SetData(sceneData, sizeof(SceneData));
	}

	void Renderer::BeginScene(const EditorCamera &camera) {
		//TODO:optimize
		auto& viewMat=camera.GetViewMatrix();
		auto viewSky=glm::mat4(glm::mat3(viewMat));
		auto& projectMat=camera.GetProjectionMatrix();
		sceneData->ViewProjectionMatrix = projectMat*viewSky;
		renderData->ubo->SetData(sceneData, sizeof(SceneData));
		RenderSkyBox();
		sceneData->ViewProjectionMatrix = projectMat*viewMat;
		renderData->ubo->SetData(sceneData, sizeof(SceneData));
	}

	void Renderer::BeginScene(const SceneCamera &camera, const glm::mat4 &transform) {
		BeginScene(Camera(camera), transform);
	}

	void Renderer::RenderSkyBox() {
		skyBox->Bind();
		skyBoxShader->Bind();
		RenderCommand::ChangeDepthTest(RenderAPI::DepthTestState::Always);
		RenderCommand::DrawIndexed(cube);
		RenderCommand::ChangeDepthTest();
	}
}