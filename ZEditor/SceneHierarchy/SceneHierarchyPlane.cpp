//
// Created by 32725 on 2023/3/28.
//

#include "SceneHierarchyPlane.h"
#include "Z/Scene/Components.h"
#include "imgui/imgui.h"

namespace Z {

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
		if (entity.HasComponent<TagComponent>()) {
			auto &tag = entity.GetComponent<TagComponent>().tag;
			static char buffer[256];
			memset(buffer, 0, 256);
			strcpy(buffer, tag.c_str());
			if (ImGui::InputText("Tag", &buffer[0], 256))
				tag = std::string(buffer);
		}

		if (entity.HasComponent<TransformComponent>()) {
			if (ImGui::TreeNodeEx((void *) typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
			                      "Transform")) {
				auto &transform = entity.GetComponent<TransformComponent>().transform;
				ImGui::DragFloat3("Position", &transform[3][0]);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			if (ImGui::TreeNodeEx((void *) typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
			                      "Sprite Renderer")) {
				auto &spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", &spriteRenderer.color[0]);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>()) {
			if (ImGui::TreeNodeEx((void *) typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
			                      "Camera")) {
				auto &component = entity.GetComponent<CameraComponent>();
				auto&camera = component.camera;
				if (ImGui::Checkbox("Primary", &component.primary)) {
					context->registry.view<CameraComponent>().each([&](auto entity, auto &cameraComponent) {
						if (entity != entity)cameraComponent.primary = !component.primary;
					});
				}
				const char *projectionTypeStrings[] = {"Perspective", "Orthographic"};
				int projectionType = (int) camera.GetProjectionType();

				if (ImGui::Combo("Projection", &projectionType, projectionTypeStrings, 2)) {
					if ((int)(camera.GetProjectionType()) != projectionType) {
						camera.SetProjectionType((SceneCamera::ProjectionType) projectionType);
						Z_CORE_WARN("Camera({0}):Projection type changed",
						            entity.GetComponent<TagComponent>().tag.c_str());
					}
				}
				if(camera.GetProjectionType()==SceneCamera::ProjectionType::Perspective){
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
				}else{
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

				ImGui::TreePop();
			}
		}

	}
}