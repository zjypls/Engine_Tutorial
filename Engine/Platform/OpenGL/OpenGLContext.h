//
// Created by 32725 on 2023/3/14.
//

#ifndef ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
#define ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
#include "Include/glfw/include/GLFW/glfw3.h"
#include "Z/Renderer/GraphicContext.h"
namespace Z {
	class OpenGLContext final: public GraphicContext {
		GLFWwindow* windowHandle;
	public:
		static void PreInit();
		void DeviceSynchronize();
		OpenGLContext(GLFWwindow*);
		void SwapBuffers() override;
		void Init() override;
		void Destroy()override;
	};

}

#endif //ENGINE_TUTORIAL_ZOPENGLCONTEXT_H
