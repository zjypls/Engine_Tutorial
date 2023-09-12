#pragma once
#include "Z/Scene/Scene.h"


namespace Z{
	template<class Ty>
	Ty& Entity::GetComponent() const{
		Z_CORE_ASSERT(HasComponent<Ty>(), "Entity does not have component!");
		return scene->registry.get<Ty>(handle);
	}

	template<class Ty>
	bool Entity::HasComponent() const{
		return scene->registry.any_of<Ty>(handle);
	}

	template<class Ty, class ...Args>
	Ty& Entity::AddComponent(Args&&... args) {
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

	template<class Ty, class ...Args>
	Ty& Entity::AddOrReplaceComponent(Args&&... args) {
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

	template<class Ty>
	void Entity::RemoveComponent() {
		if (HasComponent<Ty>()) {
			scene->registry.remove<Ty>(handle);
		}
		else{
			Z_CORE_WARN("Entity does not have component:{0}!", typeid(Ty).name());
		}
	}

}