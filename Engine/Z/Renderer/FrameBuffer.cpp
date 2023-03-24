//
// Created by 32725 on 2023/3/23.
//

#include "FrameBuffer.h"
#include "Z/Renderer/RenderAPI.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Z {
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification &specification) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::None: Z_CORE_ASSERT(false, "RenderAPI::None is not supported!");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLFrameBuffer>(specification);
		}
		Z_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}
