//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_COMPENENTS_H
#define ENGINE_TUTORIAL_COMPONENTS_H

#include "glm/glm.hpp"
#include "SceneCamera.h"
#include "ScriptEntity.h"

namespace Z {
	struct TransformComponent {
		glm::mat4 transform{1.f};

		TransformComponent(const glm::mat4 &transform = glm::mat4(1.f)) : transform(transform) {}

		inline glm::mat4 &operator()() { return transform; }

		inline const glm::mat4 &operator()() const { return transform; }
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
