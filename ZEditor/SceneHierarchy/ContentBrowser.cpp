//
// Created by 32725 on 2023/4/2.
//

#include "Z/Core/Log.h"
#include "ContentBrowser.h"

namespace Z {
	//constexpr char *RootPath = Z_SOURCE_DIR;
	const std::filesystem::path RootPath("./");

	ContentBrowser::ContentBrowser() : currentPath(RootPath) {
		loadIcons({"Assets/Icons/DirectoryIcon.png", "Assets/Icons/FileIcon.png"});
	}

	void ContentBrowser::OnImGuiRender() {
		ImGui::Begin("Content Browser");
		auto [x, y] = ImGui::GetWindowSize();
		int width = 128;
		int columns = std::max(int(x / width), 1);
		ImGui::Columns(columns, 0, false);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		if (currentPath != RootPath) {
			ImGui::ImageButton((ImTextureID) icons[0]->GetRendererID(),
			                   ImVec2{static_cast<float>(width), static_cast<float>(width)});
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				currentPath = currentPath.has_parent_path() ? currentPath.parent_path() : currentPath;
			}
			ImGui::TextWrapped("../");
			ImGui::NextColumn();
		}

		for (const auto &file: std::filesystem::directory_iterator(currentPath)) {
			ImGui::PushID(file.path().string().c_str());
			int index = file.is_directory() ? 0 : 1;
			ImGui::ImageButton(ImTextureID(icons[index]->GetRendererID()),
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
			icons[i++] = Texture2D::CreateTexture(path);
		}
	}
}