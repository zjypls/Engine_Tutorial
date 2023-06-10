//
// Created by 32725 on 2023/3/30.
//
//#include "Z/Core/Core.h"
//#include "Z/Core/Log.h"
#include "Entity.hpp"
#include "Components.h"
#include "Z/Script/ScriptEngine.h"
#include "SceneSerializer.h"
#include "yaml-cpp/yaml.h"


namespace YAML {

	template<int L, typename T, glm::qualifier Q>
	struct convert<glm::vec<L, T, Q>> {
		static Node encode(const glm::vec<L, T, Q> &rhs) {
			Node node;
			for (int i = 0; i < L; i++) {
				node.push_back(rhs[i]);
			}
			return node;
		}

		static bool decode(const Node &node, glm::vec<L, T, Q> &rhs) {
			if (!node.IsSequence() || node.size() != L) {
				return false;
			}
			for (int i = 0; i < L; i++) {
				rhs[i] = node[i].as<T>();
			}
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

	template<int L, typename T, glm::qualifier Q>
	YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec<L, T, Q> &vec) {
		out << YAML::Flow << YAML::BeginSeq;
		for (int i = 0; i < L; i++) {
			out << vec[i];
		}
		out << YAML::EndSeq;
		return out;
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, RigidBody2DComponent::BodyType type) {
		return out << YAML::Flow << ToString(type);
	}

	YAML::Emitter &operator<<(YAML::Emitter &out, const ScriptFieldBuffer &buffer) {
		out << YAML::Key << "FieldName" << YAML::Value << buffer.field.Name;
		out << YAML::Key << "Type" << YAML::Value << FieldTypeToString(buffer.field.Type);
		switch (buffer.field.Type) {
			case ScriptFieldType::Int:
				out << YAML::Key << "Value" << YAML::Value << *(int *) buffer.buffer;
				break;
			case ScriptFieldType::Float:
				out << YAML::Key << "Value" << YAML::Value << *(float *) buffer.buffer;
				break;
			case ScriptFieldType::Bool:
				out << YAML::Key << "Value" << YAML::Value << *(bool *) buffer.buffer;
				break;
			case ScriptFieldType::String:
				out << YAML::Key << "Value" << YAML::Value << (char *) buffer.buffer;
				break;
			case ScriptFieldType::Float2:
				out << YAML::Key << "Value" << YAML::Value << *(glm::vec2 *) buffer.buffer;
				break;
			case ScriptFieldType::Float3:
				out << YAML::Key << "Value" << YAML::Value << *(glm::vec3 *) buffer.buffer;
				break;
			case ScriptFieldType::Float4:
				out << YAML::Key << "Value" << YAML::Value << *(glm::vec4 *) buffer.buffer;
				break;
			default: Z_CORE_ASSERT(false, "Unknown ScriptFieldType!");
				break;
		}
		return out;
	}

	void ProcessFieldType(ScriptFieldBuffer &buffer, YAML::Node &node) {
		switch (buffer.field.Type) {
			case ScriptFieldType::Int:
				*(int *) buffer.buffer = node["Value"].as<int>();
				break;
			case ScriptFieldType::Float:
				*(float *) buffer.buffer = node["Value"].as<float>();
				break;
			case ScriptFieldType::Bool:
				*(bool *) buffer.buffer = node["Value"].as<bool>();
				break;
			case ScriptFieldType::String:
				strcpy((char *) buffer.buffer, node["Value"].as<std::string>().c_str());
				break;
			case ScriptFieldType::Float2:
				*(glm::vec2 *) buffer.buffer = node["Value"].as<glm::vec2>();
				break;
			case ScriptFieldType::Float3:
				*(glm::vec3 *) buffer.buffer = node["Value"].as<glm::vec3>();
				break;
			case ScriptFieldType::Float4:
				*(glm::vec4 *) buffer.buffer = node["Value"].as<glm::vec4>();
				break;
			default: Z_CORE_ASSERT(false, "Unknown ScriptFieldType!");
				break;
		}
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
			out << YAML::Key << "Density" << YAML::Value << Collider2D.density;
			out << YAML::Key << "Friction" << YAML::Value << Collider2D.friction;
			out << YAML::Key << "Restitution" << YAML::Value << Collider2D.restitution;
			out << YAML::Key << "MinRestitution" << YAML::Value << Collider2D.MinRestitution;
			out << YAML::Key << "ptr" << YAML::Value << *(int *) Collider2D.ptr;
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
			out << YAML::Key << "offset" << YAML::Value << collier.offset;
			out << YAML::Key << "radius" << YAML::Value << collier.radius;
			out << YAML::Key << "density" << YAML::Value << collier.density;
			out << YAML::Key << "friction" << YAML::Value << collier.friction;
			out << YAML::Key << "restitution" << YAML::Value << collier.restitution;
			out << YAML::Key << "MinRestitution" << YAML::Value << collier.MinRestitution;
			out << YAML::Key << "ptr" << YAML::Value << *(int *) collier.ptr;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<ScriptComponent>()) {
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			auto &script = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ScriptName" << YAML::Value << script.scriptName;
			if (ScriptEngine::ClassExists(script.scriptName)) {
				out << YAML::Key << "ScriptFields";
				out << YAML::BeginMap;
				auto &fields = ScriptEngine::GetFields(entity.GetUID(),
				                                       *ScriptEngine::GetScriptList().at(script.scriptName));
				for (auto &[name, field]: fields) {
					out << YAML::Key << name;
					out << YAML::BeginMap;
					out << field;
					out << YAML::EndMap;
				}
				out << YAML::EndMap;
			}
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
				Collider2D.density = boxCollider2DComponent["Density"].as<float>();
				Collider2D.friction = boxCollider2DComponent["Friction"].as<float>();
				Collider2D.restitution = boxCollider2DComponent["Restitution"].as<float>();
				Collider2D.MinRestitution = boxCollider2DComponent["MinRestitution"].as<float>();
				//Todo: improve this
				Collider2D.ptr = new int{boxCollider2DComponent["ptr"].as<int>()};
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
				collider.density = circleCollier["density"].as<float>();
				collider.friction = circleCollier["friction"].as<float>();
				collider.restitution = circleCollier["restitution"].as<float>();
				collider.MinRestitution = circleCollier["MinRestitution"].as<float>();
				collider.radius = circleCollier["radius"].as<float>();
				collider.offset = circleCollier["offset"].as<glm::vec2>();
				collider.ptr = new int{circleCollier["ptr"].as<int>()};
			}
			auto scriptComponent = Entity["ScriptComponent"];
			if (scriptComponent) {
				auto &script = entity.AddComponent<ScriptComponent>();
				script.scriptName = scriptComponent["ScriptName"].as<std::string>();
				if (ScriptEngine::ClassExists(script.scriptName)) {
					auto klass = ScriptEngine::GetScriptList().at(script.scriptName);
					ScriptEngine::RegisterEntityClassFields(id, *klass);
					auto &buffers = ScriptEngine::GetFields(id, *klass);
					auto fields = scriptComponent["ScriptFields"];
					if (fields) {
						for (auto field: buffers) {
							YAML::Node data = fields[field.second.field.Name];
							ProcessFieldType(buffers[field.second.field.Name], data);
						}
					}
				}
			}
		}
		return true;
	}
}