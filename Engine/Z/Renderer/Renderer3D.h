//
// Created by z on 2023/9/14.
//

#ifndef ENGINEALL_RENDERER3D_H
#define ENGINEALL_RENDERER3D_H

#include "Z/Core/Core.h"
#include "Z/Scene/Components.h"
#include "Z/Renderer/UniformBuffer.h"
#include "Z/Renderer/Shader.h"

namespace Z {
	class Z_API Renderer3D {
		struct UBO{
			glm::mat4 model;
			int index;
		};
		struct RenderData {
			UBO* uboData;
			Ref<UniformBuffer> ubo;
			Ref<Shader> EasySample;
		};
		struct ModelData{
			glm::mat4 transform;
			Ref<Mesh> MeshRenderer;
			uint32_t entityID;
			ModelData(glm::mat4 PTrans,const Ref<Mesh>&mesh,uint32_t id)
			:transform(PTrans), MeshRenderer(mesh),entityID(id){}
		};

		static RenderData* renderData;
		static std::vector<ModelData> Models;
		static uint32_t maxRecord;
		static void Flush();
	public:
		static void Init();
		static void BeginScene();
		static void EndScene();
		static void Draw(const glm::mat4&transform,const MeshRendererComponent& meshRenderer,uint32_t entityID=-1);
		static void Shutdown();
	};

}

#endif //ENGINEALL_RENDERER3D_H
