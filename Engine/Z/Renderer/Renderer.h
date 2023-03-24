//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_RENDERER_H
#define ENGINE_TUTORIAL_RENDERER_H
#include "RenderAPI.h"
#include "OrithGraphicCamera.h"
#include "Shader.h"
namespace Z{
	class Renderer {
	public:
		static void Init();
		static RenderAPI::API GetAPI() { return RenderAPI::GetAPI(); }
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void BeginScene(const Ref<OrithGraphicCamera>& camera);
		static void EndScene();
		static void Submit(Z::Ref<Shader>& shader,const Z::Ref<VertexArray>& vertexArray,const glm::mat4& transform=glm::mat4(1.0f));
	private:
		struct SceneData {
			glm::mat4 ViewProjectionMatrix;
		};
		static SceneData* sceneData;
	};
}


#endif //ENGINE_TUTORIAL_RENDERER_H
