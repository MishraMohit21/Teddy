#pragma once

#include "entt.hpp"

namespace Teddy
{

	class Scene
	{
	public:
		Scene();
		~Scene();

	private:
		entt::registry m_Registry;
	};

}