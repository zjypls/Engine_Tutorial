//
// Created by 32725 on 2023/4/6.
//

#ifndef ENGINE_TUTORIAL_SCRIPTENGINE_H
#define ENGINE_TUTORIAL_SCRIPTENGINE_H
#include <filesystem>
#include "Z/Core/zGUID.h"
#include "Z/Scene/Scene.h"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoDomain MonoDomain;
};

namespace Z {

	enum class ScriptFieldType {
		Char, Byte,
		Int16, UInt16, Int, UInt, Long, ULong,
		Float, Double, Float2, Float3, Float4,
		Bool, String, Entity,
	};


	std::string FieldTypeToString(ScriptFieldType);

	struct ScriptField {
		std::string Name;
		ScriptFieldType Type;
		MonoClassField *Field;
	};
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

	struct ScriptFieldBuffer {
		ScriptField field;
		int TypeSize=0;
		void GetValue(void* ptr) const{
			memcpy(ptr,buffer,TypeSize);
		}

		void SetValue(void*v){
			//std::memcpy(buffer,v,TypeSize);
			memcpy(buffer,v,TypeSize);
		}
		unsigned char buffer[32]{0};
	};

	class ScriptClass {
	public:
		ScriptClass(const std::string &nameSpace, const std::string &name);

		MonoObject *GetInstance();

		MonoMethod *GetMethod(const std::string &MethodName, int paramCount = 0);

		void InvokeMethod(MonoMethod *method, MonoObject *object, void **params = nullptr, MonoObject **exc = nullptr);

		bool IsSubClassOf(const Ref<ScriptClass> &klass);

		uint32 GetMethodCount();

		const auto &GetFields() const { return Fields; }


		void GetValue(zGUID id, const std::string &name, void* ptr) {
			InnerGetValue(id,name, ptr);
		}
		std::string GetTypeName()const{
			return NameSpace+"."+ClassName;
		}

		void SetValue(zGUID id, const std::string &name, void *ptr);

		[[nodiscard]] MonoClass *GetClass() const { return Class; }
		bool operator==(const ScriptClass&a)const{
			return NameSpace==a.NameSpace&&ClassName==a.ClassName;
		}

	private:
		void InnerGetValue(zGUID id, const std::string &name, void *ptr);

		std::unordered_map<std::string, ScriptField> Fields;
		MonoClass *Class;
		std::string NameSpace, ClassName;

		friend class ScriptEngine;
	};

	class ScriptInstance {
	public:
		ScriptInstance(Ref<ScriptClass> klass, Entity entity);

		void OnCreate();

		void OnUpdate(Entity entity, float deltaTime);

		Ref<ScriptClass> GetClass() const { return Class; }


		void SetValue(const std::string &name, void *ptr);

		void GetValue(const std::string &name,void*ptr);

	private:

		Ref<ScriptClass> Class;
		MonoObject *instance;
		MonoMethod *construct, *create, *update;
		static unsigned char buffer[64];
	};

	class ScriptEngine {
	public:
		ScriptEngine() = default;

		virtual ~ScriptEngine() = default;

		static void Init();

		static void RegisterFileWatch();

		static void ShutDown();

		static void LoadAssembly(const std::filesystem::path &path);

		static void OnRuntimeStart(Scene *scene);

		static void OnRuntimeUpdate(Entity entity, float deltaTime);

		static void OnRuntimeStop();

		static void CreateInstance(Entity entity);

		static Scene *GetContext();

		static const std::unordered_map<std::string, Ref<ScriptClass>> &GetScriptList();

		static Ref<ScriptInstance> GetInstance(zGUID id);
		static MonoDomain*GetDomain();

		static void ReCreateFields(Ref<Scene>);

		static bool HasReLoadApp();
		static void SetReLoadApp(bool);
		static void CheckLoad(Ref<Scene>&scene_){
			if(HasReLoadApp()) {
				if(scene_) ReCreateFields(scene_);
				SetReLoadApp(false);
			}
		}

		static bool ClassExists(const std::string &name);
		static bool EntityFieldExists(zGUID id, ScriptClass&klass);
		static void RegisterEntityClassFields(zGUID id, ScriptClass&klass);
		static void RemoveEntityClassFields(zGUID id, ScriptClass&klass);
		static std::unordered_map<std::string,ScriptFieldBuffer>&GetFields(zGUID id, const ScriptClass&klass);

		static MonoImage *GetCoreImage();
		static void ReLoadApp();

	private:
		static void ReCreateDomain();
		static void MonoInit();

		static void MonoShutDown();

		static void GetClasses(MonoAssembly *);

		static MonoObject *GetInstance(MonoClass *);

		static void LoadCoreAssembly(MonoAssembly *assembly);


		friend class ScriptClass;

		friend class ScriptReg;
	};

}

namespace std{
	template<>
	struct hash<Z::ScriptClass>{
		size_t operator()(const Z::ScriptClass&klass)const{
			return std::hash<std::string>()(klass.GetTypeName());
		}
	};
}


#endif //ENGINE_TUTORIAL_SCRIPTENGINE_H
