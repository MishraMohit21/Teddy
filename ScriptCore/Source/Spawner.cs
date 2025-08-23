
using System;
using Teddy;

namespace Sandbox
{
    public class Spawner : Entity
    {
        public int EntityCount = 10;

        private Random m_Random = new Random();

        public override void OnCreate()
        {
            for (int i = 0; i < EntityCount; i++)
            {
                CreateEntity();
            }
        }

        private void CreateEntity()
        {
            Entity newEntity = Scene.CreateEntity("Spawned Entity");

            // Transform
            var transform = newEntity.GetComponent<TransformComponent>();
            transform.Translation = new Vector3((float)m_Random.NextDouble() * 20.0f - 10.0f, (float)m_Random.NextDouble() * 10.0f, 0.0f);
            transform.Scale = new Vector3((float)m_Random.NextDouble() * 2.0f + 0.5f, (float)m_Random.NextDouble() * 2.0f + 0.5f, 1.0f);

            // Physics
            var rb2d = newEntity.AddComponent<Rigid2DBodyComponent>();
            rb2d.Type = Rigid2DBodyComponent.BodyType.Dynamic;

            // Shape and Collider
            if (m_Random.Next(0, 2) == 0)
            {
                // Square
                var sprite = newEntity.AddComponent<SpriteRendererComponent>();
                sprite.Color = new Vector4((float)m_Random.NextDouble(), (float)m_Random.NextDouble(), (float)m_Random.NextDouble(), 1.0f);

                var collider = newEntity.AddComponent<Box2DColliderComponent>();
                collider.Size = new Vector2(transform.Scale.X, transform.Scale.Y);
            }
            else
            {
                // Circle
                var circle = newEntity.AddComponent<CircleRendererComponent>();
                circle.Color = new Vector4((float)m_Random.NextDouble(), (float)m_Random.NextDouble(), (float)m_Random.NextDouble(), 1.0f);
                circle.Thickness = 0.1f;

                var collider = newEntity.AddComponent<Circle2DColliderComponent>();
                collider.Radius = transform.Scale.X * 0.5f;
            }
        }
    }
}
