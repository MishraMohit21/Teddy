using System;
using System.IO;
using System.Numerics; // Required for MathF functions
using Teddy;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        //private Rigid2DBodyComponent m_Rigidbody;
        private float angle = 0.0f; // Tracks rotation over time
        private float radius = 5.0f; // Radius of circular motion
        private float speed = 1.5f; // Speed of rotation

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            //m_Rigidbody = GetComponent<Rigid2DBodyComponent>();
        }

        void OnUpdate(float ts)
        {
            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.Up))
                velocity.Y = 1.0f;
            else if (Input.IsKeyDown(KeyCode.Down))
                velocity.Y = -1.0f;

            if (Input.IsKeyDown(KeyCode.Left))
                velocity.X = -1.0f;
            else if (Input.IsKeyDown(KeyCode.Right))
                velocity.X = 1.0f;

            velocity *= speed;

            Vector3 translation = Translation;
            translation += velocity * ts;
            Translation = translation;
        }
    }
}
