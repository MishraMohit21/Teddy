using System;
using System.Runtime.CompilerServices;

namespace Teddy
{
    internal static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_GetColor(ulong entityID, out Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetColor(ulong entityID, ref Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float SpriteRendererComponent_GetTilingFactor(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTilingFactor(ulong entityID, float tilingFactor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_GetColor(ulong entityID, out Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetColor(ulong entityID, ref Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleRendererComponent_GetThickness(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetThickness(ulong entityID, float thickness);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleRendererComponent_GetFade(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetFade(ulong entityID, float fade);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int CameraComponent_GetProjectionType(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraComponent_SetProjectionType(ulong entityID, int projectionType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string ScriptComponent_GetClassName(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int Rigid2DBodyComponent_GetType(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetType(ulong entityID, int bodyType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_GetOffset(ulong entityID, out Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetOffset(ulong entityID, ref Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_GetSize(ulong entityID, out Vector2 size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetSize(ulong entityID, ref Vector2 size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_GetOffset(ulong entityID, out Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetOffset(ulong entityID, ref Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Circle2DColliderComponent_GetRadius(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetRadius(ulong entityID, float radius);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_CreateEntity(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptRegistry_RegisterScripts();
    }
}