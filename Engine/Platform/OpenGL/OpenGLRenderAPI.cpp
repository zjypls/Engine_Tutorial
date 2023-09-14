//
// Created by 32725 on 2023/3/17.
//

#include "OpenGLRenderAPI.h"
#include "glad/glad.h"
#include <iostream>

namespace Z {
	void OpenGLRenderAPI::SetClearValue(const glm::vec4 &color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderAPI::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderAPI::DrawIndexed(const Ref<VertexArray> &vertexArray,unsigned int count) {
		vertexArray->Bind();
		//Fixme:out of range bug   //Random Happened???
		count = (count == 0) ? vertexArray->GetIndexBuffer()->GetCount() : count;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::Init() {
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Z_TRACE("Info : \n\tOpenGLRenderAPI::Init:");
		std::cout << "\t\tVersion:"<<glGetString(GL_VERSION) << std::endl;
		std::cout << "\t\tVendor:"<< glGetString(GL_VENDOR) << std::endl;
		std::cout << "\t\tRenderer:"<< glGetString(GL_RENDERER) << std::endl;
	}

	void OpenGLRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRenderAPI::DrawLine(const Ref<VertexArray> &vertexArray, unsigned int count) {
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, count);
	}

	void OpenGLRenderAPI::SetLineWidth(float width) {
		glLineWidth(width);
	}
	GLenum DepthTestStateToGL(RenderAPI::DepthTestState state) {
		switch (state) {
			case RenderAPI::DepthTestState::None:
			return GL_NONE;
		case RenderAPI::DepthTestState::Less:
			return GL_LESS;
		case RenderAPI::DepthTestState::LessOrEqual:
			return GL_LEQUAL;
		case RenderAPI::DepthTestState::Equal:
			return GL_EQUAL;
		case RenderAPI::DepthTestState::NotEqual:
			return GL_NOTEQUAL;
		case RenderAPI::DepthTestState::GreaterOrEqual:
			return GL_GEQUAL;
		case RenderAPI::DepthTestState::Greater:
			return GL_GREATER;
		case RenderAPI::DepthTestState::Always:
			return GL_ALWAYS;
		case RenderAPI::DepthTestState::Never:
			return GL_NEVER;
		default:
			return GL_NONE;
		}
	}
	void OpenGLRenderAPI::ChangeDepthTest(RenderAPI::DepthTestState state) {
		if (state == RenderAPI::DepthTestState::None) {
			glDisable(GL_DEPTH_TEST);
		}else {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(DepthTestStateToGL(state));
		}
	}

}