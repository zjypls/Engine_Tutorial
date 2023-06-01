namespace Z
{
    public abstract class Component
    {
        public EntityCore Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 translation
        {
            get
            {
                Internal.GetTranslation(Entity.ID, out Vector3 res);
                return res;
            }
            set { Internal.SetTranslation(Entity.ID, ref value); }
        }
    }

    public class RigidBody2DComponent : Component
    {
        public Vector2 velocity
        {
            get
            {
                Internal.Entity_GetVelocity(Entity.ID, out Vector2 res);
                return res;
            }
            set { Internal.Entity_SetVelocity(Entity.ID, ref value); }
        }
        public Vector3 position
        {
            get
            {
                return Internal.Entity_GetRigidBody2DPosition(Entity.ID);
            }
            set { Internal.Entity_SetRigidBody2DPosition(Entity.ID, ref value); }
        }

        public float Mass
        {
            get
            {
                return Internal.Entity_GetMass(Entity.ID);
            }
            set
            {
                Internal.Entity_SetMass(Entity.ID, value);
            }
        }
        public void ApplyForce(Vector2 force,Vector2 point,bool wake = true)
        {
            Internal.Entity_ApplyForce(Entity.ID, ref force,ref point,wake);
        }
        public void ApplyForce(Vector2 force,bool wake = true)
        {
            Internal.Entity_ApplyForceCenter(Entity.ID, ref force,wake);
        }
    }
    
    public class TagComponent : Component
    {
        public string Tag
        {
            get
            {
                return Internal.Entity_GetTag(Entity.ID);
            }
            set
            {
                Internal.Entity_SetTag(Entity.ID,value);
            }
        }
    }
    
    public class ScriptComponent:Component
    {
        public string[] scriptName
        {
            get { return Internal.Entity_GetScripts(Entity.ID); }
        }
    }
    
    
}