#pragma once

#include "entt.hpp"
#include <Teddy/Core/Timestep.h>
#include <Teddy/Renderer/EditorCamera.h>

namespace Teddy
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		Scene(const std::string& name);
		~Scene();


		Entity CreateEntity(const std::string name);
		Entity CreateEntity(const std::string name, glm::vec3 transform);

		void DestroyEntity(Entity entity);

		// Temprory 
		//entt::registry& getReg() { return m_Registry; }

		void OnUpdateEditor(Timestep ts, EditorCamera& cam);
		void OnUpdateRuntime(Timestep ts);
		void OnVeiwportResize(uint32_t width, uint32_t height);

		Entity GetPrimarySceneCamera();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		std::string m_SceneName;
		entt::registry m_Registry;
		uint32_t viewportWidth = 0, viewportHeight = 0 ;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}