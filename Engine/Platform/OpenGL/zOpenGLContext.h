//
// Created by 32725 on 2023/3/14.
//

#ifndef ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
#define ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
#include "Z/Renderer/zGraphicContext.h"
#include "GLFW/glfw3.h"
namespace Z {
	class zOpenGLContext :public zGraphicContext {
		GLFWwindow* windowHandle;
	public:
		zOpenGLContext(GLFWwindow*);
		virtual void SwapBuffers() override;
		virtual void Init() override;
	};

}

#endif //ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
