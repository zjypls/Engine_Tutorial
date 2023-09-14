//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCENE_H
#define ENGINE_TUTORIAL_SCENE_H

#include "Include/entt/include/entt.hpp"

#include "Z/Core/zGUID.h"
#include "Z/Renderer/EditorCamera.h"
#include "Z/Scene/Entity.h"


namespace Z {
	class Entity;
	class Scene {
		entt::registry registry;
		uint32_t viewportWidth = 0, viewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPlane;
		friend class SceneSerializer;
		void* PhysicalWorld = nullptr;
		bool Running=false,Paused=false;
		int FrameStepCount=0;
		std::unordered_map<zGUID, Entity> entities;
		//TODO:optimize
		template<class Ty>
		void OnComponentAdd(Entity entity, Ty &component);
		void Render2D();
		void Render3D();
		void NativeScriptUpdate(float deltaTime);
		void ScriptUpdate(float deltaTime);
		void OnScriptStart();
		void OnScriptStop();
		void OnPhysics2DUpdate(float deltaTime);


	public:
		Scene() = default;
		Entity GetMainCamera();
		inline bool isRunning(){return Running;}
		inline void SetFrameStepCount(int count){FrameStepCount=count;}

		static Ref<Scene> Copy(Ref<Scene>);

		Entity CreateEntity(const std::string& name="Entity");
		Entity CreateEntityWithGuid(zGUID guid, const std::string&name= "Entity");

		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnSimulateStart();
		void OnSimulateStop();
		inline bool IsPaused(){return Paused;}
		inline void SetPaused(bool paused){Paused=paused;}
		void OnPhysics2DStart();
		void OnPhysics2DStop();
		template<class... T>
		auto GetComponentView() {
			return registry.view<T...>();
		}

		void OnViewportResize(unsigned int width, unsigned int height);
		void DestroyEntity(Entity entity);
		void CopyEntity(Entity entity);
		Entity GetEntityWithGUID(zGUID guid);

		void GetEntitiesByName(const std::string& name, std::vector<zGUID>& ids);

		void OnUpdate(float);
		void OnEditorUpdate(float deltaTime, EditorCamera &camera);
		void OnPreviewUpdate(float deltaTime,Camera &camera,glm::mat4 transform);
		void OnSimulateUpdate(float deltaTime,EditorCamera &camera);

		~Scene();

	};
}


#endif //ENGINE_TUTORIAL_SCENE_H
