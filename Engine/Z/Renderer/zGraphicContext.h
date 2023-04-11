//
// Created by 32725 on 2023/3/14.
//
#pragma once
#ifndef ENGINE_TUTORIAL_ZCONTEXT_H
#define ENGINE_TUTORIAL_ZGRAPHICCONTEXT_H
#include "Z/Core/Core.h"

namespace Z {
	class Z_API zGraphicContext {
	public:
		virtual void Init()=0;
		virtual void SwapBuffers()=0;
		static Scope<zGraphicContext> Create(void* window);
	};

}


#endif //ENGINE_TUTORIAL_ZCONTEXT_H
