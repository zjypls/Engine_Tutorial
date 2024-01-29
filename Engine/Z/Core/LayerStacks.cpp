//
// Created by 32725 on 2023/3/13.
//

#include <algorithm>

#include "Z/Core/LayerStacks.h"

namespace Z {
	LayerStacks::LayerStacks():LayerIndex(0) {
	}

	LayerStacks::~LayerStacks() {
		for (Layer *layer: Layers) {
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStacks::PushLayer(Layer *layer) {
		Layers.emplace(Layers.begin()+LayerIndex, layer);
	}

	void LayerStacks::PopLayer(Layer *layer) {
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end()) {
			layer->OnDetach();
			Layers.erase(it);
			--LayerIndex;
		}
	}

	void LayerStacks::PushOverlay(Layer *overlay) {
		Layers.emplace_back(overlay);

	}

	void LayerStacks::PopOverlay(Layer *overlay) {
		auto it = std::find(Layers.begin(), Layers.end(), overlay);
		if (it != Layers.end()) {
			overlay->OnDetach();
			Layers.erase(it);
		}
	}

}