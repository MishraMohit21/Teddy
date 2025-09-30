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
        internal extern static bool Rigid2DBodyComponent_GetFixedRotation(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetFixedRotation(ulong entityID, bool fixedRotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigid2DBodyComponent_GetLinearDamping(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetLinearDamping(ulong entityID, float linearDamping);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigid2DBodyComponent_GetAngularDamping(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetAngularDamping(ulong entityID, float angularDamping);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigid2DBodyComponent_GetGravityScale(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetGravityScale(ulong entityID, float gravityScale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigid2DBodyComponent_IsContinuous(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetIsContinuous(ulong entityID, bool isContinuous);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigid2DBodyComponent_IsAwake(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetIsAwake(ulong entityID, bool isAwake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigid2DBodyComponent_IsEnabled(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_SetEnabled(ulong entityID, bool enabled);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_GetLinearVelocity(ulong entityID, out Vector2 velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_ApplyForce(ulong entityID, ref Vector2 force, ref Vector2 point, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigid2DBodyComponent_ApplyForceToCenter(ulong entityID, ref Vector2 force, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_GetOffset(ulong entityID, out Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetOffset(ulong entityID, ref Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_GetSize(ulong entityID, out Vector2 size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetSize(ulong entityID, ref Vector2 size);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Box2DColliderComponent_GetDensity(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetDensity(ulong entityID, float density);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Box2DColliderComponent_GetFriction(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetFriction(ulong entityID, float friction);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Box2DColliderComponent_GetRestitution(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetRestitution(ulong entityID, float restitution);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Box2DColliderComponent_GetRestitutionThreshold(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetRestitutionThreshold(ulong entityID, float restitutionThreshold);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Box2DColliderComponent_IsSensor(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetIsSensor(ulong entityID, bool isSensor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort Box2DColliderComponent_GetCategoryBits(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetCategoryBits(ulong entityID, ushort categoryBits);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort Box2DColliderComponent_GetMaskBits(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetMaskBits(ulong entityID, ushort maskBits);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static short Box2DColliderComponent_GetGroupIndex(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Box2DColliderComponent_SetGroupIndex(ulong entityID, short groupIndex);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_GetOffset(ulong entityID, out Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetOffset(ulong entityID, ref Vector2 offset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Circle2DColliderComponent_GetRadius(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetRadius(ulong entityID, float radius);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Circle2DColliderComponent_GetDensity(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetDensity(ulong entityID, float density);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Circle2DColliderComponent_GetFriction(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetFriction(ulong entityID, float friction);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Circle2DColliderComponent_GetRestitution(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetRestitution(ulong entityID, float restitution);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Circle2DColliderComponent_GetRestitutionThreshold(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetRestitutionThreshold(ulong entityID, float restitutionThreshold);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Circle2DColliderComponent_IsSensor(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetIsSensor(ulong entityID, bool isSensor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort Circle2DColliderComponent_GetCategoryBits(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetCategoryBits(ulong entityID, ushort categoryBits);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ushort Circle2DColliderComponent_GetMaskBits(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetMaskBits(ulong entityID, ushort maskBits);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static short Circle2DColliderComponent_GetGroupIndex(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Circle2DColliderComponent_SetGroupIndex(ulong entityID, short groupIndex);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_CreateEntity(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_DestroyEntity(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_GetPrimaryCameraEntity();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_FindEntityByName(string name);

        #region AudioSource
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string AudioSource_GetFilePath(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_SetFilePath(ulong entityID, string filePath);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float AudioSource_GetVolume(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_SetVolume(ulong entityID, float volume);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float AudioSource_GetPitch(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_SetPitch(ulong entityID, float pitch);
		
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSource_GetLoop(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_SetLoop(ulong entityID, bool loop);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSource_GetPlayOnAwake(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_SetPlayOnAwake(ulong entityID, bool playOnAwake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_Play(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSource_Stop(ulong entityID);
        #endregion


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ScriptRegistry_RegisterScripts();
    }
}