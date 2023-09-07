//
// Created by 32725 on 2023/3/17.
//
#include "Z/Core/Log.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Z{
	uint32_t GetShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:
				return 4;
			case ShaderDataType::Float2:
				return 4 * 2;
			case ShaderDataType::Float3:
				return 4 * 3;
			case ShaderDataType::Float4:
				return 4 * 4;
			case ShaderDataType::Mat3:
				return 4 * 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4 * 4;
			case ShaderDataType::Int:
				return 4;
			case ShaderDataType::Int2:
				return 4 * 2;
			case ShaderDataType::Int3:
				return 4 * 3;
			case ShaderDataType::Int4:
				return 4 * 4;
			case ShaderDataType::Bool:
				return 1;
		}
		Z_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
	Ref<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RenderAPI::API::None: Z_CORE_ASSERT(false,"RenderAPI::None is not supported!"); return nullptr;
			case RenderAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices,size);
		}
		Z_CORE_ASSERT(false,"Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RenderAPI::API::None: Z_CORE_ASSERT(false,"RenderAPI::None is not supported!"); return nullptr;
			case RenderAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}
		Z_CORE_ASSERT(false,"Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count) {
		switch (Renderer::GetAPI()) {
			case RenderAPI::API::None: Z_CORE_ASSERT(false,"RenderAPI::None is not supported!"); return nullptr;
			case RenderAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices,count);
		}

		Z_CORE_ASSERT(false,"Unknown RenderAPI!");
		return nullptr;
	}

	uint32_t BufferElement::GetComponentCount() const {
		switch (Type) {
			case ShaderDataType::Float:
				return 1;
			case ShaderDataType::Float2:
				return 2;
			case ShaderDataType::Float3:
				return 3;
			case ShaderDataType::Float4:
				return 4;
			case ShaderDataType::Mat3:
				return 3 * 3;
			case ShaderDataType::Mat4:
				return 4 * 4;
			case ShaderDataType::Int:
				return 1;
			case ShaderDataType::Int2:
				return 2;
			case ShaderDataType::Int3:
				return 3;
			case ShaderDataType::Int4:
				return 4;
			case ShaderDataType::Bool:
				return 1;
		}
		Z_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	void BufferLayout::CalculateOffsetsAndStride() {
		uint32_t offset = 0;
		Stride = 0;
		for (auto &element : Elements) {
			element.Offset = offset;
			offset += element.Size;
			Stride += element.Size;
		}

	}
}