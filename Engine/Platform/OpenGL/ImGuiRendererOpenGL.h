//
// Created by z on 2023/9/5.
//

#ifndef ENGINE_TUTORIAL_IMGUIRENDEREROPENGL_H
#define ENGINE_TUTORIAL_IMGUIRENDEREROPENGL_H

#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {
	class ImGuiRendererOpenGL final: public ImGuiRendererPlatform {
	public:
		void Begin() override;
		void End() override;
		void PlatformInit() override;
		void Shutdown() override;
		virtual ~ImGuiRendererOpenGL()= default;
	};

}


#endif //ENGINE_TUTORIAL_IMGUIRENDEREROPENGL_H
