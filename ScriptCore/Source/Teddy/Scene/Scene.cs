
namespace Teddy
{
    public static class Scene
    {
        public static Entity CreateEntity(string name)
        {
            ulong entityID = InternalCalls.Scene_CreateEntity(name);
            return new Entity(entityID);
        }

        public static Entity PrimaryCamera
        {
            get
            {
                ulong entityID = InternalCalls.Scene_GetPrimaryCameraEntity();
                return entityID != 0 ? new Entity(entityID) : null;
            }
        }

        public static Entity FindEntityByName(string name)
        {
            ulong entityID = InternalCalls.Scene_FindEntityByName(name);
            if (entityID == 0)
                return null;

            return new Entity(entityID);
        }
    }
}
