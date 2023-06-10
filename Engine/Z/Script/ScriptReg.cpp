//
// Created by 32725 on 2023/4/7.
//

#include "ScriptReg.h"
//#include "Z/Core/Core.h"
//#include "Z/Core/Log.h"
//#include "Z/Core/GUID.h"
#include "ScriptEngine.h"
#include "Z/Scene/Entity.hpp"
#include "Z/Core/Input.h"
#include "Z/Scene/Components.h"
#include "glm/glm.hpp"
#include "mono/jit/jit.h"
#include "mono/metadata/reflection.h"
#include "box2d/b2_body.h"

namespace Z {
	const void InternalCallPRT(MonoString *str) {
		Z_CORE_WARN("C#: {0}", mono_string_to_utf8(str));
	}

	void InternalCallWARN(glm::vec3 *v) {
		//Z_CORE_WARN("C#: {0}", *v);
	}

	void InternalCallDot(glm::vec3 *v, glm::vec3 *u, glm::vec3 *o) {
		*o = glm::normalize(glm::cross(*v, *u));
	}

	void GetTranslation(GUID id, glm::vec3 *o) {
		*o = ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<TransformComponent>().translation;
	}

	void SetTranslation(GUID id, glm::vec3 *o) {
		ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<TransformComponent>().translation = *o;
	}

	bool IsKeyPressed(uint16_t keycode) {
		return Input::IsKeyPressed((KeyCode) keycode);
	}

	static std::unordered_map<MonoType* , bool (*)(Entity)> ReflectionMap;
	static std::unordered_map<MonoType *,void(*)(Entity)> AddComponentMap;

	bool Entity_HasComponent(GUID id, MonoReflectionType *type) {
		Entity entity = ScriptEngine::GetContext()->GetEntityWithGUID(id);
		Z_CORE_ASSERT(entity, "No such entity");
		MonoType *monoType = mono_reflection_type_get_type(type);
		Z_CORE_ASSERT(ReflectionMap.find(monoType) != ReflectionMap.end(), "No such type");
		return ReflectionMap.at(monoType)(entity);
	}

	void Entity_AddComponent(GUID id,MonoReflectionType*type){
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

	void Entity_GetVelocity(GUID id, glm::vec2 *o) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		*o = glm::vec2(body->GetLinearVelocity().x, body->GetLinearVelocity().y);
	}
	void Entity_SetVelocity(GUID id, glm::vec2 *o) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		auto vel = body->GetLinearVelocity();
		body->SetLinearVelocity(vel+b2Vec2(o->x, o->y));
	}
	void Entity_ApplyForce(GUID id, glm::vec2 *o,glm::vec2* c,bool wake) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->ApplyForce(b2Vec2(o->x,o->y),b2Vec2(c->x,c->y),wake);
	}
	void Entity_ApplyForceCenter(GUID id, glm::vec2 *o,bool wake) {
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->ApplyForceToCenter(b2Vec2(o->x,o->y),wake);
	}
	MonoArray* Entity_GetByName(MonoString* name){
		std::vector<GUID> ids;
		ScriptEngine::GetContext()->GetEntitiesByName(mono_string_to_utf8(name),ids);
		auto array=mono_array_new(ScriptEngine::GetDomain(),mono_get_uint64_class(),ids.size());
		for(int i=0;i<ids.size();i++){
			mono_array_set(array, uint64_t, i,ids[i]);
		}
		return array;
	}
	glm::vec3 Entity_GetRigidBody2DPosition(GUID id){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		auto pos=body->GetPosition();
		return {pos.x,pos.y,0};
	}
	void Entity_SetRigidBody2DPosition(GUID id,glm::vec3* pos){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->SetTransform(b2Vec2(pos->x,pos->y),body->GetAngle());
	}

	MonoArray * Entity_GetScripts(GUID id){
		auto& scripts=ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<ScriptComponent>().scriptName;
		auto array=mono_array_new(ScriptEngine::GetDomain(),mono_get_string_class(),1);
		mono_array_set(array,MonoString*,0,mono_string_new(ScriptEngine::GetDomain(),scripts.c_str()));
		return array;
	}
	float Entity_GetMass(GUID id){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		return body->GetMass();
	}
	void Entity_SetMass(GUID id,float mass){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		b2MassData data{mass,body->GetLocalCenter(),body->GetInertia()};
		body->SetMassData(&data);
	}

	void Entity_SetRigidBody2DType(GUID id,int type){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		body->SetType((b2BodyType)type);
	}

	int Entity_GetRigidBody2DType(GUID id){
		b2Body* body = (b2Body*)ScriptEngine::GetContext()->GetEntityWithGUID(id).GetComponent<RigidBody2DComponent>().ptr;
		return body->GetType();
	}


	template<class... T>
	void RegComponent(Type<T...>) {
		(
				[]() {
					std::string_view OriginName = typeid(T).name();
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
		Z_INTERNAL_FUNC(InternalCallPRT);
		Z_INTERNAL_FUNC(InternalCallWARN);
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
		Z_INTERNAL_FUNC(Entity_GetByName);
		Z_INTERNAL_FUNC(Entity_GetRigidBody2DPosition);
		Z_INTERNAL_FUNC(Entity_SetRigidBody2DPosition);
		Z_INTERNAL_FUNC(Entity_GetScripts);
		Z_INTERNAL_FUNC(Entity_GetMass);
		Z_INTERNAL_FUNC(Entity_GetRigidBody2DType);
		Z_INTERNAL_FUNC(Entity_SetRigidBody2DType);
	}

	void ScriptReg::RegComponents() {
		RegComponent(AllTypes{});
	}
};