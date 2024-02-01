//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_ORITHGRAPHICCAMERA_H
#define ENGINE_TUTORIAL_ORITHGRAPHICCAMERA_H
#include "Include/glm/glm.hpp"

namespace Z {
	class OrithGraphicCamera {
	public:
		OrithGraphicCamera(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix();}
		void SetRotation(float rotation) { Rotation = rotation;RecalculateViewMatrix(); }
		const glm::vec3& GetPosition() const { return m_Position; }
		void ResetProjection(float left, float right, float bottom, float top) ;
		float GetRotation() const { return Rotation; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	protected:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;
		glm::vec3 m_Position = glm::vec3{ 0.0f, 0.0f, 0.0f };
		float Rotation = 0.0f;
	};

}

#endif //ENGINE_TUTORIAL_ORITHGRAPHICCAMERA_H
