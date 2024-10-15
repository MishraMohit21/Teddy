#pragma once

#include "entt.hpp"
#include <Teddy/Core/Timestep.h>

namespace Teddy
{

	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		// Temprory 
		entt::registry& getReg() { return m_Registry; }

		void OnUpdate(Timestep ts);


	private:
		entt::registry m_Registry;
	};

}