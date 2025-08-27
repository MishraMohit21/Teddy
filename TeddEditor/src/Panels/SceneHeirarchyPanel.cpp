#include "SceneHeirarchyPanel.h"
#include "Teddy/Scene/Component.h"
#include "Teddy/Scripting/ScriptingEngine.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <cstring>
/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Teddy
{
	extern const std::filesystem::path g_AssetPath;

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity& entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{

			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			
			if (ImGui::Button("-", ImVec2{ lineHeight + 5.0f, lineHeight }))
			{
				ImGui::OpenPopup("Component Settings");
			}
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			ImGui::Spacing();
			ImGui::Spacing();	

			if (open)
			{
				auto& src = entity.GetComponent<T>();
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(label.c_str());

		float dragFloatWidth = 75.0f;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}



	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{

		ImGui::Begin("Scene Heirarchy");

		for (auto* entityID = m_Context->m_Registry.storage<entt::entity>().data(),
			*end = entityID + m_Context->m_Registry.storage<entt::entity>().size();
			entityID != end; ++entityID) {

			Entity entity{ *entityID , m_Context.get() }; // Create your Entity object from the raw entity ID
			DrawEntityNode(entity); // Call your function to process the entity
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{

		/*auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
				opened = false;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}*/


		// Check if the entity is still valid before proceeding
		if (!m_Context->m_Registry.valid(entity))
			return;

		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
				opened = false;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			TD_CORE_INFO("Deleting entity: {}", tag);
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}

	}

	



	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

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

			DisplayAddComponentEntry<ScriptComponent>("Script");

			DisplayAddComponentEntry<Rigid2DBodyComponent>("RigidBody 2D");
			DisplayAddComponentEntry<CameraComponent>("BoxCollider 2D");
			DisplayAddComponentEntry<CameraComponent>("CircleCollider 2D");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();
		

		DrawComponent<TransformComponent>("Transform Component", entity, [](auto& component) {
			DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera Component", entity, [](auto& component) {
			
			auto& camera = component.Camera;
			

			ImGui::Checkbox("Primary", &component.Primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected  = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.SetOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.isFixedAspectRatio);
			}

		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {


			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			if (component.c_texture)
				ImGui::ColorEdit4("Tint Color", glm::value_ptr(component.Color));
			else 
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));


			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					component.c_texture = Texture2D::Create(texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			// Add texture selection button
			
			// Show the remove texture button only if there's a texture
			ImGui::SameLine();
			if (component.c_texture)
			{
				if (ImGui::Button("Remove Texture"))
				{
					component.c_texture = nullptr;
					TD_CORE_INFO("Texture removed successfully.");
				}
				ImGui::SliderFloat("Tilling Factor", &component.c_tilingFactor, 1.0f, 10.0f);
			}

			ImGui::PopStyleVar();
			//ImGui::SameLine();
			// Display current texture preview if it exists
			if (component.c_texture)
			{
				ImGui::Text("Current Texture: Active");
				//TD_CORE_WARN(component.c_texture->GetPath());
				

				// Optional: Add texture preview or additional texture properties here
			}
			else
			{
				ImGui::Text("Current Texture: None");
			}

		});

		DrawComponent<ScriptComponent>("Script", entity, [&](auto& component)
			{
				auto scriptClasses = ScriptingEngine::GetEntityClasses();

				// Convert map keys to a vector of strings for easier handling
				std::vector<std::string> classNames;
				for (auto const& [name, scriptClass] : scriptClasses)
				{
					classNames.push_back(name);
				}

				// Find the index of the currently selected class name
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

				// Convert vector of strings to a const char* array for ImGui's Combo function
				std::vector<const char*> cstrClassNames;
				for (const auto& name : classNames)
				{
					cstrClassNames.push_back(name.c_str());
				}

				// Display the dropdown
				if (ImGui::Combo("Class", &currentItem, cstrClassNames.data(), cstrClassNames.size()))
				{
					if (currentItem >= 0)
					{
						component.ClassName = classNames[currentItem];
						component.FieldInstances.clear(); // Clear fields when changing script
					}
				}

				// Fields
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
								float value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<float>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<float>() : field.GetDefaultValue<float>();
								
								if (ImGui::DragFloat(name.c_str(), &value, 0.1f))
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
								double value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<double>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<double>() : field.GetDefaultValue<double>();

								if (ImGui::DragScalar(name.c_str(), ImGuiDataType_Double, &value, 0.1f))
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
								bool value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<bool>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<bool>() : field.GetDefaultValue<bool>();

								if (ImGui::Checkbox(name.c_str(), &value))
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
								// NOTE: We are using DragInt for all integer types
								int value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<int>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<int>() : field.GetDefaultValue<int>();
								
								if (ImGui::DragInt(name.c_str(), &value))
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
								glm::vec2 value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<glm::vec2>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<glm::vec2>() : field.GetDefaultValue<glm::vec2>();

								if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(value), 0.1f))
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
								glm::vec3 value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<glm::vec3>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<glm::vec3>() : field.GetDefaultValue<glm::vec3>();

								if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(value), 0.1f))
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
								glm::vec4 value;
								if (sceneIsRunning)
									value = scriptInstance->GetFieldValue<glm::vec4>(name);
								else
									value = component.FieldInstances.count(name) ? component.FieldInstances.at(name).GetValue<glm::vec4>() : field.GetDefaultValue<glm::vec4>();

								if (ImGui::DragFloat4(name.c_str(), glm::value_ptr(value), 0.1f))
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
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
		});

		DrawComponent<Rigid2DBodyComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigid2DBodyComponent::BodyType)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::DragFloat2("Linear Velocity", glm::value_ptr(component.linearVelocity));
				ImGui::DragFloat2("Force Value", glm::value_ptr(component.forceValue));
				if (ImGui::Button("ApplyForce"))
				{
					component.ApplyForcebool = true;
				}
				else
				{
					component.ApplyForcebool = false;
				}
				ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
			});
		DrawComponent<Box2DColliderComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<Circle2DColliderComponent>("Circle Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});


	}


	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}

	}

}