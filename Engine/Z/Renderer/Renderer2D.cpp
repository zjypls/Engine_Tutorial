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
		data->UShader = Shader::CreateShader("../Shaders/defaultU.glsl");
		data->whiteTexture = Texture2D::CreateTexture(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		data->whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		data->textureSlots[0] = data->whiteTexture;
		data->textureSlotIndex = 1;
		data->quadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};
	}

	void Renderer2D::Shutdown() {
		delete[] data->quadVertexBufferBase;
		delete data;
	}

	void Renderer2D::BeginScene(const Ref<OrithGraphicCamera> &camera) {
		data->UShader->Bind();
		data->UShader->SetUniform("viewProject", camera->GetViewProjectionMatrix());
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
	}

	void Renderer2D::EndScene() {
		uint32_t dataSize = (uint8_t *) data->quadVertexBufferPtr - (uint8_t *) data->quadVertexBufferBase;
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
		data->quadVertexBuffer->SetData(data->quadVertexBufferBase, dataSize);
		for (int i = 0; i < data->textureSlotIndex; i++)
			data->textureSlots[i]->Bind(i);
		Flush();
	}

	void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color) {
		DrawQuad({position.x, position.y, 0.0f}, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
	                          float tilingFactor, glm::vec4 tintCol) {
		DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor);
	}

	void Renderer2D::DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation,
	                                 const glm::vec4 &color) {
		DrawQuadRotated({position.x, position.y, 0.0f}, size, rotation, color);
	}

	void Renderer2D::DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation,
	                                 const Ref<Texture2D> &texture, float tilingFactor, glm::vec4 tintCol) {
		DrawQuadRotated({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor);
	}


	void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			EndScene();
		}

		constexpr float tilingFactor = 1.0f;
		glm::vec2 texCords[]={glm::vec2(0.0f,0.0f),glm::vec2(1.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(0.0f,1.0f)};
		glm::vec3 pos[]={glm::vec3(position.x-size.x/2,position.y-size.y/2,position.z),glm::vec3(position.x+size.x/2,position.y-size.y/2,position.z),
		                 glm::vec3(position.x+size.x/2,position.y+size.y/2,position.z),glm::vec3(position.x-size.x/2,position.y+size.y/2,position.z)};
		for(int i = 0;i<4;i++){
			data->quadVertexBufferPtr->position = pos[i];
			data->quadVertexBufferPtr->color = color;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = 0;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}

		data->quadIndexCount += 6;

		stats->QuadCount++;

	}

	void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
	                          float tilingFactor, glm::vec4 tintCol) {

		if (data->quadIndexCount >= data->MaxIndexCount) {
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
		glm::vec3 pos[]={position, {position.x + size.x, position.y, position.z},
		                 {position.x + size.x, position.y + size.y, position.z},
		                 {position.x, position.y + size.y, position.z}};
		glm::vec2 TexCords[]={glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
		                      glm::vec2(0.0f, 1.0f)};
		for (int i=0;i<4;i++   ) {
			data->quadVertexBufferPtr->position = pos[i];
			data->quadVertexBufferPtr->color = tintCol;
			data->quadVertexBufferPtr->texCoord = TexCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}


		data->quadIndexCount += 6;

		stats->QuadCount++;

	}

	void Renderer2D::DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation,
	                                 const glm::vec4 &color) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			EndScene();
		}
		auto model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f})
		             * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f});

		constexpr float tilingFactor = 1.0f;
		glm::vec2 texCords[]={{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
		for(int i = 0; i < 4; i++){
			data->quadVertexBufferPtr->position = model * data->quadVertexPositions[i];
			data->quadVertexBufferPtr->color = color;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = 0;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}


		data->quadIndexCount += 6;

		stats->QuadCount++;
	}

	void Renderer2D::DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation,
	                                 const Ref<Texture2D> &texture, float tilingFactor, glm::vec4 tintCol) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			//Z_CORE_WARN("Max index count reached,Begin New Batch");
			EndScene();
		}
		auto model = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f})
		             * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f});

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
		constexpr auto color = glm::vec4(1.0f);
		for(int i = 0; i < 4; i++){
			data->quadVertexBufferPtr->position = model * data->quadVertexPositions[i];
			data->quadVertexBufferPtr->color = tintCol;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}


		data->quadIndexCount += 6;

		stats->QuadCount++;
	}


	void Renderer2D::Flush() {
		data->quadVertexArray->Bind();
		RenderCommand::DrawIndexed(data->quadVertexArray, data->quadIndexCount);
		stats->DrawCalls++;
		data->quadIndexCount = 0;
	}

	void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTex2D> &texture,
	                          float tilingFactor, glm::vec4 tintCol) {
		DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintCol);
	}

	void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTex2D> &texture,
	                          float tilingFactor, glm::vec4 tintCol) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			//Z_CORE_WARN("Max index count reached,Begin New Batch");
			EndScene();
		}
		auto tex=texture->GetTex();
		float textureIndex = 0.0f;
		for (int i = 0; i < data->textureSlotIndex; ++i) {
			if (*data->textureSlots[i] == *tex) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0.0f) {
			textureIndex = (float) data->textureSlotIndex++;
			data->textureSlots[textureIndex] = tex;
			stats->TextureCount++;
		}
		auto texCords=texture->GetCords();

		//constexpr glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
		const glm::vec3 pos[]={position,position+glm::vec3(size.x,0.0f,0.0f),position+glm::vec3(size.x,size.y,0.0f),position+glm::vec3(0.0f,size.y,0.0f)};
		for(int i=0;i<4;i++){
			data->quadVertexBufferPtr->position = pos[i];
			data->quadVertexBufferPtr->color = tintCol;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}
		data->quadIndexCount += 6;

		stats->QuadCount++;

	}

	void Renderer2D::DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation,
	                                 const Ref<SubTex2D> &texture, float tilingFactor, glm::vec4 tintCol) {
		DrawQuadRotated({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, tintCol);
	}

	void Renderer2D::DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation,
	                                 const Ref<SubTex2D> &texture, float tilingFactor, glm::vec4 tintCol) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			EndScene();
		}
		auto tex=texture->GetTex();
		float textureIndex = 0.0f;
		for (int i = 0; i < data->textureSlotIndex; ++i) {
			if (*data->textureSlots[i] == *tex) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0.0f) {
			textureIndex = (float) data->textureSlotIndex++;
			data->textureSlots[textureIndex] = tex;
			stats->TextureCount++;
		}
		auto texCords=texture->GetCords();

		auto model=glm::translate(glm::mat4(1.0f),position)*glm::rotate(glm::mat4(1.0f),rotation,glm::vec3(0.0f,0.0f,1.0f))
									*glm::scale(glm::mat4(1.0f),glm::vec3(size,1.0f));

		for(int i=0;i<4;i++){
			data->quadVertexBufferPtr->position =model*data->quadVertexPositions[i];
			data->quadVertexBufferPtr->color = tintCol;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr++;
		}

		data->quadIndexCount += 6;

		stats->QuadCount++;
	}
}