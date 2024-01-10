//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_OPENGLBUFFER_H
#define ENGINE_TUTORIAL_OPENGLBUFFER_H


#include <cstdint>
#include "Include/glad/include/glad/glad.h"
#include "Z/Renderer/Buffer.h"
namespace Z{

	class OpenGLVertexBuffer:public VertexBuffer {
		unsigned int m_RendererID;
		BufferLayout Layout;
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(uint32_t size);

		~OpenGLVertexBuffer() override;
		void Bind() const override;
		void Unbind() const override;
		void SetLayout(const BufferLayout& layout) override { Layout = layout; }
		void SetData(const void* data, uint32_t size) override;
		[[nodiscard]] const BufferLayout& GetLayout() const override { return Layout; }
	};

	class OpenGLIndexBuffer:public IndexBuffer {
		unsigned int m_RendererID;
		uint32_t m_Count;
		public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();
		virtual void Bind() const override;
		virtual void Unbind() const override;
		inline virtual uint32_t GetCount() const override { return m_Count; }
	};
}


#endif //ENGINE_TUTORIAL_OPENGLBUFFER_H
