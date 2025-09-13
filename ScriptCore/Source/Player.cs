using System;
using Teddy;

namespace Sandbox
{
    public class Player : Entity
    {
        private Rigid2DBodyComponent m_Rigidbody;

        [ShowInEditor]
        public float MovementForce = 10.0f;

        [ShowInEditor]
        public float MaxSpeed = 5.0f;

        public void OnCreate()
        {
            m_Rigidbody = GetComponent<Rigid2DBodyComponent>();
        }

        public void OnUpdate(float ts)
        {
            if (m_Rigidbody == null)
                return;

            Vector2 moveDirection = Vector2.Zero;

            if (Input.IsKeyDown(KeyCode.W))
            {
                moveDirection.Y = 1.0f;

            }
            if (Input.IsKeyDown(KeyCode.S))
                moveDirection.Y = -1.0f;
            if (Input.IsKeyDown(KeyCode.A))
                moveDirection.X = -1.0f;
            if (Input.IsKeyDown(KeyCode.D))
                moveDirection.X = 1.0f;

            if (moveDirection.Length() > 0.0f)
            {
                moveDirection.Normalize();

                Vector2 currentVelocity = m_Rigidbody.LinearVelocity;
                if (currentVelocity.Length() < MaxSpeed)
                {
                    Vector2 force = moveDirection * MovementForce;
                    m_Rigidbody.ApplyForceToCenter(force, true);
                }
            }
        }
    }
}