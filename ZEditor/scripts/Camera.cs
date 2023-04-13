using Z;

public class Camera : EntityCore
{
    private EntityCore player;
    TransformComponent transform=null;
    public float fov = 60.0f;
    public float distance = 10.0f;

    void OnCreate()
    {
        Log.log($"Camera({ID}): created");
        transform = GetComponent<TransformComponent>();
        var entities = GetEntitiesByName("Player");
        foreach (var id in entities)
        {
            EntityCore entity = GetEntity(id);
            if (entity.HasComponent<ScriptComponent>())
            {
                if(entity.GetComponent<ScriptComponent>().scriptName[0]=="Player")
                {
                    player = entity;
                    break;
                }
            }
        }
        if(player==null)
        {
            Log.log("Player not found");
            Log.log("You may need to add a player to the scene to make the camera follow it");
            Log.log("and name it Player with a script named Player too");
        }
    }

    void OnUpdate(float deltaTime)
    {
        var playerPos=player?.GetComponent<TransformComponent>().translation;
        if(playerPos!=null)
            transform.translation=playerPos.Value + new Vector3(0,0,distance);
    }
    ~Camera()
    {
        Log.log($"Camera({ID}): destroyed");
    }
}