//
// Created by 32725 on 2023/6/1.
//

#ifndef ENGINE_TUTORIAL_ASSETSSYSTEM_H
#define ENGINE_TUTORIAL_ASSETSSYSTEM_H

#include <unordered_map>
#include <string>
#include <filesystem>


#include "Z/Core/Core.h"
#include "Z/Renderer/Texture.h"


namespace Z {
	class Z_API AssetsSystem final {
		std::unordered_map<std::string, Ref<Texture2D>> TextureLibrary;
		std::filesystem::path ProjectPath;
		static Scope<AssetsSystem> instance;

		void LoadTextureInner(const std::string &path);

		void PushResource(Ref<Texture2D> &texture,const std::string &name);

	public:
		inline static bool IsExisting(const std::string &name) {
			return instance->TextureLibrary.find(name) != instance->TextureLibrary.end();
		}
		static const Scope<AssetsSystem>& Instance(){return instance;}
		static void Init(const std::filesystem::path &projectPath);

		static Ref<Texture2D> LoadTexture(const std::string &path, bool absolute = false);

	};

}

#endif //ENGINE_TUTORIAL_ASSETSSYSTEM_H
