//
// Created by z on 2023/9/7.
//

#include "./VulkanVertexArray.h"


namespace Z{
	void VulkanVertexArray::Bind() const {

	}

	void VulkanVertexArray::Unbind() const {

	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) {

	}

	void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) {

	}

	const std::vector<Z::Ref<VertexBuffer>> &VulkanVertexArray::GetVertexBuffers() const {
		return {};
	}

	const Ref<IndexBuffer> &VulkanVertexArray::GetIndexBuffer() const {
		return nullptr;
	}
}