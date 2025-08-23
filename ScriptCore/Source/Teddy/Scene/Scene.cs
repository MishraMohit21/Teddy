
namespace Teddy
{
    public static class Scene
    {
        public static Entity CreateEntity(string name)
        {
            ulong entityID = InternalCalls.Scene_CreateEntity(name);
            return new Entity(entityID);
        }
    }
}
