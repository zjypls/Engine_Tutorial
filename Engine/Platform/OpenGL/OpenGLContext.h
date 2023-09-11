//
// Created by 32725 on 2023/3/14.
//

#ifndef ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
#define ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
#include "Z/Renderer/GraphicContext.h"
#include "GLFW/glfw3.h"
namespace Z {
	class OpenGLContext final: public GraphicContext {
		GLFWwindow* windowHandle;
	public:
		static void PreInit();
		OpenGLContext(GLFWwindow*);
		void SwapBuffers() override;
		void Init() override;
		void Destroy()override;
	};

}

#endif //ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
