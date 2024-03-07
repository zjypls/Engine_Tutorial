//
// Created by 32725 on 2023/3/31.
//

#ifndef ENGINE_TUTORIAL_EDITORCAMERA_H
#define ENGINE_TUTORIAL_EDITORCAMERA_H

#include "Include/glm/glm/glm.hpp"
#include "Include/glm/glm/gtc/matrix_transform.hpp"
#include "Include/glm/glm/gtx/transform.hpp"

#include "Z/Renderer/Camera.h"
#include "Z/Core/Core.h"
#include "Z/Events/Event.h"
#include "Z/Events/MouseEvent.h"
#include "Z/Events/KeyEvent.h"
#include "Z/Events/ApplicationEvent.h"

namespace Z {



	class EditorCamera: public Camera {
		float Fov=60.f,aspectRatio=1.f,nearClip=0.1f,farClip=1000.f;
		glm::vec3 position=glm::vec3{1.f};
		glm::vec3 focus=glm::vec3{0.f};
		glm::vec3 up=glm::vec3{0.f,1.f,0.f},right=glm::vec3{1.f,0.f,0.f};
		float distance=glm::sqrt(3.f);
		glm::vec2 viewportSize=glm::vec2{800,800},lastMousePosition=glm::vec2{0.f};
		glm::mat4 viewMatrix=glm::mat4(1.f);


		bool OnMouseScrolled(MouseScrollEvent &e);

		bool OnWindowResized(WindowResizeEvent &e);
		bool OnKeyPressed(KeyPressEvent&e);

		void ViewRotate(const glm::vec2 offset);

		void MoveFocus(const glm::vec2 offset);

		void UpdateCursorPos();
		void Walk();


	public:
		EditorCamera() = default;

		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		inline const glm::mat4 &GetViewMatrix() const { return viewMatrix; }

		inline const glm::mat4 &GetProjectionMatrix() const { return projection; }

		inline const glm::mat4 &GetViewProjectionMatrix() const { return projection * viewMatrix; }

		inline const glm::vec3 &GetPosition() const { return position; }

		inline const glm::vec3 &GetFocus() const { return focus; }

		inline const glm::vec3 &GetUp() const { return up; }

		inline float GetDistance() const { return distance; }

		inline glm::vec3 GetForward() const { return glm::normalize(focus - position); }

		inline void SetFov(float fov) { Fov = fov; }

		inline void SetAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }

		inline void SetNearClip(float nearClip) { this->nearClip = nearClip; }

		inline void SetFarClip(float farClip) { this->farClip = farClip; }


		inline void SetViewportSize(float width, float height) {
			aspectRatio = width / height;
			viewportSize = glm::vec2(width, height);
			projection= glm::perspective(Fov, aspectRatio, nearClip, farClip);
		}

		void OnUpdate();

		void OnEvent(Event &e);

	};

}


#endif //ENGINE_TUTORIAL_EDITORCAMERA_H
