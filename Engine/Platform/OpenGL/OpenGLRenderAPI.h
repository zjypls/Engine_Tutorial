//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_OPENGLRENDERAPI_H
#define ENGINE_TUTORIAL_OPENGLRENDERAPI_H
#include "Z/Renderer/RenderAPI.h"

namespace Z {
	class OpenGLRenderAPI :public RenderAPI{

	public:
		virtual void Init() override;
		virtual void SetClearValue(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void GetError() override;
		virtual void ChangeDepthTest(DepthTestState state = DepthTestState::Less) override;
		virtual void SetLineWidth(float width = 0.f) override;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray,unsigned int count) override;
		virtual void DrawLine(const Ref<VertexArray>& vertexArray,unsigned int count) override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

	};

}


#endif //ENGINE_TUTORIAL_OPENGLRENDERAPI_H
