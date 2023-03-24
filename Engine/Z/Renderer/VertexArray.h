//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_VERTEXARRAY_H
#define ENGINE_TUTORIAL_VERTEXARRAY_H
#include "Z/Core/Core.h"
#include "Buffer.h"
namespace Z {
	class Z_API VertexArray {

	public:
		virtual ~VertexArray()=default;
		virtual void Bind() const=0;
		virtual void Unbind() const=0;
		virtual void AddVertexBuffer(const Z::Ref<VertexBuffer>& vertexBuffer)=0;
		virtual void SetIndexBuffer(const Z::Ref<IndexBuffer>& indexBuffer)=0;
		virtual const std::vector<Z::Ref<VertexBuffer>>& GetVertexBuffers() const=0;
		virtual const Z::Ref<IndexBuffer>& GetIndexBuffer() const=0;
		static Ref<VertexArray> Create();
	};

}


#endif //ENGINE_TUTORIAL_VERTEXARRAY_H
