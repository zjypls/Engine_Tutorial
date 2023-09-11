//
// Created by z on 2023/9/7.
//

#ifndef ENGINE_TUTORIAL_VULKANSHADER_H
#define ENGINE_TUTORIAL_VULKANSHADER_H
#include "Z/Renderer/Shader.h"

namespace Z {
	class VulkanShader final :public Shader{
	public:
		~VulkanShader() override = default;

		void Bind() const override;

		void UnBind() const override;

		std::string GetName() const override;

		void SetUniform(const char *name, int value) override;

		void SetUniform(const char *name, float value) override;

		void SetUniform(const char *name, const glm::vec3 &value) override;

		void SetUniform(const char *name, const glm::vec2 &value) override;

		void SetUniform(const char *name, const glm::vec4 &value) override;

		void SetUniform(const char *name, const glm::mat4 &value) override;

		void SetUniform(const char *name, const glm::mat3 &value) override;
	};

}


#endif //ENGINE_TUTORIAL_VULKANSHADER_H
