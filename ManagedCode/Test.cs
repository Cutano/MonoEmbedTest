using System;

namespace ManagedCode
{
    public static class ManagedClass
    {
        public static void Main()
        {
            for (int i = 0; i < 3; i++)
            {
                var comp = new Component(i, $"Tag {i * 10}");
            }
        }
    }
}