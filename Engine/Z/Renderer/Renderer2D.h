//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_RENDERER2D_H
#define ENGINE_TUTORIAL_RENDERER2D_H
#include "glm/glm.hpp"
#include"Z/Renderer/Texture.h"
#include"Z/Renderer/VertexArray.h"
#include "Z/Renderer/Shader.h"
#include "Z/Renderer/OrithGraphicCamera.h"
#include "SubTex2D.h"

namespace Z {
	class Renderer2D {
		struct QuadVertex{
			glm::vec3 position;
			glm::vec4 color;
			glm::vec2 texCoord;
			float texIndex;
			float tillingFactor;
		};
		struct RenderData{

			unsigned int MaxQuadCount=20000;
			unsigned int MaxVertexCount=MaxQuadCount*4;
			unsigned int MaxIndexCount=MaxQuadCount*6;
			constexpr static unsigned int MaxTextureSlot=32;

			Ref<VertexArray> quadVertexArray;
			Ref<VertexBuffer> quadVertexBuffer;
			Ref<Shader> UShader;
			Ref<Texture2D> whiteTexture;
			unsigned int quadIndexCount=0;
			QuadVertex *quadVertexBufferBase=nullptr;
			QuadVertex *quadVertexBufferPtr=nullptr;
			std::array<Ref<Texture2D>,MaxTextureSlot> textureSlots;
			unsigned int textureSlotIndex=1;

			glm::vec4 quadVertexPositions[4];
		};
		struct StatisticData{
			unsigned int DrawCalls=0;
			unsigned int QuadCount=0;
			unsigned int TextureCount=0;
			unsigned int GetTotalVertexCount(){return QuadCount*4;}
			unsigned int GetTotalIndexCount(){return QuadCount*6;}
			inline void Reset(){
				DrawCalls=0;
				QuadCount=0;
				TextureCount=0;
			}
		};
		static RenderData *data;
		static StatisticData *stats;
	public:
		inline static StatisticData* GetStats(){return stats;}
		static void Init();
		static void Shutdown();
		static void BeginScene(const Ref<OrithGraphicCamera> &camera);
		static void Flush();
		static void EndScene();
		static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
		static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);
		static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTex2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTex2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color);
		static void DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color);
		static void DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Ref<SubTex2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});
		static void DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<SubTex2D> &texture,float tilingFactor=1.f,glm::vec4 tintCol=glm::vec4 {1.f});

	};

}


#endif //ENGINE_TUTORIAL_RENDERER2D_H
