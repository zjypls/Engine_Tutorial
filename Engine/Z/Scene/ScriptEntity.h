//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCRIPTENTITY_H
#define ENGINE_TUTORIAL_SCRIPTENTITY_H
#include "Z/Scene/Entity.hpp"

namespace Z {
	class ScriptEntity {
		friend class Scene;

	protected:
		Entity entity;
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(float) {}
	public:
		ScriptEntity() = default;
		virtual ~ScriptEntity() = default;
		template<class _Ty>
		_Ty& GetComponent() {
			return entity.GetComponent<_Ty>();
		}
	};

}
#endif //ENGINE_TUTORIAL_SCRIPTENTITY_H
