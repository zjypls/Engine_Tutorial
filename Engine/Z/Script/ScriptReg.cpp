//
// Created by 32725 on 2023/4/7.
//
#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include "Include/glm/glm/glm.hpp"
#include "Include/mono/include/mono/jit/jit.h"
#include "Include/mono/include/mono/metadata/reflection.h"
#include "Include/box2d/include/box2d/b2_body.h"

#define Z_INTERNAL_FUNC(Name) mono_add_internal_call("Z.Internal::"#Name,(void*)Name)
#include "Z/Script/ScriptReg.h"
#include "Z/Script/ScriptEngine.h"
#include "Z/Scene/Entity.hpp"
#include "Z/Core/Input.h"
#include "Z/Scene/Components.h"

namespace Z {
	void InternalCallInfo(MonoString *str) {
		Z_CORE_INFO(" Log from C# script : {0}", mono_string_to_utf8(str));
	}

	void InternalCallWarn(MonoString* str) {
		Z_CORE_WARN(" Warn from C# script : {0}", mono_string_to_utf8(str));
	}

	void InternalCallError(MonoString* str) {
		Z_CORE_ERROR(" Error from C# script : {0}", mono_string_to_utf8(str));
	}

	void InternalCallDot(glm::vec3 *v, glm::vec3 *u, glm::vec3 *o) {
		*o = glm::normalize(glm::cross(*v, *u));
	}

	void GetTranslation(zGUID id, glm::vec3 *o) {
		*o = ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<TransformComponent>().translation;
	}

	void SetTranslation(zGUID id, glm::vec3 *o) {
		ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<TransformComponent>().translation = *o;
	}

	bool IsKeyPressed(uint16_t keycode) {
		return Input::IsKeyPressed((KeyCode) keycode);
	}

	static std::unordered_map<MonoType* , bool (*)(Entity)> ReflectionMap;
	static std::unordered_map<MonoType *,void(*)(Entity)> AddComponentMap;

	bool Entity_HasComponent(zGUID id, MonoReflectionType *type) {
		Entity entity = ScriptEngine::GetContext()->GetEntityWithGUID(id);
		Z_CORE_ASSERT(entity, "No such entity");
		MonoType *monoType = mono_reflection_type_get_type(type);
		Z_CORE_ASSERT(ReflectionMap.find(monoType) != ReflectionMap.end(), "No such type");
		return ReflectionMap.at(monoType)(entity);
	}

	void Entity_AddComponent(zGUID id, MonoReflectionType*type){
		if(Entity_HasComponent(id,type)){
			Z_CORE_WARN("Entity already has component:"+std::string(mono_type_get_name(mono_reflection_type_get_type(type))));
			return;
		}
		Entity entity = ScriptEngine::GetContext()->GetEntityWithGUID(id);
		Z_CORE_ASSERT(entity, "No such entity");
		MonoType *monoType = mono_reflection_type_get_type(type);
		Z_CORE_ASSERT(AddComponentMap.find(monoType) != AddComponentMap.end(), "No such type");
		AddComponentMap.at(monoType)(entity);
	}

	void Entity_GetVelocity(zGUID id, glm::vec2 *o) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		*o = glm::vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
	}
	void Entity_SetVelocity(zGUID id, glm::vec2 *o) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		auto vel = body->GetLinearVelocity();
		body->SetLinearVelocity(vel+b2Vec2(o->x, o->y));
	}
	void Entity_ApplyForce(zGUID id, glm::vec2 *o, glm::vec2* c, bool wake) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->ApplyForce(b2Vec2(o->x,o->y),b2Vec2(c->x,c->y),wake);
	}
	void Entity_ApplyForceCenter(zGUID id, glm::vec2 *o, bool wake) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->ApplyForceToCenter(b2Vec2(o->x,o->y),wake);
	}
	MonoArray* Entity_GetByName(MonoString* name){
		std::vector<zGUID> ids;
		ScriptEngine::GetContext()->GetEntitiesByName(mono_string_to_utf8(name),ids);
		auto array=mono_array_new(ScriptEngine::GetDomain(),mono_get_uint64_class(),ids.size());
		for(int i=0;i<ids.size();i++){
			mono_array_set(array, uint64_t, i,ids[i]);
		}
		return array;
	}
	MonoString* Entity_GetTag(zGUID id) {
		auto name= ScriptEngine::GetContext()->GetEntityWithGUID(id).GetName();
		return mono_string_new(ScriptEngine::GetDomain(),name.c_str());
	}

	void Entity_SetTag(zGUID id,MonoString* newTag) {
		auto entity=ScriptEngine::GetContext()->GetEntityWithGUID(id);
		entity.GetComponent<TagComponent>().tag=mono_string_to_utf8(newTag);
	}

	glm::vec3 Entity_GetRigidBody2DPosition(zGUID id){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		auto pos=body->GetPosition();
		return {pos.x,pos.y,0};
	}
	void Entity_SetRigidBody2DPosition(zGUID id, glm::vec3* pos){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->SetTransform(b2Vec2(pos->x,pos->y),body->GetAngle());
	}

	MonoArray * Entity_GetScripts(zGUID id){
		auto& scripts=ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<ScriptComponent>().scriptName;
		auto array=mono_array_new(ScriptEngine::GetDomain(),mono_get_string_class(),1);
		mono_array_set(array,MonoString*,0,mono_string_new(ScriptEngine::GetDomain(),scripts.c_str()));
		return array;
	}
	float Entity_GetMass(zGUID id){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		return body->GetMass();
	}
	void Entity_SetMass(zGUID id, float mass){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		b2MassData data{mass,body->GetLocalCenter(),body->GetInertia()};
		body->SetMassData(&data);
	}

	void Entity_SetRigidBody2DType(zGUID id, int type){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->SetType((b2BodyType)type);
	}

	int Entity_GetRigidBody2DType(zGUID id){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		return body->GetType();
	}

	uint64_t Entity_SingleClone(zGUID originID) {
		// Fixme:value different with C# script give on Windows but work well on Linux ???? //solved (2024/01/12 13:50)
		// a log output on windows(2024/01/12 3:20):
		/*
		 [03:09:28] Z: Log from C# script : 13666765057558020482
		 [03:09:28] Z: Log from C# script : example
		 [03:09:28] Z: Log from C# script : Test Log : Call EntityCore::Instantiate for go named example,id :13666765057558020482

		 [03:09:28] Z:SingleClone recive id : 140695316822416
		 [03:09:28] Z:Found GO named example total Size (C++ execute): 1
		 [03:09:28] Z:go[0] 13666765057558020482
		 */
		// noticed by pls
        // solved (2024/01/12 13:50)
        // noticed that the return value type of mono internal function should be base type or MonoType !
        // speculate that it works well on Linux because gcc optimize zGUID as uint64_t ?
		const auto& context=ScriptEngine::GetContext();
		const auto OriginEntity=context->GetEntityWithGUID(originID);
		auto entity=context->InstantiateEntity(OriginEntity);
		return entity.GetUID();
	}


	template<class... T>
	void RegComponent(Type<T...>) {
		(
				[] {
					std::string_view OriginName;
#ifdef __GNUC__
                    OriginName = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
#else
                    OriginName = typeid(T).name();
#endif
                    Z_CORE_WARN("Register Type : {0} !",OriginName);
					auto space = OriginName.rfind(':');
					auto name = fmt::format("Z.{}", OriginName.substr(space + 1));
					MonoType *type = mono_reflection_type_from_name(name.data(), ScriptEngine::GetCoreImage());
					if(!type) {
						Z_CORE_ERROR("No such type: {0}", name);
						return;
					}
					ReflectionMap[type] = [](Entity entity) -> bool { return entity.HasComponent<T>(); };
					AddComponentMap[type] = [](Entity entity) -> void { entity.AddComponent<T>(); };
				}(), ...);
	}




	void ScriptReg::Reg() {
		Z_INTERNAL_FUNC(InternalCallInfo);
		Z_INTERNAL_FUNC(InternalCallWarn);
		Z_INTERNAL_FUNC(InternalCallError);
		Z_INTERNAL_FUNC(InternalCallDot);
		Z_INTERNAL_FUNC(GetTranslation);
		Z_INTERNAL_FUNC(SetTranslation);
		Z_INTERNAL_FUNC(IsKeyPressed);
		Z_INTERNAL_FUNC(Entity_HasComponent);
		Z_INTERNAL_FUNC(Entity_AddComponent);
		Z_INTERNAL_FUNC(Entity_GetVelocity);
		Z_INTERNAL_FUNC(Entity_SetVelocity);
		Z_INTERNAL_FUNC(Entity_ApplyForce);
		Z_INTERNAL_FUNC(Entity_ApplyForceCenter);
		Z_INTERNAL_FUNC(Entity_GetTag);
		Z_INTERNAL_FUNC(Entity_SetTag);
		Z_INTERNAL_FUNC(Entity_GetByName);
		Z_INTERNAL_FUNC(Entity_GetRigidBody2DPosition);
		Z_INTERNAL_FUNC(Entity_SetRigidBody2DPosition);
		Z_INTERNAL_FUNC(Entity_GetScripts);
		Z_INTERNAL_FUNC(Entity_GetMass);
		Z_INTERNAL_FUNC(Entity_GetRigidBody2DType);
		Z_INTERNAL_FUNC(Entity_SetRigidBody2DType);
		Z_INTERNAL_FUNC(Entity_SingleClone);
	}

	void ScriptReg::RegComponents() {
		RegComponent(AllTypes{});
	}
};