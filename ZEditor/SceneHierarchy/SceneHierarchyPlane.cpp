//
// Created by 32725 on 2023/3/28.
//

#include "SceneHierarchyPlane.h"
#include "Z/Scene/Components.h"
#include "imgui/imgui.h"
#include "imgui_internal.h"

namespace Z {

	static void MyDrawVec(const std::string&label,glm::vec3& value,float ResetValue=.0f,float ColumnWidth=100.f){
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0,ColumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3,ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, {.0f,0,.5f,1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.0f,0,1.f,1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.0f,0,.5f,1});
		ImGui::PushStyleColor(ImGuiCol_Button, {.0f,.5f,.0f,1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.0f,1.f,.0f,1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.0f,.5f,.0f,1});
		ImGui::PushStyleColor(ImGuiCol_Button, {.5f,0,0,1});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1.f,0,0,1});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.5f,0,0,1});
		static const char* ButtonLabel[]={ "X","Y","Z" };
		static const char* DragLabel[]={ "##X","##Y","##Z" };

		for(int i=0;i<3;++i){
			if(ImGui::Button(ButtonLabel[i],buttonSize)){
				value[i]=ResetValue;
			}
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			ImGui::DragFloat(DragLabel[i],&value[i],0.1f);
			ImGui::SameLine();
			ImGui::PopItemWidth();
		}

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
		if (opened) {
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPlane::DrawComponents(Entity entity) {
		DrawComponent<TagComponent>(entity, [](Entity entity, auto &tag) {
			static char buffer[256];
			memset(buffer, 0, 256);
			strcpy(buffer, tag.tag.c_str());
			if (ImGui::InputText("Tag", &buffer[0], 256))
				tag = std::string(buffer);
		});
		DrawComponent<TransformComponent>(entity, [](Entity entity, auto &transform) {
//			ImGui::DragFloat3("Position", &transform.translation[0], 0.1f);
//			ImGui::DragFloat3("Rotation", &transform.rotation[0], 0.1f);
//			ImGui::DragFloat3("Scale", &transform.scale[0], 0.1f);
			MyDrawVec("Position",transform.translation);
			MyDrawVec("Rotation",transform.rotation);
			MyDrawVec("Scale",transform.scale,1.f);
		});
		DrawComponent<SpriteRendererComponent>(entity, [](Entity entity, SpriteRendererComponent &spriteRenderer) {
			ImGui::ColorEdit4("Color", &spriteRenderer.color[0]);
		});
		DrawComponent<CameraComponent>(entity, [](Entity entity, CameraComponent &component) {
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
					Z_CORE_WARN("Camera({0}):Projection type changed",
					            entity.GetComponent<TagComponent>().tag.c_str());
				}
			}
			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float verticalFOV = glm::degrees(camera.GetPerspectiveFOV());
				if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) {
					camera.SetPerspectiveFOV(verticalFOV);
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
	void SceneHierarchyPlane::DrawComponent(Entity entity, void (*drawFunc)(Entity, _Ty &)) {
		if (entity.HasComponent<_Ty>()) {
			if (ImGui::TreeNodeEx((void *) typeid(_Ty).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
			                      typeid(_Ty).name())) {
				auto &component = entity.GetComponent<_Ty>();
				drawFunc(entity, component);
				ImGui::TreePop();
			}
		}
	}
}


