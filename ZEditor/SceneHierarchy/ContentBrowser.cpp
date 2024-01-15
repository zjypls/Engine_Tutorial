//
// Created by 32725 on 2023/4/2.
//

#include "Z/Core/Log.h"
#include "Z/Project/Project.h"
#include "Z/Core/AssetsSystem.h"

#include "ContentBrowser.h"

namespace Z {

	ContentBrowser::ContentBrowser() : currentPath(Project::IsInited()?Project::GetProjectRootDir():"") {
		loadIcons({ROOT_PATH + "Assets/Icons/DirectoryIcon.png",
		           ROOT_PATH + "Assets/Icons/FileIcon.png"});
	}

	void ContentBrowser::OnImGuiRender() {
		ImGui::Begin("Content Browser");
        if(currentPath.empty()){
            ImGui::End();
            return;
        }
		auto [x, y] = ImGui::GetWindowSize();
		int width = 128;
		int columns = std::max(int(x / width), 1);
		ImGui::Columns(columns, nullptr, false);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (currentPath != Project::GetProjectRootDir()) {
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				currentPath = currentPath.has_parent_path() ? currentPath.parent_path() : currentPath;
			}
			ImGui::TextWrapped("..");
			ImGui::NextColumn();
		}

		for (const auto &file: std::filesystem::directory_iterator(currentPath)) {
			bool IsFile=file.is_regular_file();
			if(IsFile&&(file.path().extension()!=".zConf"))
				continue;
			std::string filePath=file.path().string();
			ImGui::PushID(filePath.c_str());
			if(IsFile){
			}
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

	void ContentBrowser::loadIcons(const std::initializer_list<std::string> &paths) {
		Z_CORE_ASSERT(paths.size() == 2, "Icon size must be 2");
		int i = 0;
		for (const auto &path: paths) {
		}
	}
}