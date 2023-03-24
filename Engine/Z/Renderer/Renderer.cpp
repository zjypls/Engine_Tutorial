//
// Created by 32725 on 2023/3/17.
//

#include "Renderer.h"
#include "RenderCommand.h"
#include"Renderer2D.h"

namespace Z{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;
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
		Renderer2D::Init();

	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}
}