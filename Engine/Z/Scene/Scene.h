//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCENE_H
#define ENGINE_TUTORIAL_SCENE_H

#include "entt.hpp"


namespace Z {

	class Scene {
		entt::registry registry;

		friend class Entity;
		friend class SceneHierarchyPlane;

	public:
		Scene() = default;

		Entity CreateEntity(const std::string& name="Entity");

		inline entt::registry &GetRegistry() { return registry;}
		void OnViewportResize(unsigned int width, unsigned int height);

		void OnUpdate(float);

		~Scene() = default;
	};
}


#endif //ENGINE_TUTORIAL_SCENE_H
