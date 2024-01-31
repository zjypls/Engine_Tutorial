//
// Created by 32725 on 2023/3/13.
//

#ifndef ENGINE_TUTORIAL_LAYERSTACKS_H
#define ENGINE_TUTORIAL_LAYERSTACKS_H
#include "Z/Core/Layer.h"
#include <vector>
namespace Z {
	class Z_API LayerStacks {
	public:
		LayerStacks();
		~LayerStacks();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);
		void PopAllLayer(){
			for(const auto&layer:Layers)
				layer->OnDetach();
			Layers.clear();
			LayerIndex=0;
		}
		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }
	private:
		std::vector<Layer*> Layers;
		uint32 LayerIndex;
	};

}

#endif //ENGINE_TUTORIAL_LAYERSTACKS_H
