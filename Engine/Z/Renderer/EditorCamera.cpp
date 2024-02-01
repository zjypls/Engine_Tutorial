//
// Created by 32725 on 2023/3/31.
//
#include "Include/glm/glm/gtx/quaternion.hpp"

#include "Z/Renderer/EditorCamera.h"
#include "Z/Core/Input.h"
#include "Z/Core/Time.h"


namespace Z {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
			: Fov(glm::radians(fov)), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip) {
		projection = glm::perspective(Fov, aspectRatio, nearClip, farClip);
		viewMatrix = glm::lookAt(position, focus, up);
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

	void EditorCamera::ViewRotate(const glm::vec2 offset) {
		auto [x, y] = Input::GetMousePosition();
		auto toFocus = glm::normalize(position - focus);
		//TODO:multiply with deltaTime
		pitch-=offset.y * 1E-2f;
		yaw -= offset.x * 1E-2f;
		right = glm::normalize(glm::cross(toFocus, up));//glm::normalize(glm::rotate(glm::quat(-offset.x * up*1E-2f), right));
		// Todo:optimize
		if(pitch<3.1f&&pitch>0.1f) {
			toFocus = glm::rotate(glm::quat(-offset.x * up * 1E-2f), toFocus);
			toFocus = glm::rotate(glm::quat(offset.y * right * 1E-2f), toFocus);
			position = focus + distance * glm::normalize(toFocus);
		}
		else
			pitch += offset.y * 1E-2f;
	}

	void EditorCamera::UpdateCursorPos() {
		auto [x, y] = Input::GetMousePosition();
		lastMousePosition = glm::vec2(x, y);
	}

	void EditorCamera::MoveFocus(const glm::vec2 offset) {
		auto toFocus = glm::normalize(focus - position);
		auto LocalRight = glm::normalize(glm::cross(toFocus, up));
		auto LocalUp = glm::normalize(glm::cross(LocalRight, toFocus));
		focus += (-offset.x * LocalRight + offset.y * LocalUp) * 1E-2f;
		position += (-offset.x * LocalRight + offset.y * LocalUp) * 1E-2f;
	}

	bool EditorCamera::OnKeyPressed(KeyPressEvent &e) {
		return false;
	}

	void EditorCamera::Walk() {
		glm::vec3 befPos = this->position;
		float deltaTimeScale = Time::DeltaTime() * 10;
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
//		else
//			return;
		this->focus += this->position - befPos;
	}
}