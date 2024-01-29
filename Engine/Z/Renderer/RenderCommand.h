//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_RENDERCOMMAND_H
#define ENGINE_TUTORIAL_RENDERCOMMAND_H

#include "Include/glm/glm/glm.hpp"

#include "Z/Core/Core.h"
#include "Z/Renderer/VertexArray.h"
#include "Z/Renderer/RenderAPI.h"

namespace Z {
	class Z_API RenderCommand {
		static Scope<RenderAPI> s_RenderAPI;
	public:
		static void Init();

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RenderAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearValue(const glm::vec4 &color) { s_RenderAPI->SetClearValue(color); };

		inline static void Clear() { s_RenderAPI->Clear(); }

		inline static void DrawIndexed(const Ref<VertexArray> &vertexArray, unsigned int count = 0) {
			s_RenderAPI->DrawIndexed(vertexArray, count);
		}
		inline static void DrawLine(const Ref<VertexArray> &vertexArray, unsigned int count) {
			s_RenderAPI->DrawLine(vertexArray, count);
		}
		static void ChangeDepthTest(RenderAPI::DepthTestState state = RenderAPI::DepthTestState::Less){
			s_RenderAPI->ChangeDepthTest(state);
		}
		static void DetachFrameBuffer() {
			s_RenderAPI->DetachFrameBuffer();
		}
	};

}


#endif //ENGINE_TUTORIAL_RENDERCOMMAND_H
