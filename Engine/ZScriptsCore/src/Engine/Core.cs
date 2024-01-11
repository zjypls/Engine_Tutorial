using System;

namespace Z
{
    public static class Log
    {
        public static void Info(string text)
        {
            Internal.InternalCallInfo(text);
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
            return new T { Entity = this };
        }

        //Todo:Instantiate
        public static void Instantiate<T>() where T : EntityCore, new()
        {
        }
    }
}