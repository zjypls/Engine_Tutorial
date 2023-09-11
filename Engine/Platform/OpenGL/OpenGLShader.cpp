//
// Created by 32725 on 2023/3/18.
//

#include <filesystem>
#include "OpenGLShader.h"
#include "glad/glad.h"
#include "Z/Core/Log.h"
#include "Z/Core/Core.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include<unordered_map>
#include "shaderc/shaderc.hpp"
#include "Z/Core/Time.h"
#include "spirv_cross/spirv_cross.hpp"
#include "spirv_cross/spirv_glsl.hpp"

namespace Z {
	const std::string ShaderCacheDir = "Shaders/Cache/OpenGL";
	namespace Temp {

		std::string spirvShaderTypeToString(shaderc_shader_kind type) {
			switch (type) {
				case shaderc_glsl_vertex_shader:
					return "shaderc_glsl_vertex_shader";
				case shaderc_glsl_fragment_shader:
					return "shaderc_glsl_fragment_shader";
				case shaderc_glsl_geometry_shader:
					return "shaderc_glsl_geometry_shader";
				default: Z_CORE_ASSERT(false, "Unknown shader type");
					return "unknown";
			}
		}

		shaderc_shader_kind StringToSpirvShaderType(const std::string &type) {
			if (type == "shaderc_glsl_vertex_shader") {
				return shaderc_glsl_vertex_shader;
			} else if (type == "shaderc_glsl_fragment_shader") {
				return shaderc_glsl_fragment_shader;
			} else if (type == "shaderc_glsl_geometry_shader")
				return shaderc_glsl_geometry_shader;
			else {
				Z_CORE_ASSERT(false, "Unknown shader type");
				return shaderc_glsl_vertex_shader;
			}
		}

		bool IsHaveCache(const size_t &hash, shaderc_shader_kind type) {
			if (!std::filesystem::exists(ShaderCacheDir)) {
				std::filesystem::create_directories(ShaderCacheDir);
				return false;
			}
			std::string path = ShaderCacheDir + "/" + std::to_string(hash) +"."+
			                   spirvShaderTypeToString(type);
			return std::filesystem::exists(path);
		}

		bool GetCache(const size_t &hash, std::vector<char> &buffer, shaderc_shader_kind type) {
			if (IsHaveCache(hash, type)) {
				std::string path =ShaderCacheDir + "/" + std::to_string(hash) +"."+
				                   spirvShaderTypeToString(type);
				std::ifstream in(path, std::ios::in | std::ios::binary | std::ios::ate);
				if (in.is_open()) {
					Z_CORE_INFO("Get shader cache:{0}", path);
					buffer.resize(in.tellg());
					memset(&buffer[0], 0, buffer.size());
					in.seekg(0);
					in.read(&buffer[0], buffer.size());
					in.close();
					return true;
				} else {
					Z_CORE_ERROR("Failed to open shader file:{0}", path);
					return false;
				}
			}
			return false;
		}

		std::string ReadFile(const std::string &path) {
			std::ifstream in(path, std::ios::in | std::ios::binary | std::ios::ate);
			std::string res;
			if (in.is_open()) {
				res.resize(in.tellg());
				in.seekg(0);
				in.read(&res[0], res.size());
				in.close();
			} else {
				Z_CORE_ERROR("Failed to open shader file:{0}", path);
			}
			return res;
		}

		RenderAPI::zShaderType GetShaderTypeFromString(const std::string &type) {
			if (type == "vertex") {
				return RenderAPI::zShaderType::Vertex;
			} else if (type == "fragment" || type == "pixel") {
				return RenderAPI::zShaderType::Fragment;
			} else if (type == "geometry")
				return RenderAPI::zShaderType::Geometry;
			Z_CORE_ASSERT(false, "Unknown Shader Type");
			return RenderAPI::zShaderType::None;
		}

		std::unordered_map<RenderAPI::zShaderType, std::string> PreProcessCode(const std::string &code) {
			std::unordered_map<RenderAPI::zShaderType, std::string> result;
			const std::string filter = "#type";
			size_t filterSize = filter.size();
			size_t pos = code.find(filter, 0);
			while (pos != std::string::npos) {
				size_t newLinePos = code.find_first_of("\r\n", pos);
				Z_CORE_ASSERT(newLinePos != std::string::npos, "Syntax Error");
				size_t begin = pos + filterSize + 1;
				auto type = GetShaderTypeFromString(code.substr(begin, newLinePos - begin));
				Z_CORE_ASSERT(type != RenderAPI::zShaderType::None, "Unknown Shader Type");
				pos = code.find(filter, newLinePos);
				result[type] = code.substr(newLinePos, pos - newLinePos);
			}
			return result;
		}

		GLenum ShaderTypeToOpenGLType(RenderAPI::zShaderType type) {
			switch (type) {
				case RenderAPI::zShaderType::Vertex:
					return GL_VERTEX_SHADER;
				case RenderAPI::zShaderType::Fragment:
					return GL_FRAGMENT_SHADER;
				case RenderAPI::zShaderType::Geometry:
					return GL_GEOMETRY_SHADER;
				case RenderAPI::zShaderType::Compute:
					return GL_COMPUTE_SHADER;
				case RenderAPI::zShaderType::TessellationControl:
					return GL_TESS_CONTROL_SHADER;
				case RenderAPI::zShaderType::TessellationEvaluation:
					return GL_TESS_EVALUATION_SHADER;
			}
			Z_CORE_ASSERT(false, "Unknown Shader Type");
			return 0;
		}

		shaderc_shader_kind ShaderTypeToSpirVType(RenderAPI::zShaderType type) {
			switch (type) {
				case RenderAPI::zShaderType::Vertex:
					return shaderc_glsl_vertex_shader;
				case RenderAPI::zShaderType::Fragment:
					return shaderc_glsl_fragment_shader;
				case RenderAPI::zShaderType::Geometry:
					return shaderc_glsl_geometry_shader;
				case RenderAPI::zShaderType::Compute:
					return shaderc_glsl_compute_shader;
				case RenderAPI::zShaderType::TessellationControl:
					return shaderc_glsl_tess_control_shader;
				case RenderAPI::zShaderType::TessellationEvaluation:
					return shaderc_glsl_tess_evaluation_shader;
			}
			Z_CORE_ASSERT(false, "Unknown Shader Type");
			return shaderc_glsl_infer_from_source;
		}

	}

	OpenGLShader::OpenGLShader(const std::string &Src, const std::string &name, bool isFile) {
		Name=name;
		auto src = Src;
		if (isFile) {
			src = Temp::ReadFile(Src);
			if (Name.empty()) {
				auto lastDot = Src.find_last_of('.');
				Name = Src.substr(0, lastDot);
			}
		}
		ProgramID = glCreateProgram();
		Z_CORE_INFO("Shader Name:\"{0}\",ID:\"{1}\"", Name, ProgramID);
		AddShader(src);
		Compile();

		auto warn = glGetError();
		if (warn != 0)
			Z_CORE_WARN("{1}:{2},Warn {0}", warn, __FILE__, __LINE__);
	}

	OpenGLShader::OpenGLShader(const std::string &name, const std::string &VertSrc, const std::string &FragSrc,
	                           bool isFile)  {
		Name=name;
		auto vertSrc = VertSrc;
		auto fragSrc = FragSrc;
		if (isFile) {
			vertSrc = Temp::ReadFile(VertSrc);
			fragSrc = Temp::ReadFile(FragSrc);
		}
		ProgramID = glCreateProgram();
		Z_CORE_INFO("Shader Name:\"{0}\",ID:\"{1}\"", Name, ProgramID);
		AddShader(vertSrc, RenderAPI::zShaderType::Vertex);
		AddShader(fragSrc, RenderAPI::zShaderType::Fragment);
		Compile();
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(ProgramID);
	}


	void OpenGLShader::AddShader(const std::string &shaderSrc, RenderAPI::zShaderType shaderType) {
		std::vector<char> spirv;
		auto hash = std::hash<std::string>()(shaderSrc);
		if (auto spvType= Temp::ShaderTypeToSpirVType(shaderType);
		!Temp::GetCache(hash, spirv, spvType)){
			Z_CORE_WARN("Can't find shader cache,compile shader:{0}", Name);
			auto start = Time::GetTime();
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
			//options.SetGenerateDebugInfo();
			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderSrc,
																			 spvType,std::to_string (hash).c_str(),options);

			if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
				Z_CORE_ERROR("Shader Compile Error:{0}", module.GetErrorMessage());
				Z_CORE_ASSERT(false, "Shader Compile Error");
			}
			std::ofstream out;
			out.open(ShaderCacheDir+"/"+std::to_string(hash)+"."+Temp::spirvShaderTypeToString(spvType), std::ios::binary);
			out.write((char *) module.cbegin(), (module.cend()-module.cbegin())*sizeof (unsigned int));
			out.close();
			spirv.resize((module.cend() - module.cbegin())*sizeof(unsigned int));
			std::memcpy(spirv.data(), module.cbegin(), spirv.size());
			auto spend=Time::GetTime()-start;
			Z_CORE_WARN("Compile Shader spend time:{1}s",Name,spend);
		}

		//std::vector<unsigned int> spirv(module.cbegin(), module.cend());

		unsigned int shader = glCreateShader(Temp::ShaderTypeToOpenGLType(shaderType));
		glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size());
		glSpecializeShader(shader, "main", 0, nullptr, nullptr);


		Compile(shader);
		glAttachShader(ProgramID, shader);
		glDeleteShader(shader);
	}

	void OpenGLShader::Bind() const {

		auto warn = glGetError();
		if (warn != 0) {
			Z_CORE_ERROR("{1}:{2},OpenGL ERROR {0}", warn, __FILE__, __LINE__);
		}
		glUseProgram(ProgramID);
	}

	void OpenGLShader::UnBind() const {
		glUseProgram(0);
	}

	void OpenGLShader::Compile() {
		glLinkProgram(ProgramID);
		auto warn = glGetError();
		if (warn != 0)
			Z_CORE_WARN("{1}:{2},Warn {0}", warn, __FILE__, __LINE__);
		int success = 0;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);

		if (!success) {
			int size = 0;
			glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &size);
			std::string infoLog(size, ' ');
			glGetProgramInfoLog(ProgramID, size, nullptr, &infoLog[0]);
			glDeleteProgram(ProgramID);
			Z_CORE_ERROR("Shader Link Error: {0}", infoLog);
			Z_CORE_ASSERT(0, "Shader Link Error");
		}
	}

	void OpenGLShader::SetUniform(const char *name, int value) {
		glUseProgram(ProgramID);
		glUniform1i(glGetUniformLocation(ProgramID, name), value);
	}

	void OpenGLShader::SetUniform(const char *name, float value) {
		glUseProgram(ProgramID);
		glUniform1f(glGetUniformLocation(ProgramID, name), value);
	}

	void OpenGLShader::SetUniform(const char *name, const glm::vec3 &value) {
		glUseProgram(ProgramID);
		glUniform3fv(glGetUniformLocation(ProgramID, name), 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniform(const char *name, const glm::vec2 &value) {
		glUseProgram(ProgramID);
		glUniform2fv(glGetUniformLocation(ProgramID, name), 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniform(const char *name, const glm::vec4 &value) {
		glUseProgram(ProgramID);
		glUniform4fv(glGetUniformLocation(ProgramID, name), 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUniform(const char *name, const glm::mat4 &value) {
		glUseProgram(ProgramID);
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, name), 1, GL_FALSE, glm::value_ptr(value));
	}


	void OpenGLShader::SetUniform(const char *name, const glm::mat3 &value) {
		glUseProgram(ProgramID);
		glUniformMatrix3fv(glGetUniformLocation(ProgramID, name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::AddShader(const std::string &Code) {
		auto map = Temp::PreProcessCode(Code);
		for (const auto &[type, code]: map) {
			AddShader(code, type);
		}
	}

	void OpenGLShader::Compile(unsigned int shader) {
		//glCompileShader(shader);
		int success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			int size = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
			std::string infoLog(size, ' ');
			glGetShaderInfoLog(shader, size, nullptr, &infoLog[0]);
			glDeleteShader(shader);
			Z_CORE_ERROR("Shader Compile Error: {0}", infoLog);
			Z_CORE_ASSERT(0, "Shader Compile Error");
		}
	}

}