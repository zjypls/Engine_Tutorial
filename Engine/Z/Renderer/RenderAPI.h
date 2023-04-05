//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_RENDERAPI_H
#define ENGINE_TUTORIAL_RENDERAPI_H
#include <memory>
#include "glm/glm.hpp"
#include "VertexArray.h"

namespace Z{
	class RenderAPI {
	public:
		enum class API{
			None = 0, OpenGL = 1
		};
		enum class zShaderType{None=0,Vertex = 1, Fragment = 2, Geometry = 3, TessellationControl = 4, TessellationEvaluation = 5, Compute = 6};
		virtual void Init()=0;
		virtual void SetClearValue(const glm::vec4&)=0;
		virtual void Clear()=0;
		virtual void GetError()=0;
		virtual void SetLineWidth(float width=0.f)=0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray,unsigned int count=0)=0;
		virtual void DrawLine(const Ref<VertexArray>& vertexArray,unsigned int count)=0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)=0;
		inline static API GetAPI() { return api; }
	private:
		static API api;
	};
}


#endif //ENGINE_TUTORIAL_RENDERAPI_H