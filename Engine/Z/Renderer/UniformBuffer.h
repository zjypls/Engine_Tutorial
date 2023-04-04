//
// Created by 32725 on 2023/4/3.
//

#ifndef ENGINE_TUTORIAL_UNIFORMBUFFER_H
#define ENGINE_TUTORIAL_UNIFORMBUFFER_H
#include "Z/Core/Core.h"

namespace Z {
	class UniformBuffer {
		public:
		virtual ~UniformBuffer() = default;
		virtual void SetData(const void* data, unsigned int size,int offset=0) = 0;
		static Ref<UniformBuffer> Create(unsigned int size, unsigned int binding);
	};

}


#endif //ENGINE_TUTORIAL_UNIFORMBUFFER_H
