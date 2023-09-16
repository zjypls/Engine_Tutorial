//
// Created by 32725 on 2023/4/2.
//

#include "Z/Core/Log.h"
#include "Z/Project/Project.h"
#include "Z/Core/AssetsSystem.h"

#include "ContentBrowser.h"

namespace Z {
	//constexpr char *RootPath = Z_SOURCE_DIR;
	const std::filesystem::path RootPath("./");

	ContentBrowser::ContentBrowser() : currentPath(Project::GetProjectRootDir()) {
		loadIcons({ZSTRCAT(Z_SOURCE_DIR, "Assets/Icons/DirectoryIcon.png"),
		           ZSTRCAT(Z_SOURCE_DIR, "Assets/Icons/FileIcon.png")});
	}

	void ContentBrowser::OnImGuiRender() {
		ImGui::Begin("Content Browser");
		auto [x, y] = ImGui::GetWindowSize();
		int width = 128;
		int columns = std::max(int(x / width), 1);
		ImGui::Columns(columns, nullptr, false);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (currentPath != Project::GetProjectRootDir()) {
			ImGui::ImageButton((ImTextureID) icons[0]->GetRendererID(),
			                   ImVec2{static_cast<float>(width), static_cast<float>(width)},
			                   {0, 1}, {1, 0});
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				currentPath = currentPath.has_parent_path() ? currentPath.parent_path() : currentPath;
			}
			ImGui::TextWrapped("../");
			ImGui::NextColumn();
		}

		for (const auto &file: std::filesystem::directory_iterator(currentPath)) {
			if(file.is_regular_file()&&(file.path().extension()!=".zConf"))
				continue;
			ImGui::PushID(file.path().string().c_str());
			auto texture=AssetsSystem::Get(file.path().string());
			unsigned int id=file.is_directory()?icons[0]->GetRendererID():(
					texture== nullptr?icons[1]->GetRendererID():texture->GetRendererID());
			ImGui::ImageButton(ImTextureID(id),
			                   ImVec2{static_cast<float>(width), static_cast<float>(width)}, {0, 1}, {1, 0});
			if (ImGui::BeginDragDropSource()) {
				const auto &filePath = file.path().string();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", filePath.c_str(), filePath.size() + 1,
				                          ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (file.is_directory()) {
					currentPath = file.path();
				}
			}
			ImGui::TextWrapped(file.path().filename().string().c_str());
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
			icons[i++] = AssetsSystem::Load<Texture>(path, true);//Texture2D::CreateTexture(path);
		}
	}
}