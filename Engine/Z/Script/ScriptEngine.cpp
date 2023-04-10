//
// Created by 32725 on 2023/4/6.
//

#include<fstream>
#include <filesystem>
#include <iostream>
#include <utility>
#include "ScriptEngine.h"
#include "ScriptReg.h"
#include "Z/Scene/Entity.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/attrdefs.h"

namespace Z {
	static std::unordered_map<std::string, ScriptFieldType> FieldTypeMap{
			{"System.Byte",    ScriptFieldType::Byte},
			{"System.Char",    ScriptFieldType::Char},
			{"System.Int16",   ScriptFieldType::Int16},
			{"System.UInt16",  ScriptFieldType::UInt16},
			{"System.Int32",   ScriptFieldType::Int},
			{"System.UInt32",  ScriptFieldType::UInt},
			{"System.Int64",   ScriptFieldType::Long},
			{"System.UInt64",  ScriptFieldType::ULong},
			{"System.Single",  ScriptFieldType::Float},
			{"System.Double",  ScriptFieldType::Double},
			{"System.Boolean", ScriptFieldType::Bool},
			{"System.String",  ScriptFieldType::String},
			{"Z.EntityCore",   ScriptFieldType::Entity},
			{"Z.Vector2",      ScriptFieldType::Float2},
			{"Z.Vector3",      ScriptFieldType::Float3},
			{"Z.Vector4",      ScriptFieldType::Float4}
	};
	namespace Temp {
		std::string ReadBytes(const std::filesystem::path &path) {
			std::ifstream file(path, std::ios::ate | std::ios::binary);
			Z_CORE_ASSERT(file.is_open(), "Failed to open file!");
			size_t fileSize = (size_t) file.tellg();
			std::string buffer(fileSize, ' ');
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();
			return buffer;
		}

		MonoAssembly *LoadMonoAssembly(const std::filesystem::path &assemblyPath) {
			uint32_t fileSize = 0;
			auto fileData = ReadBytes(assemblyPath);

			MonoImageOpenStatus status;
			MonoImage *image = mono_image_open_from_data_full(fileData.data(), fileData.size(), 1, &status, 0);
			Z_CORE_ASSERT(status == MONO_IMAGE_OK, "Failed to open Mono image! (status: {0})", status);

			auto str = assemblyPath.string();
			MonoAssembly *assembly = mono_assembly_load_from_full(image, str.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly *assembly) {
			MonoImage *image = mono_assembly_get_image(assembly);
			const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++) {
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				std::string nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				std::string name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				MonoClass *monoClass = mono_class_from_name(image, nameSpace.c_str(), name.c_str());
				static MonoClass *baseClass = mono_class_from_name(image, "Z", "ScriptCore");
				if (mono_class_is_subclass_of(monoClass, baseClass, false)) {
					Z_CORE_INFO("ScripteEngine:PrintAssemblyTypes Subclass: Type: {0}.{1}", nameSpace, name);
				} else
					Z_CORE_INFO("ScripteEngine:GiveFileMessage: Type: {0}.{1}", nameSpace, name);
			}
		}

	}

	std::string FieldTypeToString(ScriptFieldType type) {
		switch (type) {
			case ScriptFieldType::Byte:
				return "System.Byte";
			case ScriptFieldType::Char:
				return "System.Char";
			case ScriptFieldType::Int16:
				return "System.Int16";
			case ScriptFieldType::UInt16:
				return "System.UInt16";
			case ScriptFieldType::Int:
				return "System.Int32";
			case ScriptFieldType::UInt:
				return "System.UInt32";
			case ScriptFieldType::Long:
				return "System.Int64";
			case ScriptFieldType::ULong:
				return "System.UInt64";
			case ScriptFieldType::Float:
				return "System.Single";
			case ScriptFieldType::Double:
				return "System.Double";
			case ScriptFieldType::Bool:
				return "System.Boolean";
			case ScriptFieldType::String:
				return "System.String";
			case ScriptFieldType::Entity:
				return "Z.EntityCore";
			case ScriptFieldType::Float2:
				return "Z.Vector2";
			case ScriptFieldType::Float3:
				return "Z.Vector3";
			case ScriptFieldType::Float4:
				return "Z.Vector4";
			default:
				return "<Invalid>";
		}
	}

	struct ScriptEngineData {
		MonoDomain *rootDomain = nullptr, *appDomain = nullptr;
		MonoAssembly *assembly = nullptr;
		Ref<ScriptClass> Class = nullptr;
		MonoImage *image = nullptr, *appImage = nullptr;
		Scene *scene = nullptr;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<GUID, Ref<ScriptInstance>> EntityInstances;
	};
	static ScriptEngineData *scriptData;

	void ScriptEngine::Init() {
		scriptData = new ScriptEngineData();
		MonoInit();
		LoadCoreAssembly(Temp::LoadMonoAssembly("Bin-C/MSVC/ScriptCore.dll"));
		ScriptReg::Reg();
		ScriptReg::RegComponents();
	}

	void ScriptEngine::ShutDown() {
		MonoShutDown();
		delete scriptData;
	}

	void ScriptEngine::GetClasses(MonoAssembly *assembly) {
		MonoImage *image = mono_assembly_get_image(assembly);
		const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::string className;
			if (std::strlen(nameSpace))
				className = fmt::format("{0}.{1}", nameSpace, name);
			else
				className = name;
			auto klass = CreateRef<ScriptClass>(nameSpace, name);
			if (!klass->IsSubClassOf(scriptData->Class))
				continue;
			scriptData->EntityClasses[className] = klass;
			Z_CORE_INFO("ScriptEngine:AssemblyTypes Subclass: Type: {0}", className);
			uint32_t count = klass->GetMethodCount();
			Z_CORE_INFO("ScriptEngine:AssemblyTypes Subclass: MethodCount: {0}", count);
			MonoClass *monoClass = klass->GetClass();
			void *iter = nullptr;
			while (auto filed = mono_class_get_fields(monoClass, &iter)) {
				auto typeName = mono_type_get_name(mono_field_get_type(filed));
				auto name = mono_field_get_name(filed);
				auto flags = mono_field_get_flags(filed);
				if ((flags & MONO_FIELD_ATTR_PUBLIC) && FieldTypeMap.find(typeName) != FieldTypeMap.end())
					klass->Fields[name] = ScriptField{name,FieldTypeMap[typeName],filed};
			}
		}
		for (const auto &[className, klass]: scriptData->EntityClasses) {
			for (const auto &[name, type]: klass->Fields) {
				Z_CORE_INFO("\tScriptEngine:AssemblyTypes Subclass: Field: {0} {1}", name, FieldTypeToString(type.Type));
			}
		}
	}

	void ScriptEngine::MonoInit() {
		mono_set_assemblies_path("mono/lib/4.5");
		scriptData->rootDomain = mono_jit_init("ZJIT");
	}

	void ScriptEngine::MonoShutDown() {
		//mono_domain_unload(scriptData->appDomain);
		mono_jit_cleanup(scriptData->rootDomain);
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path &path) {
		auto assembly = Temp::LoadMonoAssembly(path);
		scriptData->appImage = mono_assembly_get_image(assembly);
		GetClasses(assembly);
	}

	MonoObject *ScriptEngine::GetInstance(MonoClass *Class) {
		return mono_object_new(scriptData->appDomain, Class);
	}


	void ScriptEngine::LoadCoreAssembly(MonoAssembly *assembly) {
		scriptData->image = mono_assembly_get_image(assembly);
		scriptData->Class = CreateRef<ScriptClass>("Z", "EntityCore");
		scriptData->EntityClasses.clear();
		scriptData->EntityInstances.clear();
	}

	bool ScriptEngine::ClassExists(const std::string &name) {
		return scriptData->EntityClasses.find(name) != scriptData->EntityClasses.end();
	}

	void ScriptEngine::OnRuntimeStart(Scene *scene) {
		scriptData->appDomain = mono_domain_create_appdomain("ZAppDomain", nullptr);
		scriptData->scene = scene;
	}

	void ScriptEngine::OnRuntimeUpdate(Entity entity, float deltaTime) {
		auto uuid = entity.GetUID();
		if (scriptData->EntityInstances.find(uuid) != scriptData->EntityInstances.end()) {
			scriptData->EntityInstances[uuid]->OnUpdate(entity, deltaTime);
		}
	}

	void ScriptEngine::OnRuntimeStop() {
		scriptData->EntityInstances.clear();
		mono_domain_unload(scriptData->appDomain);
		scriptData->scene = nullptr;
	}

	void ScriptEngine::CreateInstance(Entity entity) {
		auto &script = entity.GetComponent<ScriptComponent>();
		if (ClassExists(script.scriptName)) {
			auto klass = scriptData->EntityClasses[script.scriptName];
			auto instance = CreateRef<ScriptInstance>(klass, entity);
			instance->OnCreate();
			scriptData->EntityInstances[entity.GetUID()] = instance;
		} else {
			Z_CORE_ERROR("ScriptEngine:CreateInstance: Class {0} does not exist", script.scriptName);
			return;
		}
	}

	Scene *ScriptEngine::GetContext() {
		return scriptData->scene;
	}

	MonoImage *ScriptEngine::GetCoreImage() {
		return scriptData->image;
	}

	const std::unordered_map<std::string, Ref<ScriptClass>> &ScriptEngine::GetScriptList() {
		return scriptData->EntityClasses;
	}

	Ref<ScriptInstance> ScriptEngine::GetInstance(GUID id) {
		if(auto instance=scriptData->EntityInstances.find(id);instance!=scriptData->EntityInstances.end())
			return instance->second;
		else
			return nullptr;
	}

	ScriptClass::ScriptClass(const std::string &nameSpace, const std::string &name)
			: NameSpace(nameSpace), ClassName(name) {
		Class = mono_class_from_name(scriptData->appImage == nullptr ? scriptData->image : scriptData->appImage,
		                             NameSpace.c_str(), ClassName.c_str());
	}

	MonoObject *ScriptClass::GetInstance() {
		return ScriptEngine::GetInstance(Class);
	}

	MonoMethod *ScriptClass::GetMethod(const std::string &MethodName, int paramCount) {
		return mono_class_get_method_from_name(Class, MethodName.c_str(), paramCount);
	}

	void ScriptClass::InvokeMethod(MonoMethod *method, MonoObject *object, void **params, MonoObject **exc) {
		mono_runtime_invoke(method, object, params, exc);
	}

	bool ScriptClass::IsSubClassOf(const Ref<ScriptClass> &klass) {
		return mono_class_is_subclass_of(Class, klass->Class, false);
	}

	uint32_t ScriptClass::GetMethodCount() {
		return mono_class_num_fields(Class);
	}

	unsigned char ScriptClass::buffer[64];
	void ScriptClass::SetValue(const std::string &name, void *ptr) {
//		mono_field_static_set_value(mono_class_vtable(scriptData->rootDomain,Class),
//		                            Fields.at(name).Field,ptr);
	}
	void ScriptClass::InnerGetValue(const std::string &name, void *ptr) {
//		mono_field_static_get_value(mono_class_vtable(scriptData->rootDomain,Class),
//		                            Fields.at(name).Field,ptr);
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> klass, Entity entity) : Class(klass) {
		instance = Class->GetInstance();
		construct = scriptData->Class->GetMethod(".ctor", 1);
		unsigned long long id = entity.GetUID();
		void *uuid = &id;
		Class->InvokeMethod(construct, instance, &uuid, nullptr);
		create = Class->GetMethod("OnCreate");
		update = Class->GetMethod("OnUpdate", 1);
	}

	void ScriptInstance::OnCreate() {
		Class->InvokeMethod(create, instance, nullptr, nullptr);
	}

	void ScriptInstance::OnUpdate(Entity entity, float deltaTime) {
		void *args = &deltaTime;
		Class->InvokeMethod(update, instance, &args);
	}

	void ScriptInstance::GetValue(const std::string &name,void*ptr) {
		mono_field_get_value(instance, mono_class_get_field_from_name(Class->GetClass(), name.c_str()),ptr);
	}

	unsigned char ScriptInstance::buffer[64]{0};

	void ScriptInstance::SetValue(const std::string &name, void *ptr) {
		mono_field_set_value(instance, mono_class_get_field_from_name(Class->GetClass(),name.c_str()),ptr);
	}

}
