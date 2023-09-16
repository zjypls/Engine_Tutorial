//
// Created by 32725 on 2023/6/1.
//

#include <set>

#include "Include/yaml-cpp/include/yaml-cpp/yaml.h"
#include "Include/tinyobjloader/tiny_obj_loader.h"

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Scene/Components.h"

namespace Z{

	struct Vertex {
		glm::vec3 verts;
		glm::vec2 tex;
		glm::vec3 normal;

		bool operator==(const Vertex &o) const {
			return verts == o.verts && tex == o.tex && normal == o.normal;
		}
	};

	std::string ImporterTypeToString(AssetsSystem::ImporterType type){
		switch(type) {
			#define tCase(str) case Z::AssetsSystem::ImporterType:: str : return #str
			tCase(Texture2D);
			tCase(Mesh);
			tCase(Material);
			tCase(SkyBox);
			tCase(None);
			#undef tCase()
		}
		return "";
	}
	AssetsSystem::ImporterType StringToImporterType(const std::string&type){

		#define tBranch(str) if(#str == type) return Z::AssetsSystem::ImporterType:: str
		tBranch(Texture2D);
		tBranch(Material);
		tBranch(Mesh);
		tBranch(SkyBox);
		tBranch(None);
		#undef tBranch()
		return Z::AssetsSystem::ImporterType::None;
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
	struct convert<Z::AssetsSystem::ImporterType> {
		static Node encode(Z::AssetsSystem::ImporterType type){
			Node node;
			node.push_back(Z::ImporterTypeToString(type));
			return node;
		}
		static bool decode(const Node &node, Z::AssetsSystem::ImporterType& type){
			type=Z::StringToImporterType(node.as<std::string>());
			return true;
		}
	};
}

namespace Z {


	namespace Tools{

		YAML::Emitter& operator<<(YAML::Emitter&emitter,AssetsSystem::ImporterType type){
			return emitter<< ImporterTypeToString(type);
		}

		AssetsSystem::MetaData LoadConf(const std::string&confPath){
			YAML::Node node;
			try{
				node=YAML::LoadFile(confPath);
			}catch(std::exception& e){
				Z_CORE_ERROR("YAML error:{}!!!",e.what());
				Z_CORE_ASSERT(false,"Conf Load failed");
				return{0,AssetsSystem::ImporterType::None};
			}
			auto id=node["GUID"].as<uint64_t>();
			auto importer=node["Importer"].as<AssetsSystem::ImporterType>();
			return {id,importer};
		}
		void CreateConf(const zGUID&id,AssetsSystem::ImporterType type,const std::string &pathOut){
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

		Ref<Mesh> LoadMesh(const std::string&path){
			auto mesh = CreateRef<Mesh>();
			tinyobj::attrib_t attrib{};
			std::vector<tinyobj::shape_t> shapes{};
			std::vector<tinyobj::material_t> materials;
			std::string warnings, errors;
			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, path.c_str())) {
				Z_CORE_WARN("Loader give warning : {0}", warnings);
				Z_CORE_ERROR("Loader give errors : {0}", errors);
				return mesh;
			}
			std::unordered_map<Vertex, uint32_t> Vertexes{};
			std::vector<Vertex> vertexes;
			std::vector<uint32_t> indexes;
			for (const auto &shape: shapes) {
				for (const auto &indices: shape.mesh.indices) {
					Vertex vert{};
					vert.verts = {
							attrib.vertices[indices.vertex_index * 3 + 0],
							attrib.vertices[indices.vertex_index * 3 + 1],
							attrib.vertices[indices.vertex_index * 3 + 2]};
					vert.tex = {
							attrib.texcoords[indices.texcoord_index * 2 + 0],
							attrib.texcoords[indices.texcoord_index * 2 + 1]};
					vert.normal = {
							attrib.normals[indices.normal_index * 3 + 0],
							attrib.normals[indices.normal_index * 3 + 1],
							attrib.normals[indices.normal_index * 3 + 2]};
					if (Vertexes.count(vert) == 0) {
						Vertexes[vert] = vertexes.size();
						vertexes.push_back(vert);
					}
					indexes.push_back(Vertexes[vert]);
				}
			}
			auto vertexBuffer = VertexBuffer::Create((float *) vertexes.data(), vertexes.size() * sizeof(Vertex));
			vertexBuffer->SetLayout(BufferLayout{
					{Z::ShaderDataType::Float3, "vertex"},
					{Z::ShaderDataType::Float2, "texCoord"},
					{Z::ShaderDataType::Float3, "normal"}
			});

			auto indexBuffer = IndexBuffer::Create(indexes.data(), indexes.size());
			mesh->vertexArray = VertexArray::Create();
			mesh->vertexArray->AddVertexBuffer(vertexBuffer);
			mesh->vertexArray->SetIndexBuffer(indexBuffer);
			return mesh;
		}

		Ref<CubeMap> LoadSkyBox(const std::filesystem::path& dir){
			//auto _it=std::filesystem::recursive_directory_iterator(dir);
			//TODO:Optimized!!!
			std::vector<std::string> paths{
					(dir/"right.jpg").string(),
					(dir/"left.jpg").string(),
					(dir/"top.jpg").string(),
					(dir/"bottom.jpg").string(),
					(dir/"front.jpg").string(),
					(dir/"back.jpg").string(),
			};
//			for(;_it!=std::filesystem::recursive_directory_iterator();++_it){
//				paths.push_back(_it->path().string());
//			}
			auto cubeMap=CubeMap::CreateTexture(paths);
			return cubeMap;
		}

		static const std::set<std::string> TextureSheets{".png",".jpg",".jpeg",".bmp"};
		static const std::set<std::string> MeshSheets{".obj",".fbx",".gltf"};

		AssetsSystem::ImporterType ExtensionGetType(const std::string&extension){
			if(TextureSheets.find(extension)!=TextureSheets.end())
				return AssetsSystem::ImporterType::Texture2D;
			else if(MeshSheets.find(extension)!=MeshSheets.end())
				return AssetsSystem::ImporterType::Mesh;
			return AssetsSystem::ImporterType::None;
		}

	}

	static const std::set<std::string> registerTypes{".png",".jpg",".jpeg",".bmp",".obj"};
	inline bool IsRegistered(const std::string& extension){
		#if __cplusplus>=202002L
			return registerTypes.contain(extension);
		#else
			return registerTypes.find(extension)!=registerTypes.end();
		#endif
	}

	Scope<AssetsSystem> AssetsSystem::instance;

	auto AssetsSystem::LoadTextureInner(const zGUID &id) {
		return TextureLibrary.insert({id,Texture2D::CreateTexture(UIDToPath[id])});
	}


	void AssetsSystem::PreInit(){
		if(instance)
			return;
		instance=CreateScope<AssetsSystem>();
		instance->ProjectPath=Z_SOURCE_DIR;
	}

	void AssetsSystem::InitWithProject(const std::filesystem::path &projectPath) {
		Z_CORE_ASSERT(instance,"Need call PreInit() before InitWithProject!!!");
		Z_CORE_ASSERT(!projectPath.empty(), "Empty Path Init AssetsSystem Is Illegal!!!");
		//instance= CreateScope<AssetsSystem>();
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
				auto zConfFile=_path.string()+".zConf";
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


	void AssetsSystem::PushResource(Ref<Texture> &texture,const std::string&name) {
		PathToUID[name]=zGUID();
		UIDToPath[PathToUID[name]]=name;
		TextureLibrary[PathToUID[name]]=texture;
	}

	Ref<Texture> AssetsSystem::Get(const std::string &name) {
		if(IsExisting(name))
			return instance->TextureLibrary.at(instance->PathToUID[name]);
		else
			return nullptr;
	}

	void AssetsSystem::LoadWithMetaData(const AssetsSystem::MetaData &data,const std::string& path) {
		Z_CORE_ASSERT(!IsExisting(data.id),"Import using a existing id");
		instance->PathToUID[path+".zConf"]=data.id;
		instance->UIDToPath[data.id]=path+".zConf";
		switch(data.importer){
			case ImporterType::Texture2D:
				instance->TextureLibrary[data.id]=Texture2D::CreateTexture(path);
				break;
			case ImporterType::Mesh:
				instance->MeshLibrary[data.id]=Tools::LoadMesh(path);
				break;
			case ImporterType::Shader:
				instance->ShaderLibrary[data.id]=Shader::CreateShader(path);
				break;
			case ImporterType::SkyBox:{
				instance->TextureLibrary[data.id]=Tools::LoadSkyBox(std::filesystem::path(path).parent_path());
				instance->UIDToPath[data.id]=path;
				break;
			}
		}
	}

	template<>
	Ref<Texture> AssetsSystem::Get(const zGUID &id) {
		if(IsExisting(id))
			return instance->TextureLibrary.at(id);
		else
			return nullptr;
	}

	template<>
	Ref<Mesh> AssetsSystem::Get(const zGUID &id) {
		if(IsExisting(id))
			return instance->MeshLibrary.at(id);
		else
			return nullptr;
	}

	template<class Ty>
	Ref<Ty> AssetsSystem::Load(const std::string &,bool){
		Z_CORE_ASSERT(false,"fatal error");
	}

	template<>
	Ref<Texture> AssetsSystem::Load(const std::string &path, bool absolute) {
		auto _path=std::filesystem::path(path);
		if(!absolute){
			_path=instance->ProjectPath/path;
		}
		if(_path.extension()!=Z_CONF_EXTENSION)
			_path+=".zConf";
		if(IsExisting(_path.string())){
			return instance->TextureLibrary.at(instance->PathToUID[_path.string()]);
		}
		auto pathSTR=_path.string();
		auto id=zGUID{};
		instance->PathToUID[_path.string()]=id;
		instance->UIDToPath[id]=_path.string();
		//auto result=instance->LoadTextureInner(id);
		//TODO:Optimized 6 with sizeof(Z_CONF_EXTENSION)
		auto result=Texture2D::CreateTexture(pathSTR.substr(0,pathSTR.size()-6));
		Z_CORE_ASSERT(result,std::string("failed to load texture:").append(_path.string()));
		return result;
	}


	//TODO:move to assets system
	template<>
	Ref<Mesh> AssetsSystem::Load(const std::string &path,bool absolute) {
		auto _path=std::filesystem::path(path);
		if(!absolute){
			_path=instance->ProjectPath/path;
		}
		if(_path.extension()!=Z_CONF_EXTENSION)
			_path+=Z_CONF_EXTENSION;
		if(IsExisting(_path.string())){
			return instance->MeshLibrary.at(instance->PathToUID[_path.string()]);
		}
		auto pathSTR=_path.string();
		auto id=zGUID();
		instance->PathToUID[pathSTR]=id;
		instance->UIDToPath[id]=pathSTR;
		//TODO:Optimized 6 with sizeof(Z_CONF_EXTENSION)
		auto mesh=Tools::LoadMesh(pathSTR.substr(0,pathSTR.size()-6));
		auto filename=_path.filename().string();
		mesh->name=filename.substr(0,filename.rfind('.',filename.size()-7));
		instance->MeshLibrary[id]=mesh;
		return mesh;
	}

}

