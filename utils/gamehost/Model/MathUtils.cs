using System;
using System.Collections.Generic;
using System.Drawing;

namespace GameHost
{

    /// <summary>
    /// Think this stuff was ripped from LegendOfKevin, but not sure.
    /// </summary>
    /// 

    public class MathUtils
    {
        public const float M_PI = 3.14159265359f;
        public static float Max(float x, float y) { return x > y ? x : y; }
        public static float Min(float x, float y) { return x > y ? y : x; }
        public static float ToDegrees(float radians)
        {
            return radians * 360.0f / M_PI;
        }
        public static float ToRadians(float degrees)
        {
            return degrees * M_PI / 360.0f;
        }
        public static vec2 DecomposeRotation(float r)
        {
            //Turn a rotation into a vector (for chars mostly)
            float r2 = r - (float)Math.PI * 0.5f;
            vec2 dxy = new vec2(
                (float)Math.Cos(r2),
                (float)Math.Sin(r2)
                );

            return dxy;
        }

        public static float GetRotationFromLine(float x, float y, float x2, float y2)
        {
            //https://stackoverflow.com/questions/270138/how-do-i-draw-lines-using-xna
            //this returns the angle between two points in radians 
            float adj = x - x2;
            float opp = y - y2;
            float tan = opp / adj;
            float res = ToDegrees((float)Math.Atan2(opp, adj));
            res = (res - 180) % 360;
            if (res < 0) { res += 360; }
            res = ToRadians(res);
            return res;
        }
    }

    public struct Box2i
    {
        public ivec2 p0;
        public ivec2 p1;

        public int Left()
        {
            return p0.x;
        }

        public int Top()
        {
            return p0.y;
        }

        public int Right()
        {
            return p1.x;
        }

        public int Bottom()
        {
            return p1.y;
        }

        public int Width()
        {
            return p1.x - p0.x;
        }

        public int Height()
        {
            return p1.y - p0.y;
        }

        public void Construct(int left, int top, int right, int bottom)
        {
            p0.x = left;
            p0.y = top;
            p1.x = right;
            p1.y = bottom;
        }

    }

    public class vec2EqualityComparer : IEqualityComparer<vec2>
    {
        public bool Equals(vec2 x, vec2 y)
        {
            return x.x == y.x && x.y == y.y;
        }

        public int GetHashCode(vec2 x)
        {
            return x.x.GetHashCode() + x.y.GetHashCode();
        }
    }

    class mat2
    {
        //0 1
        //2 3
        public static int m00 = 0;
        public static int m01 = 1;
        public static int m10 = 2;
        public static int m11 = 3;
        public float[] m = new float[4];

        public static vec2 operator *(mat2 a, vec2 b)
        {
            vec2 r = new vec2(0, 0);

            r.x = a.m[0] * b.x + a.m[1] * b.y;
            r.y = a.m[2] * b.x + a.m[3] * b.y;
            return r;
        }
        public static mat2 GetRot(float theta)
        {
            mat2 ret = new mat2();

            //This is an incorrect rotation function - sin 10 shouldn't be negative.
            ret.m[m00] = (float)Math.Cos(theta);
            ret.m[m10] = (float)Math.Sin(theta);
            ret.m[m01] = -(float)Math.Sin(theta);
            ret.m[m11] = (float)Math.Cos(theta);

            return ret;
        }

    }

    public class ivec2EqualityComparer : IEqualityComparer<ivec2>
    {
        public bool Equals(ivec2 x, ivec2 y)
        {
            return x.x == y.x && x.y == y.y;
        }

        public int GetHashCode(ivec2 x)
        {
            return x.x.GetHashCode() + x.y.GetHashCode();
        }
    }

    public struct vec2
    {
        public vec2(Point p)
        {
            x = (float)p.X;
            y = (float)p.Y;
        }
        public float x, y;
        public void Construct(float a, float b) { x = a; y = b; }
        //public vec2() { }
        public vec2(vec2 dxy) { x = dxy.x; y = dxy.y; }
        public vec2(float dx, float dy) { x = dx; y = dy; }
        //  public vec2(Vector2 v) { x = v.X; y = v.Y; }//From XNA's Vector2
        public float Len() { return (float)Math.Sqrt((x * x) + (y * y)); }

        public vec2 Perp()
        {
            //Perpendicular
            return new vec2(y, -x);
        }
        public void Normalize()
        {
            float l = Len();
            if (l != 0)
            {
                x /= l;
                y /= l;
            }
            else
            {
                x = 0; y = 0;
            }

        }
        public vec2 Normalized()
        {
            vec2 v = new vec2(this);
            v.Normalize();
            return v;

        }
        public float Len2() { return Dot(this, this); }
        //public Vector2 toXNA() { return new Vector2(x, y); }


        static public implicit operator vec2(float f)
        {
            return new vec2(f, f);
        }
        //public static vec2 operator =(vec2 a, float f)
        //{
        //    return new vec2(f, f);
        //}
        public static float Dot(vec2 a, vec2 b)
        {
            return (a.x * b.x) + (a.y * b.y);
        }
        public float Dot(vec2 b)
        {
            return (x * b.x) + (y * b.y);
        }
        public static vec2 operator -(vec2 d)
        {
            return new vec2(-d.x, -d.y);
        }
        public static vec2 operator +(vec2 a, vec2 b)
        {
            return new vec2(a.x + b.x, a.y + b.y);
        }
        public static vec2 operator -(vec2 a, vec2 b)
        {
            return new vec2(a.x - b.x, a.y - b.y);
        }
        public static vec2 operator *(vec2 a, float b)
        {
            return new vec2(a.x * b, a.y * b);
        }
        public static vec2 operator *(vec2 a, vec2 b)
        {
            return new vec2(a.x * b.x, a.y * b.y);
        }
        public static vec2 operator /(vec2 a, float b)
        {
            return new vec2(a.x / b, a.y / b);
        }
        public static vec2 operator -(vec2 a, float f)
        {
            return new vec2(a.x - f, a.y - f);
        }
        public static vec2 Minv(vec2 a, vec2 b)
        {
            vec2 ret = new vec2();
            ret.x = (float)Math.Min(a.x, b.x);
            ret.y = (float)Math.Min(a.y, b.y);

            return ret;
        }
        public static vec2 Maxv(vec2 a, vec2 b)
        {
            vec2 ret = new vec2();
            ret.x = (float)Math.Max(a.x, b.x);
            ret.y = (float)Math.Max(a.y, b.y);
            return ret;
        }

    }

    public struct ivec2
    {
        public int x;
        public int y;

        public ivec2(int dx, int dy) { x = dx; y = dy; }

        static public implicit operator ivec2(int f)
        {
            return new ivec2(f, f);
        }
        public static ivec2 operator -(ivec2 d)
        {
            return new ivec2(-d.x, -d.y);
        }
        public static ivec2 operator +(ivec2 a, ivec2 b)
        {
            return new ivec2(a.x + b.x, a.y + b.y);
        }
        public static ivec2 operator -(ivec2 a, ivec2 b)
        {
            return new ivec2(a.x - b.x, a.y - b.y);
        }
        public static ivec2 operator *(ivec2 a, int b)
        {
            return new ivec2(a.x * b, a.y * b);
        }
        public static ivec2 operator *(ivec2 a, ivec2 b)
        {
            return new ivec2(a.x * b.x, a.y * b.y);
        }
        public static ivec2 operator /(ivec2 a, int b)
        {
            return new ivec2(a.x / b, a.y / b);
        }
        public static ivec2 operator -(ivec2 a, int f)
        {
            return new ivec2(a.x - f, a.y - f);
        }
        public void Construct(int dx, int dy)
        {
            this.x = dx;
            this.y = dy;
        }
    }

    public struct vec4
    {
        public float x, y, z, w;

        public vec4(vec4 dxy) { x = dxy.x; y = dxy.y; z = dxy.z; w = dxy.w; }
        public vec4(float dx, float dy, float dz, float dw) { x = dx; y = dy; z = dz; w = dw; }
        //public vec4(Vector4 v) { x = v.X; y = v.Y; z = v.Z; w = v.W; }//From XNA's Vector2

        //public static vec4 Clamp(vec4 v, float a, float b)
        //{
        //    vec4 ret = new vec4();
        //    ret.x = Globals.Clamp(v.x, a, b);
        //    ret.y = Globals.Clamp(v.y, a, b);
        //    ret.z = Globals.Clamp(v.z, a, b);
        //    ret.w = Globals.Clamp(v.w, a, b);
        //    return ret;
        //}
        //public void Clamp(float a, float b)
        //{
        //    this = Clamp(this, a, b);
        //}
        //public void SetMinLightValue(float val)
        //{
        //    //Make sure there's enough light for this.
        //    //Val = the minimum amount of light.
        //    //This isn't perfect
        //    float tot = x + y + z;
        //    if (tot < val)
        //    {
        //        float add = (2 - tot) / val;
        //        x += add;
        //        y += add;
        //        z += add;
        //        x = Globals.Clamp(x, 0, 1);
        //        y = Globals.Clamp(y, 0, 1);
        //        z = Globals.Clamp(z, 0, 1);
        //    }

        //}

        // public Vector4 toXNA() { return new Vector4(x, y, z, w); }
        //public Color toXNAColor() { return new Color(toXNA()); }
        public static vec4 operator -(vec4 d)
        {
            return new vec4(-d.x, -d.y, -d.z, -d.w);
        }

        public static vec4 operator +(vec4 a, vec4 b)
        {
            return new vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }
        public static vec4 operator -(vec4 a, vec4 b)
        {
            return new vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }
        public static vec4 operator *(vec4 a, vec4 b)
        {
            return new vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }
        public static vec4 operator *(vec4 a, float b)
        {
            return new vec4(a.x * b, a.y * b, a.z * b, a.w * b);
        }
        public static vec4 operator /(vec4 a, float b)
        {
            return new vec4(a.x / b, a.y / b, a.z / b, a.w / b);
        }
        public static vec4 Minv(vec4 a, vec4 b)
        {
            vec4 ret = new vec4();
            ret.x = (float)Math.Min(a.x, b.x);
            ret.y = (float)Math.Min(a.y, b.y);
            ret.z = (float)Math.Min(a.z, b.z);
            ret.w = (float)Math.Min(a.w, b.w);
            return ret;
        }
        public static vec4 Maxv(vec4 a, vec4 b)
        {
            vec4 ret = new vec4();
            ret.x = (float)Math.Max(a.x, b.x);
            ret.y = (float)Math.Max(a.y, b.y);
            ret.z = (float)Math.Max(a.z, b.z);
            ret.w = (float)Math.Max(a.w, b.w);
            return ret;
        }

    }

    public struct RaycastHit
    {
        public bool _bHit;    // Whether the ray intersected the box.
        public bool _p1Contained;
        public bool _p2Contained;
        public float _t; // - Time to hit [0,1]
                         //  public void* _pPickData; // picked object (BvhObject3*)
        public vec2 _vNormal; //The normal of the plane the raycast hit.
                              //Do not include ray data for optimization.

        //public RaycastHit()
        //{
        //    reset();
        //}
        public bool trySetClosestHit(ref float closest_t)
        {
            //Easy way of iterating a closest hit.
            if (_bHit && (_t < closest_t))
            {
                closest_t = _t;
                return true;
            }
            return false;
        }
        public void reset()
        {
            _bHit = false;
            _p1Contained = false;
            _p2Contained = false;
            _t = float.MaxValue;
            //  _pPickData = NULL;
        }
        public void copyFrom(RaycastHit bh)
        {
            _bHit = bh._bHit;
            _p1Contained = bh._p1Contained;
            _p2Contained = bh._p2Contained;
            _t = bh._t;
        }
    }

    public struct ProjectedRay
    {
        public vec2 Origin;
        public vec2 Dir;
        public float _t;
        public vec2 _vNormal;

        // Found the following two cool optimizations on WIlliams et. al (U. Utah)
        public vec2 InvDir;
        public int[] Sign;

        public bool IsOpt { get; private set; }    // - return true if  we optimized this

        public float Length;// Max length

        public vec2 Begin() { return Origin; }
        public vec2 End() { return Origin + Dir; }

        public ProjectedRay(vec2 origin, vec2 dir)
        {
            Sign = new int[2];
            Origin = origin;
            Dir = dir;

            IsOpt = false;
            Length = float.MaxValue;//Must be maxvalue
            _t = float.MaxValue;
            _vNormal = new vec2(0, 0);

            //opt()
            //            //**New - optimization
            //http://people.csail.mit.edu/amy/papers/box-jgt.pdf
            //Don't set to zero. We need infinity (or large value) here.
            InvDir.x = 1.0f / Dir.x;
            InvDir.y = 1.0f / Dir.y;

            Sign[0] = (InvDir.x < 0) ? 1 : 0;
            Sign[1] = (InvDir.y < 0) ? 1 : 0;

            IsOpt = true;
        }
        //public void opt()
        //{



        //}
        public bool isHit()
        {
            return _t >= 0.0f && _t <= 1.0f;
        }
        public vec2 HitPoint()
        {
            vec2 ret = Begin() + (End() - Begin()) * _t;
            return ret;
        }
    }

    public struct Box2f
    {
        public vec2 Min;
        public vec2 Max;

        public float Width() { return Max.x - Min.x; }
        public float Height() { return Max.y - Min.y; }

        public vec2 TopRight() { return new vec2(Max.x, Min.y); }
        public vec2 BotRight() { return new vec2(Max.x, Max.y); }
        public vec2 BotLeft() { return new vec2(Min.x, Max.y); }
        public vec2 TopLeft() { return new vec2(Min.x, Min.y); }

        public void Construct(vec2 min, vec2 max)
        {
            Min = min; Max = max;
        }
        public Box2f(float x, float y, float w, float h)
        {
            Min = new vec2(x, y);
            Max = new vec2(w, h) + Min;
        }
        public Box2f(vec2 min, vec2 max)
        {
            Min = min;
            Max = max;
        }
        public vec2 Center()
        {
            return Min + (Max - Min) * 0.5f;
        }
        public static Box2f FlipBoxH(Box2f b, float w)
        {
            //Flip the box inside of a larger box (w)
            Box2f ret = new Box2f();
            ret.Min.x = w - b.Max.x;
            ret.Max.x = w - b.Min.x;

            ret.Min.y = b.Min.y;
            ret.Max.y = b.Max.y;
            return ret;
        }
        public static Box2f FlipBoxV(Box2f b, float h)
        {
            //Flip the box inside of a larger box (h)
            Box2f ret = new Box2f();
            ret.Min.y = h - b.Max.y;
            ret.Max.y = h - b.Min.y;

            ret.Min.x = b.Min.x;
            ret.Max.x = b.Max.x;
            return ret;
        }
        public Rectangle ToXNARect()
        {
            Rectangle r = new Rectangle();

            r.X = (int)(Min.x);
            r.Y = (int)(Min.y);
            r.Width = (int)(Max.x - Min.x);
            r.Height = (int)(Max.y - Min.y);

            return r;
        }

        public static Box2f GetIntersectionBox_Inclusive(Box2f a, Box2f b)
        {
            Box2f ret = new Box2f();

            ret.Min.x = Single.MaxValue;
            ret.Min.y = Single.MaxValue;
            ret.Max.x = -Single.MaxValue;
            ret.Max.y = -Single.MaxValue;


            if (a.Min.x >= b.Min.x && a.Min.x <= b.Max.x)
            {
                ret.Min.x = Math.Min(ret.Min.x, a.Min.x);
            }
            if (a.Max.x <= b.Max.x && a.Max.x >= b.Min.x)
            {
                ret.Max.x = Math.Max(ret.Max.x, a.Max.x);
            }
            if (a.Min.y >= b.Min.y && a.Min.y <= b.Max.y)
            {
                ret.Min.y = Math.Min(ret.Min.y, a.Min.y);
            }
            if (a.Max.y <= b.Max.y && a.Max.y >= b.Min.y)
            {
                ret.Max.y = Math.Max(ret.Max.y, a.Max.y);
            }

            if (b.Min.x >= a.Min.x && b.Min.x <= a.Max.x)
            {
                ret.Min.x = Math.Min(ret.Min.x, b.Min.x);
            }
            if (b.Max.x <= a.Max.x && b.Max.x >= a.Min.x)
            {
                ret.Max.x = Math.Max(ret.Max.x, b.Max.x);
            }
            if (b.Min.y >= a.Min.y && b.Min.y <= a.Max.y)
            {
                ret.Min.y = Math.Min(ret.Min.y, b.Min.y);
            }
            if (b.Max.y <= a.Max.y && b.Max.y >= a.Min.y)
            {
                ret.Max.y = Math.Max(ret.Max.y, b.Max.y);
            }
            return ret;
        }

        public void GenResetExtents()
        {
            Min = new vec2(Single.MaxValue, Single.MaxValue);
            Max = new vec2(-Single.MaxValue, -Single.MaxValue);
        }
        public void ExpandByPoint(vec2 v)
        {
            Min = vec2.Minv(Min, v);
            Max = vec2.Maxv(Max, v);
        }
        public bool BoxIntersect_EasyOut_Inclusive(Box2f cc)
        {
            return cc.Min.x <= Max.x && cc.Min.y <= Max.y && Min.x <= cc.Max.x && Min.y <= cc.Max.y;
        }
        public bool ContainsPointInclusive(vec2 point)
        {
            if (point.x < Min.x)
                return false;
            if (point.y < Min.y)
                return false;
            if (point.x > Max.x)
                return false;
            if (point.y > Max.y)
                return false;
            return true;
        }
        private vec2 bounds(int x)
        {
            if (x == 0) return Min;
            return Max;
        }
        public bool RayIntersect(ProjectedRay ray, ref RaycastHit bh)
        {
            if (ray.IsOpt == false)
            {
                //Error.
                System.Diagnostics.Debugger.Break();
            }

            float txmin, txmax, tymin, tymax;
            bool bHit;

            txmin = (bounds(ray.Sign[0]).x - ray.Origin.x) * ray.InvDir.x;
            txmax = (bounds(1 - ray.Sign[0]).x - ray.Origin.x) * ray.InvDir.x;

            tymin = (bounds(ray.Sign[1]).y - ray.Origin.y) * ray.InvDir.y;
            tymax = (bounds(1 - ray.Sign[1]).y - ray.Origin.y) * ray.InvDir.y;

            if ((txmin > tymax) || (tymin > txmax))
            {
                // if (bh != null)
                // {
                bh._bHit = false;
                // }
                return false;
            }
            if (tymin > txmin)
            {
                txmin = tymin;
            }
            if (tymax < txmax)
            {
                txmax = tymax;
            }

            bHit = ((txmin >= 0.0f) && (txmax <= ray.Length));

            //**Note changed 20151105 - this is not [0,1] this is the lenth along the line in which 
            // the ray enters and exits the cube, so any value less than the maximum is valid

            // if (bh != null)
            // {
            bh._bHit = bHit;
            bh._t = txmin;
            // }

            return bHit;
        }
    }



}
