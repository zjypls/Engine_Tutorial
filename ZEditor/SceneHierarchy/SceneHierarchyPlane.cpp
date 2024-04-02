//
// Created by 32725 on 2023/3/28.
//
#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include <filesystem>

#include "Include/imgui/imgui.h"
#include "Include/imgui/imgui_internal.h"

#include "SceneHierarchyPlane.h"
#include "ContentBrowser.h"

namespace Z {
	ImTextureID SceneHierarchyPlane::whiteTexture,SceneHierarchyPlane::blackTexture;
	namespace Tools {

		template<class T>
		std::string GetTypeName() {
            std::string  temp;
#ifdef __GNUC__
            temp = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
#else
            temp = typeid(T).name();
#endif
			auto space = temp.rfind(':');
			auto end = temp.rfind("Component");
			return temp.substr(space + 1, end - space - 1);
		}

		template<class... T>
		void AddComponentMenu(Type<T...>, Entity entity) {
			([&]() {
				if (!entity.HasComponent<T>() && ImGui::MenuItem(GetTypeName<T>().c_str())) {
					entity.AddComponent<T>();
				}
			}(), ...);
		}

		std::set<std::string> TextureSheet{".jpg",".png",".bmp",".jpeg"};
		inline bool IsTexture(const std::string& extension){
			#if __cplusplus>=202002L
				return TextureSheet.contains(extension);
			#else
			return TextureSheet.find(extension)!=TextureSheet.end();
			#endif
		}

	}

	// return true if value changed
	static bool
	MyDrawVec(const std::string &label, glm::vec3 &value, const glm::vec3 &ResetValue = glm::vec3{.0f},
	          float ColumnWidth = 100.f) {
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

		bool res=false;
		for (int i = 0; i < 3; ++i) {
			if (ImGui::Button(ButtonLabel[i], buttonSize)) { value[i] = ResetValue[i]; }
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			if(ImGui::DragFloat(DragLabel[i], &value[i], 0.1f)){
				res=true;
			}
			if (i != 2)
				ImGui::SameLine();
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		return res;
	}

	void SceneHierarchyPlane::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
        if(context){
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
		bool opened = ImGui::TreeNodeEx((void *) (uint64) (uint32) entity, flags, tag.c_str());
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



	void SceneHierarchyPlane::DrawComponents(Entity entity) {
		DrawComponent<TagComponent>("Tag", entity, [](Entity entity, auto &tag) {
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
		if (ImGui::BeginPopup("AddComponent")) {
			Tools::AddComponentMenu(AllTypes{}, entity);
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](Entity entity, auto &transform) {
			bool changed= MyDrawVec("Position", transform.translation);
			auto rotation = glm::degrees(transform.rotation);
			changed |= MyDrawVec("Rotation", rotation);
			transform.rotation = glm::radians(rotation);
			changed |= MyDrawVec("Scale", transform.scale, glm::vec3{1.f});
		});
		DrawComponent<SpriteRendererComponent>("SpriteRenderer", entity,
		                                       [](Entity entity, SpriteRendererComponent &spriteRenderer) {
			                                       ImGui::ColorEdit4("Color", &spriteRenderer.color[0]);
			                                       if (ImGui::BeginDragDropTarget()) {
				                                       if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(
						                                       "CONTENT_BROWSER_ITEM")) {
															auto data = (DragAndDropData *) payload->Data;
														   if(data->type!=DragType::eTexture)
															   return;
					                                       Z_CORE_ASSERT(std::filesystem::exists(data->path),
					                                                     "Path does not exist!");
															spriteRenderer.path = data->path;
															spriteRenderer.data=data->ptr;
				                                       }
				                                       ImGui::EndDragDropTarget();
			                                       }else if(ImGui::IsItemHovered()&&
														   ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Right)){
													   ImGui::OpenPopup("ImageMenuPop");
												   }
													if(ImGui::BeginPopup("ImageMenuPop")){
														if(ImGui::MenuItem("Remove Image")){
															spriteRenderer.path="";
															spriteRenderer.data=nullptr;
														}
														ImGui::EndPopup();
													}
		                                       });
		DrawComponent<CircleRendererComponent>("CircleRenderer", entity,
		                                       [](Entity entity, CircleRendererComponent &component) {
			                                       ImGui::ColorEdit4("Color", &component.color[0]);
			                                       ImGui::DragFloat("thick", &component.thickness, .05f, 0.f, 1.f);
			                                       ImGui::DragFloat("Fade", &component.fade, .05f, 0.f, 1.f);
		                                       });
		DrawComponent<CameraComponent>("Camera", entity, [](Entity entity, CameraComponent &component) {
			auto &camera = component.camera;
			ImGui::Checkbox("Primary", &component.primary);
			static const char *projectionTypeStrings[] = {"Perspective", "Orthographic"};
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
		DrawComponent<RigidBody2DComponent>("RigidBody2D", entity, [](Entity entity, RigidBody2DComponent &component) {
			ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
			if (int i = (int) component.bodyType;ImGui::Combo("Body Type", &i,
			                                                  "Static\0Kinematic\0Dynamic\0\0"))
				component.bodyType = (RigidBody2DComponent::BodyType) i;
		});
		DrawComponent<BoxCollider2DComponent>("BoxCollider2D", entity,
		                                      [](Entity entity, BoxCollider2DComponent &component) {
			                                      ImGui::Checkbox("Is Trigger", &component.isTrigger);
			                                      ImGui::DragFloat2("Size", &component.size.x, .05f);
			                                      ImGui::DragFloat2("Offset", &component.offset.x, .05f);
			                                      ImGui::DragFloat("Density", &component.density, .05f);
			                                      ImGui::DragFloat("Friction", &component.friction, .05f);
			                                      ImGui::DragFloat("Restitution", &component.restitution, .05f);
			                                      ImGui::DragFloat("MinRestitution", &component.MinRestitution, .05f);
		                                      });
		DrawComponent<CircleCollider2DComponent>("CircleCollider2D", entity,
		                                         [](Entity entity, CircleCollider2DComponent &component) {
			                                         ImGui::Checkbox("Is Trigger", &component.isTrigger);
			                                         ImGui::DragFloat("Radius", &component.radius, .1f);
			                                         ImGui::DragFloat2("Offset", &component.offset.x, .05f);
			                                         ImGui::DragFloat("Density", &component.density, .1f);
			                                         ImGui::DragFloat("Friction", &component.friction, .05f);
			                                         ImGui::DragFloat("Restitution", &component.restitution, .05f);
			                                         ImGui::DragFloat("MinRestitution", &component.MinRestitution,
			                                                          .05f);
		                                         });
		DrawComponent<ScriptComponent>("Script", entity, [](Entity entity, ScriptComponent &component) {
			static char buffer[256];
			std::strcpy(buffer, component.scriptName.data());
			bool exists = ScriptEngine::ClassExists(component.scriptName);
			if (exists) {
				auto klass = ScriptEngine::GetScriptList().at(component.scriptName);
				if (!ScriptEngine::EntityFieldExists(entity.GetUID(), *klass)) {
					ScriptEngine::RegisterEntityClassFields(entity.GetUID(), *klass);
				}
			}
			ImGui::Text("Script Name:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			if (!exists)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.8f, .1f, .2f, 1));
			if (ImGui::InputText("##ScriptName", buffer, 256)) {
				component.scriptName = buffer;
				ImGui::OpenPopup("ScriptNameList");
			}
			if (!exists)
				ImGui::PopStyleColor();
			if (ImGui::BeginPopup("ScriptNameList", ImGuiWindowFlags_NoFocusOnAppearing)) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.1f, .1f, .8f, 1));
				if (ImGui::Selectable("New"))
					component.scriptName = "New";
				for (const auto &[name, script]: ScriptEngine::GetScriptList()) {
					if (ImGui::Selectable(name.c_str()))
						component.scriptName = name;
				}
				ImGui::PopStyleColor();
				ImGui::EndPopup();
				if (ScriptEngine::ClassExists(component.scriptName)) {
					if (auto klass = ScriptEngine::GetScriptList().at(
								component.scriptName);!ScriptEngine::EntityFieldExists(entity.GetUID(), *klass))
						ScriptEngine::RegisterEntityClassFields(entity.GetUID(), *klass);
				}
			}
			if (exists)
				if (entity.GetContext()->isRunning()) {
					auto instance = ScriptEngine::GetInstance(entity.GetUID());
					if (!instance)
						return;
					auto klass = instance->GetClass();
					auto &fields = klass->GetFields();
					unsigned char buffer[32]{0};
					for (const auto &[name, type]: fields) {
						instance->GetValue(name, buffer);
						ImGui::Text(name.c_str());
						ImGui::SameLine();
						switch (type.Type) {
							case ScriptFieldType::Float: {
								if (ImGui::DragFloat(("##" + name).c_str(), (float *) buffer, .5f)) {
									instance->SetValue(name, buffer);
								}
								break;
							}
							case ScriptFieldType::Int: {
								if (ImGui::DragInt(("##" + name).c_str(), (int *) buffer)) {
									instance->SetValue(name, buffer);
								}
								break;
							}
							case ScriptFieldType::Float2: {
								if (ImGui::DragFloat2(("##" + name).c_str(), (float *) buffer)) {
									instance->SetValue(name, buffer);
								}
								break;
							}
							case ScriptFieldType::Float3: {
								if (ImGui::DragFloat3(("##" + name).c_str(), (float *) buffer)) {
									instance->SetValue(name, buffer);
								}
								break;
							}
							case ScriptFieldType::Float4: {
								if (ImGui::DragFloat4(("##" + name).c_str(), (float *) buffer)) {
									instance->SetValue(name, buffer);
								}
								break;
							}
							case ScriptFieldType::Bool: {
								if (ImGui::Checkbox(("##" + name).c_str(), (bool *) buffer)) {
									instance->SetValue(name, buffer);
								}
								break;
							}
							case ScriptFieldType::String: {
								if (ImGui::InputText(("##" + name).c_str(), (char *) buffer, 32))
									instance->SetValue(name, buffer);
								break;
							}
						}
					}

				} else {
					auto klass = ScriptEngine::GetScriptList().at(component.scriptName);
					auto &fields = klass->GetFields();
					unsigned char buffer[32]{0};
					auto id = entity.GetUID();
					for (const auto &[name, type]: fields) {
						ImGui::Text(name.c_str());
						ImGui::SameLine();
						klass->GetValue(id, name, buffer);
						switch (type.Type) {
							case ScriptFieldType::Float: {
								if (ImGui::DragFloat(("##" + name).c_str(), (float *) buffer, .5f)) {
									klass->SetValue(id, name, buffer);
								}
								break;
							}
							case ScriptFieldType::Int: {
								if (ImGui::DragInt(("##" + name).c_str(), (int *) buffer)) {
									klass->SetValue(id, name, buffer);
								}
								break;
							}
							case ScriptFieldType::Float2: {
								if (ImGui::DragFloat2(("##" + name).c_str(), (float *) buffer)) {
									klass->SetValue(id, name, buffer);
								}
								break;
							}
							case ScriptFieldType::Float3: {
								if (ImGui::DragFloat3(("##" + name).c_str(), (float *) buffer)) {
									klass->SetValue(id, name, buffer);
								}
								break;
							}
							case ScriptFieldType::Float4: {
								if (ImGui::DragFloat4(("##" + name).c_str(), (float *) buffer)) {
									klass->SetValue(id, name, buffer);
								}
								break;
							}
							case ScriptFieldType::Bool: {
								if (ImGui::Checkbox(("##" + name).c_str(), (bool *) buffer)) {
									klass->SetValue(id, name, buffer);
								}
								break;
							}
							case ScriptFieldType::String: {
								if (ImGui::InputText(("##" + name).c_str(), (char *) buffer, 32))
									klass->SetValue(id, name, buffer);
								break;
							}
						}
					}
				}
		});
		DrawComponent<MeshFilterComponent>("MeshFilter", entity, [](Entity entity, MeshFilterComponent&component){
			ImGui::Text("Model:");
			ImGui::SameLine();
            ImGui::Text(component.meshPath.c_str());
		});
		DrawComponent<MeshRendererComponent>("MeshRenderer",entity,[](Entity entity,MeshRendererComponent& component){
			ImGui::Text("Material:");
			ImGui::SameLine();
			ImGui::Text(component.materialPath.c_str());
		});
	}

	template<typename Ty>
	void SceneHierarchyPlane::DrawComponent(const std::string &name, Entity entity, std::function<void(Entity, Ty &)> drawFunc) {
		if (entity.HasComponent<Ty>()) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			auto RegionWidth = ImGui::GetContentRegionAvail().x;
			auto lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void *) typeid(Ty).hash_code(),
			                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			                              ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth,
			                              name.c_str());
			ImGui::PopStyleVar();
			bool remove = false;

			ImGui::SameLine(RegionWidth - lineHeight * .5f);
			if (ImGui::Button("+", ImVec2(lineHeight, lineHeight))) {
				ImGui::OpenPopup("Remove Component");
			}
			if (ImGui::BeginPopup("Remove Component")) {
				if (ImGui::MenuItem("Remove Component")) {
					remove = true;
				}
				ImGui::EndPopup();
			}
			if (open) {
				auto &component = entity.GetComponent<Ty>();
				drawFunc(entity, component);
				ImGui::TreePop();
			}
			if (remove) {
				if (std::is_same_v<Ty, ScriptComponent>) {
					ScriptEngine::RemoveEntityClassFields(entity.GetUID(), *ScriptEngine::GetScriptList().at(
							entity.GetComponent<ScriptComponent>().scriptName));
				}
				entity.RemoveComponent<Ty>();
			}
		}
	}

}



