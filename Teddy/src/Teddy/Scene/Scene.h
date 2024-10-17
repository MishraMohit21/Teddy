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

		// Temprory 
		entt::registry& getReg() { return m_Registry; }

		void OnUpdate(Timestep ts);


	private:
		entt::registry m_Registry;

		friend class Entity;
	};

}