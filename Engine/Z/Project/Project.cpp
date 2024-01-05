//
// Created by 32725 on 2023/6/1.
//

#include "Include/yaml-cpp/include/yaml-cpp/yaml.h"

#include "Z/Project/Project.h"
#include "Z/Core/Log.h"

namespace Z {

	Ref<Project> Project::project= nullptr;
    bool Project::p_Inited = false;
	bool Project::Init(std::filesystem::path &projectFile) {
		YAML::Node data;
		try{
			 data= YAML::LoadFile(projectFile.string());
		}catch (std::exception& e){
			Z_CORE_ERROR("Error:{0}",e.what());
			Z_CORE_ERROR("Project::Init:Failed to load project file:{0}",projectFile.string());
			return false;
		}
		auto nodeData=data["Project"];
		if(!nodeData){
			Z_CORE_ERROR("Project::Init:Failed to load project file:{0}",projectFile.string());
			return false;
		}
		project = CreateRef<Project>();
		auto&configure=project->configure;
		configure.ProjectFile = projectFile;
		configure.ProjectRootDir = projectFile.parent_path();
		configure.StartScene = nodeData["StartScene"].as<std::string>();
		configure.AssetsDir = nodeData["AssetsDirectory"].as<std::string>();
		configure.ScriptsDir = nodeData["ScriptsDirectory"].as<std::string>();
		configure.editorLayout = configure.ProjectRootDir/nodeData["EditorLayout"].as<std::string>();
		Z_CORE_WARN(configure.editorLayout);
		if (nodeData["ProjectName"])
			configure.ProjectName = nodeData["ProjectName"].as<std::string>();
		else
			configure.ProjectName = projectFile.filename().string().substr(0, projectFile.filename().string().find_last_of('.'));
        p_Inited=true;
		return true;
	}

	void Project::Save() {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project";
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectName" << YAML::Value << project->configure.ProjectName;
		out << YAML::Key << "StartScene" << YAML::Value << project->configure.StartScene.string();
		out << YAML::Key << "AssetsDirectory" << YAML::Value << project->configure.AssetsDir.string();
		out << YAML::Key << "ScriptsDirectory" << YAML::Value << project->configure.ScriptsDir.string();
		out << YAML::EndMap;
		out << YAML::EndMap;
		std::ofstream fout(project->configure.ProjectFile.string());
		fout << out.c_str();
		fout.close();
	}
}