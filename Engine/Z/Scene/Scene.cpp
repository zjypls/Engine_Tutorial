//
// Created by 32725 on 2023/3/27.
//

#include "Include/box2d/include/box2d/b2_world.h"
#include "Include/box2d/include/box2d/b2_body.h"
#include "Include/box2d/include/box2d/b2_fixture.h"
#include "Include/box2d/include/box2d/b2_polygon_shape.h"
#include "Include/box2d/include/box2d/b2_circle_shape.h"
#include "Include/box2d/include/box2d/b2_collision.h"

#include "Z/Scene/Scene.h"
#include "Z/Scene/ScriptEntity.h"
#include "Z/Script/ScriptEngine.h"
#include "Z/Scene/Components.h"

namespace Z {

	namespace Tools {
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

		template<class... T>
		static void CopyEntity(Type<T...>, Entity src, Entity dst) {
			([&]() {
				auto& typeID= typeid(T);
				if(typeID == typeid(IDComponent)||typeID==typeid(TagComponent)||typeID==typeid(TransformComponent))
					return;
				if (src.HasComponent<T>())
					dst.AddOrReplaceComponent<T>(src.GetComponent<T>());
			}(), ...);
		}
	}


	void Scene::OnRuntimeStart() {
		Running = true;
		OnPhysics2DStart();
		OnScriptStart();
	}

	void Scene::OnRuntimeStop() {
		Running = false;
		OnPhysics2DStop();
		OnScriptStop();
	}
    [[deprecated("should use 3D version !")]]
	void Scene::OnPhysics2DStart() {
/*
        PhysicalWorld = new b2World(b2Vec2(0.0f, -9.8f));
		registry.view<RigidBody2DComponent>().each([&](auto id, auto &rigidBody) {
			Entity entity{id, this};
			auto &transform = entity.GetComponent<TransformComponent>();
			b2BodyDef bodyDef;
			bodyDef.type = Tools::GetBox2DType(rigidBody.bodyType);
			bodyDef.position.Set(transform.translation.x, transform.translation.y);
			bodyDef.angle = transform.rotation.z;
			bodyDef.fixedRotation = rigidBody.fixedRotation;
			b2Body *body = ((b2World *) PhysicalWorld)->CreateBody(&bodyDef);
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
				*(int *) collider.ptr = {body->GetType() != b2_staticBody};
			}
			if (entity.HasComponent<CircleCollider2DComponent>()) {
				auto &collider = entity.GetComponent<CircleCollider2DComponent>();
				b2CircleShape circle{};
				circle.m_radius = collider.radius * transform.scale.x;
				circle.m_p.Set(collider.offset.x, collider.offset.y);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circle;
				fixtureDef.density = collider.density;
				fixtureDef.friction = collider.friction;
				fixtureDef.restitution = collider.restitution;
				fixtureDef.restitutionThreshold = collider.MinRestitution;
				body->CreateFixture(&fixtureDef);
				//Todo: improve this
				*(int *) collider.ptr = {body->GetType() != b2_staticBody};
			}
		});
*/
	}

    [[deprecated("should use 3D version !")]]
	void Scene::OnPhysics2DStop() {
/*		delete PhysicalWorld;
		PhysicalWorld = nullptr;*/
	}

	Entity Scene::CreateEntity(const std::string &name) {
		Entity entity{registry.create(), this};
		auto &id = entity.AddComponent<IDComponent>();
		entity.AddComponent<TransformComponent>();
		auto &tag = entity.AddComponent<TagComponent>(name);
		if (tag.tag.empty())tag.tag = "Entity";
		entities[id.ID] = entity;
		return entity;
	}

	Entity Scene::CreateEntityWithGuid(zGUID guid, const std::string &name) {
		Entity entity{registry.create(), this};
		entity.AddComponent<IDComponent>(guid);
		entity.AddComponent<TransformComponent>();
		auto &tag = entity.AddComponent<TagComponent>(name);
		if (tag.tag.empty())tag.tag = "Entity";
		if (entities.find(guid) == entities.end())
			entities[guid] = entity;
		return entity;
	}

	void Scene::OnViewportResize(uint32 width, uint32 height) {
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
			if (view.get<CameraComponent>(entity).primary) {
				return Entity{entity, this};
			}
		}
		return {};
	}

	void Scene::OnEditorUpdate(float deltaTime, EditorCamera &camera) {
		//TODO:This should be optimized
		Render2D();
		Render3D();
	}

	void Scene::OnSimulateUpdate(float deltaTime, EditorCamera &camera) {
		if (!Paused||FrameStepCount-->0)
			OnPhysics2DUpdate(deltaTime);
		Render2D();
		Render3D();
	}

	void Scene::OnPreviewUpdate(float deltaTime, Camera &camera, glm::mat4 transform) {
		Render2D();
		Render3D();
	}

	void Scene::OnUpdate(float deltaTime) {
		Entity mainCamera = {};
		if (!Paused ||FrameStepCount-->0) {
			NativeScriptUpdate(deltaTime);
			ScriptUpdate(deltaTime);
			OnPhysics2DUpdate(deltaTime);
		}
		mainCamera = GetMainCamera();
		if (!mainCamera)return;
		Render2D();
		Render3D();
	}


	void Scene::Render2D() {
		std::for_each(registry.view<TransformComponent, SpriteRendererComponent>().begin(),
		              registry.view<TransformComponent, SpriteRendererComponent>().end(), [&](const auto &item) {
				});
		std::for_each(registry.view<TransformComponent, CircleRendererComponent>().begin(),
		              registry.view<TransformComponent, CircleRendererComponent>().end(), [&](const auto &item) {
				});
	}


	void Scene::Render3D() {
		std::for_each(registry.view<TransformComponent, MeshFilterComponent>().begin(),
                      registry.view<TransformComponent, MeshFilterComponent>().end(), [&](const auto &item) {
                    MeshFilterComponent component=registry.get<MeshFilterComponent>(item);
				});
	}

	void Scene::NativeScriptUpdate(float deltaTime) {
		registry.view<NativeScriptComponent>().each([&](auto entity, auto &script) {
			if (!script.instance) {
				script.instance = script.onConstruct();
				script.instance->entity = Entity{entity, this};
				script.instance->OnCreate();
			}
			script.instance->OnUpdate(deltaTime);
		});
	}

	void Scene::OnPhysics2DUpdate(float deltaTime) {
/*
		((b2World *) PhysicalWorld)->Step(deltaTime, 8, 3);
		registry.view<RigidBody2DComponent>().each([&](auto id, auto &rigidBody) {
			if (rigidBody.bodyType != RigidBody2DComponent::BodyType::Static) {
				Entity entity{id, this};
				auto &transform = entity.GetComponent<TransformComponent>();
				auto body = (b2Body *) rigidBody.ptr;
				const auto &position = body->GetPosition();
				//Todo: improve this
				if (!body->IsAwake()) {
					if (entity.HasComponent<BoxCollider2DComponent>()) {
						*(int *) (entity.GetComponent<BoxCollider2DComponent>().ptr) = 0;
					}
					if (entity.HasComponent<CircleCollider2DComponent>()) {
						*(int *) (entity.GetComponent<CircleCollider2DComponent>().ptr) = 0;
					}
				} else {
					if (entity.HasComponent<BoxCollider2DComponent>()) {
						*(int *) (entity.GetComponent<BoxCollider2DComponent>().ptr) = 1;
					}
					if (entity.HasComponent<CircleCollider2DComponent>()) {
						*(int *) (entity.GetComponent<CircleCollider2DComponent>().ptr) = 1;
					}
				}
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();
			}
		});
        */
	}

	void Scene::OnSimulateStart() {
		OnPhysics2DStart();
	}

	void Scene::OnSimulateStop() {
		OnPhysics2DStop();
	}

	Scene::~Scene() {
		delete PhysicalWorld;
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
			Tools::CopyEntity(NoBaseTypes{}, srcEntity, entity);
		});
		return res;
	}


	Entity Scene::InstantiateEntity(Entity entity) {
		Entity res = CreateEntity(entity.GetName());
		Tools::CopyEntity(NoBaseTypes{}, entity, res);
		return res;
	}

	void Scene::ScriptUpdate(float deltaTime) {
		registry.view<ScriptComponent>().each([&](auto id, auto &script) {
			Entity entity{id, this};
			ScriptEngine::OnRuntimeUpdate(entity, deltaTime);
		});
	}

	void Scene::OnScriptStart() {
		ScriptEngine::OnRuntimeStart(this);
		registry.view<ScriptComponent>().each([&](auto id, auto &script) {
			Entity entity{id, this};
			ScriptEngine::CreateInstance(entity);
		});
	}

	void Scene::OnScriptStop() {
		ScriptEngine::OnRuntimeStop();
	}

	Entity Scene::GetEntityWithGUID(zGUID guid) {
		if (entities.find(guid) != entities.end())
			return entities[guid];
		return Entity{};
	}

	void Scene::GetEntitiesByName(const std::string &name, std::vector<zGUID> &ids) {
		registry.view<TagComponent>().each([&](auto id, auto &tag) {
			if (tag.tag == name) {
				ids.push_back(registry.get<IDComponent>(id).ID);
			}
		});
	}

    //todo:fix
    //hide this to make dynamic work on Windows with msvc
    //need to improve
/*
    template<class Ty>
	void Scene::OnComponentAdd(Entity entity, Ty &component) {

	}

	template<>
	void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent &component) {
		component.camera.OnViewportResize(viewportWidth, viewportHeight);
	}

	template<>
	void Scene::OnComponentAdd<MeshRendererComponent>(Z::Entity entity, MeshRendererComponent &component) {

	}

	template<>
	void Scene::OnComponentAdd<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component) {

	}

	template<>
	void Scene::OnComponentAdd<ScriptComponent>(Entity entity, ScriptComponent &component) {

	}

	template<>
	void Scene::OnComponentAdd<TagComponent>(Entity entity, TagComponent &component) {

	}

	template<>
	void Scene::OnComponentAdd<TransformComponent>(Entity entity, TransformComponent &component) {}


	template<>
	void Scene::OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent &component) {}

	template<>
	void Scene::OnComponentAdd<RigidBody2DComponent>(Entity entity, RigidBody2DComponent &component) {}

	template<>
	void Scene::OnComponentAdd<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent &component) {}


	template<>
	void Scene::OnComponentAdd<IDComponent>(Entity entity, IDComponent &component) {}

	template<>
	void Scene::OnComponentAdd<CircleRendererComponent>(Entity entity, CircleRendererComponent &component) {}

	template<>
	void Scene::OnComponentAdd<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent &component) {}*/

}