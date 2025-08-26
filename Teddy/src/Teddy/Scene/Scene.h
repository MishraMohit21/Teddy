#pragma once

#include "entt.hpp"
#include <Teddy/Core/Timestep.h>
#include <Teddy/Renderer/EditorCamera.h>
#include "Teddy/Core/UUID.h"

class b2World;

namespace Teddy
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		Scene(const std::string& name);
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> scene);

		Entity CreateEntity(const std::string name);
		Entity CreateEntity(const std::string name, glm::vec3 transform);
		Entity CreateEntity(UUID id, const std::string name);
		Entity CreateEntity(UUID id, const std::string name, glm::vec3 transform);

		/*Entity GetEntityByUUID(const UUID& id);*/
		Entity GetEntityByUUID(UUID uuid);

		void DestroyEntity(Entity& entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateEditor(Timestep ts, EditorCamera& cam);
		void OnUpdateSimuation(Timestep ts, EditorCamera& cam);
		void OnUpdateRuntime(Timestep ts);
		void OnVeiwportResize(uint32_t width, uint32_t height);

		Entity GetPrimarySceneCamera();

		void DuplicateEntity(Entity entity);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}


	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();


		void RenderScene(EditorCamera& cam);

	private:
		std::string m_SceneName;
		entt::registry m_Registry;
		uint32_t viewportWidth = 0, viewportHeight = 0 ;

		b2World* m_PhysicsWorld = nullptr;
		std::unordered_map<UUID, entt::entity> m_EntityMap;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}