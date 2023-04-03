//
// Created by 32725 on 2023/3/28.
//

#include "SceneHierarchyPlane.h"
#include "Z/Scene/Components.h"
#include <filesystem>
#include "imgui/imgui.h"
#include "imgui_internal.h"

namespace Z {

	static void
	MyDrawVec(const std::string &label, glm::vec3 &value, float ResetValue = .0f, float ColumnWidth = 100.f) {
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, {.0f, 0, .5f, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.0f, 0, 1.f, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.0f, 0, .5f, 1});
		ImGui::PushStyleColor(ImGuiCol_Button, {.0f, .5f, .0f, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.0f, 1.f, .0f, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.0f, .5f, .0f, 1});
		ImGui::PushStyleColor(ImGuiCol_Button, {.5f, 0, 0, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1.f, 0, 0, 1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.5f, 0, 0, 1});
		static const char *ButtonLabel[] = {"X", "Y", "Z"};
		static const char *DragLabel[] = {"##X", "##Y", "##Z"};

		auto &io = ImGui::GetIO();
		ImGui::PushFont(io.Fonts->Fonts[0]);
		for (int i = 0; i < 3; ++i) {
			if (ImGui::Button(ButtonLabel[i], buttonSize)) { value[i] = ResetValue; }
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::DragFloat(DragLabel[i], &value[i], 0.1f);
			if (i != 2)
				ImGui::SameLine();
			ImGui::PopItemWidth();
		}
		ImGui::PopFont();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void SceneHierarchyPlane::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
		context->registry.each([&](auto entityID) {
			Entity entity{entityID, context.get()};
			DrawEntity(entity);
		});
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			selectedEntity = {};
		}
		if (ImGui::BeginPopupContextWindow(nullptr,
		                                   ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				context->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}
		ImGui::End();

		ImGui::Begin("Inspector");

		if (selectedEntity) {
			DrawComponents(selectedEntity);


		}
		ImGui::End();
	}

	void SceneHierarchyPlane::DrawEntity(Entity entity) {
		auto &tag = entity.GetComponent<TagComponent>().tag;
		ImGuiTreeNodeFlags flags =
				((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void *) (uint64_t) (uint32_t) entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			selectedEntity = entity;
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity")) {
				if (selectedEntity == entity)
					selectedEntity = {};
				context->DestroyEntity(entity);
			}
			ImGui::EndPopup();
		}

		if (opened) {
			ImGui::TreePop();
		}
	}


	template<>
	void SceneHierarchyPlane::DrawComponent<TagComponent>(const std::string &name, Entity entity,
	                                                      void (*drawFunc)(TagComponent &)) {
		auto &component = entity.GetComponent<TagComponent>();
		drawFunc(component);
	}

	template<>
	void SceneHierarchyPlane::DrawComponent<TransformComponent>(const std::string &name, Entity entity,
	                                                            void (*drawFunc)(TransformComponent &)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void *) typeid(TransformComponent).hash_code(),
		                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
		                              ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth,
		                              name.c_str());
		ImGui::PopStyleVar();

		if (open) {
			auto &component = entity.GetComponent<TransformComponent>();
			drawFunc(component);
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPlane::DrawComponents(Entity entity) {
		DrawComponent<TagComponent>("Tag", entity, [](auto &tag) {
			static char buffer[256];
			memset(buffer, 0, 256);
			strcpy(buffer, tag.tag.c_str());
			if (ImGui::InputText("##Tag", &buffer[0], 256))
				tag = std::string(buffer);
		});

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent")) {//Todo:Change logic
			if (ImGui::MenuItem("Camera")) {
				selectedEntity.AddComponent<CameraComponent>();//Todo:Change logic
			}
			if (ImGui::MenuItem("Sprite Renderer")) {
				selectedEntity.AddComponent<SpriteRendererComponent>();//Todo:Change logic
			}
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto &transform) {
			MyDrawVec("Position", transform.translation);
			auto rotation = glm::degrees(transform.rotation);
			MyDrawVec("Rotation", rotation);
			transform.rotation = glm::radians(rotation);
			MyDrawVec("Scale", transform.scale, 1.f);
		});
		DrawComponent<SpriteRendererComponent>("SpriteRenderer", entity,
		                                       [](SpriteRendererComponent &spriteRenderer) {
			                                       ImGui::ColorEdit4("Color", &spriteRenderer.color[0]);
												   ImGui::Image(spriteRenderer.texture.get()== nullptr ? nullptr :
														   (void *) spriteRenderer.texture->GetRendererID(),
																ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
												   if(ImGui::BeginDragDropTarget()){
													   if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")){
														   const char* path = (const char*)payload->Data;
														   Z_CORE_ASSERT(std::filesystem::exists(path), "Path does not exist!");
														   spriteRenderer.texture = Texture2D::CreateTexture(path);
													   }
													   ImGui::EndDragDropTarget();
												   }
		                                       });
		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent &component) {
			auto &camera = component.camera;
			if (ImGui::Checkbox("Primary", &component.primary)) {
//				context->registry.view<CameraComponent>().each([&](auto entity, auto &cameraComponent) {
//					if (entity != entity)cameraComponent.primary = !component.primary;
//				});
			}
			const char *projectionTypeStrings[] = {"Perspective", "Orthographic"};
			int projectionType = (int) camera.GetProjectionType();

			if (ImGui::Combo("Projection", &projectionType, projectionTypeStrings, 2)) {
				if ((int) (camera.GetProjectionType()) != projectionType) {
					camera.SetProjectionType((SceneCamera::ProjectionType) projectionType);
				}
			}
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float verticalFOV = glm::degrees(camera.GetPerspectiveFOV());
				if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) {
					camera.SetPerspectiveFOV(glm::radians(verticalFOV));
				}
				float nearClip = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &nearClip)) {
					camera.SetPerspectiveNearClip(nearClip);
				}
				float farClip = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &farClip)) {
					camera.SetPerspectiveFarClip(farClip);
				}
			} else {
				float orthographicSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthographicSize)) {
					camera.SetOrthographicSize(orthographicSize);
				}
				float nearClip = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &nearClip)) {
					camera.SetOrthographicNearClip(nearClip);
				}
				float farClip = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &farClip)) {
					camera.SetOrthographicFarClip(farClip);
				}
			}
		});
	}

	template<typename _Ty>
	void SceneHierarchyPlane::DrawComponent(const std::string &name, Entity entity, void (*drawFunc)(_Ty &)) {
		if (entity.HasComponent<_Ty>()) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			auto RegionWidth = ImGui::GetContentRegionAvail().x;
			auto lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void *) typeid(_Ty).hash_code(),
			                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			                              ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth,
			                              name.c_str());
			ImGui::PopStyleVar();
			bool remove = false;

			ImGui::SameLine(RegionWidth- lineHeight * .5f);
			if (ImGui::Button("+", ImVec2(lineHeight, lineHeight))) {
				ImGui::OpenPopup("Remove Component");
			}
			//ImGui::PopStyleVar();
			if (ImGui::BeginPopup("Remove Component")) {
				if (ImGui::MenuItem("Remove Component")) {
					remove = true;
				}
				ImGui::EndPopup();
			}
			if (open) {
				auto &component = entity.GetComponent<_Ty>();
				drawFunc(component);
				ImGui::TreePop();
			}
			if (remove) {
				entity.RemoveComponent<_Ty>();
			}
		}
	}

}



