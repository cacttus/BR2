// Decompiled with JetBrains decompiler
// Type: IsoPack.BinUtils
// Assembly: IsoTool, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null
// MVID: A40E7877-59D4-416C-9526-ACFD66F37CC4
// Assembly location: C:\Program Files\Iso Tool\IsoTool.exe

using Microsoft.Win32.SafeHandles;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;


namespace GameHost
{
    /// <summary>
    /// Class for serializing data.
    /// </summary>
    public static class BinUtils
    {
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool FlushFileBuffers(IntPtr handle);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool FlushFileBuffers(SafeFileHandle hFile);

        public static byte[] BufferCombine(byte[] first, byte[] second)
        {
            byte[] numArray = new byte[first.Length + second.Length];
            Buffer.BlockCopy((Array)first, 0, (Array)numArray, 0, first.Length);
            Buffer.BlockCopy((Array)second, 0, (Array)numArray, first.Length, second.Length);
            return numArray;
        }

        public static string ReadString(BinaryReader reader)
        {
            short num = reader.ReadInt16();
            return Encoding.UTF8.GetString(reader.ReadBytes((int)num), 0, (int)num);
        }

        public static Int64 ReadInt64(BinaryReader reader)
        {
            return reader.ReadInt64();
        }

        public static int ReadInt32(BinaryReader reader)
        {
            return reader.ReadInt32();
        }

        public static short ReadInt16(BinaryReader reader)
        {
            return reader.ReadInt16();
        }

        public static bool ReadBool(BinaryReader reader)
        {
            return reader.ReadBoolean();
        }

        public static float ReadFloat(BinaryReader reader)
        {
            return reader.ReadSingle();
        }

        public static void WriteBool(bool n, BinaryWriter w)
        {
            byte num = n ? (byte)1 : (byte)0;
            w.Write(n);
        }

        public static void WriteChar(char n, BinaryWriter w)
        {
            byte num = Encoding.ASCII.GetBytes(n.ToString())[0];
            w.Write(num);
        }

        public static void WriteFloat(float n, BinaryWriter w)
        {
            w.Write(n);
        }

        public static void WriteInt32(int n, BinaryWriter w)
        {
            w.Write(n);
        }

        public static void WriteInt64(Int64 n, BinaryWriter w)
        {
            w.Write(n);
        }

        public static void WriteInt16(short n, BinaryWriter w)
        {
            w.Write(BitConverter.GetBytes(n));
        }

        public static void WriteString(string s, BinaryWriter w)
        {
            if (s == null)
                s = "";
            byte[] bytes = Encoding.UTF8.GetBytes(s);
            BinUtils.WriteInt16((short)bytes.Length, w);
            BinUtils.WriteBlock(bytes, w);
        }

        public static void WriteBlock(byte[] b, BinaryWriter w)
        {
            w.Write(b);
        }

        public static byte[] ReadBlock(int count, BinaryReader w)
        {
            return w.ReadBytes(count);
        }
    }
}
