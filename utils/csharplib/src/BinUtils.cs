using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Proteus.src
{
    public class Vec4
    {
        public float x { get; set; }
        public float y { get; set; }
        public float z { get; set; }
        public float w { get; set; }
    }
    public class Vec3
    {
        public float x { get; set; }
        public float y { get; set; }
        public float z { get; set; }
    }
    public class Vec2
    {
        public float x { get; set; }
        public float y { get; set; }
    }
    public class Mat4
    {
        public float[] m { get; set; } = new float[16];
    }
    public class PRSKeyframe
    {
        public int Index { get; set; }
        public Vec3 Pos { get; set; } = new Vec3();
        public Vec4 Rot { get; set; } = new Vec4();
        public Vec3 Scale { get; set; } = new Vec3();
        public byte[] Serialize()
        {
            byte[] _final = new byte[0];

            _final = BinUtils.WriteVec3(Pos, _final);
            _final = BinUtils.WriteVec4(Rot, _final);
            _final = BinUtils.WriteVec3(Scale, _final);

            return _final;
        }
    }

    public static class BinUtils
    {
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool FlushFileBuffers(IntPtr handle);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool FlushFileBuffers(SafeFileHandle hFile);
        public static byte[] SerializeUTF8Path(string path)
        {
            byte[] ret = new byte[0];
            byte[] utf8Path = System.Text.Encoding.UTF8.GetBytes(path);
            Int16 pathlen = (Int16)utf8Path.Length;
            ret = BufferCombine(ret, BitConverter.GetBytes(pathlen));
            ret = BufferCombine(ret, utf8Path);
            return ret;
        }

        public static byte[] WriteBool(bool n, byte[] buf)
        {
            byte b = n ? (byte)1 : (byte)0;

            //https://stackoverflow.com/questions/19432426/why-bitconverter-getbytes-accept-argument-of-type-byte-and-returns-always
            return BufferCombine(buf, new byte[] { b });
        }
        public static byte[] WriteChar(char n, byte[] buf)
        {
            //https://stackoverflow.com/questions/19432426/why-bitconverter-getbytes-accept-argument-of-type-byte-and-returns-always
            byte b = Encoding.ASCII.GetBytes(n.ToString())[0];
            return BufferCombine(buf, new byte[] { b });
        }
        public static byte[] WriteByte(byte n, byte[] buf)
        {
            byte[] bb = BitConverter.GetBytes(n);
            return BufferCombine(buf, bb);
        }
        public static byte[] WriteFloat(float n, byte[] buf)
        {
            byte[] bb = BitConverter.GetBytes(n);
            return BufferCombine(buf, bb);
        }
        public static byte[] WriteInt32(Int32 n, byte[] buf)
        {
            byte[] bb = BitConverter.GetBytes(n);
            return BufferCombine(buf, bb);
        }
        public static byte[] WriteString(string s, byte[] buf)
        {
            return BufferCombine(buf, SerializeUTF8Path(s));
        }
        public static byte[] WriteMat4(Mat4 m, byte[] buf)
        {
            foreach (float f in m.m)
            {
                buf = WriteFloat(f, buf);
            }
            return buf;
        }
        public static byte[] WriteVec2(Vec2 v, byte[] buf)
        {
            buf = WriteFloat(v.x, buf);
            buf = WriteFloat(v.y, buf);
            return buf;
        }
        public static byte[] WriteVec3(Vec3 v, byte[] buf)
        {
            buf = WriteFloat(v.x, buf);
            buf = WriteFloat(v.y, buf);
            buf = WriteFloat(v.z, buf);
            return buf;
        }
        public static byte[] WriteVec4(Vec4 v, byte[] buf)
        {
            buf = WriteFloat(v.x, buf);
            buf = WriteFloat(v.y, buf);
            buf = WriteFloat(v.z, buf);
            buf = WriteFloat(v.w, buf);
            return buf;
        }
        public static byte[] BufferCombine(byte[] first, byte[] second)
        {
            byte[] ret = new byte[first.Length + second.Length];
            Buffer.BlockCopy(first, 0, ret, 0, first.Length);
            Buffer.BlockCopy(second, 0, ret, first.Length, second.Length);
            return ret;
        }
        public static void WriteBytesViaStream(string strFilePath, byte[] fileBytes)
        {
            System.IO.FileStream fs = null;
            try
            {
                fs = new System.IO.FileStream(strFilePath, System.IO.FileMode.Create, System.IO.FileAccess.Write);
                using (System.IO.BinaryWriter writer = new System.IO.BinaryWriter(fs))
                {
                    writer.Write(fileBytes, 0, fileBytes.Length);
                    writer.Flush();
#pragma warning disable 618, 612 // disable stream.Handle deprecation warning.
                    if (!FlushFileBuffers(fs.Handle))
#pragma warning restore 618, 612
                    {
                        Int32 err = Marshal.GetLastWin32Error();
                        throw new System.ComponentModel.Win32Exception(err, "Win32 FlushFileBuffers returned error for " + fs.Name);
                    }
                    fs = null;
                }
            }
            finally
            {
                if (fs != null)
                    fs.Dispose();
            }
            System.IO.File.SetAttributes(strFilePath, System.IO.FileAttributes.Normal);
        }
    }
}
