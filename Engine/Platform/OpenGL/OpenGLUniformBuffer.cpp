//
// Created by 32725 on 2023/4/3.
//

#include "Include/glad/include/glad/glad.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Z {

	OpenGLUniformBuffer::OpenGLUniformBuffer(unsigned int size, unsigned int binding) {
		glCreateBuffers(1, &ID);
		glNamedBufferData(ID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, ID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer() {
		glDeleteBuffers(1, &ID);
	}

	void OpenGLUniformBuffer::SetData(const void *data, unsigned int size, int offset) {
		glNamedBufferSubData(ID, offset, size, data);
	}
}