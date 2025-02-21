using System;
using System.IO;
using System.Numerics; // Required for MathF functions
using Teddy;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigid2DBodyComponent m_Rigidbody;
        private float angle = 0.0f; // Tracks rotation over time
        private float radius = 5.0f; // Radius of circular motion
        private float speed = 1.5f; // Speed of rotation

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigid2DBodyComponent>();
        }

        void OnUpdate(float ts)
        {
            angle += speed * ts; // Increment angle over time

            float x = (float)Math.Cos(angle) * radius;
            float y = (float)Math.Sin(angle) * radius;

            // Update the player's position in 2D space
            m_Transform.Translation = new Vector3(x, y, m_Transform.Translation.Z);
        }
    }
}
