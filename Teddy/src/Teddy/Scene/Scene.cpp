#include "tdpch.h"
#include "Scene.h"

#include "Component.h"
#include "Teddy/Renderer/Renderer2D.h"
#include "Teddy/Renderer/RenderCommand.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

#include "Entity.h"
#include <glm/glm.hpp>

namespace Teddy
{


	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<UUIDCompononet>(e).id;
			//TD_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
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
		if (viewportWidth > 0 && viewportHeight > 0)
			component.Camera.SetViewportSize(viewportWidth, viewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<UUIDCompononet>(Entity entity, UUIDCompononet& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
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

	template<>
	void Scene::OnComponentAdded<Rigid2DBodyComponent>(Entity entity, Rigid2DBodyComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Box2DColliderComponent>(Entity entity, Box2DColliderComponent& component)
	{
	}

	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigid2DBodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigid2DBodyComponent::BodyType::Static:    return b2_staticBody;
		case Rigid2DBodyComponent::BodyType::Dynamic:   return b2_dynamicBody;
		case Rigid2DBodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		}
		TD_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()
		:m_SceneName("Untitiled")
	{
	}
	Scene::Scene(const std::string& name)
		:m_SceneName(name)
	{
	}


	Scene::~Scene()
	{
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->viewportWidth = other->viewportWidth;
		newScene->viewportHeight = other->viewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<UUIDCompononet>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<UUIDCompononet>(e).id;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CppScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigid2DBodyComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Box2DColliderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string name)
	{
		return CreateEntity(UUID(), name, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	Entity Scene::CreateEntity(const std::string name, glm::vec3 transform)
	{
		return CreateEntity(UUID(), name, transform);

	}

	Entity Scene::CreateEntity(UUID id, const std::string name)
	{
		return CreateEntity(id, name, glm::vec3(0.0f, 0.0f, 0.0f));

	}

	Entity Scene::CreateEntity(UUID id, const std::string name, glm::vec3 transform)
	{
		Entity entity = { m_Registry.create(), this };
		auto& idc = entity.AddComponent<UUIDCompononet>();
		idc.id = id;
		entity.AddComponent<TransformComponent>(transform);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{

		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigid2DBodyComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.fixedRotation);
			rb2d.RunTimeBody = body;

			if (entity.HasComponent<Box2DColliderComponent>())
			{
				auto& bc2d = entity.GetComponent<Box2DColliderComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}

	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}


	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& cam	)
	{

		Renderer2D::BeginScene(cam);
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				if (sprite.c_texture)
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.c_texture, sprite.c_tilingFactor, sprite.Color, int(entity));
				else
					Renderer2D::DrawSprite(transform.GetTransform(), sprite.Color, (int)entity);
			}
		}

		/*{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}*/

		Renderer2D::EndScene();

	}

	void Scene::OnUpdateRuntime(Timestep ts)
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


		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = m_Registry.view<Rigid2DBodyComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();

				b2Body* body = (b2Body*)rb2d.RunTimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
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
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					if (sprite.c_texture)
						Renderer2D::DrawQuad(transform.GetTransform(), sprite.c_texture, sprite.c_tilingFactor, sprite.Color, int(entity));
					else
						Renderer2D::DrawSprite(transform.GetTransform(), sprite.Color, (int)entity);
				}
			}

			// Circle Rendering 
			/*{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}*/
			Renderer2D::EndScene();
		}
	}
	void Scene::OnVeiwportResize(uint32_t width, uint32_t height)
	{	
		viewportHeight = height;
		viewportWidth = wid	th;

		auto view = m_Registry.view<CameraComponent>();
		for (auto& entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.isFixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}

	}
	Entity Scene::GetPrimarySceneCamera()
	{
		auto view = m_Registry.view<CameraComponent>();

		for (auto& entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity(entity, this);
		}
		return {};
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetComponent<TagComponent>().Tag;
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<CppScriptComponent>(newEntity, entity);
		CopyComponentIfExists<Rigid2DBodyComponent>(newEntity, entity);
		CopyComponentIfExists<Box2DColliderComponent>(newEntity, entity);
	}


}