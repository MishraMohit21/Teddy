#pragma once
#include <Teddy/Scene/Entity.h>

namespace Teddy
{

	class ScriptableEntity
	{
	public:

		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}

	protected:
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnCreate() {}
		virtual void OnDestroy() {}

	private:
		Entity m_entity;
		friend class Scene;
	};

}