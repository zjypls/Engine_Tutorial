//
// Created by z on 2023/9/5.
//

#ifndef ENGINE_TUTORIAL_IMGUIRENDERERVULKAN_H
#define ENGINE_TUTORIAL_IMGUIRENDERERVULKAN_H
#include "Z/ImGui/ImGuiRendererPlatform.h"
namespace Z {
	class ImGuiRendererVulkan final : public ImGuiRendererPlatform{
	public:
		void Begin();
		void End();
		void PlatformInit();
		void Shutdown();
	};

}

#endif //ENGINE_TUTORIAL_IMGUIRENDERERVULKAN_H
