
using System.Runtime.CompilerServices;

namespace Z
{
    public class Internal
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void InternalCallPRT(string a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void InternalCallWARN(ref Vector3 a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void InternalCallDot(ref Vector3 a, ref Vector3 b, out Vector3 c);

    }
    public class EntityCore
    {
        public readonly ulong ID;

        protected EntityCore()
        {
            ID = 0;
        }

        internal EntityCore(ulong id)
        {
            ID = id;
        }

    }
}