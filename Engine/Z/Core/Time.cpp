//
// Created by 32725 on 2023/3/17.
//

#include "Z/Core/Time.h"
#include "Include/glfw/include/GLFW/glfw3.h"

namespace Z {
	float Time::lastTime = GetTime();
	float Time::deltaTime;

	float Time::lastFlushTime;

	float Time::GetTime() {
		return glfwGetTime();
	}

}
