//
// Created by 32725 on 2023/4/6.
//

#include <fstream>
#include <filesystem>
#include <utility>

#include "Include/mono/include/mono/jit/jit.h"
#include "Include/mono/include/mono/metadata/assembly.h"
#include "Include/mono/include/mono/metadata/attrdefs.h"
#include "Include/mono/include/mono/metadata/debug-helpers.h"
#include "Include/filewatch/filewatch.h"

#include "Z/Script/ScriptEngine.h"
#include "Z/Script/ScriptReg.h"
#include "Z/Scene/Entity.h"
#include "Z/Core/Application.h"
#include "Z/Scene/Components.h"


namespace std {
	template<>
	struct hash<pair<Z::zGUID, Z::ScriptClass>> {
		::size_t operator()(const pair<Z::zGUID, Z::ScriptClass> &p) const {
			return (hash<Z::zGUID>()(p.first) << 10) | (hash<Z::ScriptClass>()(p.second) >> 10);
		}
	};


}
bool operator==(const std::pair<Z::zGUID, Z::ScriptClass> &a, const std::pair<Z::zGUID, Z::ScriptClass> &b) {
	return a.first == b.first && a.second == b.second;
}

std::string Z::FieldTypeToString(ScriptFieldType type) {
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

namespace Z {

	namespace Tools {
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
		Ref<ScriptClass> Class = nullptr;
		MonoImage *image = nullptr, *appImage = nullptr;
		Scene *scene = nullptr;
		std::string domainName="ZAppDomain";
		std::filesystem::path CoreAssemblyPath,AppAssemblyPath;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<zGUID, Ref<ScriptInstance>> EntityInstances;
		using FieldMap = std::unordered_map<std::string, ScriptFieldBuffer>;
		using EntityWithClass = std::pair<zGUID, ScriptClass>;
		std::unordered_map<EntityWithClass, FieldMap> EntityFields;
		Scope<filewatch::FileWatch<std::string>> AppCoreWatch;
		bool HasReloadApp=false;
	};
	static ScriptEngineData *scriptData;

	void ScriptEngine::Init() {
		scriptData = new ScriptEngineData();
		MonoInit();
		LoadCoreAssembly(Tools::LoadMonoAssembly(scriptData->CoreAssemblyPath));
		ScriptReg::Reg();
		ScriptReg::RegComponents();
	}

	void ScriptEngine::RegisterFileWatch() {
		scriptData->AppCoreWatch= CreateScope<filewatch::FileWatch<std::string>>(scriptData->AppAssemblyPath.string(),
				[&](auto&str,auto action){
			//Todo : prevent runtime reload
			static auto lastTime = std::chrono::high_resolution_clock::now()-std::chrono::seconds(10);
			if(auto now = std::chrono::high_resolution_clock::now();
			now-lastTime>std::chrono::milliseconds (500)&&action==filewatch::Event::modified){
				lastTime=now;
				Application::Get().SubmitFunc([]{
					ScriptEngine::ReLoadApp();
				});
			}
		});
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
					klass->Fields[name] = ScriptField{name, FieldTypeMap[typeName], filed};
			}
		}
		for (const auto &[className, klass]: scriptData->EntityClasses) {
			for (const auto &[name, type]: klass->Fields) {
				Z_CORE_INFO("\tScriptEngine:AssemblyTypes Subclass: Field: {0} {1}", name,
				            FieldTypeToString(type.Type));
			}
		}
	}

	void ScriptEngine::MonoInit() {
		scriptData->CoreAssemblyPath="bin/ScriptCore.dll";
		scriptData->AppAssemblyPath="bin/scripts.dll";
		mono_set_assemblies_path("Assets/mono/lib/4.5");
		scriptData->rootDomain = mono_jit_init("ZJIT");
		//scriptData->appDomain = mono_domain_create_appdomain(scriptData->domainName.data(), nullptr);
		//mono_domain_set(scriptData->appDomain, false);
	}

	void ScriptEngine::MonoShutDown() {
		mono_jit_cleanup(scriptData->rootDomain);
	}

	void ScriptEngine::ReCreateDomain() {
		scriptData->appImage= nullptr;
		mono_domain_set(mono_get_root_domain(), false);
		scriptData->appDomain = mono_domain_create_appdomain(scriptData->domainName.data(), nullptr);
		mono_domain_set(scriptData->appDomain, false);
		LoadCoreAssembly(Tools::LoadMonoAssembly(scriptData->CoreAssemblyPath));
		ScriptReg::Reg();
		ScriptReg::RegComponents();
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path &path) {
		scriptData->EntityInstances.clear();
		scriptData->EntityFields.clear();
		scriptData->EntityClasses.clear();
		auto assembly = Tools::LoadMonoAssembly(path);
		scriptData->appImage = mono_assembly_get_image(assembly);
		Z_CORE_ASSERT(scriptData->appImage, "ScriptEngine:LoadAssembly: Failed to load assembly");
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
        scriptData->appDomain = mono_domain_create_appdomain(scriptData->domainName.data(), nullptr);
		mono_domain_set(scriptData->appDomain, false);
		scriptData->scene = scene;
	}

	void ScriptEngine::OnRuntimeUpdate(Entity entity, float deltaTime) {
		auto uuid = entity.GetUID();
		if (scriptData->EntityInstances.find(uuid) != scriptData->EntityInstances.end()) {
			scriptData->EntityInstances[uuid]->OnUpdate(entity, deltaTime);
		}
	}

	void ScriptEngine::OnRuntimeStop() {
        // change domain
        mono_domain_set(scriptData->rootDomain,false);
        // call destructor func defined in C# scripts load by mono
        mono_domain_finalize(scriptData->appDomain,false);
        // unload appdomain
        mono_domain_unload(scriptData->appDomain);
        scriptData->EntityInstances.clear();
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

	Ref<ScriptInstance> ScriptEngine::GetInstance(zGUID id) {
		if (auto instance = scriptData->EntityInstances.find(id);instance != scriptData->EntityInstances.end())
			return instance->second;
		else
			return nullptr;
	}

	MonoDomain *ScriptEngine::GetDomain() {
		return scriptData->appDomain;
	}


	bool ScriptEngine::EntityFieldExists(zGUID id, ScriptClass &klass) {
		return scriptData->EntityFields.find(std::pair{id, klass}) != scriptData->EntityFields.end();
	}

	void ScriptEngine::RegisterEntityClassFields(zGUID id, ScriptClass &klass) {
		std::unordered_map<std::string, ScriptFieldBuffer> values;
		auto key = std::make_pair(id, klass);
		for (const auto &[name, field]: klass.Fields) {
			int align = 0;
			auto size = mono_type_size(mono_field_get_type(field.Field), &align);
			auto fieldBuffer = ScriptFieldBuffer{field, size};
			values[name] = fieldBuffer;
		}
		scriptData->EntityFields[key] = values;
	}

	std::unordered_map<std::string, ScriptFieldBuffer> &ScriptEngine::GetFields(zGUID id, const ScriptClass &klass) {
		return scriptData->EntityFields.at({id, klass});
	}

	void ScriptEngine::ReLoadApp() {
		ReCreateDomain();
		LoadAssembly(scriptData->AppAssemblyPath);
		scriptData->HasReloadApp=true;
	}

	void ScriptEngine::ReCreateFields(Ref<Scene> scene) {
		scene->GetComponentView<ScriptComponent>().each([&](auto id, ScriptComponent &script) {
			Entity entity = {id, scene.get()};
			if (ScriptEngine::ClassExists(script.scriptName)) {
				auto &klass = scriptData->EntityClasses[script.scriptName];
				RegisterEntityClassFields(entity.GetUID(), *klass);
			}
		});
	}

	bool ScriptEngine::HasReLoadApp() {
		return scriptData->HasReloadApp;
	}

	void ScriptEngine::SetReLoadApp(bool state) {
		scriptData->HasReloadApp = state;
	}

	void ScriptEngine::RemoveEntityClassFields(zGUID id, ScriptClass &klass) {
		scriptData->EntityFields.erase({id, klass});
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


	void ScriptClass::SetValue(zGUID id, const std::string &name, void *ptr) {
//		mono_field_static_set_value(mono_class_vtable(scriptData->rootDomain,Class),
//		                            Fields.at(name).Field,ptr);
		auto bufferMap = scriptData->EntityFields.find(std::pair{id, *this});
		auto &buffer = bufferMap->second.at(name);
		buffer.SetValue(ptr);
	}

	void ScriptClass::InnerGetValue(zGUID id, const std::string &name, void *ptr) {
		auto bufferMap = scriptData->EntityFields.find(std::pair{id, *this});
		auto buffer = bufferMap->second.at(name);
		buffer.GetValue(ptr);
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> klass, Entity entity) : Class(std::move(klass)) {
		instance = Class->GetInstance();
		construct = scriptData->Class->GetMethod(".ctor", 1);
		unsigned long long id = entity.GetUID();
		const auto &bufferPair = scriptData->EntityFields.find(std::pair{id, *Class});
		const auto &bufferMap = bufferPair->second;
		unsigned char buffer[8]{0};
		for (auto &[name, field]: bufferMap) {
			field.GetValue(buffer);
			SetValue(name, buffer);
		}
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

	void ScriptInstance::GetValue(const std::string &name, void *ptr) {
		mono_field_get_value(instance, mono_class_get_field_from_name(Class->GetClass(), name.c_str()), ptr);
	}

	unsigned char ScriptInstance::buffer[64]{0};

	void ScriptInstance::SetValue(const std::string &name, void *ptr) {
		mono_field_set_value(instance, mono_class_get_field_from_name(Class->GetClass(), name.c_str()), ptr);
	}

}
