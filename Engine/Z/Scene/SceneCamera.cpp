//
// Created by 32725 on 2023/3/27.
//
#include "Z/Scene/SceneCamera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Z {
	void SceneCamera::SetGraphicSize(float size, float nearClip, float farClip) {
		GraphicSize = size;
		this->nearClip = nearClip;
		this->farClip = farClip;
		UpdateProjection();
	}

	void SceneCamera::OnViewportResize(unsigned int width, unsigned int height) {
		aspectRatio = (float) width / (float) height;
		UpdateProjection();
	}

	void SceneCamera::UpdateProjection() {
		if (projectionType == ProjectionType::Perspective)
			projection = glm::perspective(fov, aspectRatio, PNearClip, PFarClip);
		else
			projection = glm::ortho(-aspectRatio * GraphicSize / 2.f, aspectRatio * GraphicSize / 2.f,
			                        -GraphicSize / 2.f, GraphicSize / 2.f, nearClip, farClip);
	}

}
