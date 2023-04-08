using System;
using System.Threading;
using Z;

public struct Vector3
{
    public Vector3(float x, float y, float z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public float x;
    public float y;
    public float z;

    public static Vector3 operator +(Vector3 a, Vector3 b)
    {
        return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    public override string ToString()
    {
        return $"({x},{y},{z})";
    }
}

public class Log
{
    public static void Out(string text)
    {
        Internal.InternalCallPRT(text);
    }

    public static void Out(Vector3 vec)
    {
        Internal.InternalCallWARN(ref vec);
    }

    public static void Out(Vector3 a, Vector3 b, out Vector3 vout)
    {
        Internal.InternalCallDot(ref a, ref b, out vout);
    }
    
}

public class Player : EntityCore
{
    void OnCreate()
    {
        Console.WriteLine($"OnCreate:{this}");
    }

    void OnUpdate(float deltaTime)
    {
        Log.Out($"{ID} OnUpdate,deltaTime:{deltaTime} seconds");
    }
    
    ~Player()
    {
        Console.WriteLine($"Destruct {this}");
    }

}