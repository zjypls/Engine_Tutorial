//
// Created by 32725 on 2023/6/1.
//

#include <unordered_set>
#include "Include/stb/stb_image.h"
#include "Include/yaml-cpp/include/yaml-cpp/yaml.h"
#include "Include/tinyobjloader/tiny_obj_loader.h"
#include "Include/glm/glm.hpp"
#include "Include/glm/gtx/hash.hpp"
#include "Include/glm/gtc/type_ptr.hpp"

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Scene/Components.h"
#include "Z/Renderer/RenderManager.h"
#include "Z/Renderer/RenderResourceTypes.h"
#include "Z/Utils/ZUtils.h"

namespace Z{
	static GraphicInterface* s_Context=nullptr;

	std::string ImporterTypeToString(Z::AssetsImporterType type){
		switch(type) {
			#define tCase(str) case Z::AssetsImporterType:: str : return #str
			tCase(Texture2D);
			tCase(Mesh);
			tCase(Material);
			tCase(SkyBox);
			tCase(Shader);
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
		tBranch(Shader);
		tBranch(None);
		#undef tBranch()
		return Z::AssetsImporterType::None;
	}
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
			else if(extension.compare(".glsl")==0)
				return Z::AssetsImporterType::Shader;
			Z_CORE_ASSERT(false,"Unknow extension type : {0}",extension);
			return Z::AssetsImporterType::None;
		}

		void* LoadSkyBox(const std::string& directory){
			const std::vector path{
				directory+"/right.jpg",
				directory+"/left.jpg",
				directory+"/top.jpg",
				directory+"/bottom.jpg",
				directory+"/front.jpg",
				directory+"/back.jpg"
			};
			auto skybox=new Z::Skybox();
			std::array<void*,6> pixelsData{};
			int width,height,format;
			//stbi_set_flip_vertically_on_load(true);
			for(int i=0;i<6;i++){
				auto pixelData=stbi_load(path[i].c_str(),&width,&height,&format,STBI_rgb_alpha);
				Z_CORE_ASSERT(pixelData,"Failed to load skybox image");
				pixelsData[i]=pixelData;
			}
			Z::ImageInfo imageInfo{};
			imageInfo.format=Z::Format::R8G8B8A8_UNORM;
			imageInfo.extent={static_cast<uint32>(width),static_cast<uint32>(height),1};
			imageInfo.arrayLayers=6;
			imageInfo.mipMapLevels=1;
			imageInfo.usageFlag=Z::ImageUsageFlag::SAMPLED|Z::ImageUsageFlag::TRANSFER_DST;
			imageInfo.memoryPropertyFlag=Z::MemoryPropertyFlag::DEVICE_LOCAL;
			imageInfo.sampleCount=Z::SampleCountFlagBits::e1_BIT;
			imageInfo.tilling=Z::ImageTiling::OPTIMAL;
			imageInfo.initialLayout=Z::ImageLayout::UNDEFINED;
			s_Context->CreateCubeMap(imageInfo,skybox->image,skybox->memory,skybox->imageView,pixelsData);
			for(int i=0;i<6;i++){
				stbi_image_free(pixelsData[i]);
			}
			skybox->path=directory;
			skybox->type=AssetsImporterType::SkyBox;
			return skybox;
		}

		void* LoadTexture2D(const std::string&path){
			int width,height,format;
			auto pixelData=stbi_load(path.c_str(),&width,&height,&format,STBI_rgb_alpha);
			Z_CORE_ASSERT(pixelData,"Failed to load texture2D image");
			auto texture=new Z::Texture2D();
			Z::ImageInfo imageInfo{};
			imageInfo.format=Z::Format::R8G8B8A8_UNORM;
			imageInfo.extent={static_cast<uint32>(width),static_cast<uint32>(height),1};
			imageInfo.usageFlag=Z::ImageUsageFlag::SAMPLED|Z::ImageUsageFlag::TRANSFER_DST;
			imageInfo.memoryPropertyFlag=Z::MemoryPropertyFlag::DEVICE_LOCAL;
			imageInfo.sampleCount=Z::SampleCountFlagBits::e1_BIT;
			imageInfo.tilling=Z::ImageTiling::OPTIMAL;
			imageInfo.initialLayout=Z::ImageLayout::UNDEFINED;
			imageInfo.mipMapLevels=1;
			imageInfo.arrayLayers=1;
			s_Context->CreateImage(imageInfo,texture->image,texture->memory,texture->imageView,pixelData);
			stbi_image_free(pixelData);
			texture->path=path;
			texture->type=AssetsImporterType::Texture2D;
			return texture;
		}

		void* LoadMesh(const std::string& path){
			auto mesh=new Z::MeshRes();
			return mesh;
		}

		std::vector<ShaderStageFlag> CountShaderStages(const std::string& source){
			std::vector<ShaderStageFlag> stages{};
			if(source.find("Z_VERTEX")!=std::string::npos)
				stages.push_back(ShaderStageFlag::VERTEX);
			if(source.find("Z_FRAGMENT")!=std::string::npos)
				stages.push_back(ShaderStageFlag::FRAGMENT);
			if(source.find("Z_GEOMERTY")!=std::string::npos)
				stages.push_back(ShaderStageFlag::GEOMETRY);
			if(source.find("Z_COMPUTE")!=std::string::npos)
				stages.push_back(ShaderStageFlag::COMPUTE);
			return stages;
		}

		ShaderRes* LoadShader(const std::string&path){
			auto shader=new ShaderRes();
			shader->source=Utils::ReadFile(path);
			shader->stages=CountShaderStages(shader->source);
			shader->path=path;
			shader->type=AssetsImporterType::Shader;
			return shader;
		}

		void DestroySkyBox(void*skybox){
			auto sky=static_cast<Z::Skybox*>(skybox);
			s_Context->DestroyImage(sky->image,sky->memory,sky->imageView);
			delete sky->image;
			delete sky->imageView;
			delete sky->memory;
			delete sky;
		}

		void DestroyTexture2D(void*texture){
			auto tex=static_cast<Z::Texture2D*>(texture);
			s_Context->DestroyImage(tex->image,tex->memory,tex->imageView);
			delete tex->image;
			delete tex->imageView;
			delete tex->memory;
			delete tex;
		}

		void DestroyMesh(void*mesh){
			auto m=static_cast<Z::MeshRes*>(mesh);
			for(int i=0;i<m->vertexBufferCount;++i){
				s_Context->DestroyBuffer(m->vertexBuffer[i],m->vertexMemory[i]);
			}
			s_Context->DestroyBuffer(m->indexBuffer,m->indexMemory);
			s_Context->DestroyBuffer(m->boneBuffer,m->boneMemory);
			delete m->boneBuffer;
			delete m->boneMemory;
			delete[] m->vertexBuffer;
			delete[] m->vertexMemory;
			delete m->indexBuffer;
			delete m->indexMemory;
			delete m;
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


	void* AssetsSystem::LoadAsset(const std::filesystem::path &path){
		if(!std::filesystem::exists(path)){
			Z_CORE_ERROR("File not found: {0}",path.string());
			return nullptr;
		}
		auto confFile=path;
		auto extension=confFile.extension().string();
		if(extension.compare(Z_CONF_EXTENSION)!=0){
			confFile=confFile.string()+Z_CONF_EXTENSION;
			if(!std::filesystem::exists(confFile))
				Tools::CreateConf(zGUID{},Tools::ExtensionGetType(extension),confFile.string());
		}
		auto metaData=Tools::LoadConf(confFile.string());
		instance->PathToUID[path.string()]=metaData.id;
		instance->UIDToPath[metaData.id]=path.string();
		void* ptr;
		switch(metaData.importer){
			case AssetsImporterType::Texture2D:
				ptr = Tools::LoadTexture2D(path.string());
				break;
			case AssetsImporterType::Mesh:
				ptr = Tools::LoadMesh(path.string());
				break;
			case AssetsImporterType::Material:
				break;
			case AssetsImporterType::SkyBox:{
				ptr = Tools::LoadSkyBox(path.parent_path().string());
			}
			case AssetsImporterType::None:
				break;
			case AssetsImporterType::Shader:
				ptr=Tools::LoadShader(path.string());
				break;
		}
		Z_CORE_ASSERT(ptr,"Failed to load asset");
		instance->resourceLibrary[metaData.id]=ptr;
		return ptr;
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

	void AssetsSystem::Destroy(){
		for(auto asset:instance->resourceLibrary){
			switch(*((AssetsImporterType*)asset.second)){
				case AssetsImporterType::Texture2D:
					Tools::DestroyTexture2D(asset.second);
					break;
				case AssetsImporterType::Mesh:
					Tools::DestroyMesh(asset.second);
					break;
				case AssetsImporterType::SkyBox:
					Tools::DestroySkyBox(asset.second);
					break;
				case AssetsImporterType::Material:
					break;
				case AssetsImporterType::None:
					break;
			}
		}
		instance->resourceLibrary.clear();
		instance->UIDToPath.clear();
		instance->PathToUID.clear();
		instance=nullptr;
	}



	void AssetsSystem::LoadWithMetaData(const AssetsSystem::MetaData &data,const std::string& path) {
		Z_CORE_ASSERT(!IsExisting(data.id),"Import using a existing id");
		instance->PathToUID[path+Z_CONF_EXTENSION]=data.id;
		instance->UIDToPath[data.id]=path+Z_CONF_EXTENSION;
		switch(data.importer){
		}
	}

}

