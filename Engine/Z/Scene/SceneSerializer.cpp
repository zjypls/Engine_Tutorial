//
// Created by 32725 on 2023/3/30.
//
#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "SceneSerializer.h"
#include "yaml-cpp/yaml.h"
#include "Entity.h"
#include "Components.h"


namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3 &rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node &node, glm::vec3 &rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2 &rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node &node, glm::vec2 &rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}

	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4 &rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node &node, glm::vec4 &rhs) {
			if (!node.IsSequence() || node.size() != 4) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}


namespace Z {

	inline std::string ToString(RigidBody2DComponent::BodyType type) {
		switch (type) {
			case RigidBody2DComponent::BodyType::Static:
				return "Static";
			case RigidBody2DComponent::BodyType::Dynamic:
				return "Dynamic";
			case RigidBody2DComponent::BodyType::Kinematic:
				return "Kinematic";
		}
		Z_CORE_ASSERT(false, "Unknown RigidBody2DComponent::BodyType!");
		return "Unknown";
	}

	inline RigidBody2DComponent::BodyType ToBodyType(const std::string &type) {
		if (type == "Static") {
			return RigidBody2DComponent::BodyType::Static;
		}
		if (type == "Dynamic") {
			return RigidBody2DComponent::BodyType::Dynamic;
		}
		if (type == "Kinematic") {
			return RigidBody2DComponent::BodyType::Kinematic;
		}
		Z_CORE_ASSERT(false, "Unknown RigidBody2DComponent::BodyType!");
		return RigidBody2DComponent::BodyType::Static;
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec2 &vec) {
		out << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &vec) {
		out << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &vec) {
		out << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, RigidBody2DComponent::BodyType type) {
		return out << YAML::Flow << ToString(type);
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : scene(scene) {

	}

	static void SerializerEntity(YAML::Emitter &out, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUID();
		auto &tag = entity.GetComponent<TagComponent>();
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Tag" << YAML::Value << tag.tag;
			out << YAML::EndMap;
		}
		//
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto &transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transform.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.scale;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto &camera = entity.GetComponent<CameraComponent>();
			auto &cameraProps = camera.camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int) cameraProps.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << cameraProps.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << cameraProps.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << cameraProps.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << cameraProps.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << cameraProps.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << cameraProps.GetOrthographicFarClip();
			out << YAML::Key << "Primary" << YAML::Value << camera.primary;
			out << YAML::EndMap;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto &sprite = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << sprite.color;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<RigidBody2DComponent>()) {
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap;
			auto &rigid2D = entity.GetComponent<RigidBody2DComponent>();
			out << YAML::Key << "bodyType" << YAML::Value << rigid2D.bodyType;
			out << YAML::Key << "fixedRotation" << YAML::Value << rigid2D.fixedRotation;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<BoxCollider2DComponent>()) {
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;
			auto &Collider2D = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "size" << YAML::Value << Collider2D.size;
			out << YAML::Key << "offset" << YAML::Value << Collider2D.offset;
			out << YAML::Key << "IsTrigger" << YAML::Value << Collider2D.isTrigger;
			out << YAML::Key << "visualize" << YAML::Value << Collider2D.visualize;
			out << YAML::Key << "Density" << YAML::Value << Collider2D.density;
			out << YAML::Key << "Friction" << YAML::Value << Collider2D.friction;
			out << YAML::Key << "Restitution" << YAML::Value << Collider2D.restitution;
			out << YAML::Key << "MinRestitution" << YAML::Value << Collider2D.MinRestitution;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CircleRendererComponent>()) {
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;
			auto &circle = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circle.color;
			out << YAML::Key << "thickness" << YAML::Value << circle.thickness;
			out << YAML::Key << "fade" << YAML::Value << circle.fade;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CircleCollider2DComponent>()) {
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;
			auto &collier = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "IsTrigger" << YAML::Value << collier.isTrigger;
			out << YAML::Key << "visualize" << YAML::Value << collier.visualize;
			out << YAML::Key << "offset" << YAML::Value << collier.offset;
			out << YAML::Key << "radius" << YAML::Value << collier.radius;
			out << YAML::Key << "density" << YAML::Value << collier.density;
			out << YAML::Key << "friction" << YAML::Value << collier.friction;
			out << YAML::Key << "restitution" << YAML::Value << collier.restitution;
			out << YAML::Key << "MinRestitution" << YAML::Value << collier.MinRestitution;
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string &filepath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		scene->registry.each([&](auto ID) {
			Entity entity(ID, scene.get());
			if (!entity)
				return;
			SerializerEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filepath, std::ios::out | std::ios::trunc);
		fout << out.c_str();
		fout.close();
		Z_CORE_INFO("Scene saved to {0}", filepath);
	}

	void SceneSerializer::SerializeRuntime(std::stringstream &data) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		scene->registry.each([&](auto ID) {
			Entity entity(ID, scene.get());
			if (!entity)
				return;
			SerializerEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;
		data << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string &filepath) {
		std::ifstream in(filepath);
		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();
		return DeserializeRuntime(buffer);
	}

	bool SceneSerializer::DeserializeRuntime(std::stringstream &data) {
		YAML::Node root = YAML::Load(data.str());
		if (!root["Scene"])
			return false;
		auto sceneName = root["Scene"].as<std::string>();
		Z_CORE_INFO("Deserializing scene {0}", sceneName);
		auto Entities = root["Entities"];
		for (auto Entity: Entities) {
			auto id = Entity["Entity"].as<uint64_t>();
			std::string name;
			auto tagComponent = Entity["TagComponent"];
			name = tagComponent["Tag"].as<std::string>();
			Z_CORE_INFO("Deserializing entity with name {0}", name);
			auto entity = scene->CreateEntityWithGuid(id, name);
			auto transformComponent = Entity["TransformComponent"];
			auto &transform = entity.GetComponent<TransformComponent>();
			transform.translation = transformComponent["Translation"].as<glm::vec3>();
			transform.rotation = transformComponent["Rotation"].as<glm::vec3>();
			transform.scale = transformComponent["Scale"].as<glm::vec3>();

			auto cameraComponent = Entity["CameraComponent"];
			if (cameraComponent) {
				auto &camera = entity.AddComponent<CameraComponent>();
				auto cameraProps = cameraComponent["Camera"];
				camera.camera.SetProjectionType((SceneCamera::ProjectionType) cameraProps["ProjectionType"].as<int>());
				camera.camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
				camera.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				camera.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
				camera.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
				camera.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
				camera.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
				camera.primary = cameraProps["Primary"].as<bool>();
			}
			auto spriteRendererComponent = Entity["SpriteRendererComponent"];
			if (spriteRendererComponent) {
				auto &sprite = entity.AddComponent<SpriteRendererComponent>();
				sprite.color = spriteRendererComponent["Color"].as<glm::vec4>();
			}
			auto rigidBody2DComponent = Entity["RigidBody2DComponent"];
			if (rigidBody2DComponent) {
				auto &rigid2D = entity.AddComponent<RigidBody2DComponent>();
				rigid2D.bodyType = ToBodyType(rigidBody2DComponent["bodyType"].as<std::string>());
				rigid2D.fixedRotation = rigidBody2DComponent["fixedRotation"].as<bool>();
			}
			auto boxCollider2DComponent = Entity["BoxCollider2DComponent"];
			if (boxCollider2DComponent) {
				auto &Collider2D = entity.AddComponent<BoxCollider2DComponent>();
				Collider2D.size = boxCollider2DComponent["size"].as<glm::vec2>();
				Collider2D.offset = boxCollider2DComponent["offset"].as<glm::vec2>();
				Collider2D.isTrigger = boxCollider2DComponent["IsTrigger"].as<bool>();
				Collider2D.visualize = boxCollider2DComponent["visualize"].as<bool>();
				Collider2D.density = boxCollider2DComponent["Density"].as<float>();
				Collider2D.friction = boxCollider2DComponent["Friction"].as<float>();
				Collider2D.restitution = boxCollider2DComponent["Restitution"].as<float>();
				Collider2D.MinRestitution = boxCollider2DComponent["MinRestitution"].as<float>();
			}
			auto circleRender = Entity["CircleRendererComponent"];
			if (circleRender) {
				auto &circle = entity.AddComponent<CircleRendererComponent>();
				circle.color = circleRender["Color"].as<glm::vec4>();
				circle.thickness = circleRender["thickness"].as<float>();
				circle.fade = circleRender["fade"].as<float>();
			}
			auto circleCollier = Entity["CircleCollider2DComponent"];
			if (circleCollier) {
				auto &collider = entity.AddComponent<CircleCollider2DComponent>();
				collider.isTrigger = circleCollier["IsTrigger"].as<bool>();
				collider.visualize = boxCollider2DComponent["visualize"].as<bool>();
				collider.density = circleCollier["density"].as<float>();
				collider.friction = circleCollier["friction"].as<float>();
				collider.restitution = circleCollier["restitution"].as<float>();
				collider.MinRestitution = circleCollier["MinRestitution"].as<float>();
				collider.radius = circleCollier["radius"].as<float>();
				collider.offset = circleCollier["offset"].as<glm::vec2>();
			}
		}
		return true;
	}
}