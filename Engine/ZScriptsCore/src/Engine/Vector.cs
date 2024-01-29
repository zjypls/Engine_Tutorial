namespace Z
{
    public struct Vector3
    {
        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vector3(Vector2 xy, float z)
        {
            this.x = xy.x;
            this.y = xy.y;
            this.z = z;
        }
    
        public float x;
        public float y;
        public float z;
        public static Vector3 Zero => new Vector3(0, 0, 0);
    
        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }
    
        public static Vector3 operator *(Vector3 a, float b)
        {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }
        public static Vector3 operator *(Vector3 a, Vector3 b)
        {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }
        public static Vector3 operator *(float b, Vector3 a)
        {
            return a*b;
        }
    
        public override string ToString()
        {
            return $"({x},{y},{z})";
        }
    }
    public struct Vector2
    {
        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
    
        public float x;
        public float y;
        public static Vector2 Zero => new Vector2(0, 0);
    
        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x + b.x, a.y + b.y);
        }
    
        public static Vector2 operator *(Vector2 a, float b)
        {
            return new Vector2(a.x * b, a.y * b);
        }
        public static Vector2 operator *(Vector2 a, Vector2 b)
        {
            return new Vector2(a.x * b.x, a.y * b.y);
        }
        public static Vector2 operator *(float b, Vector2 a)
        {
            return a*b;
        }
    
        public override string ToString()
        {
            return $"({x},{y})";
        }
    }
}