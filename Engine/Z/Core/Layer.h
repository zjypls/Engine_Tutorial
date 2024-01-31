//
// Created by 32725 on 2023/3/13.
//

#ifndef ENGINE_TUTORIAL_LAYER_H
#define ENGINE_TUTORIAL_LAYER_H
#include <string>

#include "Z/Events/Event.h"

namespace Z {
	class Z_API Layer {
	protected:
		std::string DebugLayerName;
	public:
		Layer(const std::string& name="Layer");
		virtual ~Layer()=default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}
		inline virtual const std::string& GetName()const {return DebugLayerName;}
	};

}

#endif //ENGINE_TUTORIAL_LAYER_H
