//
// Created by z on 2023/9/7.
//

#ifndef ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
#define ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H

#include "Z/Renderer/VertexArray.h"

namespace Z {
	class VulkanVertexArray final : public VertexArray{
	public:
		~VulkanVertexArray() override = default;

		void Bind() const override;

		void Unbind() const override;

		void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) override;

		void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) override;

		const std::vector<Z::Ref<VertexBuffer>> &GetVertexBuffers() const override;

		const Ref<IndexBuffer> &GetIndexBuffer() const override;
	};

}


#endif //ENGINE_TUTORIAL_OPENGLVERTEXARRAY_H
