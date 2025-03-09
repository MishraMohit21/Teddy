using Teddy;

namespace Teddy
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }
    }
}