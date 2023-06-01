using Z;

public class Player : EntityCore
{
    TransformComponent transform;
    RigidBody2DComponent rigidBody;
    public float mass = 1.0f;
    public float Force = 980.0f;
    public Vector2 velicity = Vector2.Zero;

    void OnCreate()
    {
        Log.log($"Player({ID}): Create");//Test
        transform = GetComponent<TransformComponent>();
        rigidBody = GetComponent<RigidBody2DComponent>();
        if (rigidBody != null)
        {
            mass = rigidBody.Mass;
        }

        var rd= AddComponent<RigidBody2DComponent>();
    }

    void OnUpdate(float deltaTime)
    {
        Vector2 vec = Vector2.Zero;
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
        transform.translation +=new Vector3(vec * deltaTime,0);
    }

    ~Player()
    {
        Log.log($"Player({ID}): Destroy");//Test
    }
}