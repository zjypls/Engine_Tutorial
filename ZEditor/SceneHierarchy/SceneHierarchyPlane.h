//
// Created by 32725 on 2023/3/28.
//

#ifndef ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
#define ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H

#include "z.h"

namespace Z {
	class SceneHierarchyPlane {
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
			if (id > -1)
				return selectedEntity = Entity{entt::entity(id), context.get()};
			else
				return selectedEntity = {};
		}

		void OnImGuiRender();
		static auto GetWhiteTexture(){
			if(whiteTexture)return whiteTexture;
			whiteTexture= RenderManager::CreateImGuiTexture(AssetsSystem::Load<Texture2D>(":/Inner/whiteTexture"));
			return whiteTexture;
		}
		static auto GetBlackTexture(){
			if(whiteTexture)return blackTexture;
			blackTexture= RenderManager::CreateImGuiTexture(AssetsSystem::Load<Texture2D>(":/Inner/blackTexture"));
			return blackTexture;
		}
		auto& GetTextureMap(){return descriptorSetTextureMap;}
	private:
		static ImTextureID whiteTexture,blackTexture;
		Ref<Scene> context;
		Entity selectedEntity;
		std::unordered_map<zGUID,std::unordered_map<std::string,ImTextureID>> descriptorSetTextureMap;

		void DrawEntity(Entity entity);

		void DrawComponents(Entity entity);

		template<typename Ty>
		void DrawComponent(const std::string &, Entity entity, std::function<void(Entity,Ty &)>);

	};

}


#endif //ENGINE_TUTORIAL_SCENEHIERARCHYPLANE_H
