using Z;

public class Player : EntityCore
{
    TransformComponent transform;
    RigidBody2DComponent rigidBody;
    public float Speed = 980.0f;
    public Vector2 velicity = Vector2.Zero;

    void OnCreate()
    {
        transform = GetComponent<TransformComponent>();
        rigidBody = GetComponent<RigidBody2DComponent>();
    }

    void OnUpdate(float deltaTime)
    {
        Vector2 vec = Vector2.Zero;
        Vector2 pos;
        if (Input.IsKeyPressed(KeyCode.W))
        {
            vec.y = 1;
        }
        else if (Input.IsKeyPressed(KeyCode.S))
        {
            vec.y = -1;
        }

        if (Input.IsKeyPressed(KeyCode.A))
        {
            vec.x = -1;
        }
        else if (Input.IsKeyPressed(KeyCode.D))
        {
            vec.x = 1;
        }


        if (rigidBody != null)
        {
            velicity = rigidBody.velocity;
            if (vec.x != 0 || vec.y != 0)
                rigidBody.ApplyForce(vec * (deltaTime * Speed));
        }
    }

    ~Player()
    {
        Log.log("Player destroyed");
    }
}