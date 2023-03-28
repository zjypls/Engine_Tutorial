//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_ENTITY_H
#define ENGINE_TUTORIAL_ENTITY_H
#include "Z/Core/Log.h"
#include "Scene.h"
namespace Z {
	class Entity {
		entt::entity handle=entt::null;
		Scene *scene=nullptr;
	public:
		Entity(entt::entity handle, Scene *scene) : handle(handle), scene(scene) {}

		Entity()=default;
		operator bool() const { return handle != entt::null && scene ; }
		[[nodiscard]] uint32_t ID() const { return (uint32_t)handle; }
		operator uint32_t () const { return (uint32_t)handle; }
		bool operator==(const Entity &other) const { return handle == other.handle && scene == other.scene; }
		bool operator!=(const Entity &other) const { return !(*this == other); }
		template<class _Ty>
		_Ty &GetComponent() {
			Z_CORE_ASSERT(HasComponent<_Ty>(), "Entity does not have component!");
			return scene->registry.get<_Ty>(handle);
		}

		template<class _Ty>
		bool HasComponent() {
			return scene->registry.any_of<_Ty>(handle);
		}

		template<class _Ty,class ...Args>
		_Ty &AddComponent(Args&&... args) {
			Z_CORE_ASSERT(!HasComponent<_Ty>(), "Entity already has component!");
			return scene->registry.emplace<_Ty>(handle,std::forward<Args>(args)...);
		}

		template<class _Ty>
		void RemoveComponent() {
			Z_CORE_ASSERT(HasComponent<_Ty>(), "Entity does not have component!");
			return scene->registry.remove<_Ty>(handle);
		}

	};

}

#endif //ENGINE_TUTORIAL_ENTITY_H
