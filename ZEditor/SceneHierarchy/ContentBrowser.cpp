//
// Created by 32725 on 2023/4/2.
//

#include "ContentBrowser.h"

namespace Z {

	static const std::string zDefaultFileIconIndex = "Z_DefaultFileIcon";
	static const std::string zDirectoryIconIndex = "Z_DirectoryIcon";
	ContentBrowser::ContentBrowser() : currentPath(Project::IsInited()?Project::GetProjectRootDir():"") {
		loadIcons({"Assets/Icons/DirectoryIcon.png",
		           "Assets/Icons/FileIcon.png"
				   });
	}

	void ContentBrowser::OnImGuiRender() {
		ImGui::Begin("Content Browser");
        if(currentPath.empty()){
            ImGui::End();
            return;
        }
		auto [x, y] = ImGui::GetWindowSize();
		float width = 128;
		int columns = std::max(int(x / width), 1);
		ImGui::Columns(columns, nullptr, false);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (currentPath != Project::GetProjectRootDir()) {
			ImGui::ImageButton(icons[0],ImVec2{width, width});
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				currentPath = currentPath.has_parent_path() ? currentPath.parent_path() : currentPath;
			}
			ImGui::TextWrapped("..");
			ImGui::NextColumn();
		}

		for (const auto &file: std::filesystem::directory_iterator(currentPath)) {
			bool IsFile=file.is_regular_file();
			if(IsFile&&(file.path().extension()!=Z_CONF_EXTENSION))
				continue;
			std::string filePath=file.path().string();
			ImGui::PushID(filePath.c_str());
			auto texture = icons[0];
			if(IsFile){
				if(auto tex= textureMap.find(file.path());tex!=textureMap.end()){
					texture=tex->second;
				}else{
					auto result=AssetsSystem::Load<Texture2D>(filePath);
					if(result && result->type==AssetsImporterType::Texture2D){
						texture=RenderManager::CreateImGuiTexture(result);
						textureMap[file.path()]=texture;
					}else{
						texture=icons[1];
					}
				}
			}
			ImGui::ImageButton(texture, ImVec2(width, width));
			if (IsFile&&ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filePath.c_str(), filePath.size() + 1,
				                          ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (!IsFile) {
					currentPath = file.path();
				}
			}
			auto firstPos= filePath.find_last_of(Z_SEPARATOR) + 1;
			ImGui::TextWrapped(filePath.substr(firstPos,filePath.find_last_of('.')-firstPos).c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void ContentBrowser::loadIcons(const std::vector<std::string> &paths) {
		auto dirIcon = RenderManager::CreateImGuiTexture(AssetsSystem::Load<Texture2D>(paths[eDirectoryIcon]));
		icons[0]=dirIcon;
		textureMap[zDirectoryIconIndex] = dirIcon;
		auto fileIcon = RenderManager::CreateImGuiTexture(AssetsSystem::Load<Texture2D>(paths[eFileIcon]));
		icons[1]=fileIcon;
		textureMap[zDefaultFileIconIndex] = fileIcon;
	}
}