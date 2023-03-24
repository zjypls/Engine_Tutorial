//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
#define ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
#include "glad/glad.h"
#include "Z/Renderer/VertexArray.h"

namespace Z {
	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
	class OpenGLVertexArray : public VertexArray {
		public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffers; }

	private:
		unsigned int m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffers;
	};
}

#endif //ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
