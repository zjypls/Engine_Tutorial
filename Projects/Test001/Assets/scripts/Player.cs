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

        var example = GetEntitiesByName("example");
        if(example is null || example.Length<=0)
        {
            Log.Warn("Need a go named example to test Instantiate func !");
        }
        else
        {
            Log.Info($"{example[0].ID}");
            Log.Info($"{example[0].GetComponent<TagComponent>()}");
            var newPlay = Instantiate(example[0]);
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
        Log.Warn($"Destructor call from {GetComponent<TagComponent>()}");
    }
}