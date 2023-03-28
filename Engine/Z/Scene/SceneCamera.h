//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCENECAMERA_H
#define ENGINE_TUTORIAL_SCENECAMERA_H
#include "Z/Renderer/Camera.h"

namespace Z {
	class SceneCamera :public Camera {
	public:
		enum class ProjectionType {Perspective = 0, Orthographic = 1} ;
		SceneCamera()=default;
		SceneCamera(const glm::mat4& projection):Camera(projection){}
		SceneCamera(const Camera& camera):Camera(camera){}
		void OnViewportResize(unsigned int width, unsigned int height);
		void SetGraphicSize(float size,float nearClip=-1.f,float farClip=1.f);
		[[nodiscard]] inline ProjectionType GetProjectionType() const { return projectionType; }
		void SetProjectionType(ProjectionType type){projectionType=type;UpdateProjection();}
		void SetOrthographicSize(float size){GraphicSize=size;UpdateProjection();}
		void SetOrthographicNearClip(float nearClip){this->nearClip=nearClip;UpdateProjection();}
		void SetOrthographicFarClip(float farClip){this->farClip=farClip;UpdateProjection();}
		void SetPerspectiveFOV(float fov){this->fov=glm::radians(fov);UpdateProjection();}
		void SetPerspectiveNearClip(float nearClip){PNearClip=nearClip;UpdateProjection();}
		void SetPerspectiveFarClip(float farClip){PFarClip=farClip;UpdateProjection();}
		[[nodiscard]] inline float GetOrthographicSize() const { return GraphicSize; }
		[[nodiscard]] inline float GetOrthographicNearClip() const { return nearClip; }
		[[nodiscard]] inline float GetOrthographicFarClip() const { return farClip; }
		[[nodiscard]] inline float GetPerspectiveFOV() const { return fov; }
		[[nodiscard]] inline float GetPerspectiveNearClip() const { return PNearClip; }
		[[nodiscard]] inline float GetPerspectiveFarClip() const { return PFarClip; }
	private:
		float aspectRatio=1.f;
		float GraphicSize=10.f;
		float nearClip=-1.f, farClip=1.f;
		float fov=glm::radians(45.f);
		float PNearClip=.01f, PFarClip=1000.f;
		void UpdateProjection();
		ProjectionType projectionType=ProjectionType::Orthographic;
	};

}


#endif //ENGINE_TUTORIAL_SCENECAMERA_H
