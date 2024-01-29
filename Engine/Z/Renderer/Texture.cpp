//
// Created by 32725 on 2023/3/18.
//

#include "Z/Renderer/Texture.h"
#include "Z/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTextures.h"

namespace Z {
	Ref<Texture2D> Texture2D::CreateTexture(const std::string &path) {
		Z_CORE_INFO("Create Texture2D with path {0}.", path);
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
				return CreateRef<OpenGLTexture2D>(width, height);
		}
		Z_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture3D> Texture3D::CreateTexture(unsigned int width, unsigned int height, unsigned int depth) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLTexture3D>(width, height, depth);
		}
	}

	Ref<CubeMap> CubeMap::CreateTexture(const std::vector<std::string> &paths) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLSkyBox>(paths);
		}
	}

	Ref<CubeMap> CubeMap::CreateTexture(unsigned int width, unsigned int height) {
		switch (RenderAPI::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return CreateRef<OpenGLSkyBox>(width, height);
		}
	}
}
