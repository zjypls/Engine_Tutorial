//
// Created by 32725 on 2023/3/19.
//

#include "Renderer2D.h"
#include "glm/gtx/transform.hpp"
#include "Z/Renderer/RenderCommand.h"

namespace Z {
	Renderer2D::RenderData *Renderer2D::data;
	Renderer2D::StatisticData *Renderer2D::stats;


	void Renderer2D::Init() {
		data = new RenderData();
		stats = new StatisticData{0, 0, 0};
		data->quadVertexArray = VertexArray::Create();
		data->quadVertexBufferBase = new QuadVertex[data->MaxVertexCount];

		data->quadVertexBuffer = Z::VertexBuffer::Create(data->MaxVertexCount * sizeof(QuadVertex));
		{
			auto layout = Z::CreateScope<Z::BufferLayout>(Z::BufferLayout{
					{Z::ShaderDataType::Float3, "position"},
					{Z::ShaderDataType::Float4, "color"},
					{Z::ShaderDataType::Float2, "texCoord"},
					{Z::ShaderDataType::Float,  "texIndex"},
					{Z::ShaderDataType::Float,  "tillingFactor"}});
			data->quadVertexBuffer->SetLayout(*layout);
		}
		data->quadVertexArray->AddVertexBuffer(data->quadVertexBuffer);
		uint32_t *indices = new uint32_t[data->MaxIndexCount];
		for (int i = 0, j = 0; i < data->MaxIndexCount; i += 6) {
			indices[i + 0] = j + 0;
			indices[i + 1] = j + 1;
			indices[i + 2] = j + 2;
			indices[i + 3] = j + 2;
			indices[i + 4] = j + 3;
			indices[i + 5] = j + 0;
			j += 4;
		}
		auto indexBuffer = Z::IndexBuffer::Create(indices, data->MaxIndexCount);
		data->quadVertexArray->SetIndexBuffer(indexBuffer);
		delete[] indices;
		data->quadVertexArray->Unbind();
		data->UShader = Shader::CreateShader(std::string(Z_SOURCE_DIR)+"/Shaders/defaultU.glsl");
		data->whiteTexture = Texture2D::CreateTexture(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		data->whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		data->textureSlots[0] = data->whiteTexture;
		data->textureSlotIndex = 1;
		data->quadVertexPositions[0] = glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[1] = glm::vec4{0.5f, -0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[2] = glm::vec4{0.5f, 0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[3] = glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f};
	}

	void Renderer2D::Shutdown() {
		delete[] data->quadVertexBufferBase;
		delete data;
	}

	void Renderer2D::BeginScene(const EditorCamera &camera) {
		data->UShader->Bind();
		auto viewProject = camera.GetViewProjectionMatrix();
		data->UShader->SetUniform("viewProject", viewProject);
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Ref<OrithGraphicCamera> &camera) {
		data->UShader->Bind();
		data->UShader->SetUniform("viewProject", camera->GetViewProjectionMatrix());
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera&camera,const glm::mat4 &transform){
		data->UShader->Bind();
		glm::mat4 viewProject = camera() * glm::inverse(transform);
		data->UShader->SetUniform("viewProject", viewProject);
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
	}

	void Renderer2D::Flush() {
		data->quadVertexArray->Bind();
		RenderCommand::DrawIndexed(data->quadVertexArray, data->quadIndexCount);
		stats->DrawCalls++;
		data->quadIndexCount = 0;
	}
	void Renderer2D::EndScene() {
		uint32_t dataSize = (uint8_t *) data->quadVertexBufferPtr - (uint8_t *) data->quadVertexBufferBase;
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
		data->quadVertexBuffer->SetData(data->quadVertexBufferBase, dataSize);
		for (int i = 0; i < data->textureSlotIndex; i++)
			data->textureSlots[i]->Bind(i);
		Flush();
	}

	void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			EndScene();
		}
		const glm::vec2 texCords[] = {glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
		                                  glm::vec2(0.0f, 1.0f)};
		constexpr float textureIndex = 0.0f;
		constexpr float tilingFactor = 1.0f;
		for (int i = 0; i < 4; i++) {
			data->quadVertexBufferPtr->position = glm::vec3(transform * data->quadVertexPositions[i]);
			data->quadVertexBufferPtr->color = color;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}

		data->quadIndexCount += 6;

		stats->QuadCount++;

	}

	void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, float tilingFactor,
	                          const glm::vec4& tintCol) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			//Z_CORE_WARN("Max index count reached,Begin New Batch");
			EndScene();
		}

		float textureIndex = 0.0f;
		for (int i = 0; i < data->textureSlotIndex; ++i) {
			if (*data->textureSlots[i] == *texture) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0.0f) {
			textureIndex = (float) data->textureSlotIndex++;
			data->textureSlots[textureIndex] = texture;
			stats->TextureCount++;
		}

		glm::vec2 texCords[]={glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(0.0f,1.0f)};

		for(int i = 0; i < 4; i++){
			data->quadVertexBufferPtr->position = glm::vec3(transform * data->quadVertexPositions[i]);
			data->quadVertexBufferPtr->color = tintCol;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}


		data->quadIndexCount += 6;

		stats->QuadCount++;
	}


	void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<SubTex2D> &texture,float tilingFactor,const glm::vec4& tintCol){
		if (data->quadIndexCount >= data->MaxIndexCount) {
			//Z_CORE_WARN("Max index count reached,Begin New Batch");
			EndScene();
		}

		float textureIndex = 0.0f;
		for (int i = 0; i < data->textureSlotIndex; ++i) {
			if (*data->textureSlots[i] == *texture->GetTex()) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0.0f) {
			textureIndex = (float) data->textureSlotIndex++;
			data->textureSlots[textureIndex] = texture->GetTex();
			stats->TextureCount++;
		}

		auto texCords=texture->GetCords();

		for(int i = 0; i < 4; i++){
			data->quadVertexBufferPtr->position = glm::vec3(transform * data->quadVertexPositions[i]);
			data->quadVertexBufferPtr->color = tintCol;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}
		stats->QuadCount++;
		data->quadIndexCount += 6;
	}


}