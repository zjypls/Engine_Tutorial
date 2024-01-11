using System;

namespace Z
{
    public static class Log
    {
        public static void Info(string text)
        {
            Internal.InternalCallInfo(text);
        }

        public static void Warn(string text)
        {
            Internal.InternalCallWarn(text);
        }

        public static void Error(string text)
        {
            Internal.InternalCallError(text);
        }
    }


    public class EntityCore
    {
        public readonly ulong ID;

        public EntityCore()
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

        public EntityCore[] GetEntitiesByName(string name)
        {
            var iRes= Internal.Entity_GetByName(name);
            var res = new EntityCore[iRes.Length];
            for (int i = 0; i < iRes.Length; ++i)
            {
                res[i] = new EntityCore(iRes[i]);
            }

            return res;
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
        public static EntityCore Instantiate<T>(T instance) where T : EntityCore, new()
        {
            if (instance is null) throw new Exception();
            Log.Info($"Test Log : Call EntityCore::Instantiate for go named {instance.GetComponent<TagComponent>()}");
            return new EntityCore(Internal.Entity_SingleClone(instance.ID));
        }
    }
}