
#include "tdpch.h"
#include "Scene.h"

#include "Component.h"
#include "Teddy/Renderer/Renderer2D.h"
#include "Teddy/Renderer/RenderCommand.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include "Teddy/Scripting/ScriptingEngine.h"
#include "Teddy/Audio/AudioSystem.h"

#include "Entity.h"
#include <glm/glm.hpp>

namespace Utils
{
	static b2Vec2 Convert(const glm::vec2 & vec) {
		return b2Vec2(vec.x, vec.y);
	}

	// Convert b2Vec2 to glm::vec2
	static glm::vec2 Convert(const b2Vec2 & vec) {
		return glm::vec2(vec.x, vec.y);
	}
}
namespace Teddy
{

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


	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<UUIDComponent>(e).id;
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
	void Scene::OnComponentAdded<UUIDComponent>(Entity entity, UUIDComponent& component)
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
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigid2DBodyComponent>(Entity entity, Rigid2DBodyComponent& component)
	{
		if (!m_PhysicsWorld)
			return;

		auto& transform = entity.GetComponent<TransformComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = Rigidbody2DTypeToBox2DBody(component.Type);
		bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
		bodyDef.angle = transform.Rotation.z;
		bodyDef.linearDamping = component.LinearDamping;
		bodyDef.angularDamping = component.AngularDamping;
		bodyDef.gravityScale = component.GravityScale;
		bodyDef.fixedRotation = component.FixedRotation;
		bodyDef.awake = component.IsAwake;
		bodyDef.enabled = component.Enabled;
		bodyDef.bullet = component.IsContinuous;

		component.RuntimeBody = m_PhysicsWorld->CreateBody(&bodyDef);
	}

	template<>
	void Scene::OnComponentAdded<Box2DColliderComponent>(Entity entity, Box2DColliderComponent& component)
	{
		if (!m_PhysicsWorld)
			return;

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
		TD_CORE_ASSERT(rb2d.RuntimeBody, "RigidBody2DComponent does not have a runtime body!");
		b2Body* body = (b2Body*)rb2d.RuntimeBody;

		b2PolygonShape boxShape;
		boxShape.SetAsBox(component.Size.x * transform.Scale.x, component.Size.y * transform.Scale.y, Utils::Convert(component.Offset), 0.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		fixtureDef.density = component.Density;
		fixtureDef.friction = component.Friction;
		fixtureDef.restitution = component.Restitution;
		fixtureDef.restitutionThreshold = component.RestitutionThreshold;
		fixtureDef.isSensor = component.IsSensor;
		fixtureDef.filter.categoryBits = component.CategoryBits;
		fixtureDef.filter.maskBits = component.MaskBits;
		fixtureDef.filter.groupIndex = component.GroupIndex;
		component.RuntimeFixture = body->CreateFixture(&fixtureDef);
	}

	template<>
	void Scene::OnComponentAdded<Circle2DColliderComponent>(Entity entity, Circle2DColliderComponent& component)
	{
		if (!m_PhysicsWorld)
			return;

		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
		TD_CORE_ASSERT(rb2d.RuntimeBody, "RigidBody2DComponent does not have a runtime body!");
		b2Body* body = (b2Body*)rb2d.RuntimeBody;

		b2CircleShape circleShape;
		circleShape.m_p.Set(component.Offset.x, component.Offset.y);
		circleShape.m_radius = component.Radius * transform.Scale.x;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.density = component.Density;
		fixtureDef.friction = component.Friction;
		fixtureDef.restitution = component.Restitution;
		fixtureDef.restitutionThreshold = component.RestitutionThreshold;
		fixtureDef.isSensor = component.IsSensor;
		fixtureDef.filter.categoryBits = component.CategoryBits;
		fixtureDef.filter.maskBits = component.MaskBits;
		fixtureDef.filter.groupIndex = component.GroupIndex;
		component.RuntimeFixture = body->CreateFixture(&fixtureDef);
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
		auto idView = srcSceneRegistry.view<UUIDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<UUIDComponent>(e).id;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntity(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		// Add this logging
		TD_CORE_TRACE("Copying ScriptComponent...");
		CopyComponent<ScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		CopyComponent<Rigid2DBodyComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Box2DColliderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		        CopyComponent<Circle2DColliderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
				CopyComponent<AudioSourceComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
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
		auto& idc = entity.AddComponent<UUIDComponent>();
		idc.id = id;
		entity.AddComponent<TransformComponent>(transform);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[id] = entity;

		return entity;
	}


	void Scene::DestroyEntity(Entity& entity)
	{
		TD_CORE_WARN("Deleting the entity properly");
		m_EntityMap.erase(entity.GetComponent<UUIDComponent>().id);
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{

		OnPhysics2DStart();
		OnAudioStart();

		{

			ScriptingEngine::OnRuntimeStart(this);

			auto& view = m_Registry.view<ScriptComponent>();

			// Add this logging
			TD_CORE_TRACE("Found {} entities with ScriptComponent", view.size());

			for (auto& e : view)
			{
				Entity entity = { e, this };
				ScriptingEngine::OnCreateEntity(entity);
			}

		}
		

	}

	void Scene::OnRuntimeStop()
	{
		OnPhysics2DStop();
		OnAudioStop();

		ScriptingEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
		OnAudioStart();
	}
	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
		OnAudioStop();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& cam	)
	{
		OnAudioUpdate(ts);
		RenderScene(cam);
	}

	void Scene::OnUpdateSimuation(Timestep ts, EditorCamera& cam)
	{
		OnAudioUpdate(ts);

		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptingEngine::OnUpdateEntity(entity, ts);
			}
		}

		m_PhysicsAccumulator += ts;
		while (m_PhysicsAccumulator >= m_PhysicsTimeStep)
		{
			m_PhysicsWorld->Step(m_PhysicsTimeStep, m_VelocityIterations, m_PositionIterations);
			m_PhysicsAccumulator -= m_PhysicsTimeStep;
		}

		// Retrieve transform from Box2D
		auto view = m_Registry.view<Rigid2DBodyComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			const auto& position = body->GetPosition();

			transform.Translation.x = position.x;
			transform.Translation.y = position.y;
			transform.Rotation.z = body->GetAngle();
		}

		RenderScene(cam);

	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		OnAudioUpdate(ts);

		{
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptingEngine::OnUpdateEntity(entity, ts);
			}
		}
		// Physics
		{
			m_PhysicsAccumulator += ts;
			while (m_PhysicsAccumulator >= m_PhysicsTimeStep)
			{
				m_PhysicsWorld->Step(m_PhysicsTimeStep, m_VelocityIterations, m_PositionIterations);
				m_PhysicsAccumulator -= m_PhysicsTimeStep;
			}

			// Retrieve transform from Box2D
			auto view = m_Registry.view<Rigid2DBodyComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
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
			//TD_CORE_WARN("Searching for primary camera in {} entities...", view.size_hint());
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					//TD_CORE_WARN("Found primary camera on entity {0}!", (uint32_t)entity);
					break;
				}
			}
		}

		if (mainCamera)
		{
			//TD_CORE_WARN("Primary camera found! Rendering scene...");
			//TD_CORE_WARN("  Camera Transform: [{}, {}, {}]", cameraTransform[3][0], cameraTransform[3][1], cameraTransform[3][2]);

			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				//TD_CORE_WARN("  Found {} sprites to render.", group.size());
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					
					glm::mat4 spriteTransform = transform.GetTransform();
					//TD_CORE_WARN("    - Drawing sprite at: [{}, {}, {}]", spriteTransform[3][0], spriteTransform[3][1], spriteTransform[3][2]);

					if (sprite.c_texture)
						Renderer2D::DrawQuad(transform.GetTransform(), sprite.c_texture, sprite.c_tilingFactor, sprite.Color, int(entity));
					else
						Renderer2D::DrawSprite(transform.GetTransform(), sprite.Color, (int)entity);
				}
			}

			// Circle Rendering 
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
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
		CopyComponentIfExists<Rigid2DBodyComponent>(newEntity, entity);
		CopyComponentIfExists<Box2DColliderComponent>(newEntity, entity);
		CopyComponentIfExists<Circle2DColliderComponent>(newEntity, entity);
	}

	void Scene::OnAudioStart()
	{
		TD_CORE_TRACE("Scene::OnAudioStart");
		auto view = m_Registry.view<AudioSourceComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			if (entity.GetComponent<AudioSourceComponent>().bPlayOnAwake)
				AudioSystem::Play(entity);
		}
	}

	void Scene::OnAudioUpdate(Timestep ts)
	{
		// TD_CORE_TRACE("Scene::OnAudioUpdate"); // This is too noisy
		AudioSystem::OnUpdate(this, ts);
	}

	void Scene::OnAudioStop()
	{
		TD_CORE_TRACE("Scene::OnAudioStop");
		auto view = m_Registry.view<AudioSourceComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			AudioSystem::Stop(entity);
		}
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		return {};
	}


	void Scene::OnPhysics2DStart()
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
			bodyDef.linearDamping = rb2d.LinearDamping;
			bodyDef.angularDamping = rb2d.AngularDamping;
			bodyDef.gravityScale = rb2d.GravityScale;
			bodyDef.fixedRotation = rb2d.FixedRotation;
			bodyDef.awake = rb2d.IsAwake;
			bodyDef.enabled = rb2d.Enabled;
			bodyDef.bullet = rb2d.IsContinuous;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<Box2DColliderComponent>())
			{
				auto& bc2d = entity.GetComponent<Box2DColliderComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, Utils::Convert(bc2d.Offset), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				fixtureDef.isSensor = bc2d.IsSensor;
				fixtureDef.filter.categoryBits = bc2d.CategoryBits;
				fixtureDef.filter.maskBits = bc2d.MaskBits;
				fixtureDef.filter.groupIndex = bc2d.GroupIndex;
				bc2d.RuntimeFixture = body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<Circle2DColliderComponent>())
			{
				auto& cc2d = entity.GetComponent<Circle2DColliderComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius * transform.Scale.x;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				fixtureDef.isSensor = cc2d.IsSensor;
				fixtureDef.filter.categoryBits = cc2d.CategoryBits;
				fixtureDef.filter.maskBits = cc2d.MaskBits;
				fixtureDef.filter.groupIndex = cc2d.GroupIndex;
				cc2d.RuntimeFixture = body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& cam)
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

		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}


}