//
// Created by 32725 on 2023/3/17.
//

#include "Include/glad/include/glad/glad.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Z {

	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
				return GL_FLOAT;
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
				return GL_FLOAT;
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
				return GL_INT;
			case ShaderDataType::Bool:
				return GL_BOOL;
		}
		Z_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const {
		glBindVertexArray(m_RendererID);
		for (const auto &vertexBuffer: m_VertexBuffers) {
			vertexBuffer->Bind();
		}
		m_IndexBuffers->Bind();
	}

	void OpenGLVertexArray::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) {
		Z_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto &layout = vertexBuffer->GetLayout();
		uint32_t m_VertexBufferIndex = 0;
		for (const auto &element: layout) {
			glEnableVertexAttribArray(m_VertexBufferIndex);

			switch (element.Type) {
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
					glVertexAttribPointer(m_VertexBufferIndex,
					                      element.GetComponentCount(),
					                      ShaderDataTypeToOpenGLBaseType(element.Type),
					                      element.Normalized ? GL_TRUE : GL_FALSE,
					                      layout.GetStride(),
					                      (const void *) element.Offset);
					break;
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
					glVertexAttribIPointer(m_VertexBufferIndex,
					                       element.GetComponentCount(),
					                       ShaderDataTypeToOpenGLBaseType(element.Type),
					                       layout.GetStride(),
					                       (const void *) element.Offset);
					break;
				default: Z_CORE_ASSERT(false, "Unknown ShaderDataType!");
					return;
			}
			m_VertexBufferIndex++;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) {
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffers = indexBuffer;
	}
}