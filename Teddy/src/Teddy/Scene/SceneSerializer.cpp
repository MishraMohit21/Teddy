#include "tdpch.h"
#include "SceneSerializer.h"
#include "glm/glm.hpp"

namespace YAML {
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Teddy
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{	
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity& entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "EntityId" << YAML::Value << "12837192831273";   // Fix this id system

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto& tc = entity.GetComponent<TransformComponent>();

			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;


			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjection();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthoSize();
			out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera.GetOrthoNearClip();
			out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera.GetOrthoFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.isFixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& color = entity.GetComponent<SpriteRendererComponent>().Color;
			out << YAML::Key << "Color" << YAML::Value << color;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CppScriptComponent>())
		{
			out << YAML::Key << "CppScriptComponent";
			out << YAML::BeginMap;
			std::string className = entity.GetComponent<CppScriptComponent>().scriptClass;
			out << YAML::Key << "Class" << YAML::Value << className;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		
		for (auto* entityID = m_Scene->m_Registry.storage<entt::entity>().data(),
			*end = entityID + m_Scene->m_Registry.storage<entt::entity>().size();
			entityID != end; ++entityID) {

			Entity entity{ *entityID , m_Scene.get() }; 
			if (!entity)
				return;

			SerializeEntity(out, entity);	
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;
		

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filePath)
	{

		TD_CORE_ASSERT(false, "Not Implemented");

	}

	bool SceneSerializer::DeSerialize(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;
		std::string sceneName = data["Scene"].as<std::string>();
		TD_CORE_TRACE("Deserializing scene '{0}'", sceneName);
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["EntityId"].as<uint64_t>(); // TODO
				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				TD_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
				Entity deserializedEntity = m_Scene->CreateEntity(name);
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}
				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNearClip"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFarClip"].as<float>());
					cc.Camera.SetOrthoSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthoNearClip(cameraProps["OrthographicNearClip"].as<float>());
					cc.Camera.SetOrthoFarClip(cameraProps["OrthographicFarClip"].as<float>());
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.isFixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}
				/*auto cppScriptComponent = entity["CppScriptComponent"];
				if (cppScriptComponent)
				{
					cppScriptComponent["Class"].as<function
					auto& csc = deserializedEntity.AddComponent<Sprte
				}*/
			}
		}
		return true;
	}
	
	bool SceneSerializer::DeSerializeRuntime(const std::string& filePath)
	{
		TD_CORE_ASSERT(false, "Not Implemented");
		return false;
	}

}