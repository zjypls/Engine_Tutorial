//
// Created by 32725 on 2023/3/30.
//

#ifndef ENGINE_TUTORIAL_SCENESERIALIZER_H
#define ENGINE_TUTORIAL_SCENESERIALIZER_H

#include "Scene.h"

namespace Z {
	class SceneSerializer {
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);
		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> scene;
	};

}

#endif //ENGINE_TUTORIAL_SCENESERIALIZER_H
