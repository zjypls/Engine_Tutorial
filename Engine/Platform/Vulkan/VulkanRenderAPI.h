//
// Created by z on 2023/9/7.
//

#ifndef ENGINE_TUTORIAL_VULKANRENDERAPI_H
#define ENGINE_TUTORIAL_VULKANRENDERAPI_H

#include "Z/Renderer/RenderAPI.h"

namespace Z {
	class VulkanRenderAPI final: public RenderAPI{
	public:
		void Init() override;

		void SetClearValue(const glm::vec4 &vec4) override;

		void Clear() override;

		void ChangeDepthTest(DepthTestState state) override;

		void DetachFrameBuffer()override;

		void SetLineWidth(float d) override;

		void DrawIndexed(const Ref<VertexArray> &vertexArray, unsigned int count) override;

		void DrawLine(const Ref<VertexArray> &vertexArray, unsigned int count) override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	};

}

#endif //ENGINE_TUTORIAL_VULKANRENDERAPI_H
