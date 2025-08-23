namespace Teddy
{
    public struct Vector4
    {
        public float X, Y, Z, A;

        // ✅ Corrected: Returns a Vector4, not a Vector2.
        public static Vector4 Zero => new Vector4(0.0f);

        // ✅ Corrected: Constructor name matches struct name (Vector4).
        // Initializes all four components to the same scalar value.
        public Vector4(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
            A = scalar;
        }

        // ✅ Corrected: Constructor name matches struct name.
        // Initializes all four components with distinct values.
        public Vector4(float x, float y, float z, float a)
        {
            X = x;
            Y = y;
            Z = z;
            A = a;
        }

        // ✅ Corrected: Operator works with Vector4 and handles all four components.
        public static Vector4 operator +(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.A + b.A);
        }

        // ✅ Corrected: Operator works with Vector4 and handles all four components.
        public static Vector4 operator *(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X * scalar, vector.Y * scalar, vector.Z * scalar, vector.A * scalar);
        }
    }
}