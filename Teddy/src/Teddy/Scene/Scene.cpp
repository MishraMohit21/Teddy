#include "tdpch.h"
#include "Scene.h"

#include "Component.h"
#include "Teddy/Renderer/Renderer2D.h"

#include "Entity.h"
#include <glm/glm.hpp>

namespace Teddy
{
	
	Scene::Scene(const std::string& name)
		:m_SceneName(name)
	{
	}


	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}


	
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetProjection(viewportWidth, viewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CppScriptComponent>(Entity entity, CppScriptComponent& component)
	{
	}


	void Scene::OnUpdate(Timestep ts)
	{


		{
			m_Registry.view<CppScriptComponent>().each([=](auto entity, auto& ncs) 
			{
				if (!ncs.Instance)
				{
					ncs.Instance = ncs.InstantiateScript();
					ncs.Instance->m_entity = Entity{ entity, this };
					ncs.Instance->OnCreate();
				}

				ncs.Instance->OnUpdate(ts);
			});
		}


		Camera* mainCamera = nullptr;
		

		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
			Renderer2D::EndScene();
		}
	}
	void Scene::OnVeiwportResize(uint32_t width, uint32_t height)
	{	
		viewportHeight = height;
		viewportWidth = width;

		auto view = m_Registry.view<CameraComponent>();
		for (auto& entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.isFixedAspectRatio)
			{
				cameraComponent.Camera.SetProjection(width, height);
			}
		}

	}
}