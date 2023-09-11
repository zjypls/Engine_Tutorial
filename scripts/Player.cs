using Z;

public class Player : EntityCore
{
    TransformComponent _transform;
    RigidBody2DComponent _rigidBody;
    public float Mass = 1.0f;
    public float Force = 980.0f;
    public Vector2 Velicity = Vector2.Zero;

    void OnCreate()
    {
        _transform = GetComponent<TransformComponent>();
        _rigidBody = GetComponent<RigidBody2DComponent>();
        if (_rigidBody != null)
        {
            Mass = _rigidBody.Mass;
        }
        else
        {
            _rigidBody = AddComponent<RigidBody2DComponent>();
            Mass = _rigidBody.Mass;
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

        Velicity = _rigidBody.velocity;
        _transform.translation +=new Vector3(vec * deltaTime,0);
    }

    ~Player()
    {
    }
}