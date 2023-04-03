//
// Created by 32725 on 2023/3/31.
//

#include "EditorCamera.h"
#include "Z/Core/Input.h"
#include "glm/gtx/quaternion.hpp"


namespace Z {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
			: Fov(glm::radians(fov)), aspectRatio(aspectRatio), nearClip(nearClip), farClip(farClip) {
		projection = glm::perspective(Fov, aspectRatio, nearClip, farClip);
		viewMatrix = glm::lookAt(position, focus, up);
		UpdatePos();
	}

	void EditorCamera::OnEvent(Event &e) {
		EventDispatcher dispatcher(e);
		dispatcher.Handle<MouseScrollEvent>(Z_BIND_EVENT_FUNC(EditorCamera::OnMouseScrolled));
	}

	void EditorCamera::OnUpdate() {
		auto [x, y] = Input::GetMousePosition();
		if (Input::IsKeyPressed(KeyCode::LeftAlt)) {
			auto offset = glm::vec2(x, y) - lastMousePosition;
			if (Input::IsMouseButtonPressed(MouseCode::ButtonRight)) {
				ViewRotate(offset);
			}
			if (Input::IsMouseButtonPressed(MouseCode::ButtonMiddle)) {
				MoveFocus(offset);
			}
		}
		lastMousePosition = glm::vec2(x, y);
		projection = glm::perspective(Fov, aspectRatio, nearClip, farClip);
		viewMatrix = glm::lookAt(position, focus, up);
	}

	bool EditorCamera::OnMouseScrolled(MouseScrollEvent &e) {
		distance -= e.GetYOffset() * 3E-2f * distance;
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
		pitch -= offset.y * 1E-2f;
		yaw -= offset.x * 1E-2f;
		right = glm::normalize(
				glm::cross(toFocus, up));//glm::normalize(glm::rotate(glm::quat(-offset.x * up*1E-2f), right));
		toFocus = glm::rotate(glm::quat(-offset.x * up * 1E-2f), toFocus);
		toFocus = glm::rotate(glm::quat(offset.y * right * 1E-2f), toFocus);
		position = focus + distance * glm::normalize(toFocus);
	}

	void EditorCamera::UpdatePos() {
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
}