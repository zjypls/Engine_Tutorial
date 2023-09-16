//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_RENDERER_H
#define ENGINE_TUTORIAL_RENDERER_H
#include "Z/Renderer/RenderAPI.h"
#include "Z/Renderer/OrithGraphicCamera.h"
#include "Z/Renderer/Shader.h"
#include "Z/Renderer/Texture.h"
#include "Z/Renderer/Camera.h"
#include "Z/Renderer/EditorCamera.h"
#include "Z/Renderer/UniformBuffer.h"
#include "Z/Scene/SceneCamera.h"
namespace Z{
	class Renderer {
	public:
		static void Init();
		static void ShutDown();
		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(const Ref<OrithGraphicCamera>& camera);
		static void BeginScene(const Camera&camera,const glm::mat4 &transform);
		static void BeginScene(const EditorCamera&camera);
		static void BeginScene(const SceneCamera&camera,const glm::mat4 &transform);
		static void EndScene();
		static void RenderSkyBox();
		[[deprecated("May be delete")]]
		static void Submit(Z::Ref<Shader>& shader,const Z::Ref<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};
		struct RenderData{
			Ref<UniformBuffer> ubo;
		};
		static SceneData* sceneData;
		static RenderData* renderData;
		static Ref<Shader> skyBoxShader;
		static Ref<Texture> skyBox;
		static Ref<VertexArray> cube;
	};
}


#endif //ENGINE_TUTORIAL_RENDERER_H
