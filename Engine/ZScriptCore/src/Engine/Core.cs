using System;
using System.Runtime.CompilerServices;

namespace Z
{
    public static class Log
    {
        public static void log(string text)
        {
            Internal.InternalCallPRT(text);
        }

        public static void log(Vector3 vec)
        {
            Internal.InternalCallWARN(ref vec);
        }

        public static void log(Vector3 a, Vector3 b, out Vector3 vout)
        {
            Internal.InternalCallDot(ref a, ref b, out vout);
        }
    }


    public class EntityCore
    {
        public readonly ulong ID;

        protected EntityCore()
        {
            ID = 0;
        }

        protected EntityCore(ulong id)
        {
            ID = id;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            return Internal.Entity_HasComponent(ID, typeof(T));
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>()) return new T() { Entity = this };
            return null;
        }

        public ulong[] GetEntitiesByName(string name)
        {
            return Internal.Entity_GetByName(name);
        }

        public EntityCore GetEntity(ulong id)
        {
            return new EntityCore(id);
        }

        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>()) return GetComponent<T>();
            Internal.Entity_AddComponent(ID, typeof(T));
            return new T() { Entity = this };
        }

        //Todo:Instantiate
        public static void Instantiate<T>() where T : EntityCore, new()
        {
        }
    }
}