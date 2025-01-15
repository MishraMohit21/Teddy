using System;
using Teddy;

namespace Game
{

    class Player : Entity
    {

        void OnCreate()
        {
            Console.WriteLine("OnCreate Called for Player");
        }


        void OnUpdate(float ts)
        {
            Console.WriteLine($"OnUpdate Called for Player : {ts}");

        }
    }
}