//
// Created by 32725 on 2023/3/18.
//

#include "Texture.h"
#include "Platform/OpenGL/OpenGLTextures.h"
#include "Renderer.h"

namespace Z {
	Ref<Texture2D> Texture2D::CreateTexture(const std::string &path) {
		Z_CORE_INFO("Create Texture2D with path {0}.",path);
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::None:
				Z_CORE_ASSERT(false, "RenderAPI::None is not supported!");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2D>(path);
		}
		Z_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::CreateTexture(unsigned int width, unsigned int height) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::None:
			Z_CORE_ASSERT(false, "RenderAPI::None is not supported!");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLTexture2D>(width,height);
		}
		Z_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}
