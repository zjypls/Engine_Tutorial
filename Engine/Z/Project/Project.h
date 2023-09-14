//
// Created by 32725 on 2023/6/1.
//

#ifndef ENGINE_TUTORIAL_PROJECT_H
#define ENGINE_TUTORIAL_PROJECT_H
#include <filesystem>

#include "Z/Core/Core.h"

namespace Z {
	struct ProjectConfigure{
		std::filesystem::path ProjectFile;
		std::filesystem::path ProjectRootDir;
		std::filesystem::path StartScene;
		std::filesystem::path editorLayout;
		//Todo:change to AssetsSystem
		std::filesystem::path AssetsDir;
		std::filesystem::path ScriptsDir;
		std::string ProjectName;
	};
	class Project {
	public:
		static bool Init(std::filesystem::path &projectFile);
		static void Save();
		static auto GetProject() { return project; }
		static std::filesystem::path GetProjectRootDir() { return project->configure.ProjectRootDir; }
		static std::filesystem::path GetStartScene() { return project->configure.StartScene; }
		static std::filesystem::path GetAssetsDir() { return project->configure.AssetsDir; }
		static std::filesystem::path GetScriptsDir() { return project->configure.ScriptsDir; }
		static std::string GetProjectName() { return project->configure.ProjectName; }
		static std::filesystem::path GetProjectFile() { return project->configure.ProjectFile; }
		static ProjectConfigure GetConfigure() { return project->configure; }
		static const std::filesystem::path& GetEditorLayoutConfiguration(){return project->configure.editorLayout;}
	private:
		ProjectConfigure configure;
		static Ref<Project> project;
	};

}

#endif //ENGINE_TUTORIAL_PROJECT_H
