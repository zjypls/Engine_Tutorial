//
// Created by 32725 on 2023/4/3.
//

#ifndef ENGINE_TUTORIAL_OPENGLUNIFORMBUFFER_H
#define ENGINE_TUTORIAL_OPENGLUNIFORMBUFFER_H
#include "Z/Renderer/UniformBuffer.h"

namespace Z {
	class OpenGLUniformBuffer: public UniformBuffer {
		unsigned int ID;
	public:
		OpenGLUniformBuffer(unsigned int size, unsigned int binding);
		virtual ~OpenGLUniformBuffer() override;
		virtual void SetData(const void* data, unsigned int size,int offset=0) override;

	};

}


#endif //ENGINE_TUTORIAL_OPENGLUNIFORMBUFFER_H
