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
#include "Include/assimp/include/assimp/Importer.hpp"
#include "Include/assimp/include/assimp/postprocess.h"
#include "Include/assimp/include/assimp/scene.h"

namespace Z{
	static GraphicInterface* s_Context=nullptr;
    const std::unordered_map<std::string,std::string> AssetsSystem::easyNameMap{
            {"diffuse",AssetsSystem::DefaultDiffusePath},
            {"normal",AssetsSystem::DefaultNormalTexture},
            {"emission",AssetsSystem::DefaultEmissionTexture},
            {"specular",AssetsSystem::DefaultSpecularTexture},
            {"metallic",AssetsSystem::DefaultMetallicTexture},
            {"roughness",AssetsSystem::DefaultRoughnessTexture}
    };
	template<typename T>
	using Container=std::vector<T>;

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
			return type!=Z::AssetsImporterType::None;
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

		static const std::set<std::string> TextureSheets{".png",".jpg",".jpeg",".bmp",".hdr"};
		static const std::set<std::string> MeshSheets{".obj",".fbx",".dae",".pmx"};

		Z::AssetsImporterType ExtensionGetType(const std::string&extension){
			if(TextureSheets.find(extension)!=TextureSheets.end())
				return Z::AssetsImporterType::Texture2D;
			else if(MeshSheets.find(extension)!=MeshSheets.end())
				return Z::AssetsImporterType::Mesh;
			else if(extension==".glsl")
				return Z::AssetsImporterType::Shader;
			Z_CORE_ASSERT(false,"Unknown extension type : {0}",extension);
			return Z::AssetsImporterType::None;
		}

		void* LoadSkyBox(const std::string& directory){
            // to be optimized
			const std::vector path{
				directory+"/right.jpg",
				directory+"/left.jpg",
				directory+"/top.jpg",
				directory+"/bottom.jpg",
				directory+"/front.jpg",
				directory+"/back.jpg"
			};
			std::array<void*,6> pixelsData{};
			int width,height,format;
			for(int i=0;i<6;i++){
				auto pixelData=stbi_load(path[i].c_str(),&width,&height,&format,STBI_rgb_alpha);
				if(!pixelData){
					Z_CORE_ERROR("Failed to load image: path: {0} , message : {1}",path[i],stbi_failure_reason());
					for(int j=0;j<i;j++){
						stbi_image_free(pixelsData[j]);
					}
					return nullptr;
				}
				pixelsData[i]=pixelData;
			}
			auto skybox=new Z::Skybox();
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
            void* pixelData;
            auto extension = path.substr(path.rfind('.'));
            Z::Format imageFormat=Z::Format::R8G8B8A8_UNORM;
            // to be improved
            if(extension==".hdr") {
                pixelData = stbi_loadf(path.c_str(), &width, &height, &format, STBI_rgb_alpha);
                imageFormat=Z::Format::R32G32B32A32_SFLOAT;
            }
            else
                pixelData=stbi_load(path.c_str(),&width,&height,&format,STBI_rgb_alpha);
			if(!pixelData){
				Z_CORE_ERROR("Failed to load image: path: {0} , message : {1}",path,stbi_failure_reason());
				return nullptr;
			}
			auto texture=new Z::Texture2D();
			Z::ImageInfo imageInfo{};
			imageInfo.format=imageFormat;
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
            texture->width=width;
            texture->height=height;
			return texture;
		}

		void ProcessMesh(aiMesh* mesh,const aiScene* scene , Container<Vertex>& vertices , Container<VertexBlending>&blending ,
                         std::map<std::string,BoneInfo>&bones, std::vector<uint32>& indices){
			auto vertexNums=vertices.size();
			vertices.resize(vertexNums+mesh->mNumVertices);
			for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex{};
				vertex.position.x = mesh->mVertices[i].x;
				vertex.position.y = mesh->mVertices[i].y;
				vertex.position.z = mesh->mVertices[i].z;
				if (mesh->mTextureCoords[0]) {
					vertex.uv.x = mesh->mTextureCoords[0][i].x;
					vertex.uv.y = mesh->mTextureCoords[0][i].y;
				}
				else {
					vertex.uv = { 0.0f,0.0f };
				}
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
				if (mesh->mTangents) {
					vertex.tangent.x = mesh->mTangents[i].x;
					vertex.tangent.y = mesh->mTangents[i].y;
					vertex.tangent.z = mesh->mTangents[i].z;
				}
				vertices[vertexNums+i]=vertex;
			}
			blending.resize(vertexNums+mesh->mNumVertices);
			auto boneNums=bones.size();
            int32 index=0;
			for (unsigned int i = 0; i < mesh->mNumBones; i++) {
				aiBone* bone = mesh->mBones[i];
                std::string boneName = bone->mName.C_Str();
                auto id=boneNums + index;
                if(bones.find(boneName)==bones.end()){
                    BoneInfo boneInfo{};
                    boneInfo.id=index+boneNums;
                    boneInfo.offset= AssimpMatrixToGLM(bone->mOffsetMatrix);
                    bones[boneName]=boneInfo;
                    ++index;
                }else{
                    id=bones[boneName].id;
                }
				for (unsigned int j = 0; j < bone->mNumWeights; j++) {
					aiVertexWeight weight = bone->mWeights[j];
					auto& vertex = blending[weight.mVertexId+vertexNums];
					for (int k = 0; k < 4; ++k) {
						if (vertex.boneIndex[k] < 0) {
							vertex.boneIndex[k] = id;  // bone index
							vertex.blending[k] = weight.mWeight;  // bone weight
							break;
						}
					}
				}
			}
			for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				for (uint32_t j = 0; j < face.mNumIndices; j++) {
					indices.push_back(face.mIndices[j]);
				}
			}
		}

		void ProcessNode(aiNode *node, const aiScene *scene , Container<Vertex>& vertices , Container<VertexBlending>& blending ,
                         std::map<std::string,BoneInfo>& bones, std::vector<uint32>& indices) {
			for (uint32_t i = 0; i < node->mNumMeshes; i++) {
				aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
				ProcessMesh(mesh, scene, vertices , blending,bones, indices);
			}
			for (uint32_t i = 0; i < node->mNumChildren; i++) {
				ProcessNode(node->mChildren[i], scene,vertices,blending ,bones,indices);
			}
		}

		void* LoadMesh(const std::string& path){
			Assimp::Importer importer;
			auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs|
                                        aiProcess_GenNormals|aiProcess_CalcTangentSpace);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				Z_CORE_ERROR("Assimp error: {0}", importer.GetErrorString());
				return nullptr;
			}
			auto mesh=new Z::MeshRes();
			mesh->vertexBuffer=new Buffer*[mesh->vertexBufferCount];
			mesh->vertexMemory=new DeviceMemory*[mesh->vertexBufferCount];
			Container<Vertex> vertices{};
			Container<VertexBlending> blending{};
            std::map<std::string,BoneInfo> boneMap;
			std::vector<uint32> indices{};
			ProcessNode(scene->mRootNode, scene,vertices,blending,boneMap,indices);
			BufferInfo vertexBufferInfo{};
			vertexBufferInfo.properties=MemoryPropertyFlag::DEVICE_LOCAL;
			vertexBufferInfo.size=sizeof(Vertex)*vertices.size();
			vertexBufferInfo.usage=BufferUsageFlag::VERTEX_BUFFER;
			s_Context->CreateBuffer(vertexBufferInfo,mesh->vertexBuffer[0],mesh->vertexMemory[0],vertices.data());
			vertexBufferInfo.size=sizeof(VertexBlending)*blending.size();
			s_Context->CreateBuffer(vertexBufferInfo,mesh->vertexBuffer[1],mesh->vertexMemory[1],blending.data());
			BufferInfo indexBufferInfo{};
			indexBufferInfo.size=sizeof(uint32)*indices.size();
			indexBufferInfo.usage=BufferUsageFlag::INDEX_BUFFER;
			indexBufferInfo.properties=MemoryPropertyFlag::DEVICE_LOCAL;
			s_Context->CreateBuffer(indexBufferInfo,mesh->indexBuffer,mesh->indexMemory,indices.data());
			if(!boneMap.empty()){
                mesh->animator=new Animator(scene,boneMap);
                mesh->animator->Update(0);
				BufferInfo boneBufferInfo{};
				boneBufferInfo.size=sizeof(glm::mat4)*boneMap.size();
				boneBufferInfo.usage=BufferUsageFlag::STORAGE_BUFFER;
				boneBufferInfo.properties=MemoryPropertyFlag::HOST_VISIBLE|MemoryPropertyFlag::HOST_COHERENT;
				s_Context->CreateBuffer(boneBufferInfo,mesh->boneBuffer,mesh->boneMemory,mesh->animator->GetMatrices().data());
			}
			mesh->indicesCount=indices.size();
			mesh->path=path;
			mesh->type=AssetsImporterType::Mesh;
			return mesh;
		}

		std::vector<ShaderStageFlag> CountShaderStages(const std::string& source){
			std::vector<ShaderStageFlag> stages{};
			if(source.find("Z_VERTEX")!=std::string::npos)
				stages.push_back(ShaderStageFlag::VERTEX);
			if(source.find("Z_FRAGMENT")!=std::string::npos)
				stages.push_back(ShaderStageFlag::FRAGMENT);
			if(source.find("Z_GEOMETRY")!=std::string::npos)
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
			delete sky;
		}

		void DestroyTexture2D(void*texture){
			auto tex=static_cast<Z::Texture2D*>(texture);
			s_Context->DestroyImage(tex->image,tex->memory,tex->imageView);
			delete tex;
		}

		void DestroyMesh(void*mesh){
			auto m=static_cast<Z::MeshRes*>(mesh);
			for(int i=0;i<m->vertexBufferCount;++i){
				s_Context->DestroyBuffer(m->vertexBuffer[i],m->vertexMemory[i]);
			}
			s_Context->DestroyBuffer(m->indexBuffer,m->indexMemory);
			if(m->boneBuffer!=nullptr){
				s_Context->DestroyBuffer(m->boneBuffer,m->boneMemory);
                m->animator->Clear();
                delete m->animator;
			}
			delete m;
		}


	}

	static const std::set<std::string> registerTypes{".png",".jpg",".jpeg",".bmp",".obj",".fbx",".dae"};
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
		InitInnerAssets();
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
				break;
			}
			case AssetsImporterType::Shader:
				ptr=Tools::LoadShader(path.string());
				break;
			case AssetsImporterType::None:
				return nullptr;
				break;
		}
		if(ptr)
			instance->resourceLibrary[metaData.id]=ptr;
		return ptr;
	}

	void AssetsSystem::InitInnerAssets(){
		ImageInfo imageInfo{};
		imageInfo.extent.width=1;
		imageInfo.extent.height=1;
		imageInfo.extent.depth=1;
		imageInfo.arrayLayers=1;
		imageInfo.format=Format::R8G8B8A8_UNORM;
		imageInfo.initialLayout=ImageLayout::UNDEFINED;
		imageInfo.tilling=ImageTiling::OPTIMAL;
		imageInfo.usageFlag=ImageUsageFlag::SAMPLED|ImageUsageFlag::TRANSFER_DST;
		imageInfo.mipMapLevels=1;
		imageInfo.memoryPropertyFlag=MemoryPropertyFlag::DEVICE_LOCAL;
		imageInfo.sampleCount=SampleCountFlagBits::e1_BIT;

		auto whiteTexture=new Texture2D;
		uint8 whitePixel[]={255,255,255,255};
		instance->Context->CreateImage(imageInfo,whiteTexture->image,whiteTexture->memory,whiteTexture->imageView,whitePixel);

		whiteTexture->path=DefaultWhiteTexture;
		zGUID whiteID{};
		instance->PathToUID[DefaultWhiteTexture]=whiteID;
		instance->UIDToPath[whiteID]=DefaultWhiteTexture;
		instance->resourceLibrary[whiteID]=whiteTexture;

		auto blackTexture=new Texture2D;
		uint8 blackPixel[]={0,0,0,0};
		instance->Context->CreateImage(imageInfo,blackTexture->image,blackTexture->memory,blackTexture->imageView,blackPixel);

		zGUID blackID{};
		blackTexture->path=DefaultBlackTexture;
		instance->PathToUID[DefaultBlackTexture]=blackID;
		instance->UIDToPath[blackID]=DefaultBlackTexture;
		instance->resourceLibrary[blackID]=blackTexture;

        auto defaultDiffuse=new Texture2D;
        uint8 diffusePixel[]{255,255,255,255};
        instance->Context->CreateImage(imageInfo,defaultDiffuse->image,defaultDiffuse->memory,defaultDiffuse->imageView,diffusePixel);

        zGUID diffuseID{};
        defaultDiffuse->path=DefaultDiffusePath;
        instance->PathToUID[DefaultDiffusePath]=diffuseID;
        instance->UIDToPath[diffuseID]=DefaultDiffusePath;
        instance->resourceLibrary[diffuseID]=defaultDiffuse;

        auto defaultNormal=new Texture2D;
        uint8 normalPixel[]={255/2,255/2,255,255};
        instance->Context->CreateImage(imageInfo,defaultNormal->image,defaultNormal->memory,defaultNormal->imageView,normalPixel);

        zGUID normalID{};
        defaultNormal->path=DefaultNormalTexture;
        instance->PathToUID[DefaultNormalTexture]=normalID;
        instance->UIDToPath[normalID]=DefaultNormalTexture;
        instance->resourceLibrary[normalID]=defaultNormal;


        auto defaultEmission=new Texture2D;
        uint8 emissionPixel[]={0,0,0,255};
        instance->Context->CreateImage(imageInfo,defaultEmission->image,defaultEmission->memory,defaultEmission->imageView,emissionPixel);

        zGUID emissionID{};
        defaultEmission->path=DefaultEmissionTexture;
        instance->PathToUID[DefaultEmissionTexture]=emissionID;
        instance->UIDToPath[emissionID]=DefaultEmissionTexture;
        instance->resourceLibrary[emissionID]=defaultEmission;


        auto defaultSpecular=new Texture2D;
        uint8 specularPixel[]={0,0,0,255};
        instance->Context->CreateImage(imageInfo,defaultSpecular->image,defaultSpecular->memory,defaultSpecular->imageView,specularPixel);

        zGUID specularID{};
        defaultSpecular->path=DefaultSpecularTexture;
        instance->PathToUID[DefaultSpecularTexture]=specularID;
        instance->UIDToPath[specularID]=DefaultSpecularTexture;
        instance->resourceLibrary[specularID]=defaultSpecular;

		auto defaultMetallic = new Texture2D;
		uint8 metallicPixel[]={0};
		imageInfo.format=Format::R8_UNORM;
		instance->Context->CreateImage(imageInfo,defaultMetallic->image,defaultMetallic->memory,defaultMetallic->imageView,metallicPixel);

		zGUID metallicID{};
		defaultMetallic->path=DefaultMetallicTexture;
		instance->PathToUID[DefaultMetallicTexture]=metallicID;
		instance->UIDToPath[metallicID]=DefaultMetallicTexture;
		instance->resourceLibrary[metallicID]=defaultMetallic;


		auto defaultRoughness = new Texture2D;
		uint8 roughnessPixel[]={0};
		instance->Context->CreateImage(imageInfo,defaultRoughness->image,defaultRoughness->memory,defaultRoughness->imageView,roughnessPixel);

		zGUID roughnessID{};
		defaultRoughness->path=DefaultRoughnessTexture;
		instance->PathToUID[DefaultRoughnessTexture]=roughnessID;
		instance->UIDToPath[roughnessID]=DefaultRoughnessTexture;
		instance->resourceLibrary[roughnessID]=defaultRoughness;

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
			case AssetsImporterType::Texture2D:
				instance->resourceLibrary[data.id]=Tools::LoadTexture2D(path);
				break;
			case AssetsImporterType::Mesh:
				instance->resourceLibrary[data.id]=Tools::LoadMesh(path);
				break;
			case AssetsImporterType::Material:
				break;
			case AssetsImporterType::SkyBox:
				instance->resourceLibrary[data.id]=Tools::LoadSkyBox(path);
				break;
			case AssetsImporterType::Shader:
				instance->resourceLibrary[data.id]=Tools::LoadShader(path);
				break;
			case AssetsImporterType::None:
				break;
		}
	}

}

