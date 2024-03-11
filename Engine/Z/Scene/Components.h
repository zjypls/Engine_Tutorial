//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_COMPONENTS_H
#define ENGINE_TUTORIAL_COMPONENTS_H
#pragma once
#include "Include/glm/glm.hpp"
#include "Include/glm/gtc/matrix_transform.hpp"
#include "Include/glm/gtx/quaternion.hpp"

#include "Z/Core/zGUID.h"
#include "Z/Scene/SceneCamera.h"

namespace Z {

	struct IDComponent{
		zGUID ID{};
		IDComponent()=default;
		IDComponent(const zGUID& id): ID(id){}
	};

	struct TransformComponent {
		glm::vec3 translation{0.f};
		glm::vec3 rotation{0.f};
		glm::vec3 scale{1.f};

		TransformComponent(const glm::vec3& position=glm::vec3{0.f},const glm::vec3& rotation=glm::vec3{0.f},
			const glm::vec3&scale=glm::vec3{1.f})
			: translation(position), rotation(rotation), scale(scale) {}

		glm::mat4 GetTransform()const {
			return glm::translate(glm::mat4(1.f), translation)
			       * glm::toMat4(glm::quat(rotation))
			       * glm::scale(glm::mat4(1.f), scale);
		}
	};

	struct SpriteRendererComponent {
		glm::vec4 color{1.f};
		std::string path;
		void* data;
		SpriteRendererComponent() = default;

		SpriteRendererComponent(const glm::vec4 &color) : color(color) {}

		inline glm::vec4 &operator()() { return color; }

		inline const glm::vec4 &operator()() const { return color; }
	};

	struct CircleRendererComponent {
		glm::vec4 color{1.f};
		float thickness = 1.f,fade=.005f;

		CircleRendererComponent() = default;

		CircleRendererComponent(const glm::vec4 &color) : color(color) {}

		inline glm::vec4 &operator()() { return color; }

		inline const glm::vec4 &operator()() const { return color; }
	};

	struct TagComponent {
		std::string tag;

		TagComponent() = default;

		TagComponent(const std::string &tag) : tag(tag) {}

		inline std::string &operator()() { return tag; }

		inline const std::string &operator()() const { return tag; }
	};

	struct CameraComponent {
		SceneCamera camera{};
		bool primary = true;

		CameraComponent() = default;

		CameraComponent(const glm::mat4 &trans) : camera(trans) {}

		CameraComponent(const Camera &camera) : camera(camera) {}

		inline Camera &operator()() { return camera; }

		inline const Camera &operator()() const { return camera; }
	};

	struct ScriptComponent{
		std::string scriptName;
		ScriptComponent()=default;
	};

	class ScriptEntity;
	struct NativeScriptComponent {
		ScriptEntity* instance;
		ScriptEntity* (*onConstruct)();
		void (*onDestruct)(ScriptEntity*);

		template<class Ty>
		void Bind() {
			onConstruct=[]()->ScriptEntity*{auto res=(new Ty());return res;};
			onDestruct=[](ScriptEntity* ins){delete (Ty*)ins;ins= nullptr;};
		}
	};


	struct RigidBody2DComponent{
		enum class BodyType{
			Static,
			Kinematic,
			Dynamic
		};
		BodyType bodyType{BodyType::Static};
		bool fixedRotation{false};

		//Todo : change
		void* ptr= nullptr;
		RigidBody2DComponent()=default;

	};

	struct BoxCollider2DComponent{
		glm::vec2 offset{0.f,0.f};
		glm::vec2 size{1.f,1.f};
		bool isTrigger{false};
		//Todo : change !!!!
		void* ptr= new int{1};

		float density{0.5f};
		float friction{0.5f};
		float restitution{0.f};
		float MinRestitution{0.5f};
		BoxCollider2DComponent()=default;
	};

	struct CircleCollider2DComponent{
		glm::vec2 offset{0.f,0.f};
		float radius{.5f};
		bool isTrigger{false};
		//Todo : change !!!!
		void* ptr= new int{1};

		float density{0.5f};
		float friction{0.5f};
		float restitution{0.f};
		float MinRestitution{0.5f};
		CircleCollider2DComponent()=default;
	};

	struct MeshFilterComponent{
		std::string meshPath;
	};

    struct MeshRendererComponent{
        std::string materialPath;
    };

	template<class... T>
	struct Type{
	};

	using AllTypes=Type<MeshRendererComponent,MeshFilterComponent,TransformComponent,SpriteRendererComponent,CircleRendererComponent,TagComponent,
	CameraComponent,ScriptComponent,NativeScriptComponent,RigidBody2DComponent,BoxCollider2DComponent,CircleCollider2DComponent>;
	using NoBaseTypes=Type<MeshRendererComponent,MeshFilterComponent,SpriteRendererComponent,CircleRendererComponent,
			CameraComponent,ScriptComponent,NativeScriptComponent,RigidBody2DComponent,BoxCollider2DComponent,CircleCollider2DComponent>;

}

#endif //ENGINE_TUTORIAL_COMPONENTS_H
