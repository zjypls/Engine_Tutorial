//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
#define ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
#include "Include/glad/include/glad/glad.h"
#include "Z/Renderer/VertexArray.h"

namespace Z {
	GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);
	class OpenGLVertexArray final : public VertexArray {
		public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffers; }

	private:
		unsigned int m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffers;
	};
}

#endif //ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
