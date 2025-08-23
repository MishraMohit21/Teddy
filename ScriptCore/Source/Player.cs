using System;
using Teddy;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigid2DBodyComponent m_Rigidbody;
        private SpriteRendererComponent m_SpriteRenderer;

        // --- Member variables for motion ---
        private float angle = 0.0f;
        private float radius = 5.0f;
        private float speed = 1.5f;

        public void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigid2DBodyComponent>();
            m_SpriteRenderer = GetComponent<SpriteRendererComponent>();
        }

        public void OnUpdate(float ts)
        {
            // --- 1. Circular Motion Logic ---
            angle += speed * ts;

            float x = (float)Math.Cos(angle) * radius;
            float y = (float)Math.Sin(angle) * radius;

            m_Transform.Translation = new Vector3(x, y, m_Transform.Translation.Z);

            // --- 2. Rainbow Color Logic (Synchronized with Angle) ---
            // The angle is in radians (0 to 2*PI). We normalize this to a 0-1 range
            // to use as the 'Hue' for the color.
            float hue = (angle % (2.0f * (float)Math.PI)) / (2.0f * (float)Math.PI);

            // Convert the Hue to an RGB color and apply it.
            // Saturation and Value are set to 1.0f for the brightest, most vibrant colors.
            m_SpriteRenderer.Color = HsvToRgb(hue, 1.0f, 1.0f);
        }

        /// <summary>
        /// Converts an HSV color value to an RGB value.
        /// </summary>
        /// <param name="h">Hue (0.0 to 1.0)</param>
        /// <param name="s">Saturation (0.0 to 1.0)</param>
        /// <param name="v">Value (0.0 to 1.0)</param>
        /// <returns>A Vector4 representing the RGBA color (0 to 255).</returns>
        public static Vector4 HsvToRgb(float h, float s, float v)
        {
            int i = (int)Math.Floor(h * 6);
            float f = h * 6 - i;
            float p = v * (1 - s);
            float q = v * (1 - f * s);
            float t = v * (1 - (1 - f) * s);

            float r = 0, g = 0, b = 0;
            switch (i % 6)
            {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
            }

            return new Vector4(r * 255, g * 255, b * 255, 255);
        }
    }
}
