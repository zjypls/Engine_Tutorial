//
// Created by 32725 on 2023/3/19.
//

#include "Renderer2D.h"
#include "glm/gtx/transform.hpp"
#include <thread>
#include "Z/Renderer/RenderCommand.h"

namespace Z {
	Renderer2D::RenderData *Renderer2D::data;
	Renderer2D::StatisticData *Renderer2D::stats;


	void Renderer2D::Init() {
		data = new RenderData();
		data->QuadShader = Shader::CreateShader(std::string(Z_SOURCE_DIR) + "/Shaders/Quad.glsl");
		data->CircleShader = Shader::CreateShader(std::string(Z_SOURCE_DIR) + "/Shaders/Circle.glsl");
		data->LineShader = Shader::CreateShader(std::string(Z_SOURCE_DIR) + "/Shaders/Line.glsl");
		stats = new StatisticData{0, 0, 0};
		data->quadVertexArray = VertexArray::Create();
		data->quadVertexBufferBase = new QuadVertex[data->MaxVertexCount];

		data->quadVertexBuffer = Z::VertexBuffer::Create(data->MaxVertexCount * sizeof(QuadVertex));
		{
			auto layout = Z::CreateScope<Z::BufferLayout>(Z::BufferLayout{
					{Z::ShaderDataType::Float3, "position"},
					{Z::ShaderDataType::Float4, "color"},
					{Z::ShaderDataType::Float2, "texCoord"},
					{Z::ShaderDataType::Int,    "texIndex"},
					{Z::ShaderDataType::Float,  "tillingFactor"},
					{Z::ShaderDataType::Int,    "EntityID"},});
			data->quadVertexBuffer->SetLayout(*layout);
		}
		data->quadVertexArray->AddVertexBuffer(data->quadVertexBuffer);
		auto indices = new uint32_t[data->MaxIndexCount];
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
		data->quadVertexArray->Unbind();

		data->circleVertexArray = VertexArray::Create();
		data->circleVertexBufferBase = new CircleVertex[data->MaxVertexCount];
		data->circleVertexBuffer = Z::VertexBuffer::Create(data->MaxVertexCount * sizeof(CircleVertex));
		{
			auto layout = Z::CreateScope<Z::BufferLayout>(Z::BufferLayout{
					{Z::ShaderDataType::Float3, "LocalPosition"},
					{Z::ShaderDataType::Float3, "WorldPosition"},
					{Z::ShaderDataType::Float4, "color"},
					{Z::ShaderDataType::Float,  "thickness"},
					{Z::ShaderDataType::Float,  "fade"},
					{Z::ShaderDataType::Int,    "EntityID"},});
			data->circleVertexBuffer->SetLayout(*layout);
		}
		data->circleVertexArray->AddVertexBuffer(data->circleVertexBuffer);
		data->circleVertexArray->SetIndexBuffer(indexBuffer);
		data->circleVertexArray->Unbind();

		data->lineVertexArray = VertexArray::Create();
		data->lineVertexBufferBase = new LineVertex[data->MaxVertexCount];
		data->lineVertexBuffer = Z::VertexBuffer::Create(data->MaxVertexCount * sizeof(LineVertex));
		{
			auto layout = Z::CreateScope<Z::BufferLayout>(Z::BufferLayout{
					{Z::ShaderDataType::Float3, "Position"},
					{Z::ShaderDataType::Float4, "color"},
					{Z::ShaderDataType::Int,    "EntityID"},});
			data->lineVertexBuffer->SetLayout(*layout);
		}
		data->lineVertexArray->AddVertexBuffer(data->lineVertexBuffer);
		data->lineVertexArray->SetIndexBuffer(indexBuffer);//TODO: 为什么这里不用重新创建一个indexBuffer
		data->lineVertexArray->Unbind();

		delete[] indices;
		data->whiteTexture = Texture2D::CreateTexture(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		data->whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		data->textureSlots[0] = data->whiteTexture;
		data->textureSlotIndex = 1;
		data->quadVertexPositions[0] = glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[1] = glm::vec4{0.5f, -0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[2] = glm::vec4{0.5f, 0.5f, 0.0f, 1.0f};
		data->quadVertexPositions[3] = glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f};

		data->cameraBuffer = UniformBuffer::Create(sizeof(CameraData), 1);
	}

	void Renderer2D::Shutdown() {
		delete[] data->quadVertexBufferBase;
		delete[] data->circleVertexBufferBase;
		delete[] data->lineVertexBufferBase;
		delete data;
	}

	void Renderer2D::BeginScene(const EditorCamera &camera) {
		data->cameraData.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		data->cameraBuffer->SetData(&data->cameraData, sizeof(CameraData));
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
		data->circleVertexBufferPtr = data->circleVertexBufferBase;
		data->lineVertexBufferPtr = data->lineVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Ref<OrithGraphicCamera> &camera) {
		data->cameraData.ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		data->cameraBuffer->SetData(&data->cameraData, sizeof(CameraData));
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
		data->circleVertexBufferPtr = data->circleVertexBufferBase;
		data->lineVertexBufferPtr = data->lineVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera &camera, const glm::mat4 &transform) {
		data->cameraData.ViewProjectionMatrix = camera() * glm::inverse(transform);
		data->cameraBuffer->SetData(&data->cameraData, sizeof(CameraData));
		data->quadVertexBufferPtr = data->quadVertexBufferBase;
		data->circleVertexBufferPtr = data->circleVertexBufferBase;
		data->lineVertexBufferPtr = data->lineVertexBufferBase;
	}

	void Renderer2D::BeginScene(const SceneCamera &camera, const glm::mat4 &transform) {
		BeginScene(Camera(camera), transform);
	}
	void Renderer2D::Flush() {
		if (data->quadIndexCount > 0) {
			data->QuadShader->Bind();
			for (uint32_t i = 0; i < data->textureSlotIndex; i++) {
				data->textureSlots[i]->Bind(i);
			}
			RenderCommand::DrawIndexed(data->quadVertexArray, data->quadIndexCount);
			stats->DrawCalls++;
		}
		if (data->circleIndexCount > 0) {
			data->CircleShader->Bind();
			RenderCommand::DrawIndexed(data->circleVertexArray, data->circleIndexCount);
			stats->DrawCalls++;
		}
		if (data->lineIndexCount > 0) {
			data->LineShader->Bind();
			RenderCommand::DrawLine(data->lineVertexArray, data->lineIndexCount);
			stats->DrawCalls++;
		}
		data->quadIndexCount = 0;
		data->circleIndexCount = 0;
		data->lineIndexCount = 0;
	}

	void Renderer2D::EndScene() {
		{
			uint32_t dataSize = (uint8_t *) data->quadVertexBufferPtr - (uint8_t *) data->quadVertexBufferBase;
			data->quadVertexBufferPtr = data->quadVertexBufferBase;
			data->quadVertexBuffer->SetData(data->quadVertexBufferBase, dataSize);
		}
		{
			uint32_t dataSize = (uint8_t *) data->circleVertexBufferPtr - (uint8_t *) data->circleVertexBufferBase;
			data->circleVertexBufferPtr = data->circleVertexBufferBase;
			data->circleVertexBuffer->SetData(data->circleVertexBufferBase, dataSize);
		}
		{
			uint32_t dataSize = (uint8_t *) data->lineVertexBufferPtr - (uint8_t *) data->lineVertexBufferBase;
			data->lineVertexBufferPtr = data->lineVertexBufferBase;
			data->lineVertexBuffer->SetData(data->lineVertexBufferBase, dataSize);
		}
		Flush();
	}

	void Renderer2D::DrawQuad(const glm::mat4 &transform, const SpriteRendererComponent &sprite, int EntityID) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			EndScene();
		}
		const glm::vec2 texCords[] = {glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
		                              glm::vec2(0.0f, 1.0f)};
		int textureIndex = 0;

		if (sprite.texture != nullptr) {
			for (int i = 0; i < data->textureSlotIndex; ++i) {
				if (*data->textureSlots[i] == *(sprite.texture)) {
					textureIndex = i;
					break;
				}
			}
			if (textureIndex == 0) {
				textureIndex = data->textureSlotIndex++;
				data->textureSlots[textureIndex] = (sprite.texture);
				stats->TextureCount++;
			}
		}

		constexpr float tilingFactor = 1.0f;
		for (int i = 0; i < 4; i++) {
			data->quadVertexBufferPtr->position = glm::vec3(transform * data->quadVertexPositions[i]);
			data->quadVertexBufferPtr->color = sprite.color;
			data->quadVertexBufferPtr->texCoord = texCords[i];
			data->quadVertexBufferPtr->texIndex = textureIndex;
			data->quadVertexBufferPtr->tillingFactor = tilingFactor;
			data->quadVertexBufferPtr->EntityID = EntityID;
			data->quadVertexBufferPtr++;
		}

		data->quadIndexCount += 6;

		stats->QuadCount++;

	}

	void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, float tilingFactor,
	                          const glm::vec4 &tintCol) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			//Z_CORE_WARN("Max index count reached,Begin New Batch");
			EndScene();
		}

		int textureIndex = 0;
		for (int i = 0; i < data->textureSlotIndex; ++i) {
			if (*data->textureSlots[i] == *texture) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0) {
			textureIndex = data->textureSlotIndex++;
			data->textureSlots[textureIndex] = texture;
			stats->TextureCount++;
		}

		glm::vec2 texCords[] = {glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f),
		                        glm::vec2(0.0f, 1.0f)};

		for (int i = 0; i < 4; i++) {
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


	void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<SubTex2D> &texture, float tilingFactor,
	                          const glm::vec4 &tintCol) {
		if (data->quadIndexCount >= data->MaxIndexCount) {
			//Z_CORE_WARN("Max index count reached,Begin New Batch");
			EndScene();
		}

		int textureIndex = 0;
		for (int i = 0; i < data->textureSlotIndex; ++i) {
			if (*data->textureSlots[i] == *texture->GetTex()) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0) {
			textureIndex = data->textureSlotIndex++;
			data->textureSlots[textureIndex] = texture->GetTex();
			stats->TextureCount++;
		}

		auto texCords = texture->GetCords();

		for (int i = 0; i < 4; i++) {
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

	void Renderer2D::DrawCircle(const glm::mat4 &transform, const CircleRendererComponent &circle, int EntityID) {
		DrawCircle(transform,circle.color,circle.thickness,circle.fade,EntityID);
	}

	void Renderer2D::DrawLine(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec4 &color, int entityID) {
		data->lineVertexBufferPtr->Position = v0;
		data->lineVertexBufferPtr->color = color;
		data->lineVertexBufferPtr->EntityID = entityID;
		data->lineVertexBufferPtr++;
		data->lineVertexBufferPtr->Position = v1;
		data->lineVertexBufferPtr->color = color;
		data->lineVertexBufferPtr->EntityID = entityID;
		data->lineVertexBufferPtr++;
		data->lineIndexCount += 2;
	}

	void Renderer2D::DrawLine(const glm::mat4 &transform, const glm::vec3 &center, const glm::vec3 &size,
	                          const glm::vec4 &color, int entityID) {
		glm::vec3 v0 = glm::vec3(transform * glm::vec4(center - size / 2.f, 1));
		glm::vec3 v1 = glm::vec3(transform * glm::vec4(center + size / 2.f, 1));
		DrawLine(v0, v1, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4 &transform, const glm::vec3 &size,
	                          const glm::vec4 &color, int entityID) {
		glm::vec3 v0 = glm::vec3(transform * (data->quadVertexPositions[0] * (size, 1.f)));
		glm::vec3 v1 = glm::vec3(transform * (data->quadVertexPositions[1] * (size, 1.f)));
		glm::vec3 v2 = glm::vec3(transform * (data->quadVertexPositions[2] * (size, 1.f)));
		glm::vec3 v3 = glm::vec3(transform * (data->quadVertexPositions[3] * (size, 1.f)));
		DrawLine(v0, v1, color, entityID);
		DrawLine(v1, v2, color, entityID);
		DrawLine(v2, v3, color, entityID);
		DrawLine(v3, v0, color, entityID);
		stats->QuadCount++;
	}


	void
	Renderer2D::DrawRect(const glm::vec3 &leftBottom, const glm::vec3 &rightTop, const glm::vec4 &color, int entityID) {
		DrawLine(leftBottom, {rightTop.x, leftBottom.y, rightTop.z}, color, entityID);
		DrawLine({rightTop.x, leftBottom.y, rightTop.z}, rightTop, color, entityID);
		DrawLine(rightTop, {leftBottom.x, rightTop.y, leftBottom.z}, color, entityID);
		DrawLine({leftBottom.x, rightTop.y, leftBottom.z}, leftBottom, color, entityID);

		stats->QuadCount++;
	}

	void Renderer2D::DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness,float fade, int EntityID) {
		for (auto quadVertexPosition: data->quadVertexPositions) {
			data->circleVertexBufferPtr->WorldPosition = glm::vec3(transform * quadVertexPosition);
			data->circleVertexBufferPtr->LocalPosition = glm::vec3(quadVertexPosition) * 2.f;
			data->circleVertexBufferPtr->color = color;
			data->circleVertexBufferPtr->thickness = thickness;
			data->circleVertexBufferPtr->fade = fade;
			data->circleVertexBufferPtr->EntityID = EntityID;
			data->circleVertexBufferPtr++;
		}
		data->circleIndexCount += 6;
		stats->QuadCount++;
	}

	void Renderer2D::ChangeDepthTest(RenderAPI::DepthTestState state) {
		RenderCommand::ChangeDepthTest(state);
	}


}