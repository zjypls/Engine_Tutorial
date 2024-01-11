using System;
using System.Runtime.CompilerServices;

namespace Z
{
    internal abstract class Internal
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void InternalCallInfo(string a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void InternalCallWarn(ref Vector3 a);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void InternalCallDot(ref Vector3 a, ref Vector3 b, out Vector3 c);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTranslation(ulong id, out Vector3 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTranslation(ulong id, ref Vector3 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyPressed(KeyCode key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool Entity_HasComponent(ulong id, Type component);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_AddComponent(ulong id,Type component);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_GetVelocity(ulong id, out Vector2 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_SetVelocity(ulong id, ref Vector2 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_ApplyForce(ulong id, ref Vector2 force, ref Vector2 point, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_ApplyForceCenter(ulong id, ref Vector2 force, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern ulong[] Entity_GetByName(string name);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string Entity_GetTag(ulong id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_SetTag(ulong id,string tag);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Vector3 Entity_GetRigidBody2DPosition(ulong id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_SetRigidBody2DPosition(ulong id,ref Vector3 pos);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string[] Entity_GetScripts(ulong id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float Entity_GetMass(ulong id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float Entity_SetMass(ulong id,float mass);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Entity_SetRigidBody2DType(ulong id,int type);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int Entity_GetRigidBody2DType(ulong id);
        
    }
}