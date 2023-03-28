//
// Created by 32725 on 2023/3/27.
//

#include "Scene.h"
#include "Z/Renderer/Renderer2D.h"
#include "Entity.h"
#include "Components.h"
namespace Z {
	void Scene::OnUpdate(float deltaTime ) {
		Camera *mainCamera = nullptr;
		glm::mat4 cameraTransform;

		registry.view<ScriptComponent>().each([&](auto entity, auto &script) {
			if (!script.instance) {
				script.instance = script.onConstruct();
				script.instance->entity = Entity{entity, this};
				script.instance->OnCreate();
			}
			script.instance->OnUpdate(deltaTime);
		});

		std::for_each(registry.group<CameraComponent,TransformComponent>().begin(),
		              registry.group<CameraComponent,TransformComponent>().end(), [&](const auto &item) {
					if(registry.get<CameraComponent>(item).primary){
						mainCamera = &registry.get<CameraComponent>(item).camera;
						cameraTransform = registry.get<TransformComponent>(item).GetTransform();
					}
				});
		if(!mainCamera)return;
		Renderer2D::BeginScene(*mainCamera, cameraTransform);
		std::for_each(registry.view<TransformComponent, SpriteRendererComponent>().begin(),
		              registry.view<TransformComponent, SpriteRendererComponent>().end(), [&](const auto &item) {
					Renderer2D::DrawQuad(registry.get<TransformComponent>(item).GetTransform(),
					                     registry.get<SpriteRendererComponent>(item).color);});
		Renderer2D::EndScene();
	}
	Entity Scene::CreateEntity(const std::string& name) {
		Entity entity{registry.create(),this};
		entity.AddComponent<TransformComponent>();
		auto& tag=entity.AddComponent<TagComponent>(name);
		if(tag.tag.empty())tag.tag="Entity";
		return entity;
	}

	void Scene::OnViewportResize(unsigned int width, unsigned int height) {
		std::for_each(registry.view<CameraComponent>().begin(),
		              registry.view<CameraComponent>().end(), [&](const auto &item) {
					registry.get<CameraComponent>(item).camera.OnViewportResize(width, height);
				});
	}
}