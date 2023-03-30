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

	YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &vec) {
		out << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &vec) {
		out << YAML::Flow << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene> &scene) : scene(scene) {

	}

	static void SerializerEntity(YAML::Emitter &out, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetComponent<TagComponent>().tag;
		auto &tag = entity.GetComponent<TagComponent>();
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Tag" << YAML::Value << tag.tag;
			out << YAML::EndMap;
		}

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

	void SceneSerializer::SerializeRuntime(const std::string &filepath) {

	}

	bool SceneSerializer::Deserialize(const std::string &filepath) {
		std::ifstream in(filepath);
		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();
		YAML::Node data = YAML::Load(buffer.str());
		if (!data["Scene"])
			return false;
		auto sceneName = data["Scene"].as<std::string>();
		Z_CORE_INFO("Deserializing scene {0}", sceneName);
		auto Entities = data["Entities"];
		for (auto Entity: Entities) {
			std::string name;
			auto tagComponent = Entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();
			Z_CORE_INFO("Deserializing entity with name {0}", name);
			auto entity = scene->CreateEntity(name);
			auto transformComponent = Entity["TransformComponent"];
			if (transformComponent) {
				auto &transform = entity.GetComponent<TransformComponent>();
				transform.translation = transformComponent["Translation"].as<glm::vec3>();
				transform.rotation = transformComponent["Rotation"].as<glm::vec3>();
				transform.scale = transformComponent["Scale"].as<glm::vec3>();
			}
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
		}
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string &filepath) {
		return false;
	}
}
