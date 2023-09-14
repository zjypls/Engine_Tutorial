//
// Created by 32725 on 2023/6/1.
//

#include <set>

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Scene/Components.h"



namespace Z {

	static const std::set<std::string> registerTypes{".png",".jpg",".jpeg",".bmp"};

	Scope<AssetsSystem> AssetsSystem::instance;

	auto AssetsSystem::LoadTextureInner(const std::string &path) {
		return TextureLibrary.insert({path,Texture2D::CreateTexture(path)});
	}

	Ref<Texture2D> AssetsSystem::LoadTexture(const std::string &path,bool absolute) {
		if(IsExisting(path)){
			return instance->TextureLibrary.at(path);
		}
		auto _path=path;
		if(!absolute){
			_path=(instance->ProjectPath/path).string();
		}
		auto result=instance->LoadTextureInner(_path);
		Z_CORE_ASSERT(result.second,std::string("failed to load texture:").append(_path));
		return result.first->second;
	}

	void AssetsSystem::Init(const std::filesystem::path &projectPath) {
		if(instance)
			return;
		Z_CORE_ASSERT(!projectPath.empty(), "Empty Path Init AssetsSystem Is Illegal!!!");
		instance= CreateScope<AssetsSystem>();
		instance->ProjectPath=projectPath;
		std::filesystem::recursive_directory_iterator _it(projectPath);
		for(;_it!=std::filesystem::recursive_directory_iterator();++_it){
			if(_it->is_regular_file()){
				if(auto type=registerTypes.find(_it->path().extension().string());type!=registerTypes.end()){
					instance->TextureLibrary.insert({_it->path().string(),Texture2D::CreateTexture(_it->path().string())});
				}
			}
		}
	}


	void AssetsSystem::PushResource(Ref<Texture2D> &texture,const std::string&name) {
		TextureLibrary[name]=texture;
	}

	Ref<Texture2D> AssetsSystem::Get(const std::string &name) {
		if(IsExisting(name))
			return instance->TextureLibrary.at(name);
		else
			return nullptr;
	}
}