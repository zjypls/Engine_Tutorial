//
// Created by 32725 on 2023/6/1.
//

#ifndef ENGINE_TUTORIAL_ASSETSSYSTEM_H
#define ENGINE_TUTORIAL_ASSETSSYSTEM_H

#include <unordered_map>
#include <string>
#include <filesystem>

#include "Z/Core/Core.h"
#include "Z/Core/zGUID.h"
#include "Z/Utils/Model.h"


namespace Z {
	class Z_API AssetsSystem final {
	public:
		enum class ImporterType {
			None, Texture2D, Mesh, Material, Shader , SkyBox
		};

		struct MetaData {
			zGUID id;
			ImporterType importer;

			MetaData(const zGUID &id, ImporterType type) : id(id), importer(type) {}
		};

	private:

		std::unordered_map<zGUID, Ref<Mesh>> MeshLibrary{};
		std::unordered_map<zGUID, std::string> UIDToPath{};
		std::unordered_map<std::string, zGUID> PathToUID{};

		std::filesystem::path ProjectPath;
		static Scope<AssetsSystem> instance;

		auto LoadTextureInner(const zGUID &id);


		static void LoadWithMetaData(const MetaData &data, const std::string &path);

	public:
		inline static bool IsExisting(const std::string &name) {
			#if __cplusplus >= 202002L
			return instance->PathToUID.contains(name);
			#else
			return instance->PathToUID.find(name) != instance->PathToUID.end();
			#endif
		}

		inline static bool IsExisting(const zGUID &id) {
			#if __cplusplus >= 202002L
			return instance->UIDToPath.contains(id);
			#else
			return instance->UIDToPath.find(id) != instance->UIDToPath.end();
			#endif
		}

		static const Scope<AssetsSystem> &Instance() { return instance; }

		static void InitWithProject(const std::filesystem::path &projectPath);
		//Init Path with Source dir and create instance
		static void PreInit();
		static void Destroy() {
			instance=nullptr;
		}


		template<class Ty>
		static Ref<Ty> Load(const std::string &path, bool absolute = false);

		template<class Ty>
		static Ref<Ty> Get(const zGUID &id);

	};

}

#endif //ENGINE_TUTORIAL_ASSETSSYSTEM_H
