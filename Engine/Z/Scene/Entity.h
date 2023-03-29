//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_ENTITY_H
#define ENGINE_TUTORIAL_ENTITY_H

#include "Z/Core/Log.h"
#include "Scene.h"

namespace Z {
	class Entity {
		entt::entity handle = entt::null;
		Scene *scene = nullptr;
	public:
		Entity(entt::entity handle, Scene *scene) : handle(handle), scene(scene) {}

		Entity() = default;

		operator bool() const { return handle != entt::null && scene; }

		[[nodiscard]] uint32_t ID() const { return (uint32_t) handle; }

		operator uint32_t() const { return (uint32_t) handle; }

		operator entt::entity() const { return handle; }

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

		template<class _Ty, class ...Args>
		_Ty &AddComponent(Args &&... args) {
			if (!HasComponent<_Ty>()) {
				auto& component= scene->registry.emplace<_Ty>(handle, std::forward<Args>(args)...);
				scene->OnComponentAdd<_Ty>(*this, component);
				return component;
			}
			else{
				Z_CORE_WARN("Entity already has component:{0}!", typeid(_Ty).name());
				return GetComponent<_Ty>();
			}

		}

		template<class _Ty>
		void RemoveComponent() {
			Z_CORE_ASSERT(HasComponent<_Ty>(), "Entity does not have component!");
			scene->registry.remove<_Ty>(handle);
		}

	};

}

#endif //ENGINE_TUTORIAL_ENTITY_H
