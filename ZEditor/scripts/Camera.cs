using Z;

public class Camera : EntityCore
{
    TransformComponent transform=null;
    public float fov = 60.0f;

    void OnCreate()
    {
        Log.log("Camera created");
        transform = GetComponent<TransformComponent>();
    }

    void OnUpdate(float deltaTime)
    {
        var offset = Vector3.Zero;
        float speed = 1.0f*deltaTime;
        if (Input.IsKeyPressed(KeyCode.Up))
        {
            offset.y = 1;
        }
        else if (Input.IsKeyPressed(KeyCode.Down))
        {
            offset.y = -1;
        }

        if (Input.IsKeyPressed(KeyCode.Left))
        {
            offset.x = -1;
        }
        else if (Input.IsKeyPressed(KeyCode.Right))
        {
            offset.x = 1;
        }
        transform.translation += offset * speed;
    }
}