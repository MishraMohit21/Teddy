
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Teddy;

namespace Teddy
{
    public class Entity
    {
        protected Entity() { ID = 0; }

        private Dictionary<Type, Component> m_ComponentCache = new Dictionary<Type, Component>();

        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(ID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(ID, ref value);
            }
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public T AddComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);

            if (HasComponent<T>())
                return GetComponent<T>();

            InternalCalls.Entity_AddComponent(ID, componentType);

            T newComponent = new T() { Entity = this };
            m_ComponentCache.Add(componentType, newComponent);
            return newComponent;
        }

        public T GetComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            if (m_ComponentCache.TryGetValue(componentType, out var component))
                return (T)component;

            if (HasComponent<T>())
            {
                T newComponent = new T() { Entity = this };
                m_ComponentCache.Add(componentType, newComponent);
                return newComponent;
            }
            return null;
        }

        public void Destroy()
        {
            InternalCalls.Scene_DestroyEntity(ID);
        }

    }

}
