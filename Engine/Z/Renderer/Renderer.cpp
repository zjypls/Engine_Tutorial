//
// Created by 32725 on 2023/3/17.
//

#include "Z/Renderer/Renderer.h"
#include "Z/Renderer/RenderCommand.h"
#include "Z/Renderer/Renderer2D.h"
#include "Z/Renderer/Renderer3D.h"

namespace Z{
	Renderer::SceneData* Renderer::sceneData;
	Renderer::RenderData* Renderer::renderData;
	void Renderer::BeginScene(const Ref<OrithGraphicCamera>&camera) {
		sceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
	}


	void Renderer::EndScene() {

	}

	void Renderer::Submit(Z::Ref<Shader>& shader,const Z::Ref<VertexArray> &vertexArray,const glm::mat4& transform){
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
		sceneData=new SceneData{};
		sceneData->ViewProjectionMatrix=glm::mat4{1};
		renderData=new RenderData{};
		renderData->ubo=UniformBuffer::Create(sizeof(SceneData),1);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::ShutDown() {
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::BeginScene(const Camera &camera, const glm::mat4 &transform) {
		sceneData->ViewProjectionMatrix=camera()*glm::inverse(transform);
		renderData->ubo->SetData(sceneData,sizeof(SceneData));
	}

	void Renderer::BeginScene(const EditorCamera &camera) {
		sceneData->ViewProjectionMatrix=camera.GetViewProjectionMatrix();
		renderData->ubo->SetData(sceneData,sizeof(SceneData));
	}

	void Renderer::BeginScene(const SceneCamera &camera, const glm::mat4 &transform) {
		BeginScene(Camera(camera),transform);
	}
}