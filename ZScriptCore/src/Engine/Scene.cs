namespace Z
{
    public abstract class Component
    {
        public EntityCore EntityCore { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 translation
        {
            get
            {
                Internal.GetTranslation(EntityCore.ID, out Vector3 res);
                return res;
            }
            set { Internal.SetTranslation(EntityCore.ID, ref value); }
        }
    }

    public class RigidBody2DComponent : Component
    {
        public Vector2 velocity
        {
            get
            {
                Internal.Entity_GetVelocity(EntityCore.ID, out Vector2 res);
                return res;
            }
            set { Internal.Entity_SetVelocity(EntityCore.ID, ref value); }
        }
        public Vector3 position
        {
            get
            {
                return Internal.Entity_GetRigidBody2DPosition(EntityCore.ID);
            }
            set { Internal.Entity_SetRigidBody2DPosition(EntityCore.ID, ref value); }
        }
        public void ApplyForce(Vector2 force,Vector2 point,bool wake = true)
        {
            Internal.Entity_ApplyForce(EntityCore.ID, ref force,ref point,wake);
        }
        public void ApplyForce(Vector2 force,bool wake = true)
        {
            Internal.Entity_ApplyForceCenter(EntityCore.ID, ref force,wake);
        }
    }
    
    public class TagComponent : Component
    {
        public string Tag
        {
            get
            {
                return Internal.Entity_GetTag(EntityCore.ID);
            }
            set
            {
                Internal.Entity_SetTag(EntityCore.ID,value);
            }
        }
    }
    
    
}