//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_RENDERER2D_H
#define ENGINE_TUTORIAL_RENDERER2D_H
#pragma once
#include "Include/glm/glm/glm.hpp"
#include "Include/glm/glm/gtc/matrix_transform.hpp"
#include "Z/Renderer/Texture.h"
#include "Z/Renderer/VertexArray.h"
#include "Z/Renderer/Shader.h"
#include "Z/Renderer/OrithGraphicCamera.h"
#include "Z/Renderer/Camera.h"
#include "Z/Renderer/EditorCamera.h"
#include "Z/Renderer/UniformBuffer.h"
#include "Z/Renderer/SubTex2D.h"
#include "Z/Scene/Components.h"

namespace Z {
	class Renderer2D {
		struct QuadVertex{
			glm::vec3 position;
			glm::vec4 color;
			glm::vec2 texCoord;
			int texIndex;
			float tillingFactor;
			int EntityID;
		};
		struct CircleVertex{
			glm::vec3 WorldPosition;
			glm::vec3 LocalPosition;
			glm::vec4 color;
			float thickness;
			float fade;
			int EntityID;
		};
		struct LineVertex{
			glm::vec3 Position;
			glm::vec4 color;
			int EntityID;
		};
		struct CameraData{
			glm::mat4 ViewProjectionMatrix;
		};
		struct RenderData{

			unsigned int MaxQuadCount=20000;
			unsigned int MaxVertexCount=MaxQuadCount*4;
			unsigned int MaxIndexCount=MaxQuadCount*6;
			constexpr static unsigned int MaxTextureSlot=32;

			Ref<VertexArray> quadVertexArray;
			Ref<VertexBuffer> quadVertexBuffer;
			Ref<VertexArray> circleVertexArray;
			Ref<VertexBuffer> circleVertexBuffer;
			Ref<VertexArray> lineVertexArray;
			Ref<VertexBuffer> lineVertexBuffer;
			Ref<Shader> QuadShader;
			Ref<Shader> CircleShader;
			Ref<Shader> LineShader;
			Ref<Texture2D> whiteTexture;
			unsigned int quadIndexCount=0;
			QuadVertex *quadVertexBufferBase=nullptr;
			QuadVertex *quadVertexBufferPtr=nullptr;
			unsigned int circleIndexCount=0;
			CircleVertex *circleVertexBufferBase=nullptr;
			CircleVertex *circleVertexBufferPtr=nullptr;
			unsigned int lineIndexCount=0;
			LineVertex *lineVertexBufferBase=nullptr;
			LineVertex *lineVertexBufferPtr=nullptr;
			std::array<Ref<Texture>,MaxTextureSlot> textureSlots;
			unsigned int textureSlotIndex=1;

			glm::vec4 quadVertexPositions[4];
			CameraData cameraData;
			Ref<UniformBuffer> cameraBuffer;
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
		static void BeginScene(const Camera&camera,const glm::mat4 &transform);
		static void BeginScene(const EditorCamera&camera);
		static void BeginScene(const SceneCamera&camera,const glm::mat4 &transform);
		static void BeginScene();
		static void Flush();
		static void EndScene();
		static void ChangeDepthTest(RenderAPI::DepthTestState state=RenderAPI::DepthTestState::Less);
		inline static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color){
			DrawQuad(glm::vec3{position.x,position.y,0.f},size,color);
		}
		inline static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color){
			auto transform=glm::translate(glm::mat4{1.f},position)*glm::scale(glm::mat4{1.f},glm::vec3{size,1.f});
			DrawQuad(transform,color);
		}
		inline static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			DrawQuad(glm::vec3{position.x,position.y,0.f},size,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			auto transform=glm::translate(glm::mat4{1.f},position)*glm::scale(glm::mat4{1.f},glm::vec3{size,1.f});
			DrawQuad(transform,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTex2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			DrawQuad(glm::vec3{position.x,position.y,0.f},size,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTex2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			auto transform=glm::translate(glm::mat4{1.f},position)*glm::scale(glm::mat4{1.f},glm::vec3{size,1.f});
			DrawQuad(transform,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color){
			DrawQuadRotated(glm::vec3{position.x,position.y,0.f},size,rotation,color);
		}
		inline static void DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color){
			auto transform=glm::translate(glm::mat4{1.f},position)*glm::rotate(glm::mat4{1.f},rotation,glm::vec3{0.f,0.f,1.f})*glm::scale(glm::mat4{1.f},glm::vec3{size,1.f});
			DrawQuad(transform,color);
		}
		inline static void DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			DrawQuadRotated(glm::vec3{position.x,position.y,0.f},size,rotation,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			auto transform=glm::translate(glm::mat4{1.f},position)*glm::rotate(glm::mat4{1.f},rotation,glm::vec3{0.f,0.f,1.f})*glm::scale(glm::mat4{1.f},glm::vec3{size,1.f});
			DrawQuad(transform,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuadRotated(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Ref<SubTex2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			DrawQuadRotated(glm::vec3{position.x,position.y,0.f},size,rotation,texture,tilingFactor,tintCol);
		}
		inline static void DrawQuadRotated(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<SubTex2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f}){
			auto transform=glm::translate(glm::mat4{1.f},position)*glm::rotate(glm::mat4{1.f},rotation,glm::vec3{0.f,0.f,1.f})*glm::scale(glm::mat4{1.f},glm::vec3{size,1.f});
			DrawQuad(transform,texture,tilingFactor,tintCol);
		}
		static void DrawLine(const glm::vec3& v0,const glm::vec3& v1,const glm::vec4& color,int entityID=-1);
		static void DrawLine(const glm::mat4& transform,const glm::vec3& center,const glm::vec3& size,const glm::vec4& color,int entityID=-1);
		inline static void DrawRect(const glm::mat4& transform,const glm::vec2& size,const glm::vec4& color,int entityID=-1){
			DrawRect(transform,glm::vec3{size,0.f},color,entityID);
		}
		static void DrawRect(const glm::mat4& transform,const glm::vec3& size,const glm::vec4& color,int entityID=-1);
		[[deprecated]]
		static void DrawRect(const  glm::vec3&leftBottom,const glm::vec3&rightTop,const glm::vec4& color,int entityID=-1);
		inline static void DrawLine(const glm::vec2& v0,const glm::vec2& v1,const glm::vec4& color,int entityID=-1){
			DrawLine(glm::vec3{v0.x,v0.y,0.f},glm::vec3{v1.x,v1.y,0.f},color,entityID);
		}
		static void DrawCircle(const glm::mat4&transform,const CircleRendererComponent&circle,int EntityID=-1);
		static void DrawCircle(const glm::mat4&transform,const glm::vec4& color,float thickness=0.02f,float fade=.005f,int EntityID=-1);
		static void DrawQuad(const glm::mat4 &transform, const SpriteRendererComponent &sprite,int EntityID=-1);
		static void DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f});
		static void DrawQuad(const glm::mat4 &transform, const Ref<SubTex2D> &texture,float tilingFactor=1.f,const glm::vec4& tintCol=glm::vec4 {1.f});
	};

}


#endif //ENGINE_TUTORIAL_RENDERER2D_H
