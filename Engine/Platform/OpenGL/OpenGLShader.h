//
// Created by 32725 on 2023/3/18.
//

#ifndef ENGINE_TUTORIAL_OPENGLSHADER_H
#define ENGINE_TUTORIAL_OPENGLSHADER_H
#include "glm/glm.hpp"
#include "Z/Renderer/Shader.h"
namespace Z {
	class OpenGLShader:public Shader {
		unsigned int ProgramID;
		std::string Name;
		void Compile(unsigned int ) ;
		void AddShader(const std::string& shaderSrc, RenderAPI::zShaderType shaderType) ;
		void AddShader(const std::string&Code) ;
		void Compile();
	public:
		OpenGLShader(const std::string&Src,const std::string& name,bool isFile);
		OpenGLShader(const std::string&name,const std::string& VertSrc,const std::string&FragSrc,bool isFile);

		virtual ~OpenGLShader() override;
		virtual void Bind() const override;
		virtual void UnBind() const override;
		inline virtual std::string GetName() const override{return Name;}
		virtual void SetUniform(const char* name, const glm::mat4& matrix) override;
		virtual void SetUniform(const char* name, const glm::mat3& matrix) override;
		virtual void SetUniform(const char* name, const glm::vec4& vector) override;
		virtual void SetUniform(const char* name, const glm::vec3& vector) override;
		virtual void SetUniform(const char* name, const glm::vec2& vector) override;
		virtual void SetUniform(const char* name, float value) override;
		virtual void SetUniform(const char* name, int value) override;

	};

}


#endif //ENGINE_TUTORIAL_OPENGLSHADER_H
