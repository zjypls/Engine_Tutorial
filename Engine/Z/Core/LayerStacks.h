//
// Created by 32725 on 2023/3/13.
//

#ifndef ENGINE_TUTORIAL_LAYERSTACKS_H
#define ENGINE_TUTORIAL_LAYERSTACKS_H
#include "Layer.h"
#include <vector>
namespace Z {
	class LayerStacks {
	public:
		LayerStacks();
		~LayerStacks();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopOverlay(Layer* overlay);
		inline std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		inline std::vector<Layer*>::iterator end() { return Layers.end(); }
	private:
		std::vector<Layer*> Layers;
		unsigned  int LayerIndex;
	};

}

#endif //ENGINE_TUTORIAL_LAYERSTACKS_H
