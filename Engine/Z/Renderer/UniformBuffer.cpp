//
// Created by 32725 on 2023/4/3.
//

#include "UniformBuffer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "RenderAPI.h"

namespace Z {

	Ref<UniformBuffer> UniformBuffer::Create(unsigned int size, unsigned int binding) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			default:
				return nullptr;
		}
	}
}