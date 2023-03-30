//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_COMPENENTS_H
#define ENGINE_TUTORIAL_COMPONENTS_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "SceneCamera.h"
#include "ScriptEntity.h"

namespace Z {
	struct TransformComponent {
		glm::vec3 translation{0.f};
		glm::vec3 rotation{0.f};
		glm::vec3 scale{1.f};

		TransformComponent(const glm::vec3& position=glm::vec3{0.f},const glm::vec3& rotation=glm::vec3{0.f},
			const glm::vec3&scale=glm::vec3{1.f})
			: translation(position), rotation(rotation), scale(scale) {}

		glm::mat4 GetTransform() {
			return glm::translate(glm::mat4(1.f), translation)
			       * glm::toMat4(glm::quat(rotation))
			       * glm::scale(glm::mat4(1.f), scale);
		}
	};

	struct SpriteRendererComponent {
		glm::vec4 color{1.f};

		SpriteRendererComponent() = default;

		SpriteRendererComponent(const glm::vec4 &color) : color(color) {}

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

	struct ScriptComponent {
		ScriptEntity* instance;
		ScriptEntity* (*onConstruct)();
		void (*onDestruct)(ScriptEntity*);

		template<class _Ty>
		void Bind() {
			onConstruct=[]()->ScriptEntity*{auto res=(new _Ty());return res;};
			onDestruct=[](ScriptEntity* ins){delete (_Ty*)ins;ins= nullptr;};
		}
	};

}

#endif //ENGINE_TUTORIAL_COMPONENTS_H
