//
// Created by z on 2023/9/5.
//

#ifndef ENGINE_TUTORIAL_IMGUIRENDERERPLATFORM_H
#define ENGINE_TUTORIAL_IMGUIRENDERERPLATFORM_H

#include "Z/Core/Core.h"

namespace Z {
	class ImGuiRendererPlatform {
	public:
		static void Init();
		virtual void Begin()=0;
		virtual void End()=0;
		virtual void PlatformInit()=0;
		virtual void Shutdown()=0;
		[[nodiscard("This is the imGui init tool for different platforms")]]
		inline static auto& GetRenderer(){return renderer;}
	private:
		static Ref<ImGuiRendererPlatform> renderer;
	};

}
#endif //ENGINE_TUTORIAL_IMGUIRENDERERPLATFORM_H
