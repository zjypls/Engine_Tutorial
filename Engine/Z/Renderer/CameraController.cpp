//
// Created by 32725 on 2023/3/19.
//

#include "CameraController.h"
#include "Z/Core/Input.h"
#include "Z/Core/KeyCodes.h"
#include "Z/Core/MouseButtonCodes.h"

namespace Z {

	bool CameraController::OnWindowResize(WindowResizeEvent &event) {
		OnResize((float) event.GetWidth(), (float) event.GetHeight());
		return false;
	}

	bool CameraController::OnMouseScroll(MouseScrollEvent &event) {
		zoomLevel -= event.GetYOffset() * 0.25f;
		zoomLevel = std::max(zoomLevel, 0.7f);
		MoveSpeed = zoomLevel;
		camera->ResetProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);
		return false;
	}

	void CameraController::OnUpdate(float deltaTime) {
		glm::vec3 offSet{0, 0, 0};
		if(!MouseCtrl){
			if (Z::Input::IsKeyPressed(Z_KEY_W)) {
				offSet.y += MoveSpeed;
			} else if (Z::Input::IsKeyPressed(Z_KEY_S)) {
				offSet.y -= MoveSpeed;
			}
			if (Z::Input::IsKeyPressed(Z_KEY_A)) {
				offSet.x -= MoveSpeed;
			} else if (Z::Input::IsKeyPressed(Z_KEY_D)) {
				offSet.x += MoveSpeed;
			}
		}
		if (enableRotation) {
			if (Z::Input::IsKeyPressed(Z_KEY_Q)) {
				cameraRotation += RotateSpeed * deltaTime;
			} else if (Z::Input::IsKeyPressed(Z_KEY_E)) {
				cameraRotation -= RotateSpeed * deltaTime;
			}
		}

		cameraPosition += offSet * deltaTime;
		camera->SetPosition(cameraPosition);
		camera->SetRotation(cameraRotation);
	}

	void CameraController::OnEvent(Event &event) {
		EventDispatcher dispatcher(event);
		dispatcher.Handle<WindowResizeEvent>(Z_BIND_EVENT_FUNC(CameraController::OnWindowResize));
		dispatcher.Handle<MouseScrollEvent>(Z_BIND_EVENT_FUNC(CameraController::OnMouseScroll));
		dispatcher.Handle<MouseButtonPressedEvent>(Z_BIND_EVENT_FUNC(CameraController::OnMouseButtonPressed));
		dispatcher.Handle<MouseButtonReleasedEvent>(Z_BIND_EVENT_FUNC(CameraController::OnMouseReleased));
		dispatcher.Handle<MouseMovedEvent>(Z_BIND_EVENT_FUNC(CameraController::OnMouseMoved));
	}

	void CameraController::OnImGuiRender() {
	}

	void CameraController::SetCamera(const Ref<OrithGraphicCamera> &camera) {
		this->camera = camera;
	}

	CameraController::CameraController(float AspectRatio, bool EnableRotation)
			: aspectRatio(AspectRatio), enableRotation(EnableRotation),
			  camera(CreateRef<OrithGraphicCamera>(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel,
			                                       zoomLevel)) {}

	glm::vec2 CameraController::GetSize() const {
		return glm::vec2{aspectRatio * zoomLevel, zoomLevel};
	}

	bool CameraController::OnMouseButtonPressed(MouseButtonPressedEvent &event) {
		if((MouseCode)event.GetButton()==Z_MOUSE_BUTTON_MIDDLE){
			MouseCtrl=true;
		}
		return false;
	}

	bool CameraController::OnMouseMoved(MouseMovedEvent &event) {
		static glm::vec2 lastPos=glm::vec2(event.GetX(),event.GetY());
		if(MouseCtrl){
			cameraPosition.x+=(lastPos.x-event.GetX())*0.005f;
			cameraPosition.y-=(lastPos.y- event.GetY())*0.005f;
		}
		lastPos=glm::vec2(event.GetX(),event.GetY());
		return false;
	}

	bool CameraController::OnMouseReleased(MouseButtonReleasedEvent &event) {
		if((MouseCode)event.GetButton()==Z_MOUSE_BUTTON_MIDDLE){
			MouseCtrl=false;
		}
		return false;
	}
}
