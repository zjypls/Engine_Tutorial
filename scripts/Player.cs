using Z;

public class Player : EntityCore
{
    TransformComponent transform;
    RigidBody2DComponent rigidBody;
    public float Mass = 1.0f;
    public float Force = 980.0f;
    public Vector2 Velicity = Vector2.Zero;
    private string name;

    void OnCreate()
    {
        name = GetComponent<TagComponent>().Tag;
        Log.Info($"Player : {name} !");
        transform = GetComponent<TransformComponent>();
        rigidBody = GetComponent<RigidBody2DComponent>();
        if (rigidBody != null)
        {
            Mass = rigidBody.Mass;
        }
        else
        {
            rigidBody = AddComponent<RigidBody2DComponent>();
            Mass = rigidBody.Mass;
        }
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

        Velicity = rigidBody.velocity;
        transform.translation +=new Vector3(vec * deltaTime,0);
    }

    ~Player()
    {
    }
}