//
// Created by z on 2023/9/7.
//

#include "./VulkanShader.h"

namespace Z{

	void VulkanShader::Bind() const {

	}

	void VulkanShader::UnBind() const {

	}

	std::string VulkanShader::GetName() const {
		return std::string();
	}

	void VulkanShader::SetUniform(const char *name, int value) {

	}

	void VulkanShader::SetUniform(const char *name, float value) {

	}

	void VulkanShader::SetUniform(const char *name, const glm::vec3 &value) {

	}

	void VulkanShader::SetUniform(const char *name, const glm::vec2 &value) {

	}

	void VulkanShader::SetUniform(const char *name, const glm::vec4 &value) {

	}

	void VulkanShader::SetUniform(const char *name, const glm::mat4 &value) {

	}

	void VulkanShader::SetUniform(const char *name, const glm::mat3 &value) {

	}
}