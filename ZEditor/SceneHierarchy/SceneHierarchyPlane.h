//
// Created by 32725 on 2023/3/28.
//

#ifndef ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
#define ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
#include "Z/Core/Core.h"
#include "Z/Scene/Entity.h"
#include"Z/Scene/Scene.h"

namespace Z {
	class SceneHierarchyPlane {
		Ref<Scene> context;
		Entity selectedEntity;
		void DrawEntity(Entity entity);
		void DrawComponents(Entity entity);
		template<typename _Ty>
		void DrawComponent(Entity entity, void (*drawFunc)(Entity,_Ty&));
	public:
		SceneHierarchyPlane() = default;
		SceneHierarchyPlane(const Ref<Scene>& context) : context(context) {}

		inline void SetScene(const Ref<Scene>& context) { this->context = context; }
		inline void SetContext(const Ref<Scene> &context) { this->context = context; }

		void OnImGuiRender();
	};

}


#endif //ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
