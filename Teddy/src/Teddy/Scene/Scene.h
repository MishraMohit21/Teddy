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


		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Timestep ts, EditorCamera& cam);
		void OnUpdateRuntime(Timestep ts);
		void OnVeiwportResize(uint32_t width, uint32_t height);

		Entity GetPrimarySceneCamera();


		void DuplicateEntity(Entity entity);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		std::string m_SceneName;
		entt::registry m_Registry;
		uint32_t viewportWidth = 0, viewportHeight = 0 ;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}