//
// Created by 32725 on 2023/3/28.
//

#ifndef ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
#define ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H

#include "Z/Scene/Entity.hpp"

namespace Z {
	class SceneHierarchyPlane {
		Ref<Scene> context;
		Entity selectedEntity;

		void DrawEntity(Entity entity);

		void DrawComponents(Entity entity);

		template<typename Ty>
		void DrawComponent(const std::string &, Entity entity, void (*drawFunc)(Entity,Ty &));

	public:
		SceneHierarchyPlane() = default;

		SceneHierarchyPlane(const Ref<Scene> &context) : context(context) {}

        [[deprecated("Should Use SetContext !!!")]]
		inline void SetScene(const Ref<Scene> &context) {
            SetContext(context);
		}

		inline void SetContext(const Ref<Scene> &context) {
            selectedEntity = {};
			this->context = context;
		}

		inline Entity GetSelectedEntity() const { return selectedEntity; }

		inline Entity SetSelectedEntity(int id) {
			if (id != -1)
				return selectedEntity = Entity{entt::entity(id), context.get()};
			else
				return selectedEntity = {};
		}

		void OnImGuiRender();
	};

}


#endif //ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
