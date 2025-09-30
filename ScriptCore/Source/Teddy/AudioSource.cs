namespace Teddy
{
    public class AudioSourceComponent : Component
    {
        public string FilePath
        {
            get => InternalCalls.AudioSource_GetFilePath(Entity.ID);
            set => InternalCalls.AudioSource_SetFilePath(Entity.ID, value);
        }

        public float Volume
        {
            get => InternalCalls.AudioSource_GetVolume(Entity.ID);
            set => InternalCalls.AudioSource_SetVolume(Entity.ID, value);
        }

        public float Pitch
        {
            get => InternalCalls.AudioSource_GetPitch(Entity.ID);
            set => InternalCalls.AudioSource_SetPitch(Entity.ID, value);
        }

        public bool Loop
        {
            get => InternalCalls.AudioSource_GetLoop(Entity.ID);
            set => InternalCalls.AudioSource_SetLoop(Entity.ID, value);
        }

        public bool PlayOnAwake
        {
            get => InternalCalls.AudioSource_GetPlayOnAwake(Entity.ID);
            set => InternalCalls.AudioSource_SetPlayOnAwake(Entity.ID, value);
        }

        public void Play()
        {
            InternalCalls.AudioSource_Play(Entity.ID);
        }

        public void Stop()
        {
            InternalCalls.AudioSource_Stop(Entity.ID);
        }
    }
}
