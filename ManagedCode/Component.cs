using System;
using System.Runtime.CompilerServices;

namespace ManagedCode
{
    public class Component
    {
        private IntPtr _nativeHandle = IntPtr.Zero;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr NativeCallComponentNew(int id, string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeCallComponentDelete(IntPtr nativeHandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int NativeCallGetId(IntPtr nativeHandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeCallSetId(IntPtr nativeHandle, int id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string NativeCallGetTagInternal(IntPtr nativeHandle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void NativeCallSetTagInternal(IntPtr nativeHandle, string tag);

        public int Id
        {
            get => NativeCallGetId(_nativeHandle);
            set => NativeCallSetId(_nativeHandle, value);
        }

        public string Tag
        {
            get => NativeCallGetTagInternal(_nativeHandle);
            set => NativeCallSetTagInternal(_nativeHandle, value);
        }

        public Component(int id, string tag)
        {
            _nativeHandle = NativeCallComponentNew(id, tag);
            if (_nativeHandle != IntPtr.Zero)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine($"ComponentWrapper #{id} constructed in managed code with tag: {Tag}!");
                Console.ForegroundColor = ConsoleColor.White;
            }
        }

        ~Component()
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine($"ComponentWrapper #{Id} deconstructed in managed code!");
            Console.ForegroundColor = ConsoleColor.White;
            NativeCallComponentDelete(_nativeHandle);
            _nativeHandle = IntPtr.Zero;
        }
    }
}