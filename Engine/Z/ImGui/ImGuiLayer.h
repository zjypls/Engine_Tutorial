//
// Created by 32725 on 2023/3/13.
//

#ifndef ENGINE_TUTORIAL_IMGUILAYER_H
#define ENGINE_TUTORIAL_IMGUILAYER_H
#include "Include/imgui/imgui.h"

#include "Z/Core/Core.h"
#include "Z/Core/Layer.h"
#include "Z/Core/Log.h"
#include "Z/Events/KeyEvent.h"
#include "Z/Events/MouseEvent.h"
#include "Z/Events/ApplicationEvent.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {
	class Z_API ImGuiLayer: public Layer {
		bool BlockEvents=false;
	public:
		ImGuiLayer();
		~ImGuiLayer() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
		virtual void SetBlockEvents(bool block) { BlockEvents = block; }

		void Begin();
		void End();

	};

}

#endif //ENGINE_TUTORIAL_IMGUILAYER_H
