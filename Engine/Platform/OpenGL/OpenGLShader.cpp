//
// Created by 32725 on 2023/3/18.
//

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "shaderc/shaderc.hpp"
#include "spirv_cross/spirv_cross.hpp"
#include "Include/glad/include/glad/glad.h"
#include "Include/glm/glm/gtc/type_ptr.hpp"
#include "Z/Core/Log.h"
#include "Z/Core/Core.h"
#include "Z/Core/Time.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Z {
	const std::string ShaderCacheDir = "ShaderCaches/Cache/OpenGL";
	namespace Tools {

		std::string GetShaderStageMacroInfo(RenderAPI::zShaderType type){
			switch (type)
			{
				case RenderAPI::zShaderType::Vertex:
					return "Z_VERTEX";
				case RenderAPI::zShaderType::Fragment:
					return "Z_FRAGMENT";
				case RenderAPI::zShaderType::Geometry:
					return "Z_GEOMETRY";
				case RenderAPI::zShaderType::Compute:
					return "Z_COMPUTE";
				case RenderAPI::zShaderType::TessellationControl:
					return "Z_TESS_CONTROL";
				case RenderAPI::zShaderType::TessellationEvaluation:
					return "Z_TESS_EVALUATION";
				default:
					Z_CORE_ASSERT(false,"Unknown Shader Type");
					return "Z_UNKNOWN";
			}
		}

		std::vector<RenderAPI::zShaderType> findShaderTypeTotalFromSource(const std::string&src){
			std::vector<RenderAPI::zShaderType> result;
			if(src.find("Z_VERTEX")!=std::string::npos)
				result.push_back(RenderAPI::zShaderType::Vertex);
			if(src.find("Z_FRAGMENT")!=std::string::npos)
				result.push_back(RenderAPI::zShaderType::Fragment);
			if(src.find("Z_GEOMETRY")!=std::string::npos)
				result.push_back(RenderAPI::zShaderType::Geometry);
			if(src.find("Z_COMPUTE")!=std::string::npos)
				result.push_back(RenderAPI::zShaderType::Compute);
			if(src.find("Z_TESS_CONTROL")!=std::string::npos)
				result.push_back(RenderAPI::zShaderType::TessellationControl);
			if(src.find("Z_TESS_EVALUATION")!=std::string::npos)
				result.push_back(RenderAPI::zShaderType::TessellationEvaluation);
			return result;
		}

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
			src = Tools::ReadFile(Src);
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

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(ProgramID);
	}


	void OpenGLShader::AddShader(const std::string &shaderSrc) {
		auto hash = std::hash<std::string>()(shaderSrc);
		auto types = Tools::findShaderTypeTotalFromSource(shaderSrc);
		for(auto shaderType:types){
			std::vector<char> spirv;
			if (auto spvType= Tools::ShaderTypeToSpirVType(shaderType);
			!Tools::GetCache(hash, spirv, spvType)){
				Z_CORE_WARN("Can't find shader cache,compile shader:{0}", Name);
				auto start = Time::GetTime();
				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetOptimizationLevel(shaderc_optimization_level_performance);
				options.AddMacroDefinition(Tools::GetShaderStageMacroInfo(shaderType));
				//options.SetGenerateDebugInfo();
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderSrc,
																				spvType,std::to_string (hash).c_str(),options);

				if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
					Z_CORE_ERROR("Shader Compile Error:{0}", module.GetErrorMessage());
					Z_CORE_ASSERT(false, "Shader Compile Error");
				}

				std::ofstream out;
				out.open(ShaderCacheDir + "/" + std::to_string(hash) + "." + Tools::spirvShaderTypeToString(spvType), std::ios::binary);
				out.write((char *) module.cbegin(), (module.cend()-module.cbegin())*sizeof (unsigned int));
				out.close();
				spirv.resize((module.cend() - module.cbegin())*sizeof(unsigned int));
				std::memcpy(spirv.data(), module.cbegin(), spirv.size());
				auto spend=Time::GetTime()-start;
				Z_CORE_WARN("Compile Shader spend time:{0} s",spend);
			}

			unsigned int shader = glCreateShader(Tools::ShaderTypeToOpenGLType(shaderType));
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size());
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);


			Compile(shader);
			glAttachShader(ProgramID, shader);
			glDeleteShader(shader);
		}
	}

	void OpenGLShader::Bind() const {
		glUseProgram(ProgramID);
	}

	void OpenGLShader::UnBind() const {
		glUseProgram(0);
	}

	void OpenGLShader::Compile() {
		glLinkProgram(ProgramID);
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