#include "tdpch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include "Teddy/Core/UUID.h"
#include "Teddy/Core/KeyCodes.h"
#include "Teddy/Core/Input.h"

#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/Entity.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace Teddy {

    static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

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

    static void Rigid2DBodyComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RunTimeBody;
        body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
    }

    static void Rigid2DBodyComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
        b2Body* body = (b2Body*)rb2d.RunTimeBody;
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
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

    static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
    {
        Scene* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(entityID);
        MonoType* managedType = mono_reflection_type_get_type(componentType);

		auto it = s_EntityHasComponentFuncs.find(managedType);
		if (it == s_EntityHasComponentFuncs.end())
			return false;
        return it->second(entity);
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

    template<typename... Component>
    static void RegisterComponent()
    {
        ([]()
            {
                std::string_view typeName = typeid(Component).name();
                size_t pos = typeName.find_last_of(':');
                std::string_view structName = typeName.substr(pos + 1);
                std::string managedTypename = fmt::format("Teddy.{}", structName);

                MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptingEngine::GetCoreAssemblyImage());
                if (!managedType)
                {
                    TD_CORE_ERROR("Could not find component type {}", managedTypename);
                    return;
                }
                s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
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
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_ApplyLinearImpulse);
        TD_ADD_INTERNAL_CALL(Rigid2DBodyComponent_ApplyLinearImpulseToCenter);

        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetOffset);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetOffset);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_GetSize);
        TD_ADD_INTERNAL_CALL(Box2DColliderComponent_SetSize);

        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetOffset);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetOffset);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_GetRadius);
        TD_ADD_INTERNAL_CALL(Circle2DColliderComponent_SetRadius);

        TD_ADD_INTERNAL_CALL(Input_IsKeyDown);
    }
}