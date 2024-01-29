//
// Created by 32725 on 2023/3/19.
//

#ifndef ENGINE_TUTORIAL_CAMERACONTROLLER_H
#define ENGINE_TUTORIAL_CAMERACONTROLLER_H
#include "Z/Events/ApplicationEvent.h"
#include "Z/Events/MouseEvent.h"
#include "Z/Renderer/OrithGraphicCamera.h"

namespace Z {
	//Maybe delete
	class CameraController {
		float aspectRatio;
		float zoomLevel = 1.0f;
		bool enableRotation,MouseCtrl=false;
		Ref<OrithGraphicCamera> camera;
		glm::vec3 cameraPosition = glm::vec3{0.0f, 0.0f, 0.0f};
		float cameraRotation = 0.0f;
		float MoveSpeed=1.f, RotateSpeed=1.f;
	protected:
		virtual bool OnWindowResize(WindowResizeEvent& event) ;
		virtual bool OnMouseScroll(MouseScrollEvent& event) ;
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& event) ;
		virtual bool OnMouseMoved(MouseMovedEvent& event) ;
		virtual bool OnMouseReleased(MouseButtonReleasedEvent& event) ;
	public:
		CameraController(float AspectRatio,bool EnableRotation=false);
		inline virtual void OnResize(float width, float height){
			aspectRatio = width / height;
			camera->ResetProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel);}
		virtual ~CameraController() = default;
		virtual void OnUpdate(float deltaTime);
		virtual void OnEvent(Event& event);
		virtual glm::vec2 GetSize() const;
		virtual void OnImGuiRender() ;
		virtual void SetCamera(const Ref<OrithGraphicCamera>& camera) ;
		inline virtual Ref<OrithGraphicCamera> GetCamera(){return camera;};
	};

}

#endif //ENGINE_TUTORIAL_CAMERACONTROLLER_H
