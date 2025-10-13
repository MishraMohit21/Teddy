#include "PropertiesPanel.h"
#include "Teddy/Scene/Component.h"
#include "Teddy/Scripting/ScriptingEngine.h"
#include "UIUtils.h"
#include "Teddy/Project/Project.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <cstring>

#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Teddy
{
	void PropertiesPanel::SetContext(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void PropertiesPanel::OnImGuiRender()
	{
		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	template<typename T>
	void PropertiesPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	    void PropertiesPanel::DrawComponents(Entity entity)
	    {		if (entity.HasComponent<TagComponent>())
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
				if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}
		ImGui::SameLine();
			if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<CameraComponent>("Camera");
			ImGui::Separator();
			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>() && !m_SelectionContext.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::Separator();
			}

			if (!m_SelectionContext.HasComponent<CircleRendererComponent>() && !m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					m_SelectionContext.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::Separator();
			}

			DisplayAddComponentEntry<TextComponent>("Text Component");

			DisplayAddComponentEntry<ScriptComponent>("Script");

			DisplayAddComponentEntry<Rigid2DBodyComponent>("RigidBody 2D");
			DisplayAddComponentEntry<Box2DColliderComponent>("BoxCollider 2D");
			DisplayAddComponentEntry<Circle2DColliderComponent>("CircleCollider 2D");

			ImGui::Separator();
			DisplayAddComponentEntry<AudioSourceComponent>("Audio Source");

			ImGui::EndPopup();
		}


		DrawComponent<TransformComponent>("Transform Component", entity, [](auto& component) {
			DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera Component", entity, [](auto& component) {

			auto& camera = component.Camera;

			UI::DrawCheckbox("Primary", component.Primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			int projectionType = (int)camera.GetProjectionType();
			if (UI::DrawCombo("Projection", projectionType, projectionTypeStrings, 2))
				camera.SetProjectionType((SceneCamera::ProjectionType)projectionType);

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (UI::DrawDragFloat("Vertical FOV", perspectiveVerticalFov))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (UI::DrawDragFloat("Near", perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (UI::DrawDragFloat("Far", perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (UI::DrawDragFloat("Size", orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (UI::DrawDragFloat("Near", orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (UI::DrawDragFloat("Far", orthoFar))
					camera.SetOrthographicFarClip(orthoFar);

				UI::DrawCheckbox("Fixed Aspect Ratio", component.isFixedAspectRatio);
			}

			});

				DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {
		
					UI::DrawColorEdit4("Color", component.Color);
		
					ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = Project::GetActive()->GetAssetDirectory() / path;
							component.c_texture = Texture2D::Create(texturePath.string());
						}
						ImGui::EndDragDropTarget();
					}
		
					ImGui::SameLine();
					if (component.c_texture)
					{
						if (ImGui::Button("Remove Texture"))
						{
							component.c_texture = nullptr;
							TD_CORE_INFO("Texture removed successfully.");
						}
						UI::DrawSliderFloat("Tilling Factor", component.c_tilingFactor, 1.0f, 10.0f);
					}
		
					if (component.c_texture)
						ImGui::Text("Current Texture: Active");
					else
						ImGui::Text("Current Texture: None");
		
					});
		
				DrawComponent<AudioSourceComponent>("Audio Source", entity, [](auto& component)
					{
						ImGui::Text("Audio File");
						ImGui::SameLine();
		
						ImGui::InputText("##AudioFilePath", (char*)component.FilePath.c_str(), component.FilePath.size(), ImGuiInputTextFlags_ReadOnly);
		
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								const wchar_t* path = (const wchar_t*)payload->Data;
								std::filesystem::path audioPath = Project::GetActive()->GetAssetDirectory() / path;
								component.FilePath = audioPath.string();
							}
							ImGui::EndDragDropTarget();
						}
		
						UI::DrawDragFloat("Volume", component.Volume, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Pitch", component.Pitch, 0.01f, 0.0f, 10.0f);
						UI::DrawCheckbox("Loop", component.bLoop);
						UI::DrawCheckbox("Play On Awake", component.bPlayOnAwake);
					});
		
				DrawComponent<ScriptComponent>("Script", entity, [&](auto& component)
					{
						auto scriptClasses = ScriptingEngine::GetEntityClasses();

						if (scriptClasses.empty())
						{
							ImGui::Text("No script classes found.");
							return;
						}

						std::vector<std::string> classNames;
						for (auto const& [name, scriptClass] : scriptClasses)
							classNames.push_back(name);

						int currentItem = -1;
						if (!component.ClassName.empty())
						{
							for (int i = 0; i < classNames.size(); i++)
							{
								if (classNames[i] == component.ClassName)
								{
									currentItem = i;
									break;
								}
							}
						}

						std::vector<const char*> cstrClassNames;
						for (const auto& name : classNames)
							cstrClassNames.push_back(name.c_str());

						if (UI::DrawCombo("Class", currentItem, cstrClassNames.data(), cstrClassNames.size()))
						{
							if (currentItem >= 0)
							{
								component.ClassName = classNames[currentItem];
								component.FieldInstances.clear();
							}
						}

						bool sceneIsRunning = ScriptingEngine::GetSceneContext() != nullptr;
						Ref<ScriptClass> scriptClass = scriptClasses.count(component.ClassName) ? scriptClasses.at(component.ClassName) : nullptr;
						if (scriptClass)
						{
							const auto& fields = scriptClass->GetFields();
							for (const auto& [name, field] : fields)
							{
								Ref<ScriptInstance> scriptInstance = sceneIsRunning ? ScriptingEngine::GetEntityScriptInstance(entity.GetComponent<UUIDComponent>().id) : nullptr;

								switch (field.Type)
								{
									case ScriptFieldType::Float:
									{
										float value = sceneIsRunning ? scriptInstance->GetFieldValue<float>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<float>() : field.GetDefaultValue<float>();
										if (UI::DrawFloatScriptField(name, value, field.GetDefaultValue<float>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Double:
									{
										double value = sceneIsRunning ? scriptInstance->GetFieldValue<double>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<double>() : field.GetDefaultValue<double>();
										if (UI::DrawDoubleScriptField(name, value, field.GetDefaultValue<double>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Bool:
									{
										bool value = sceneIsRunning ? scriptInstance->GetFieldValue<bool>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<bool>() : field.GetDefaultValue<bool>();
										if (UI::DrawBoolScriptField(name, value, field.GetDefaultValue<bool>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Char:
									case ScriptFieldType::Byte:
									case ScriptFieldType::Short:
									case ScriptFieldType::Int:
									case ScriptFieldType::Long:
									case ScriptFieldType::UByte:
									case ScriptFieldType::UShort:
									case ScriptFieldType::UInt:
									case ScriptFieldType::ULong:
									{
										int value = sceneIsRunning ? scriptInstance->GetFieldValue<int>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<int>() : field.GetDefaultValue<int>();
										if (UI::DrawIntScriptField(name, value, field.GetDefaultValue<int>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Vector2:
									{
										glm::vec2 value = sceneIsRunning ? scriptInstance->GetFieldValue<glm::vec2>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<glm::vec2>() : field.GetDefaultValue<glm::vec2>();
										if (UI::DrawVec2ScriptField(name, value, field.GetDefaultValue<glm::vec2>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Vector3:
									{
										glm::vec3 value = sceneIsRunning ? scriptInstance->GetFieldValue<glm::vec3>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<glm::vec3>() : field.GetDefaultValue<glm::vec3>();
										if (UI::DrawVec3ScriptField(name, value, field.GetDefaultValue<glm::vec3>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Vector4:
									{
										glm::vec4 value = sceneIsRunning ? scriptInstance->GetFieldValue<glm::vec4>(name) : component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<glm::vec4>() : field.GetDefaultValue<glm::vec4>();
										if (UI::DrawVec4ScriptField(name, value, field.GetDefaultValue<glm::vec4>()))
										{
											if (sceneIsRunning)
												scriptInstance->SetFieldValue(name, value);
											else
												component.FieldInstances[name].SetValue(value);
										}
										break;
									}
									case ScriptFieldType::Entity:
									{
										ImGui::Text("%s (Entity)", name.c_str());
										// TODO: Entity picking UI
										break;
									}
								}
							}
						}
					});
		
				DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
				{
					UI::DrawColorEdit4("Color", component.Color);
					UI::DrawDragFloat("Thickness", component.Thickness, 0.025f, 0.0f, 1.0f);
					UI::DrawDragFloat("Fade", component.Fade, 0.00025f, 0.0f, 1.0f);
				});
		
				DrawComponent<Rigid2DBodyComponent>("Rigidbody 2D", entity, [](auto& component)
					{
						const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
						int bodyType = (int)component.Type;
						if (UI::DrawCombo("Body Type", bodyType, bodyTypeStrings, 3))
							component.Type = (Rigid2DBodyComponent::BodyType)bodyType;
		
						UI::DrawCheckbox("Fixed Rotation", component.FixedRotation);
						UI::DrawDragFloat("Linear Damping", component.LinearDamping, 0.01f, 0.0f, 100.0f);
						UI::DrawDragFloat("Angular Damping", component.AngularDamping, 0.01f, 0.0f, 100.0f);
						UI::DrawDragFloat("Gravity Scale", component.GravityScale, 0.01f, -100.0f, 100.0f);
						UI::DrawCheckbox("Is Continuous", component.IsContinuous);
						UI::DrawCheckbox("Is Awake", component.IsAwake);
						UI::DrawCheckbox("Enabled", component.Enabled);
					});
		
				DrawComponent<Box2DColliderComponent>("Box Collider 2D", entity, [](auto& component)
					{
						UI::DrawDragFloat2("Offset", component.Offset);
						UI::DrawDragFloat2("Size", component.Size);
						UI::DrawDragFloat("Density", component.Density, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Friction", component.Friction, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Restitution", component.Restitution, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Restitution Threshold", component.RestitutionThreshold, 0.01f, 0.0f);
						UI::DrawCheckbox("Is Sensor", component.IsSensor);
						UI::DrawInputScalar("Category Bits", ImGuiDataType_U16, &component.CategoryBits);
						UI::DrawInputScalar("Mask Bits", ImGuiDataType_U16, &component.MaskBits);
						UI::DrawInputScalar("Group Index", ImGuiDataType_S16, &component.GroupIndex);
					});
		
				DrawComponent<Circle2DColliderComponent>("Circle Collider 2D", entity, [](auto& component)
					{
						UI::DrawDragFloat2("Offset", component.Offset);
						UI::DrawDragFloat("Radius", component.Radius);
						UI::DrawDragFloat("Density", component.Density, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Friction", component.Friction, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Restitution", component.Restitution, 0.01f, 0.0f, 1.0f);
						UI::DrawDragFloat("Restitution Threshold", component.RestitutionThreshold, 0.01f, 0.0f);
						UI::DrawCheckbox("Is Sensor", component.IsSensor);
						UI::DrawInputScalar("Category Bits", ImGuiDataType_U16, &component.CategoryBits);
						UI::DrawInputScalar("Mask Bits", ImGuiDataType_U16, &component.MaskBits);
						        UI::DrawInputScalar("Group Index", ImGuiDataType_S16, &component.GroupIndex);
						    });

				DrawComponent<TextComponent>("Text Component", entity, [](auto& component) {
					char buffer[256];
					memset(buffer, 0, sizeof(buffer));
					strncpy_s(buffer, sizeof(buffer), component.TextString.c_str(), sizeof(buffer));
					if (ImGui::InputTextMultiline("Text", buffer, sizeof(buffer)))
					{
						component.TextString = std::string(buffer);
					}

					UI::DrawColorEdit4("Color", component.Color);
					UI::DrawDragFloat("Kerning", component.Kerning, 0.025f);
					                    UI::DrawDragFloat("Line Spacing", component.LineSpacing, 0.025f);
					                    UI::DrawDragFloat("Size", component.Size, 0.025f);				});
						}}
