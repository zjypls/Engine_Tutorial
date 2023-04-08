//
// Created by 32725 on 2023/4/6.
//

#include "ScriptEngine.h"
#include "ScriptReg.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "glm/glm.hpp"
#include "Z/Scene/Entity.h"
#include "Z/Scene/Components.h"
#include<fstream>
#include <filesystem>
#include <iostream>
#include <utility>
#include <Windows.h>

namespace Z {
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

	struct ScriptEngineData {
		MonoDomain *rootDomain = nullptr, *appDomain = nullptr;
		MonoAssembly *assembly = nullptr;
		ScriptClass *Class = nullptr;
		MonoImage *image = nullptr;
		Scene *scene = nullptr;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<GUID, Ref<EntityInstance>> EntityInstances;
	};
	static ScriptEngineData *scriptData;

	void ScriptEngine::Init() {
		scriptData = new ScriptEngineData();
		MonoInit();
	}

	void ScriptEngine::ShutDown() {
		MonoShutDown();
		delete scriptData;
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
		scriptData->appDomain = mono_domain_create_appdomain("ZAppDomain", nullptr);
		mono_domain_set(scriptData->appDomain, true);
		ScriptReg::Reg();
		scriptData->assembly = Temp::LoadMonoAssembly(path);
		scriptData->image = mono_assembly_get_image(scriptData->assembly);
		scriptData->Class=new ScriptClass("Z", "EntityCore");
		LoadAssemblyClasses(scriptData->assembly);
		for (auto &[name, klass]: scriptData->EntityClasses) {
			Z_CORE_INFO("ScripteEngine:Entity Type:Name: {0}", name);
		}
	}

	MonoObject *ScriptEngine::GetInstance(MonoClass *Class) {
		MonoObject *object = mono_object_new(scriptData->appDomain, Class);
		//mono_runtime_object_init(object);
		return object;
	}

	void ScriptEngine::LoadAssemblyClasses(MonoAssembly *assembly) {
		scriptData->EntityClasses.clear();
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

			MonoClass *monoClass = mono_class_from_name(image, nameSpace, name);
			static MonoClass *baseClass = mono_class_from_name(image, "Z", "EntityCore");
			auto test = ScriptClass(nameSpace, name);
			if (mono_class_is_subclass_of(monoClass, baseClass, false)) {
				scriptData->EntityClasses[className] = CreateRef<ScriptClass>(nameSpace, name);
				Z_CORE_INFO("ScripteEngine:PrintAssemblyTypes Subclass: Type: {0}", className);
			} else
				Z_CORE_INFO("ScripteEngine:GiveFileMessage: Type: {0}", className);
		}
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
		mono_domain_unload(scriptData->appDomain);
		scriptData->scene = nullptr;
	}

	void ScriptEngine::CreateInstance(Entity entity) {
		auto &script = entity.GetComponent<ScriptComponent>();
		if (ClassExists(script.scriptName)) {
			auto klass = scriptData->EntityClasses[script.scriptName];
			auto instance = CreateRef<EntityInstance>(klass, entity);
			instance->OnCreate();
			scriptData->EntityInstances[entity.GetUID()] = instance;
		} else {
			Z_CORE_ERROR("ScriptEngine:CreateInstance: Class {0} does not exist", script.scriptName);
			return;
		}
	}

	ScriptClass::ScriptClass(const std::string &nameSpace, const std::string &name)
			: NameSpace(nameSpace), ClassName(name) {
		Class = mono_class_from_name(scriptData->image, NameSpace.c_str(), ClassName.c_str());
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

	EntityInstance::EntityInstance(Ref<ScriptClass> klass,Entity entity) : Class(std::move(klass)) {
		instance = Class->GetInstance();
		construct =scriptData->Class->GetMethod(".ctor", 1);
		unsigned long long id=entity.GetUID();
		void* uuid= &id;
		Class->InvokeMethod(construct, instance, &uuid, nullptr);
		create = Class->GetMethod("OnCreate");
		update = Class->GetMethod("OnUpdate", 1);
	}

	void EntityInstance::OnCreate() {
		Class->InvokeMethod(create, instance, nullptr, nullptr);
	}

	void EntityInstance::OnUpdate(Entity entity, float deltaTime) {
		void *args = &deltaTime;
		Class->InvokeMethod(update, instance, &args);
	}

}
