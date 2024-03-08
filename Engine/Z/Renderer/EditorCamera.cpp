//
// Created by 32725 on 2023/3/31.
//
#include "Include/glm/gtx/quaternion.hpp"

#include "Z/Core/Input.h"
#include "Z/Core/Time.h"
#include "Z/Core/Log.h"
#include "Z/Renderer/EditorCamera.h"


namespace Z {
	constexpr float walkSpeedScale=1.f,rotateSpeedScale=1.f,moveSpeedScale=1.f;
	constexpr glm::vec3 g_UP = glm::vec3(0.0f, 1.0f, 0.0f);
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
			: Fov(glm::radians(fov)), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip) {
		OnUpdate();
	}

	void EditorCamera::OnEvent(Event &e) {
		EventDispatcher dispatcher(e);
		dispatcher.Handle<MouseScrollEvent>(Z_BIND_EVENT_FUNC(EditorCamera::OnMouseScrolled));
		dispatcher.Handle<WindowResizeEvent>(Z_BIND_EVENT_FUNC(EditorCamera::OnWindowResized));
		dispatcher.Handle<KeyPressEvent>(Z_BIND_EVENT_FUNC(EditorCamera::OnKeyPressed));
	}

	void EditorCamera::OnUpdate() {
		auto [x, y] = Input::GetMousePosition();
		auto offset = glm::vec2(x, y) - lastMousePosition;
		//ToDO:Optimize
		if (Input::IsMouseButtonPressed(MouseCode::ButtonRight)) {
			ViewRotate(offset);
			Walk();
		} else if (Input::IsMouseButtonPressed(MouseCode::ButtonMiddle)) {
			MoveFocus(offset);
		}
		lastMousePosition = glm::vec2(x, y);
		projection = glm::perspective(Fov, aspectRatio, nearClip, farClip);
		projection[1][1]*=-1;
		viewMatrix = glm::lookAt(position, focus, up);
	}

	bool EditorCamera::OnMouseScrolled(MouseScrollEvent &e) {
		distance -= e.GetYOffset() * 0.1f * distance;
		distance = std::max(distance, 0.1f);
		position = focus - distance * glm::normalize(focus - position);
		return false;
	}

	bool EditorCamera::OnWindowResized(WindowResizeEvent &e) {
		viewportSize = glm::vec2(e.GetWidth(), e.GetHeight());
		aspectRatio = viewportSize.x / viewportSize.y;
		return false;
	}

	void EditorCamera::ViewRotate(glm::vec2 offset) {
		auto deltaTime=Time::DeltaTime()*rotateSpeedScale;
		glm::vec3 toFocus = position - focus;
    	glm::quat rotation = glm::angleAxis(-offset.x* deltaTime, g_UP) * glm::angleAxis(offset.y * deltaTime, right);
		
    	toFocus = rotation * toFocus;
		position = focus + distance * glm::normalize(toFocus);
    	up = rotation * up;
		right = glm::cross(toFocus, up);
	}

	void EditorCamera::UpdateCursorPos() {
		auto [x, y] = Input::GetMousePosition();
		lastMousePosition = glm::vec2(x, y);
	}

	void EditorCamera::MoveFocus(glm::vec2 offset) {
		auto deltaTime=Time::DeltaTime()*moveSpeedScale;
		auto toFocus = glm::normalize(focus - position);
		focus += (offset.x * right + offset.y * up) * deltaTime;
		position += (offset.x * right + offset.y * up) * deltaTime;
	}

	bool EditorCamera::OnKeyPressed(KeyPressEvent &e) {
		return false;
	}

	void EditorCamera::Walk() {
		glm::vec3 befPos = this->position;
		float deltaTimeScale = Time::DeltaTime() * walkSpeedScale;
		if (Input::IsKeyPressed(KeyCode::W))
			this->position += glm::normalize(this->focus - this->position) * deltaTimeScale;
		else if (Input::IsKeyPressed(KeyCode::S))
			this->position -= glm::normalize(this->focus - this->position) * deltaTimeScale;
		if (Input::IsKeyPressed(KeyCode::A))
			this->position += right * deltaTimeScale;
		else if (Input::IsKeyPressed(KeyCode::D))
			this->position -= right * deltaTimeScale;
		if (Input::IsKeyPressed(KeyCode::E))
			this->position += this->up * deltaTimeScale;
		else if (Input::IsKeyPressed(KeyCode::Q))
			this->position -= this->up * deltaTimeScale;
		this->focus += this->position - befPos;
	}
}