#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Teddy/Scene/SceneCamera.h>
#include <Teddy/Scene/ScriptableEntity.h>

#include <Teddy/Renderer/RenderCommand.h>
#include "Teddy/Renderer/Renderer2D.h"
#include "Teddy/Core/UUID.h"

#include <type_traits>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Teddy
{

	struct UUIDCompononet
	{
		UUID id;
		UUIDCompononet() = default;
		UUIDCompononet(const UUIDCompononet&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{

		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}
		
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));


			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

	};
	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		Ref<Texture2D> c_texture = nullptr;
		float c_tilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}

		SpriteRendererComponent(const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f))
			: Color(tintColor), c_texture(texture) {}

		SpriteRendererComponent(const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor = glm::vec4(1.0f))
			: Color(tintColor), c_texture(texture), c_tilingFactor(tilingFactor) {}

	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;
		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color)
			: Color(color) {
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool isFixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		/*CameraComponent(const glm::mat4& projection)
			: Camera(projection) {}*/
	};


	struct CppScriptComponent {
		std::string scriptClass;
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(CppScriptComponent*);


		template <typename T>
		void Bind()
		{
			scriptClass = typeid(T).name();
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](CppScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };

		}
	};

	struct ScriptComponent
	{
		std::string ClassName;
		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};


	struct Rigid2DBodyComponent
	{

		static enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool fixedRotation = false;
		void* RunTimeBody = nullptr;

		glm::vec2 linearVelocity = glm::vec2(0);
		glm::vec2 forceValue = glm::vec2(0);
		bool ApplyForcebool = false;

		Rigid2DBodyComponent() = default;
		Rigid2DBodyComponent(const Rigid2DBodyComponent&) = default;
			
	};

	struct Box2DColliderComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		Box2DColliderComponent() = default;
		Box2DColliderComponent(const Box2DColliderComponent&) = default;	
	};
	
	struct Circle2DColliderComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		Circle2DColliderComponent() = default;
		Circle2DColliderComponent(const Circle2DColliderComponent&) = default;
	};

}