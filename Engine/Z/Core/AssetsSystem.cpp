//
// Created by 32725 on 2023/6/1.
//

#include <set>
#include "Include/stb/stb_image.h"
#include "Include/yaml-cpp/include/yaml-cpp/yaml.h"
#include "Include/tinyobjloader/tiny_obj_loader.h"
#include "Include/glm/glm.hpp"
#include "Include/glm/gtx/hash.hpp"

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Scene/Components.h"
#include "Z/Renderer/RenderManager.h"

namespace Z{
	static GraphicInterface* s_Context=nullptr;
	struct Vertex {
		glm::vec3 verts;
		glm::vec2 tex;
		glm::vec3 normal;

		bool operator==(const Vertex &o) const {
			return verts == o.verts && tex == o.tex && normal == o.normal;
		}
	};

	std::string ImporterTypeToString(Z::AssetsImporterType type){
		switch(type) {
			#define tCase(str) case Z::AssetsImporterType:: str : return #str
			tCase(Texture2D);
			tCase(Mesh);
			tCase(Material);
			tCase(SkyBox);
			tCase(None);
			#undef tCase()
		}
		return "";
	}
	Z::AssetsImporterType StringToImporterType(const std::string&type){

		#define tBranch(str) if(#str == type) return Z::AssetsImporterType:: str
		tBranch(Texture2D);
		tBranch(Material);
		tBranch(Mesh);
		tBranch(SkyBox);
		tBranch(None);
		#undef tBranch()
		return Z::AssetsImporterType::None;
	}
}



namespace std {
	template<>
	struct hash<Z::Vertex> {
		size_t operator()(const Z::Vertex &v) const {
			return hash<glm::vec3>()(v.verts) << 1
			       | hash<glm::vec3>()(v.normal)
			       | hash<glm::vec2>()(v.tex);
		}
	};
}


namespace YAML{

	template<>
	struct convert<Z::AssetsImporterType> {
		static Node encode(Z::AssetsImporterType type){
			Node node;
			node.push_back(Z::ImporterTypeToString(type));
			return node;
		}
		static bool decode(const Node &node, Z::AssetsImporterType& type){
			type=Z::StringToImporterType(node.as<std::string>());
			return true;
		}
	};
}

namespace Z {

	namespace Tools{

		YAML::Emitter& operator<<(YAML::Emitter&emitter,AssetsImporterType type){
			return emitter<< ImporterTypeToString(type);
		}

		AssetsSystem::MetaData LoadConf(const std::string&confPath){
			YAML::Node node;
			try{
				node=YAML::LoadFile(confPath);
			}catch(std::exception& e){
				Z_CORE_ERROR("YAML error:{}!!!",e.what());
				Z_CORE_ASSERT(false,"Conf Load failed");
				return{0,Z::AssetsImporterType::None};
			}
			auto id=node["GUID"].as<uint64>();
			auto importer=node["Importer"].as<Z::AssetsImporterType>();
			return {id,importer};
		}
		void CreateConf(const zGUID&id,Z::AssetsImporterType type,const std::string &pathOut){
			auto typeSTR= ImporterTypeToString(type);
			YAML::Emitter emitter;
			emitter<<YAML::BeginMap;
			emitter<<YAML::Key<<"GUID"<<YAML::Value<<id;
			emitter<<YAML::Key<<"Importer"<<YAML::Value<< typeSTR;
			emitter<<YAML::EndMap;

			std::ofstream out{pathOut,std::ios_base::out|std::ios_base::trunc};
			Z_CORE_ASSERT(out.is_open(),"Failed to create conf files");
			out<<emitter.c_str();
			out.close();
			Z_CORE_WARN("UID:{0},Type:{1},write to {2}!",id,typeSTR,pathOut);
		}

		// Mesh* LoadMesh(const std::string&path){
		// 	auto mesh = new Mesh();
		// 	tinyobj::attrib_t attrib{};
		// 	std::vector<tinyobj::shape_t> shapes{};
		// 	std::vector<tinyobj::material_t> materials;
		// 	std::string warnings, errors;
		// 	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, path.c_str())) {
		// 		Z_CORE_WARN("Loader give warning : {0}", warnings);
		// 		Z_CORE_ERROR("Loader give errors : {0}", errors);
		// 		return mesh;
		// 	}
		// 	std::unordered_map<Vertex, uint32> Vertexes{};
		// 	std::vector<Vertex> vertexes;
		// 	std::vector<uint32> indexes;
		// 	for (const auto &shape: shapes) {
		// 		for (const auto &indices: shape.mesh.indices) {
		// 			Vertex vert{};
		// 			vert.verts = {
		// 					attrib.vertices[indices.vertex_index * 3 + 0],
		// 					attrib.vertices[indices.vertex_index * 3 + 1],
		// 					attrib.vertices[indices.vertex_index * 3 + 2]};
		// 			vert.tex = {
		// 					attrib.texcoords[indices.texcoord_index * 2 + 0],
		// 					attrib.texcoords[indices.texcoord_index * 2 + 1]};
		// 			vert.normal = {
		// 					attrib.normals[indices.normal_index * 3 + 0],
		// 					attrib.normals[indices.normal_index * 3 + 1],
		// 					attrib.normals[indices.normal_index * 3 + 2]};
		// 			if (Vertexes.count(vert) == 0) {
		// 				Vertexes[vert] = vertexes.size();
		// 				vertexes.push_back(vert);
		// 			}
		// 			indexes.push_back(Vertexes[vert]);
		// 		}
		// 	}

		// 	return mesh;
		// }

		static const std::set<std::string> TextureSheets{".png",".jpg",".jpeg",".bmp"};
		static const std::set<std::string> MeshSheets{".obj",".fbx",".gltf"};

		Z::AssetsImporterType ExtensionGetType(const std::string&extension){
			if(TextureSheets.find(extension)!=TextureSheets.end())
				return Z::AssetsImporterType::Texture2D;
			else if(MeshSheets.find(extension)!=MeshSheets.end())
				return Z::AssetsImporterType::Mesh;
			return Z::AssetsImporterType::None;
		}


	}

	static const std::set<std::string> registerTypes{".png",".jpg",".jpeg",".bmp",".obj",".fbx"};
	inline bool IsRegistered(const std::string& extension){
		#if __cplusplus>=202002L
			return registerTypes.contains(extension);
		#else
			return registerTypes.find(extension)!=registerTypes.end();
		#endif
	}

	Scope<AssetsSystem> AssetsSystem::instance=nullptr;

	void AssetsSystem::PreInit(){
		if(instance)
			return;
		instance=CreateScope<AssetsSystem>();
		instance->Context=RenderManager::GetInstance();
		Z::s_Context=instance->Context.get();
	}

	void AssetsSystem::InitWithProject(const std::filesystem::path &projectPath) {
		Z_CORE_ASSERT(instance,"Need call PreInit() before InitWithProject!!!");
		Z_CORE_ASSERT(!projectPath.empty(), "Empty Path Init AssetsSystem Is Illegal!!!");
		instance->ProjectPath=projectPath;
		std::filesystem::recursive_directory_iterator _it(projectPath);
		std::set<std::filesystem::path> zConfDetect{};

		for(auto iterator=_it;iterator!=std::filesystem::recursive_directory_iterator();++iterator){
			if(iterator->is_regular_file()&&iterator->path().extension()==Z_CONF_EXTENSION){
				zConfDetect.insert(iterator->path());
			}
		}
		_it=std::filesystem::recursive_directory_iterator(projectPath);

		for(;_it!=std::filesystem::recursive_directory_iterator();++_it){
			auto _path=_it->path();
			if(_it->is_regular_file()&& IsRegistered(_path.extension().string())&&_path.extension()!=Z_CONF_EXTENSION){
				auto zConfFile=_path.string()+Z_CONF_EXTENSION;
				if(zConfDetect.find(zConfFile)!=zConfDetect.end()){
					zConfDetect.erase(zConfFile);
				}else{
					Tools::CreateConf(zGUID{},Tools::ExtensionGetType(_path.extension().string()),_path.string()+Z_CONF_EXTENSION);
				}
				auto metaData=Tools::LoadConf(zConfFile);
				LoadWithMetaData(metaData,_it->path().string());
			}
		}
		for(const auto& conf:zConfDetect){
			LoadWithMetaData(Tools::LoadConf(conf.string()),conf.string());
		}
	}



	void AssetsSystem::LoadWithMetaData(const AssetsSystem::MetaData &data,const std::string& path) {
		Z_CORE_ASSERT(!IsExisting(data.id),"Import using a existing id");
		instance->PathToUID[path+Z_CONF_EXTENSION]=data.id;
		instance->UIDToPath[data.id]=path+Z_CONF_EXTENSION;
		switch(data.importer){
		}
	}

}

