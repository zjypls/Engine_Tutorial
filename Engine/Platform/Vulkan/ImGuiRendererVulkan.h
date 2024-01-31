//
// Created by z on 2023/9/5.
//

#ifndef ENGINE_TUTORIAL_IMGUIRENDERERVULKAN_H
#define ENGINE_TUTORIAL_IMGUIRENDERERVULKAN_H
#include "Z/ImGui/ImGuiRendererPlatform.h"
namespace Z {
	class ImGuiRendererVulkan final : public ImGuiRendererPlatform{
	public:
		void Begin() override;
		void End() override;
		void PlatformInit() override;
		void Shutdown() override;
		void InitUIRenderBackend(RenderPassInterface *renderPassInterface,uint32 uiIndex) override;

	};

}

#endif //ENGINE_TUTORIAL_IMGUIRENDERERVULKAN_H
