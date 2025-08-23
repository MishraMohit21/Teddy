#include "tdpch.h"
#include "SceneSerializer.h"
#include "glm/glm.hpp"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};


	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
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
			node.SetStyle(EmitterStyle::Flow);
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
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(Rigid2DBodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigid2DBodyComponent::BodyType::Static:    return "Static";
		case Rigid2DBodyComponent::BodyType::Dynamic:   return "Dynamic";
		case Rigid2DBodyComponent::BodyType::Kinematic: return "Kinematic";
		}
		TD_CORE_ASSERT(false, "Unknown body type");
		return {};
	}
	static Rigid2DBodyComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Rigid2DBodyComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigid2DBodyComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigid2DBodyComponent::BodyType::Kinematic;

		TD_CORE_ASSERT(false, "Unknown body type");
		return Rigid2DBodyComponent::BodyType::Static;
	}

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

		out << YAML::Key << "EntityId" << YAML::Value << entity.GetComponent<UUIDCompononet>().id;   // Fix this id system

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
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.isFixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "TintColor" << YAML::Value << src.Color;
			if (src.c_texture)
			{
				out << YAML::Key << "TextureFile" << YAML::Value << src.c_texture->GetPath();
			}
			else
			{
				out << YAML::Key << "TextureFile" << YAML::Value << NULL;
			}
				out << YAML::Key << "TilingFactor" << YAML::Value << src.c_tilingFactor;
			out << YAML::EndMap;
		}
		
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;
			out << YAML::EndMap; // ScriptComponent
		}


		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent
			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;
			out << YAML::EndMap; // CircleRendererComponent
		}

		

		if (entity.HasComponent<Rigid2DBodyComponent>())
		{
			out << YAML::Key << "Rigid2DBodyComponent";
			out << YAML::BeginMap; // Rigid2DBodyComponent
			auto& rb2dComponent = entity.GetComponent<Rigid2DBodyComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.fixedRotation;
			out << YAML::EndMap; // Rigid2DBodyComponent
		}
		if (entity.HasComponent<Box2DColliderComponent>())
		{
			out << YAML::Key << "Box2DColliderComponent";
			out << YAML::BeginMap; // Box2DColliderComponent
			auto& bc2dComponent = entity.GetComponent<Box2DColliderComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;
			out << YAML::EndMap; // Box2DColliderComponent
		}

		if (entity.HasComponent<Circle2DColliderComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent
			auto& cc2dComponent = entity.GetComponent<Circle2DColliderComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;
			out << YAML::EndMap; // CircleCollider2DComponent
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
		YAML::Node data = YAML::LoadFile(filePath);

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
				Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);
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
					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNearClip"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFarClip"].as<float>());
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.isFixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["TintColor"].as<glm::vec4>();
					std::string path = spriteRendererComponent["TextureFile"].as<std::string>();
					if (path != "0")
					{
						//TD_CORE_(path);
						src.c_texture = Texture2D::Create(path);
					}
					src.c_tilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptComponent["ClassName"].as<std::string>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
					crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.Thickness = circleRendererComponent["Thickness"].as<float>();
					crc.Fade = circleRendererComponent["Fade"].as<float>();
				}

				//auto cppScriptComponent = entity["CppScriptComponent"];
				//if (cppScriptComponent) {
				//	std::string scriptClassName = cppScriptComponent["Class"].as<std::string>();

				//	// Add the CppScriptComponent
				//	auto& csc = deserializedEntity.AddComponent<CppScriptComponent>();

				//	// Bind the component
				//	csc.scriptClass = scriptClassName;

				//	// Create script instance using the registry
				//	csc.Instance = ScriptRegistry::CreateScript(scriptClassName);

				//	// Set up destroy function
				//	if (csc.Instance) {
				//		csc.DestroyScript = [](CppScriptComponent* nsc) {
				//			delete nsc->Instance;
				//			nsc->Instance = nullptr;
				//			};
				//	}
				//}

				auto rigidbody2DComponent = entity["Rigid2DBodyComponent"];
				if (rigidbody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigid2DBodyComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.fixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}
				auto boxCollider2DComponent = entity["Box2DColliderComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<Box2DColliderComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& cc2d = deserializedEntity.AddComponent<Circle2DColliderComponent>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}

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