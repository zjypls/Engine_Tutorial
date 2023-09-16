//
// Created by z on 2023/9/14.
//

#include "Z/Renderer/Renderer3D.h"
#include "Z/Renderer/RenderCommand.h"


namespace Z {


	Renderer3D::RenderData *Renderer3D::renderData;
	std::vector<Renderer3D::ModelData> Renderer3D::Models;
	uint32_t Renderer3D::maxRecord = 15;

	void
	Renderer3D::Draw(const glm::mat4 &transform, const MeshRendererComponent &meshRenderer, uint32_t entityID) {
//		renderData->uboData->model=transform;
//		renderData->uboData->index=entityID;
//		renderData->ubo->SetData(renderData->uboData,sizeof(UBO));
//		renderData->EasySample->Bind();
//		RenderCommand::DrawIndexed(meshRenderer->vertexArray);
		//TODO:may be recognized with a better way?
		Models.emplace_back(transform, meshRenderer.mesh, entityID);
		if(Models.size()>=maxRecord)
			Flush();
	}

	void Renderer3D::Init() {
		if (renderData) {
			Z_CORE_WARN("Renderer3D Has Inited!!!");
			return;
		}
		renderData = new RenderData{};
		renderData->uboData = new UBO{};
		renderData->EasySample = Shader::CreateShader("Shaders/EasySample.glsl");
		renderData->ubo = UniformBuffer::Create(sizeof(UBO), 3);
		renderData->uboData->index = -1;
		renderData->uboData->model = glm::mat4{1};
	}

	void Renderer3D::Shutdown() {
		Z_CORE_WARN("Renderer3D Shutdown!!!");
		delete renderData->uboData;
		delete renderData;
	}

	void Renderer3D::BeginScene() {

	}


	void Renderer3D::EndScene() {
		Flush();
	}

	void Renderer3D::Flush() {
		renderData->EasySample->Bind();
		for (const auto &model: Models) {
			//TODO:optimize with ref???
			renderData->uboData->model = model.transform;
			renderData->uboData->index = model.entityID;
			renderData->ubo->SetData(renderData->uboData, sizeof(UBO));
			RenderCommand::DrawIndexed(model.MeshRenderer->vertexArray);
		}
		Models.clear();
	}
}
