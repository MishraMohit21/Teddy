#include "tdpch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include "Teddy/Core/UUID.h"
#include "Teddy/Core/KeyCodes.h"
#include "Teddy/Core/Input.h"

#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/Entity.h"

#include "Teddy/Audio/AudioSystem.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace Teddy {

    static std::unordered_map<std::string, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
    static std::unordered_map<std::string, std::function<void(Entity)>> s_EntityAddComponentFuncs;

#define TD_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Teddy.InternalCalls::" #Name, Name)

    // TransformComponent
    static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<TransformComponent>().Translation = *translation;
    }

    // SpriteRendererComponent
    static void SpriteRendererComponent_GetColor(UUID entityID, glm::vec4* outColor)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outColor = entity.GetComponent<SpriteRendererComponent>().Color;
    }

    static void SpriteRendererComponent_SetColor(UUID entityID, glm::vec4* color)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<SpriteRendererComponent>().Color = *color;
    }

    static float SpriteRendererComponent_GetTilingFactor(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<SpriteRendererComponent>().c_tilingFactor;
    }

    static void SpriteRendererComponent_SetTilingFactor(UUID entityID, float tilingFactor)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<SpriteRendererComponent>().c_tilingFactor = tilingFactor;
    }

    // CircleRendererComponent
    static void CircleRendererComponent_GetColor(UUID entityID, glm::vec4* outColor)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outColor = entity.GetComponent<CircleRendererComponent>().Color;
    }

    static void CircleRendererComponent_SetColor(UUID entityID, glm::vec4* color)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<CircleRendererComponent>().Color = *color;
    }

    static float CircleRendererComponent_GetThickness(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<CircleRendererComponent>().Thickness;
    }

    static void CircleRendererComponent_SetThickness(UUID entityID, float thickness)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<CircleRendererComponent>().Thickness = thickness;
    }

    static float CircleRendererComponent_GetFade(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<CircleRendererComponent>().Fade;
    }

    static void CircleRendererComponent_SetFade(UUID entityID, float fade)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<CircleRendererComponent>().Fade = fade;
    }

    // CameraComponent
    static int CameraComponent_GetProjectionType(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return (int)entity.GetComponent<CameraComponent>().Camera.GetProjectionType();
    }

    static void CameraComponent_SetProjectionType(UUID entityID, int projectionType)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<CameraComponent>().Camera.SetProjectionType((SceneCamera::ProjectionType)projectionType);
    }

    // ScriptComponent
    static MonoString* ScriptComponent_GetClassName(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& sc = entity.GetComponent<ScriptComponent>();
        return mono_string_new(ScriptingEngine::GetCoreAppDomain(), sc.ClassName.c_str());
    }

    // Rigid2DBodyComponent
    static int Rigid2DBodyComponent_GetType(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return (int)entity.GetComponent<Rigid2DBodyComponent>().Type;
    }

    static void Rigid2DBodyComponent_SetType(UUID entityID, int bodyType)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().Type = (Rigid2DBodyComponent::BodyType)bodyType;
    }

    static bool Rigid2DBodyComponent_GetFixedRotation(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().FixedRotation;
    }

    static void Rigid2DBodyComponent_SetFixedRotation(UUID entityID, bool fixedRotation)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().FixedRotation = fixedRotation;
    }

    static float Rigid2DBodyComponent_GetLinearDamping(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().LinearDamping;
    }

    static void Rigid2DBodyComponent_SetLinearDamping(UUID entityID, float linearDamping)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().LinearDamping = linearDamping;
    }

    static float Rigid2DBodyComponent_GetAngularDamping(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().AngularDamping;
    }

    static void Rigid2DBodyComponent_SetAngularDamping(UUID entityID, float angularDamping)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().AngularDamping = angularDamping;
    }

    static float Rigid2DBodyComponent_GetGravityScale(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().GravityScale;
    }

    static void Rigid2DBodyComponent_SetGravityScale(UUID entityID, float gravityScale)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().GravityScale = gravityScale;
    }

    static bool Rigid2DBodyComponent_IsContinuous(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().IsContinuous;
    }

    static void Rigid2DBodyComponent_SetIsContinuous(UUID entityID, bool isContinuous)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().IsContinuous = isContinuous;
    }

    static bool Rigid2DBodyComponent_IsAwake(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().IsAwake;
    }

    static void Rigid2DBodyComponent_SetIsAwake(UUID entityID, bool isAwake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().IsAwake = isAwake;
    }

    static bool Rigid2DBodyComponent_IsEnabled(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Rigid2DBodyComponent>().Enabled;
    }

    static void Rigid2DBodyComponent_SetEnabled(UUID entityID, bool enabled)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Rigid2DBodyComponent>().Enabled = enabled;
    }

    static void Rigid2DBodyComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void Rigid2DBodyComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
    }

    static void Rigid2DBodyComponent_GetLinearVelocity(UUID entityID, glm::vec2* outVelocity)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        const b2Vec2& velocity = body->GetLinearVelocity();
        *outVelocity = glm::vec2(velocity.x, velocity.y);
    }

    static void Rigid2DBodyComponent_ApplyForce(UUID entityID, glm::vec2* force, glm::vec2* point, bool wake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyForce(b2Vec2(force->x, force->y), b2Vec2(point->x, point->y), wake);
    }

    static void Rigid2DBodyComponent_ApplyForceToCenter(UUID entityID, glm::vec2* force, bool wake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RuntimeBody;
        body->ApplyForceToCenter(b2Vec2(force->x, force->y), wake);
    }

    // Box2DColliderComponent
    static void Box2DColliderComponent_GetOffset(UUID entityID, glm::vec2* outOffset)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outOffset = entity.GetComponent<Box2DColliderComponent>().Offset;
    }

    static void Box2DColliderComponent_SetOffset(UUID entityID, glm::vec2* offset)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().Offset = *offset;
    }

    static void Box2DColliderComponent_GetSize(UUID entityID, glm::vec2* outSize)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outSize = entity.GetComponent<Box2DColliderComponent>().Size;
    }

    static void Box2DColliderComponent_SetSize(UUID entityID, glm::vec2* size)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().Size = *size;
    }

    static float Box2DColliderComponent_GetDensity(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().Density;
    }

    static void Box2DColliderComponent_SetDensity(UUID entityID, float density)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().Density = density;
    }

    static float Box2DColliderComponent_GetFriction(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().Friction;
    }

    static void Box2DColliderComponent_SetFriction(UUID entityID, float friction)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().Friction = friction;
    }

    static float Box2DColliderComponent_GetRestitution(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().Restitution;
    }

    static void Box2DColliderComponent_SetRestitution(UUID entityID, float restitution)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().Restitution = restitution;
    }

    static float Box2DColliderComponent_GetRestitutionThreshold(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().RestitutionThreshold;
    }

    static void Box2DColliderComponent_SetRestitutionThreshold(UUID entityID, float restitutionThreshold)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().RestitutionThreshold = restitutionThreshold;
    }

    static bool Box2DColliderComponent_IsSensor(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().IsSensor;
    }

    static void Box2DColliderComponent_SetIsSensor(UUID entityID, bool isSensor)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().IsSensor = isSensor;
    }

    static uint16_t Box2DColliderComponent_GetCategoryBits(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().CategoryBits;
    }

    static void Box2DColliderComponent_SetCategoryBits(UUID entityID, uint16_t categoryBits)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().CategoryBits = categoryBits;
    }

    static uint16_t Box2DColliderComponent_GetMaskBits(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().MaskBits;
    }

    static void Box2DColliderComponent_SetMaskBits(UUID entityID, uint16_t maskBits)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().MaskBits = maskBits;
    }

    static int16_t Box2DColliderComponent_GetGroupIndex(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Box2DColliderComponent>().GroupIndex;
    }

    static void Box2DColliderComponent_SetGroupIndex(UUID entityID, int16_t groupIndex)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Box2DColliderComponent>().GroupIndex = groupIndex;
    }

    // Circle2DColliderComponent
    static void Circle2DColliderComponent_GetOffset(UUID entityID, glm::vec2* outOffset)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        *outOffset = entity.GetComponent<Circle2DColliderComponent>().Offset;
    }

    static void Circle2DColliderComponent_SetOffset(UUID entityID, glm::vec2* offset)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().Offset = *offset;
    }

    static float Circle2DColliderComponent_GetRadius(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().Radius;
    }

    static void Circle2DColliderComponent_SetRadius(UUID entityID, float radius)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().Radius = radius;
    }

    static float Circle2DColliderComponent_GetDensity(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().Density;
    }

    static void Circle2DColliderComponent_SetDensity(UUID entityID, float density)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().Density = density;
    }

    static float Circle2DColliderComponent_GetFriction(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().Friction;
    }

    static void Circle2DColliderComponent_SetFriction(UUID entityID, float friction)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().Friction = friction;
    }

    static float Circle2DColliderComponent_GetRestitution(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().Restitution;
    }

    static void Circle2DColliderComponent_SetRestitution(UUID entityID, float restitution)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().Restitution = restitution;
    }

    static float Circle2DColliderComponent_GetRestitutionThreshold(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().RestitutionThreshold;
    }

    static void Circle2DColliderComponent_SetRestitutionThreshold(UUID entityID, float restitutionThreshold)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().RestitutionThreshold = restitutionThreshold;
    }

    static bool Circle2DColliderComponent_IsSensor(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().IsSensor;
    }

    static void Circle2DColliderComponent_SetIsSensor(UUID entityID, bool isSensor)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().IsSensor = isSensor;
    }

    static uint16_t Circle2DColliderComponent_GetCategoryBits(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().CategoryBits;
    }

    static void Circle2DColliderComponent_SetCategoryBits(UUID entityID, uint16_t categoryBits)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().CategoryBits = categoryBits;
    }

    static uint16_t Circle2DColliderComponent_GetMaskBits(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().MaskBits;
    }

    static void Circle2DColliderComponent_SetMaskBits(UUID entityID, uint16_t maskBits)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().MaskBits = maskBits;
    }

    static int16_t Circle2DColliderComponent_GetGroupIndex(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<Circle2DColliderComponent>().GroupIndex;
    }

    static void Circle2DColliderComponent_SetGroupIndex(UUID entityID, int16_t groupIndex)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<Circle2DColliderComponent>().GroupIndex = groupIndex;
    }

    // AudioSourceComponent
    static MonoString* AudioSource_GetFilePath(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        const auto& component = entity.GetComponent<AudioSourceComponent>();
        return mono_string_new(ScriptingEngine::GetCoreAppDomain(), component.FilePath.c_str());
    }

    static void AudioSource_SetFilePath(UUID entityID, MonoString* filePath)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& component = entity.GetComponent<AudioSourceComponent>();
        char* cStr = mono_string_to_utf8(filePath);
        component.FilePath = cStr;
        mono_free(cStr);
    }

    static float AudioSource_GetVolume(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<AudioSourceComponent>().Volume;
    }

    static void AudioSource_SetVolume(UUID entityID, float volume)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<AudioSourceComponent>().Volume = volume;
    }

    static float AudioSource_GetPitch(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<AudioSourceComponent>().Pitch;
    }

    static void AudioSource_SetPitch(UUID entityID, float pitch)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<AudioSourceComponent>().Pitch = pitch;
    }

    static bool AudioSource_GetLoop(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<AudioSourceComponent>().bLoop;
    }

    static void AudioSource_SetLoop(UUID entityID, bool loop)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<AudioSourceComponent>().bLoop = loop;
    }

    static bool AudioSource_GetPlayOnAwake(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        return entity.GetComponent<AudioSourceComponent>().bPlayOnAwake;
    }

    static void AudioSource_SetPlayOnAwake(UUID entityID, bool playOnAwake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        entity.GetComponent<AudioSourceComponent>().bPlayOnAwake = playOnAwake;
    }

    static void AudioSource_Play(UUID entityID)
    {
        TD_CORE_TRACE("AudioSource_Play called from C# for entity {0}", entityID);
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        AudioSystem::Play(entity);
    }

    static void AudioSource_Stop(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        AudioSystem::Stop(entity);
    }

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        if (!scene)
        {
            TD_CORE_ERROR("No active scene context to check for component!");
            return false;
        }

        Entity entity = scene->GetEntityByUUID(entityID);
        if (!entity)
        {
            TD_CORE_ERROR("Invalid entity ID passed to Entity_HasComponent: {}", entityID);
            return false;
        }

        MonoType* managedType = mono_reflection_type_get_type(componentType);

		MonoClass* klass = mono_type_get_class(managedType);
		const char* nameSpace = mono_class_get_namespace(klass);
		const char* className = mono_class_get_name(klass);
		std::string fullTypeName = fmt::format("{}.{}", nameSpace, className);

		TD_CORE_WARN("Looking for component: {}", fullTypeName);

		auto it = s_EntityHasComponentFuncs.find(fullTypeName);
		if (it == s_EntityHasComponentFuncs.end())
        {
            TD_CORE_WARN("Attempted to check for unregistered component type: {}", fullTypeName);
			return false;
        }
        return it->second(entity);
    }

    static void Entity_AddComponent(UUID entityID, MonoReflectionType* componentType)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        MonoType* managedType = mono_reflection_type_get_type(componentType);

        MonoClass* klass = mono_type_get_class(managedType);
        const char* nameSpace = mono_class_get_namespace(klass);
        const char* className = mono_class_get_name(klass);
        std::string fullTypeName = fmt::format("{}.{}", nameSpace, className);

        auto it = s_EntityAddComponentFuncs.find(fullTypeName);
        if (it == s_EntityAddComponentFuncs.end())
        {
            TD_CORE_WARN("Attempted to add unregistered component type: {}", fullTypeName);
            return;
        }
        it->second(entity);
    }

    // Input
    static bool Input_IsKeyDown(KeyCode keycode)
    {
        return Input::IsKeyPressed(keycode);
    }

    static uint64_t Scene_CreateEntity(MonoString* name)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        char* cStr = mono_string_to_utf8(name);
        std::string entityName(cStr);
        mono_free(cStr);
        Entity entity = scene->CreateEntity(entityName);
        return entity.GetComponent<UUIDComponent>().id;
    }

    static void Scene_DestroyEntity(UUID entityID)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        if (entity)
            scene->DestroyEntity(entity);
    }

    static uint64_t Scene_GetPrimaryCameraEntity()
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity camera = scene->GetPrimarySceneCamera();
        return camera ? camera.GetComponent<UUIDComponent>().id : 0;
    }

    static uint64_t Scene_FindEntityByName(MonoString* name)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        char* cStr = mono_string_to_utf8(name);
        std::string entityName(cStr);
        mono_free(cStr);

        auto view = scene->GetAllEntitiesWith<TagComponent, UUIDComponent>();
        for (auto entity : view)
        {
            const auto& tag = view.get<TagComponent>(entity);
            if (tag.Tag == entityName)
            {
                const auto& uuid = view.get<UUIDComponent>(entity);
                return uuid.id;
            }
        }
        return 0;
    }

    template<typename... Component>
    static void RegisterComponent()
    {
        ([]()
            {
                std::string_view typeName = typeid(Component).name();
                size_t pos = typeName.find_last_of(':');
                std::string_view structName = typeName.substr(pos + 1);
                std::string managedTypename = fmt::format("Teddy.{}", structName);

                TD_CORE_TRACE("Registering component type: {}", managedTypename);

                MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptingEngine::GetCoreAssemblyImage());
                if (!managedType)
                {
                    TD_CORE_ERROR("Could not find component type {}", managedTypename);
                    return;
                }
                TD_CORE_WARN("Registering component: {}", managedTypename);
                s_EntityHasComponentFuncs[managedTypename] = [](Entity entity) { return entity.HasComponent<Component>(); };
                s_EntityAddComponentFuncs[managedTypename] = [](Entity entity) { entity.AddComponent<Component>(); };
            }(), ...);
    }

    template<typename... Component>
    static void RegisterComponent(ComponentGroup<Component...>){
        RegisterComponent<Component...>();
    }

    void ScriptGlue::RegisterComponents()
    {
        RegisterComponent(AllComponents{});
    }

    void ScriptGlue::RegisterFunctions()
    {
        TD_ADD_INTERNAL_CALL(Entity_HasComponent);
        TD_ADD_INTERNAL_CALL(Entity_AddComponent);

        TD_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
        TD_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

        TD_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
        TD_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
        TD_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTilingFactor);
        TD_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTilingFactor);

        TD_ADD_INTERNAL_CALL(CircleRendererComponent_GetColor);
        TD_ADD_INTERNAL_CALL(CircleRendererComponent_SetColor);
        TD_ADD_INTERNAL_CALL(CircleRendererComponent_GetThickness);
        TD_ADD_INTERNAL_CALL(CircleRendererComponent_SetThickness);
        TD_ADD_INTERNAL_CALL(CircleRendererComponent_GetFade);
        TD_ADD_INTERNAL_CALL(CircleRendererComponent_SetFade);

        TD_ADD_INTERNAL_CALL(CameraComponent_GetProjectionType);
        TD_ADD_INTERNAL_CALL(CameraComponent_SetProjectionType);

        TD_ADD_INTERNAL_CALL(ScriptComponent_GetClassName);

        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_GetType);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetType);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_GetFixedRotation);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetFixedRotation);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_GetLinearDamping);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetLinearDamping);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_GetAngularDamping);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetAngularDamping);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_GetGravityScale);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetGravityScale);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_IsContinuous);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetIsContinuous);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_IsAwake);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetIsAwake);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_IsEnabled);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_SetEnabled);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_ApplyLinearImpulse);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_ApplyLinearImpulseToCenter);

        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_GetLinearVelocity);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_ApplyForce);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_ApplyForceToCenter);

        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetOffset);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetOffset);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetSize);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetSize);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetDensity);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetDensity);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetFriction);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetFriction);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetRestitution);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetRestitution);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetRestitutionThreshold);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetRestitutionThreshold);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_IsSensor);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetIsSensor);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetCategoryBits);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetCategoryBits);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetMaskBits);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetMaskBits);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetGroupIndex);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetGroupIndex);

        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetOffset);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetOffset);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetRadius);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetRadius);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetDensity);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetDensity);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetFriction);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetFriction);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetRestitution);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetRestitution);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetRestitutionThreshold);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetRestitutionThreshold);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_IsSensor);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetIsSensor);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetCategoryBits);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetCategoryBits);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetMaskBits);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetMaskBits);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetGroupIndex);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetGroupIndex);

        TD_ADD_INTERNAL_CALL(Input_IsKeyDown);
        TD_ADD_INTERNAL_CALL(Scene_CreateEntity);
        TD_ADD_INTERNAL_CALL(Scene_DestroyEntity);
        TD_ADD_INTERNAL_CALL(Scene_GetPrimaryCameraEntity);
        TD_ADD_INTERNAL_CALL(Scene_FindEntityByName);

        TD_ADD_INTERNAL_CALL(AudioSource_GetFilePath);
        TD_ADD_INTERNAL_CALL(AudioSource_SetFilePath);
        TD_ADD_INTERNAL_CALL(AudioSource_GetVolume);
        TD_ADD_INTERNAL_CALL(AudioSource_SetVolume);
        TD_ADD_INTERNAL_CALL(AudioSource_GetPitch);
        TD_ADD_INTERNAL_CALL(AudioSource_SetPitch);
        TD_ADD_INTERNAL_CALL(AudioSource_GetLoop);
        TD_ADD_INTERNAL_CALL(AudioSource_SetLoop);
        TD_ADD_INTERNAL_CALL(AudioSource_GetPlayOnAwake);
        TD_ADD_INTERNAL_CALL(AudioSource_SetPlayOnAwake);
        TD_ADD_INTERNAL_CALL(AudioSource_Play);
        TD_ADD_INTERNAL_CALL(AudioSource_Stop);
    }
}