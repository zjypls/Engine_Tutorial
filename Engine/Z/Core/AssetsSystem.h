//
// Created by 32725 on 2023/6/1.
//

#ifndef ENGINE_TUTORIAL_ASSETSSYSTEM_H
#define ENGINE_TUTORIAL_ASSETSSYSTEM_H

#include <unordered_map>
#include <string>
#include <filesystem>

#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Z/Core/zGUID.h"
#include "Z/Renderer/GraphicInterface.h"


namespace Z {
	enum class AssetsImporterType {
		None, Texture2D, Mesh, Material, Shader , SkyBox
	};
	struct TypeInterface{
		AssetsImporterType type;
	};
	struct Texture2D {
		AssetsImporterType type;
		std::string path;
		Image* image;
		ImageView* imageView;
		DeviceMemory* memory;
	};

	struct MeshRes{
		AssetsImporterType type;
		std::string path;
		Buffer** vertexBuffer;
		Buffer* indexBuffer;
		DeviceMemory** vertexMemory;
		DeviceMemory* indexMemory;
		Buffer* boneBuffer;
		DeviceMemory* boneMemory;
		const uint32 vertexBufferCount=2;
		uint32 indicesCount;
	};


	struct Skybox {
		AssetsImporterType type;
		std::string path;
		Image* image;
		ImageView* imageView;
		DeviceMemory* memory;
	};

	struct Material {
		AssetsImporterType type;
		std::string path;
	};

	struct ShaderRes {
		AssetsImporterType type;
		std::string path;
		std::string source;
		std::vector<ShaderStageFlag> stages;
	};

	class Z_API AssetsSystem final {
	public:
		struct MetaData {
			zGUID id;
			AssetsImporterType importer;

			MetaData(const zGUID &id, AssetsImporterType type) : id(id), importer(type) {}
		};

	private:

		std::unordered_map<zGUID, void*> resourceLibrary{};
		std::unordered_map<zGUID, std::string> UIDToPath{};
		std::unordered_map<std::string, zGUID> PathToUID{};

		std::filesystem::path ProjectPath;
		Ref<GraphicInterface> Context;
		static Scope<AssetsSystem> instance;


		static void LoadWithMetaData(const MetaData &data, const std::string &path);

		static void* LoadAsset(const std::filesystem::path &path);

	public:
		inline static bool IsExisting(const std::string &path) {
			#if __cplusplus >= 202002L
			return instance->PathToUID.contains(path);
			#else
			return instance->PathToUID.find(path) != instance->PathToUID.end();
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
		static void Destroy();


		template<class Ty>
		static Ty* Load(const std::filesystem::path &path){
			Z_CORE_ASSERT(!path.empty(),"Empty Path Load Is Illegal!!!");
			if(IsExisting(path.string())){
				return Get<Ty>(instance->PathToUID[path.string()]);
			}
			return (Ty*)LoadAsset(path);
		}

		template<class Ty>
		static Ty* Get(const zGUID &id){
			Z_CORE_ASSERT(IsExisting(id),"Get a non-existing id");
			return (Ty*)instance->resourceLibrary.at(id);
		}

	};

}

#endif //ENGINE_TUTORIAL_ASSETSSYSTEM_H
