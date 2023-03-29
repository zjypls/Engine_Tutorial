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

	void OpenGLRenderAPI::DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray,unsigned int count) {
		//count = (count == 0) ? vertexArray->GetIndexBuffer()->GetCount() : count;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::Init() {
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Z_TRACE("Info : \n\tOpenGLRenderAPI::Init:");
		std::cout << "\t\tVersion:"<<glGetString(GL_VERSION) << std::endl;
		std::cout << "\t\tVendor:"<< glGetString(GL_VENDOR) << std::endl;
		std::cout << "\t\tRenderer:"<< glGetString(GL_RENDERER) << std::endl;
	}

	void OpenGLRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRenderAPI::GetError() {
		auto warn = glGetError();
		if (warn != 0)
			Z_CORE_ERROR("{1}:{2},OpenGL ERROR {0}", warn, __FILE__, __LINE__);
	}

}