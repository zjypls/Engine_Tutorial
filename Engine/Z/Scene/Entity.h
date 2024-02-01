//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_ENTITY_H
#define ENGINE_TUTORIAL_ENTITY_H
#include "Include/entt/include/entt.hpp"

#include "Z/Core/Log.h"

namespace Z {
	class Scene;
	class zGUID;
	class Z_API Entity {
		entt::entity handle = entt::null;
		Scene *scene = nullptr;
	public:
		Entity(entt::entity handle, Scene *scene) : handle(handle), scene(scene) {}

		Entity() = default;

		operator bool() const { return handle != entt::null && scene; }

		[[nodiscard]] uint32 ID() const { return (uint32) handle; }

		operator uint32() const { return (uint32) handle; }

		operator entt::entity() const { return handle; }
		Scene* GetContext()const{return scene;}

		bool operator==(const Entity &other) const { return handle == other.handle && scene == other.scene; }

		bool operator!=(const Entity &other) const { return !(*this == other); }

		template<class Ty>
		Ty &GetComponent() const;

		zGUID GetUID();
		const std::string& GetName() const ;

		template<class Ty>
		bool HasComponent() const;

		template<class Ty, class ...Args>
		Ty &AddComponent(Args &&... args);

		template<class Ty,class ...Args>
		Ty& AddOrReplaceComponent(Args&&... args);

		template<class Ty>
		void RemoveComponent() ;

	};

}

#endif //ENGINE_TUTORIAL_ENTITY_H
