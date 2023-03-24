//
// Created by 32725 on 2023/3/17.
//

#include "VertexArray.h"
#include "Z/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Z {
	Ref<VertexArray> Z::VertexArray::Create() {
		switch (Renderer::GetAPI()) {
			case RenderAPI::API::None:
				Z_CORE_ASSERT(false, "RendererAPI::None is not supported!");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLVertexArray>();
		}
		Z_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}