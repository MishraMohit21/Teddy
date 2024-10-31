#pragma once
#include "Scene.h"
#include "Teddy/Scene/Entity.h"
#include "Teddy/Scene/Component.h"


#include <yaml-cpp/yaml.h>

namespace Teddy
{

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filePath);
		void SerializeRuntime(const std::string& filePath);

		bool DeSerialize(const std::string& filePath);
		bool DeSerializeRuntime(const std::string& filePath);


	private:
		Ref<Scene> m_Scene;
	};

}