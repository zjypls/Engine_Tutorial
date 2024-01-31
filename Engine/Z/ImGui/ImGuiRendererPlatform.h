//
// Created by z on 2023/9/5.
//

#ifndef ENGINE_TUTORIAL_IMGUIRENDERERPLATFORM_H
#define ENGINE_TUTORIAL_IMGUIRENDERERPLATFORM_H

#include "Z/Core/Core.h"
#include "Z/Renderer/RenderResource.h"

namespace Z {
	class ImGuiRendererPlatform {
	public:
		static void Init();
		virtual void Begin()=0;
		virtual void End()=0;
		virtual void PlatformInit()=0;
		virtual void Shutdown()=0;
		virtual void InitUIRenderBackend(RenderPassInterface* renderPassInterface,uint32 uiIndex=0)=0;
		[[nodiscard("This is the imGui init tool for different platforms")]]
		static auto& GetRenderer(){return renderer;}
	private:
		static Ref<ImGuiRendererPlatform> renderer;
	};

}
#endif //ENGINE_TUTORIAL_IMGUIRENDERERPLATFORM_H
