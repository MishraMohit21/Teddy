// Teddy\src\Teddy\Scene\Component.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <type_traits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <Teddy/Scene/SceneCamera.h>
#include <Teddy/Renderer/RenderCommand.h>
#include "Teddy/Renderer/Texture.h"
#include "Teddy/Core/UUID.h"
#include "Teddy/Renderer/Font.h"

namespace Teddy
{

	struct UUIDComponent
	{
		UUID id;
		UUIDComponent() = default;
		UUIDComponent(const UUIDComponent&) = default;
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

		Ref<Texture2D> c_texture = nullptr;  float c_tilingFactor = 1.0f;

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






	struct ScriptFieldInstance
	{
		// Stores the value of a script field. The type is known by the ScriptClass.
		// Max size is 16 bytes (e.g. for a Vector4 or a future GUID type).
		uint8_t m_Buffer[16];

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue() const
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(const T& value)
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	};

	struct ScriptComponent
	{
		std::string ClassName;
		std::unordered_map<std::string, ScriptFieldInstance> FieldInstances;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	struct Rigid2DBodyComponent
	{

		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		float LinearDamping = 0.0f;
		float AngularDamping = 0.0f;
		float GravityScale = 1.0f;
		bool IsContinuous = false;
		bool IsAwake = true;
		bool Enabled = true;

		void* RuntimeBody = nullptr;

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

		bool IsSensor = false;
		uint16_t CategoryBits = 0x0001;
		uint16_t MaskBits = 0xFFFF;
		int16_t GroupIndex = 0;

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

		bool IsSensor = false;
		uint16_t CategoryBits = 0x0001;
		uint16_t MaskBits = 0xFFFF;
		int16_t GroupIndex = 0;

		void* RuntimeFixture = nullptr;

		Circle2DColliderComponent() = default;
		Circle2DColliderComponent(const Circle2DColliderComponent&) = default;
	};

	struct AudioSourceComponent
	{
		        std::string FilePath;
		// Configuration
		float Volume = 1.0f;
		float Pitch = 1.0f;
		bool bLoop = false;
		bool bPlayOnAwake = true;

		// --- Runtime State (Managed by AudioSystem) ---
		enum class State { Stopped, Playing, Paused };
		State CurrentState = State::Stopped;

		// Handle from the audio backend (SoLoud) for the active sound
		unsigned int RuntimeHandle = 0;

		AudioSourceComponent() = default;
		AudioSourceComponent(const AudioSourceComponent&) = default;
	};

	struct TextComponent
	{
		std::string TextString;
		std::string FontPath; // Added for dynamic font selection
		Ref<Font> FontAsset;
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
		float Size = 1.0f;
		bool AlwaysOnTop = false;
		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
		TextComponent(const std::string& text)
			: TextString(text) {}
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		CircleRendererComponent, CameraComponent, ScriptComponent,
		Rigid2DBodyComponent, Box2DColliderComponent,
		Circle2DColliderComponent, AudioSourceComponent, TextComponent>;

}