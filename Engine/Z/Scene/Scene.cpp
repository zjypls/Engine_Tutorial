//
// Created by 32725 on 2023/3/27.
//

#include "ScriptEntity.h"
#include "Scene.h"
#include "Z/Renderer/Renderer2D.h"
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_collision.h"

namespace Z {

	namespace Temp {
		b2BodyType GetBox2DType(RigidBody2DComponent::BodyType type) {
			switch (type) {
				case RigidBody2DComponent::BodyType::Static:
					return b2_staticBody;
				case RigidBody2DComponent::BodyType::Dynamic:
					return b2_dynamicBody;
				case RigidBody2DComponent::BodyType::Kinematic:
					return b2_kinematicBody;
			}
			return b2_staticBody;
		}

		template<typename T>
		void CopyComponent(Entity dst, Entity src) {
			if (src.HasComponent<T>()) {
				dst.AddOrReplaceComponent<T>(src.GetComponent<T>());
			}
		}
	}

	void Scene::OnUpdate(float deltaTime) {
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
		PhysicalWorld->Step(deltaTime, 8, 3);
		registry.view<RigidBody2DComponent>().each([&](auto id, auto &rigidBody) {
			if (rigidBody.bodyType != RigidBody2DComponent::BodyType::Static) {
				Entity entity{id, this};
				auto &transform = entity.GetComponent<TransformComponent>();
				auto body = (b2Body *) rigidBody.ptr;
				const auto &position = body->GetPosition();
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();
			}
		});

		std::for_each(registry.group<CameraComponent, TransformComponent>().begin(),
		              registry.group<CameraComponent, TransformComponent>().end(), [&](const auto &item) {
					if (registry.get<CameraComponent>(item).primary) {
						mainCamera = &registry.get<CameraComponent>(item).camera;
						cameraTransform = registry.get<TransformComponent>(item).GetTransform();
					}
				});
		if (!mainCamera)return;
		Renderer2D::BeginScene(*mainCamera, cameraTransform);
		std::for_each(registry.view<TransformComponent, SpriteRendererComponent>().begin(),
		              registry.view<TransformComponent, SpriteRendererComponent>().end(), [&](const auto &item) {
					Renderer2D::DrawQuad(registry.get<TransformComponent>(item).GetTransform(),
					                     registry.get<SpriteRendererComponent>(item), uint32_t(item));
				});
		std::for_each(registry.view<TransformComponent, CircleRendererComponent>().begin(),
		              registry.view<TransformComponent, CircleRendererComponent>().end(), [&](const auto &item) {
					Renderer2D::DrawCircle(registry.get<TransformComponent>(item).GetTransform(),
					                       registry.get<CircleRendererComponent>(item), uint32_t(item));
				});
/*		if (VisualizeCollider) {
			registry.view<TransformComponent, RigidBody2DComponent>().each(
					[&](auto id, auto &transform, auto &rigidBody) {
						Entity entity{id, this};
						auto color = glm::vec4(.8, .5, .3, 1);
						if (rigidBody.bodyType == RigidBody2DComponent::BodyType::Static)
							color = glm::vec4(.5, .3, .1, 1);
						if (entity.HasComponent<BoxCollider2DComponent>()) {
							auto &boxCollider = entity.GetComponent<BoxCollider2DComponent>();
							if (boxCollider.visualize) {
								Renderer2D::DrawRect(transform.GetTransform(), boxCollider.size, color, uint32_t(id));
							}
						}
						if (entity.HasComponent<CircleCollider2DComponent>()) {
							auto &circleCollider = entity.GetComponent<CircleCollider2DComponent>();
							if (circleCollider.visualize) {
								Renderer2D::DrawCircle(transform.GetTransform(), color);
							}
						}
					});
		}*/
		Renderer2D::EndScene();

	}

	Entity Scene::CreateEntity(const std::string &name) {
		Entity entity{registry.create(), this};
		entity.AddComponent<IDComponent>();
		entity.AddComponent<TransformComponent>();
		auto &tag = entity.AddComponent<TagComponent>(name);
		if (tag.tag.empty())tag.tag = "Entity";
		return entity;
	}

	Entity Scene::CreateEntityWithGuid(GUID guid, const std::string &name) {
		Entity entity{registry.create(), this};
		entity.AddComponent<IDComponent>(guid);
		entity.AddComponent<TransformComponent>();
		auto &tag = entity.AddComponent<TagComponent>(name);
		if (tag.tag.empty())tag.tag = "Entity";
		return entity;
	}

	void Scene::OnViewportResize(unsigned int width, unsigned int height) {
		viewportWidth = width;
		viewportHeight = height;
		std::for_each(registry.view<CameraComponent>().begin(),
		              registry.view<CameraComponent>().end(), [&](const auto &item) {
					registry.get<CameraComponent>(item).camera.OnViewportResize(width, height);
				});
	}

	void Scene::DestroyEntity(Entity entity) {
		registry.destroy(entity);
	}

	Entity Scene::GetMainCamera() {
		auto view = registry.view<CameraComponent>();
		for (auto entity: view) {
			auto &camera = view.get<CameraComponent>(entity);
			if (camera.primary) {
				return Entity{entity, this};
			}
		}
		return {};
	}

	void Scene::OnEditorUpdate(float deltaTime, EditorCamera &camera) {
		Renderer2D::BeginScene(camera);
		std::for_each(registry.view<TransformComponent, SpriteRendererComponent>().begin(),
		              registry.view<TransformComponent, SpriteRendererComponent>().end(), [&](const auto &item) {
					Renderer2D::DrawQuad(registry.get<TransformComponent>(item).GetTransform(),
					                     registry.get<SpriteRendererComponent>(item), uint32_t(item));
//					Renderer2D::DrawRect(registry.get<TransformComponent>(item).GetTransform(),glm::vec3(1),
//					                     registry.get<SpriteRendererComponent>(item).color, uint32_t(item));
				});
		std::for_each(registry.view<TransformComponent, CircleRendererComponent>().begin(),
		              registry.view<TransformComponent, CircleRendererComponent>().end(), [&](const auto &item) {
					Renderer2D::DrawCircle(registry.get<TransformComponent>(item).GetTransform(),
					                       registry.get<CircleRendererComponent>(item), uint32_t(item));
				});
/*		if (VisualizeCollider)
			registry.view<TransformComponent, RigidBody2DComponent>().each(
					[&](auto id, auto &transform, auto &rigidBody) {
						Entity entity{id, this};
						auto color = glm::vec4(.8, .5, .3, 1);
						if (rigidBody.bodyType == RigidBody2DComponent::BodyType::Static)
							color = glm::vec4(.5, .3, .1, 1);
						if (entity.HasComponent<BoxCollider2DComponent>()) {
							auto &boxCollider = entity.GetComponent<BoxCollider2DComponent>();
							if (boxCollider.visualize) {
								Renderer2D::DrawRect(transform.GetTransform(), boxCollider.size, color, uint32_t(id));
							}
						}
						if (entity.HasComponent<CircleCollider2DComponent>()) {
							auto &circleCollider = entity.GetComponent<CircleCollider2DComponent>();
							if (circleCollider.visualize) {
								Renderer2D::DrawCircle(transform.GetTransform(), color);
							}
						}
					});*/
		Renderer2D::EndScene();
	}

	void Scene::OnRuntimeStart() {
		PhysicalWorld = new b2World(b2Vec2(0.0f, -9.8f));
		registry.view<RigidBody2DComponent>().each([&](auto id, auto &rigidBody) {
			Entity entity{id, this};
			auto &transform = entity.GetComponent<TransformComponent>();
			b2BodyDef bodyDef;
			bodyDef.type = Temp::GetBox2DType(rigidBody.bodyType);
			bodyDef.position.Set(transform.translation.x, transform.translation.y);
			bodyDef.angle = transform.rotation.z;
			bodyDef.fixedRotation = rigidBody.fixedRotation;
			b2Body *body = PhysicalWorld->CreateBody(&bodyDef);
			rigidBody.ptr = body;

			if (entity.HasComponent<BoxCollider2DComponent>()) {
				auto &collider = entity.GetComponent<BoxCollider2DComponent>();
				b2PolygonShape box{};
				auto size = collider.size * glm::vec2{transform.scale} / 2.f;
				box.SetAsBox(size.x, size.y, b2Vec2{collider.offset.x, collider.offset.y}, 0);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &box;
				fixtureDef.density = collider.density;
				fixtureDef.friction = collider.friction;
				fixtureDef.restitution = collider.restitution;
				fixtureDef.restitutionThreshold = collider.MinRestitution;
				body->CreateFixture(&fixtureDef);
				collider.ptr = body->GetFixtureList();
			}
			if (entity.HasComponent<CircleCollider2DComponent>()) {
				auto &collider = entity.GetComponent<CircleCollider2DComponent>();
				b2CircleShape circle{};
				circle.m_radius = collider.radius* transform.scale.x;
				circle.m_p.Set(collider.offset.x,collider.offset.y);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circle;
				fixtureDef.density = collider.density;
				fixtureDef.friction = collider.friction;
				fixtureDef.restitution = collider.restitution;
				fixtureDef.restitutionThreshold = collider.MinRestitution;
				body->CreateFixture(&fixtureDef);
				collider.ptr = body->GetFixtureList();
			}
		});

	}

	void Scene::OnRuntimeStop() {
		delete PhysicalWorld;
		PhysicalWorld = nullptr;
	}

	Scene::~Scene() {
	}

	Ref<Scene> Scene::Copy(Ref<Scene> src) {
		auto res = CreateRef<Scene>();
		res->viewportWidth = src->viewportWidth;
		res->viewportHeight = src->viewportHeight;
		auto &resRegistry = res->registry;
		auto &srcRegistry = src->registry;

		srcRegistry.view<IDComponent>().each([&](auto id, auto &idComponent) {
			Entity srcEntity{id, src.get()};
			auto entity = res->CreateEntityWithGuid(idComponent.ID, srcRegistry.get<TagComponent>(id).tag);
			auto &trans = srcEntity.GetComponent<TransformComponent>();
			resRegistry.emplace_or_replace<TransformComponent>(entity, trans);
			if (srcEntity.HasComponent<CameraComponent>()) {
				auto &camera = srcEntity.GetComponent<CameraComponent>();
				resRegistry.emplace_or_replace<CameraComponent>(entity, camera);
			}
			if (srcEntity.HasComponent<SpriteRendererComponent>()) {
				auto &sprite = srcEntity.GetComponent<SpriteRendererComponent>();
				resRegistry.emplace_or_replace<SpriteRendererComponent>(entity, sprite);
			}
			if (srcEntity.HasComponent<ScriptComponent>()) {
				auto &script = srcEntity.GetComponent<ScriptComponent>();
				resRegistry.emplace_or_replace<ScriptComponent>(entity, script);
			}
			if (srcEntity.HasComponent<RigidBody2DComponent>()) {
				auto &rigidBody = srcEntity.GetComponent<RigidBody2DComponent>();
				resRegistry.emplace_or_replace<RigidBody2DComponent>(entity, rigidBody);
			}
			if (srcEntity.HasComponent<BoxCollider2DComponent>()) {
				auto &collider = srcEntity.GetComponent<BoxCollider2DComponent>();
				resRegistry.emplace_or_replace<BoxCollider2DComponent>(entity, collider);
			}
			if (srcEntity.HasComponent<CircleRendererComponent>()) {
				auto &circle = srcEntity.GetComponent<CircleRendererComponent>();
				resRegistry.emplace_or_replace<CircleRendererComponent>(entity, circle);
			}
			if (srcEntity.HasComponent<CircleCollider2DComponent>()) {
				auto &circle = srcEntity.GetComponent<CircleCollider2DComponent>();
				resRegistry.emplace_or_replace<CircleCollider2DComponent>(entity, circle);
			}
		});
		return res;
	}

	void Scene::CopyEntity(Entity entity) {
		Entity res = CreateEntity(entity.GetName());
		Temp::CopyComponent<TransformComponent>(res, entity);
		Temp::CopyComponent<CameraComponent>(res, entity);
		Temp::CopyComponent<SpriteRendererComponent>(res, entity);
		Temp::CopyComponent<ScriptComponent>(res, entity);
		Temp::CopyComponent<RigidBody2DComponent>(res, entity);
		Temp::CopyComponent<CircleRendererComponent>(res, entity);
		Temp::CopyComponent<BoxCollider2DComponent>(res, entity);
		Temp::CopyComponent<CircleCollider2DComponent>(res, entity);
	}


	template<class Ty>
	void Scene::OnComponentAdd(Entity entity, Ty &component) {

	}

	template<>
	void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent &component) {
		component.camera.OnViewportResize(viewportWidth, viewportHeight);
	}

	template<>
	void Scene::OnComponentAdd<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component) {

	}

	template<>
	void Scene::OnComponentAdd<TagComponent>(Entity entity, TagComponent &component) {

	}

	template<>
	void Scene::OnComponentAdd<TransformComponent>(Entity entity, TransformComponent &component) {}


	template<>
	void Scene::OnComponentAdd<ScriptComponent>(Entity entity, ScriptComponent &component) {}

	template<>
	void Scene::OnComponentAdd<RigidBody2DComponent>(Entity entity, RigidBody2DComponent &component) {}

	template<>
	void Scene::OnComponentAdd<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent &component) {}


	template<>
	void Scene::OnComponentAdd<IDComponent>(Entity entity, IDComponent &component) {}

	template<>
	void Scene::OnComponentAdd<CircleRendererComponent>(Entity entity, CircleRendererComponent &component) {}

	template<>
	void Scene::OnComponentAdd<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent &component) {}

}