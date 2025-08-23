using System;

namespace Teddy
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }


    }

    public class SpriteRendererComponent : Component
    {
        public Vector4 Color
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }
            set
            {
                InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, ref value);
            }
        }

        public float TilingFactor
        {
            get
            {
                return InternalCalls.SpriteRendererComponent_GetTilingFactor(Entity.ID);
            }
            set
            {
                InternalCalls.SpriteRendererComponent_SetTilingFactor(Entity.ID, value);
            }
        }
    }

    public class CircleRendererComponent : Component
    {
        public Vector4 Color
        {
            get
            {
                InternalCalls.CircleRendererComponent_GetColor(Entity.ID, out Vector4 color);
                return color;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetColor(Entity.ID, ref value);
            }
        }

        public float Thickness
        {
            get
            {
                return InternalCalls.CircleRendererComponent_GetThickness(Entity.ID);
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetThickness(Entity.ID, value);
            }
        }

        public float Fade
        {
            get
            {
                return InternalCalls.CircleRendererComponent_GetFade(Entity.ID);
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetFade(Entity.ID, value);
            }
        }
    }

    public class CameraComponent : Component
    {
        public enum ProjectionType { Perspective = 0, Orthographic = 1 };

        public ProjectionType Projection
        {
            get { return (ProjectionType)InternalCalls.CameraComponent_GetProjectionType(Entity.ID); }
            set { InternalCalls.CameraComponent_SetProjectionType(Entity.ID, (int)value); }
        }
    }

    public class ScriptComponent : Component
    {
        public string ClassName
        {
            get
            {
                return InternalCalls.ScriptComponent_GetClassName(Entity.ID);
            }
        }
    }

    public class Rigid2DBodyComponent : Component
    {
        public enum BodyType { Static = 0, Dynamic, Kinematic }

        public BodyType Type
        {
            get { return (BodyType)InternalCalls.Rigid2DBodyComponent_GetType(Entity.ID); }
            set { InternalCalls.Rigid2DBodyComponent_SetType(Entity.ID, (int)value); }
        }

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigid2DBodyComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigid2DBodyComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }

    public class Box2DColliderComponent : Component
    {
        public Vector2 Offset
        {
            get
            {
                InternalCalls.Box2DColliderComponent_GetOffset(Entity.ID, out Vector2 offset);
                return offset;
            }
            set
            {
                InternalCalls.Box2DColliderComponent_SetOffset(Entity.ID, ref value);
            }
        }

        public Vector2 Size
        {
            get
            {
                InternalCalls.Box2DColliderComponent_GetSize(Entity.ID, out Vector2 size);
                return size;
            }
            set
            {
                InternalCalls.Box2DColliderComponent_SetSize(Entity.ID, ref value);
            }
        }
    }

    public class Circle2DColliderComponent : Component
    {
        public Vector2 Offset
        {
            get
            {
                InternalCalls.Circle2DColliderComponent_GetOffset(Entity.ID, out Vector2 offset);
                return offset;
            }
            set
            {
                InternalCalls.Circle2DColliderComponent_SetOffset(Entity.ID, ref value);
            }
        }

        public float Radius
        {
            get
            {
                return InternalCalls.Circle2DColliderComponent_GetRadius(Entity.ID);
            }
            set
            {
                InternalCalls.Circle2DColliderComponent_SetRadius(Entity.ID, value);
            }
        }
    }
}