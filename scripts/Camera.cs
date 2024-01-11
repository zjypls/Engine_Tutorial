using System;
using Z;

public class Camera : EntityCore
{
    private EntityCore _player;
    TransformComponent _transform;
    private RigidBody2DComponent _rigidBody2D;
    public float Fov = 60.0f;
    public float Distance = 10.0f;
    private const string PlayerName = "Player";
    
    void OnCreate()
    {
        _transform = GetComponent<TransformComponent>();
        var entities = GetEntitiesByName(PlayerName);
        foreach (var id in entities)
        {
            var entity = GetEntity(id);
            if (entity.HasComponent<ScriptComponent>())
            {
                if(entity.GetComponent<ScriptComponent>().scriptName[0]=="Player")
                {
                    _player = entity;
                    break;
                }
            }
        }
        if(_player==null)
        {
            Log.Info("Player not found,You may need to add a player to the scene to make the camera follow it," +
                    "and named it Player with a script named Player too");
        }
        else
        {
            _rigidBody2D = _player.GetComponent<RigidBody2DComponent>();
        }
    }

    void OnUpdate(float deltaTime)
    {
        Log.Info($"Delta time : {deltaTime} ms !");
        var playerPos=_player?.GetComponent<TransformComponent>().translation;
        if(playerPos!=null)
        {
            Distance = 10 + (float)Math.Sqrt(Math.Pow(_rigidBody2D.velocity.x, 2) + Math.Pow(_rigidBody2D.velocity.x, 2));
            _transform.translation = playerPos.Value + new Vector3(0, 0, Distance);
        }
    }
    ~Camera()
    {
    }
}