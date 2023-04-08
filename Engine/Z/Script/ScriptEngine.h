//
// Created by 32725 on 2023/4/6.
//

#ifndef ENGINE_TUTORIAL_SCRIPTENGINE_H
#define ENGINE_TUTORIAL_SCRIPTENGINE_H

#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Z/Core/GUID.h"
#include "Z/Scene/Scene.h"

extern "C" {
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoAssembly MonoAssembly;
};

namespace Z {


	class ScriptClass {
	public:
		ScriptClass(const std::string&  nameSpace,const std::string&  name);

		MonoObject *GetInstance();

		MonoMethod *GetMethod(const std::string &MethodName, int paramCount=0);

		void InvokeMethod(MonoMethod *method, MonoObject *object, void **params= nullptr, MonoObject **exc= nullptr);

	private:
		MonoClass *Class;
		std::string NameSpace, ClassName;
	};

	class EntityInstance{
	public:
		EntityInstance(Ref<ScriptClass>klass,Entity entity);
		void OnCreate();
		void OnUpdate(Entity entity,float deltaTime);
	private:
		Ref<ScriptClass>Class;
		MonoObject *instance;
		MonoMethod *construct,*create,*update;
	};

	class ScriptEngine {
	public:
		ScriptEngine() = default;

		virtual ~ScriptEngine() = default;

		static void Init();

		static void ShutDown();

		static void LoadAssembly(const std::filesystem::path &path);
		static void OnRuntimeStart(Scene*scene);
		static void OnRuntimeUpdate(Entity entity,float deltaTime);
		static void OnRuntimeStop();
		static void CreateInstance(Entity entity);

		static bool ClassExists(const std::string &name);

	private:
		static void MonoInit();

		static void MonoShutDown();

		static MonoObject *GetInstance(MonoClass *);

		static void LoadAssemblyClasses(MonoAssembly *assembly);

		friend class ScriptClass;
	};

}


#endif //ENGINE_TUTORIAL_SCRIPTENGINE_H
