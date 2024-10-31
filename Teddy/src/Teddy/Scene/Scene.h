#pragma once

#include "entt.hpp"
#include <Teddy/Core/Timestep.h>

namespace Teddy
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string name);
		void DestroyEntity(Entity entity);

		// Temprory 
		//entt::registry& getReg() { return m_Registry; }

		void OnUpdate(Timestep ts);
		void OnVeiwportResize(uint32_t width, uint32_t height);

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t viewportWidth = 0, viewportHeight = 0 ;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}