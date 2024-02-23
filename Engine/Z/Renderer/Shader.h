//
// Created by 32725 on 2023/3/16.
//

#ifndef ENGINE_TUTORIAL_SHADER_H
#define ENGINE_TUTORIAL_SHADER_H
#include "Include/glm/glm/glm.hpp"
#include "Z/Renderer/RenderAPI.h"
namespace Z {
	class Shader {
	public:
		static Ref<Shader> CreateShader(const std::string& Src,const std::string& name="",bool isFile=true);//empty name will use the file name
		Shader()= default;
		virtual ~Shader()= default;
		virtual void Bind() const=0;
		virtual void UnBind() const=0;
		[[nodiscard]] virtual std::string GetName() const=0;
		//Todo:improve
		virtual void SetUniform(const char* name, int value)=0;
		virtual void SetUniform(const char* name, float value)=0;
		virtual void SetUniform(const char* name, const glm::vec3& value)=0;
		virtual void SetUniform(const char* name, const glm::vec2& value)=0;
		virtual void SetUniform(const char* name, const glm::vec4& value)=0;
		virtual void SetUniform(const char* name, const glm::mat4& value)=0;
		virtual void SetUniform(const char* name, const glm::mat3& value)=0;

	protected:
		std::string Name;
	};

	//Todo : change into as a prt of  assets manager
	class ShaderLibrary{
		static std::unordered_map<std::string, std::shared_ptr<Shader>> Shaders;
	public:
		Ref<Shader> Add(const Ref<Shader>& shader);
		inline Ref<Shader> Get(const std::string& name){return Shaders[name];}
		Ref<Shader> Load( const std::string& ShaderSrc,const std::string&name="",bool isFile=true);//empty name will use the file name
		static auto begin(){return Shaders.begin();}
		static auto end(){return Shaders.end();}

		static std::string GetDefaultName();
		static inline bool Exists(const std::string& name){return Shaders.find(name)!=Shaders.end();}
	};
}


#endif //ENGINE_TUTORIAL_SHADER_H
