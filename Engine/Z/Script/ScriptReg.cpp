//
// Created by 32725 on 2023/4/7.
//

#include "ScriptReg.h"
#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "glm/glm.hpp"
#include "mono/jit/jit.h"
namespace Z {
	void InternalCallPRT(MonoString *str) {
		Z_CORE_WARN("CXX WARN:PRINT: {0}", mono_string_to_utf8(str));
	}
	void InternalCallWARN(glm::vec3 *v) {
		Z_CORE_WARN("CXX WARN:PRINT: {0}", *v);
	}
	void InternalCallDot(glm::vec3 *v,glm::vec3*u,glm::vec3*o) {
		*o=glm::normalize(glm::cross(*v,*u));
	}


	void ScriptReg::Reg() {
		Z_INTERNAL_FUNC(InternalCallPRT);
		Z_INTERNAL_FUNC(InternalCallWARN);
		Z_INTERNAL_FUNC(InternalCallDot);
	}

}