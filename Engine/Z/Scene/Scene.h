//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCENE_H
#define ENGINE_TUTORIAL_SCENE_H

#include "entt.hpp"


namespace Z {

	class Scene {
		entt::registry registry;
		uint32_t viewportWidth = 0, viewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPlane;
		friend class SceneSerializer;

		template<class _Ty>
		void OnComponentAdd(Entity entity, _Ty &component);

	public:
		Scene() = default;

		Entity CreateEntity(const std::string& name="Entity");

		inline entt::registry &GetRegistry() { return registry;}
		void OnViewportResize(unsigned int width, unsigned int height);
		void DestroyEntity(Entity entity);

		void OnUpdate(float);

		~Scene() = default;
	};
}


#endif //ENGINE_TUTORIAL_SCENE_H
