//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_SCENECAMERA_H
#define ENGINE_TUTORIAL_SCENECAMERA_H
#include "Z/Renderer/Camera.h"

namespace Z {
	class Z_API SceneCamera :public Camera {
	public:
		enum class ProjectionType {Perspective = 0, Orthographic = 1} ;
		SceneCamera()=default;
		SceneCamera(const glm::mat4& projection):Camera(projection){}
		SceneCamera(const Camera& camera):Camera(camera){}
		void OnViewportResize(uint32 width, uint32 height);
        void SetAspectRatio(float asp){aspectRatio=asp;UpdateProjection();}
		void SetGraphicSize(float size,float nearClip=-1.f,float farClip=1.f);
		void SetProjectionType(ProjectionType type){projectionType=type;UpdateProjection();}
		void SetOrthographicSize(float size){GraphicSize=size;UpdateProjection();}
		void SetOrthographicNearClip(float nearClip){this->nearClip=nearClip;UpdateProjection();}
		void SetOrthographicFarClip(float farClip){this->farClip=farClip;UpdateProjection();}
		void SetPerspectiveFOV(float fov){this->fov=fov;UpdateProjection();}
		void SetPerspectiveNearClip(float nearClip){PNearClip=nearClip;UpdateProjection();}
		void SetPerspectiveFarClip(float farClip){PFarClip=farClip;UpdateProjection();}
        [[nodiscard]] ProjectionType GetProjectionType() const { return projectionType; }
		[[nodiscard]] float GetOrthographicSize() const { return GraphicSize; }
		[[nodiscard]] float GetOrthographicNearClip() const { return nearClip; }
		[[nodiscard]] float GetOrthographicFarClip() const { return farClip; }
		[[nodiscard]] float GetPerspectiveFOV() const { return fov; }
		[[nodiscard]] float GetPerspectiveNearClip() const { return PNearClip; }
		[[nodiscard]] float GetPerspectiveFarClip() const { return PFarClip; }
	private:
		float aspectRatio=12.f/8.f;
		float GraphicSize=10.f;
		float nearClip=-1.f, farClip=1.f;
		float fov=glm::radians(45.f);
		float PNearClip=.01f, PFarClip=1000.f;
		void UpdateProjection();
		ProjectionType projectionType=ProjectionType::Orthographic;
	};

}


#endif //ENGINE_TUTORIAL_SCENECAMERA_H
