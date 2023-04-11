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
		Scene* GetContext()const{return scene;}

		bool operator==(const Entity &other) const { return handle == other.handle && scene == other.scene; }

		bool operator!=(const Entity &other) const { return !(*this == other); }

		template<class Ty>
		Ty &GetComponent() const{
			Z_CORE_ASSERT(HasComponent<Ty>(), "Entity does not have component!");
			return scene->registry.get<Ty>(handle);
		}

		unsigned long long GetUID();
		const std::string& GetName() const ;

		template<class Ty>
		bool HasComponent() const{
			return scene->registry.any_of<Ty>(handle);
		}

		template<class Ty, class ...Args>
		Ty &AddComponent(Args &&... args) {
			if (!HasComponent<Ty>()) {
				auto& component= scene->registry.emplace<Ty>(handle, std::forward<Args>(args)...);
				scene->OnComponentAdd<Ty>(*this, component);
				return component;
			}
			else{
				Z_CORE_WARN("Entity already has component:{0}!", typeid(Ty).name());
				return GetComponent<Ty>();
			}
		}

		template<class Ty,class ...Args>
		Ty& AddOrReplaceComponent(Args&&... args){
			auto&component= scene->registry.emplace_or_replace<Ty>(handle, std::forward<Args>(args)...);
			scene->OnComponentAdd<Ty>(*this, component);
			return component;
		}

		template<class Ty>
		void RemoveComponent() {
			Z_CORE_ASSERT(HasComponent<Ty>(), "Entity does not have component!");
			scene->registry.remove<Ty>(handle);
		}

	};

}

#endif //ENGINE_TUTORIAL_ENTITY_H
