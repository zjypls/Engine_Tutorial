//
// Created by 32725 on 2023/3/16.
//
#include "Z/Core/Log.h"
#include "Z/Core/Core.h"
#include "Shader.h"
#include "glad/glad.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>

namespace Z {
	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::Shaders;

	Ref<Shader> Shader::CreateShader(const std::string &Src, const std::string &name, bool isFile) {
		switch (Renderer::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(Src, name, isFile);
			default: Z_CORE_ASSERT(0, "Unknown RendererAPI");
				return nullptr;
		}
	}

	Ref<Shader>
	Shader::CreateShader(const std::string &name, const std::string &vertSrc, const std::string &fragSrc, bool isFile) {
		switch (Renderer::GetAPI()) {
			case RenderAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(name, vertSrc, fragSrc, isFile);
			default: Z_CORE_ASSERT(0, "Unknown RendererAPI");
				return nullptr;
		}
	}

	std::string ShaderLibrary::GetDefaultName() {
		auto size = Shaders.size();
		auto name = "Shader" + std::to_string(size);
		while (Shaders.find(name) != Shaders.end()) {
			size++;
			name = "Shader" + std::to_string(size);
		}
		return name;
	}

	Ref<Shader> ShaderLibrary::Add(const Ref<Shader> &shader) {
		auto name = shader->GetName();
		if (Exists(name)) {
			Z_CORE_WARN("Shader {0} already exists", name);
			auto NewName = GetDefaultName();
			Z_CORE_WARN("Shader {0} has been renamed to {1}", name, NewName);
			Shaders[NewName] = shader;
			return shader;
		}
		Shaders[name] = shader;
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string &ShaderSrc, const std::string &name, bool isFile) {
		return Add(Shader::CreateShader(ShaderSrc, name, isFile));
	}

	Ref<Shader>
	ShaderLibrary::Load(const std::string &name, const std::string &vertSrc, const std::string &fragSrc, bool isFile) {
		return Add(Shader::CreateShader(name, vertSrc, fragSrc, isFile));
	}
}
