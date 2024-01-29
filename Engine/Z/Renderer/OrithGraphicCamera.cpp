//
// Created by 32725 on 2023/3/17.
//

#include "Include/glm/glm/gtc/matrix_transform.hpp"

#include "Z/Renderer/OrithGraphicCamera.h"
namespace Z{

	void OrithGraphicCamera::RecalculateViewMatrix() {
		auto transform = glm::translate(glm::mat4(1.f), m_Position) * glm::rotate(glm::mat4(1.f), glm::radians(Rotation), glm::vec3(0, 0, 1));
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	OrithGraphicCamera::OrithGraphicCamera(float left, float right, float bottom, float top) {
		m_Position= { 0,0,0 };
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewMatrix = glm::mat4(1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrithGraphicCamera::ResetProjection(float left, float right, float bottom, float top) {
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}