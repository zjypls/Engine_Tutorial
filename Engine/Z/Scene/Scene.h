//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCENE_H
#define ENGINE_TUTORIAL_SCENE_H
#include "Z/Core/GUID.h"
#include "entt.hpp"
#include "Z/Renderer/EditorCamera.h"

class b2World;
namespace Z {

	class Scene {
		entt::registry registry;
		uint32_t viewportWidth = 0, viewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPlane;
		friend class SceneSerializer;
		b2World* PhysicalWorld = nullptr;
		template<class _Ty>
		void OnComponentAdd(Entity entity, _Ty &component);

	public:
		Scene() = default;
		Entity GetMainCamera();

		static Ref<Scene> Copy(Ref<Scene>);

		Entity CreateEntity(const std::string& name="Entity");
		Entity CreateEntityWithGuid(GUID guid,const std::string&name= "Entity");

		void OnRuntimeStart();
		void OnRuntimeStop();

		inline entt::registry &GetRegistry() { return registry;}
		void OnViewportResize(unsigned int width, unsigned int height);
		void DestroyEntity(Entity entity);
		void CopyEntity(Entity entity);

		void OnUpdate(float);
		void OnEditorUpdate(float deltaTime, EditorCamera &camera);

		~Scene();
	};
}


#endif //ENGINE_TUTORIAL_SCENE_H
