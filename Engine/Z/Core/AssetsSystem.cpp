//
// Created by 32725 on 2023/6/1.
//

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Scene/Components.h"



namespace Z {

	Scope<AssetsSystem> AssetsSystem::instance;
	Ref<Texture2D> AssetsSystem::LoadTexture(const std::string &path,bool absolute) {
		if(IsExisting(path)){
			return instance->TextureLibrary.at(path);
		}
		auto _path=path;
		if(!absolute){
			_path=(instance->ProjectPath/path).string();
		}
		auto texture= Texture2D::CreateTexture(path);
		instance->TextureLibrary[_path]=texture;
		return texture;
	}

	void AssetsSystem::Init(const std::filesystem::path &projectPath) {
		if(instance)
			return;
		Z_CORE_ASSERT(!projectPath.empty(), "Empty Path Init AssetsSystem Is Illegal!!!");
		instance= CreateScope<AssetsSystem>();
		instance->ProjectPath=projectPath;
	}

	void AssetsSystem::LoadTextureInner(const std::string &path) {
		TextureLibrary.insert({path,Texture2D::CreateTexture(path)});
	}

	void AssetsSystem::PushResource(Ref<Texture2D> &texture,const std::string&name) {
		TextureLibrary[name]=texture;
	}
}