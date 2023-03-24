//
// Created by 32725 on 2023/3/14.
//

#include "Input.h"
#include "Z/Core/Application.h"
#include "GLFW/glfw3.h"

namespace Z {
	bool Input::IsKeyPressed(Z::KeyCode keycode) {
		auto *window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window,int( keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(Z::MouseCode button) {
		auto *window=static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state= glfwGetMouseButton(window,int(button));
		return state==GLFW_PRESS||state==GLFW_REPEAT;
	}

	std::pair<float,float> Input::GetMousePosition() {
		auto *window=static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos,yPos;
		glfwGetCursorPos(window,&xPos, &yPos);
		return std::make_pair(xPos,yPos);
	}

}