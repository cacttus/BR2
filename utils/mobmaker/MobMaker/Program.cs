using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Collections;

namespace MobMaker
{
    public enum MobMakerMode {  RunOnce, Poll, Wait}
    public class MobMakerArgs
    {
        public bool ShowTitle { get; set; } = true;
        public bool Force { get; set; } = true;
        public int PollMilliseconds { get; set; } = 200;
        public MobMakerMode MobMakerMode { get; set; } = MobMakerMode.RunOnce;
        public string GameDat { get; set; }
        string GetCleanArg(string[] args, ref int i)
        {
            if (i >= args.Length)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Error:\r\n  invalid argument switch specified or no parameter given.");
                return "";
            }
            else
            {
                return args[i++].ToLower().Replace("\"","");
            }
        }
        public void Parse(string[] args)
        {
            int iArg=0;
            while(iArg< args.Length)
            {
                string tok = GetCleanArg(args, ref iArg);
                if(tok.ToLower() == "-gdat")
                {
                    GameDat = GetCleanArg(args, ref iArg);
                }
                else if (tok.ToLower() == "-force")
                {
                    Force = true;
                }
                else if (tok == "-poll")
                {
                    int p = 200;
                    Int32.TryParse(GetCleanArg(args, ref iArg), out p);
                    PollMilliseconds = p;
                    MobMakerMode = MobMakerMode.Poll;
                }
                else if (tok == "-waitforenterkey")
                {
                    MobMakerMode = MobMakerMode.Wait;
                }
                else if (tok == "-runonce")
                {
                    MobMakerMode = MobMakerMode.RunOnce;
                }
                else if (tok == "-notitle")
                {
                    ShowTitle = false;
                }
            }
        }
        public void Validate()
        {
            if(MobMakerMode == MobMakerMode.Poll && Force)
            {
                Console.WriteLine("Error:\r\n  -force invalid with -poll");
                Force = false;
            }
            if (PollMilliseconds < 50)
            {
                Console.WriteLine("Error:\r\n  Poll milliseconds to0 low! default to 50ms");

                PollMilliseconds = 50;
            }
            if (GameDat == "")
            {
                GameDat = System.IO.Path.Combine(Environment.CurrentDirectory, "game.dat");
            }
            if (string.IsNullOrEmpty(GameDat))
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Error:\r\n  game.dat wasn't supplied! Place this exe in the same directory as game.dat," +
                    "\r\n or add the path to game.dat as first argument.");
                Console.ForegroundColor = ConsoleColor.White;
                Console.ReadKey();
                System.Environment.Exit(0);
            }
            else if (!System.IO.File.Exists(GameDat))
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Error:\r\n  " + GameDat +
                    "\r\n doesn't exist! Place this exe in the same directory as game.dat," +
                    "\r\n or add the path to game.dat as first argument.");
                Console.ForegroundColor = ConsoleColor.White;
                Console.ReadKey();
                System.Environment.Exit(0);
            }
            else
            {
                Console.WriteLine("Found '" + GameDat + "'");
            }
        }
    }
    class Program
    {
        static float c_ufVersion = 0.02f;
        static void Main(string[] args)
        {

            MobMakerArgs pArgs = new MobMakerArgs();
            pArgs.Parse(args);
            pArgs.Validate();

            if (pArgs.ShowTitle)
            {
                Console.WriteLine("#################################");
                Console.WriteLine("# Blender Batch Converter " + c_ufVersion);
                Console.WriteLine("# github/MetalMario971 12/29/17, 1/26/18");
                Console.WriteLine("# Usage: set MOB_BPATH MOB_PYCON MOB_BMDIR in game.dat");
                Console.WriteLine("#            specify blender .blend files with MOB [x] ");
                Console.WriteLine("# Args: [-gdat \"game.dat\"] [-poll \"millis\" -runonce -waitforenterkey] -notitle -force");
            }

            //Set the current dir tot he dir of the game.dat
            string dirname = System.IO.Path.GetDirectoryName(pArgs.GameDat);
            Console.WriteLine("Directory set to '" + dirname + "'");
            System.IO.Directory.SetCurrentDirectory(dirname);

            MobMaker mm = new MobMaker();

            if (pArgs.MobMakerMode == MobMakerMode.Poll)
            {
                Console.WriteLine("\nPolling for changes..");
            }
            while (true)
            {
                if (pArgs.MobMakerMode == MobMakerMode.Poll)
                {
                    System.Threading.Thread.Sleep(pArgs.PollMilliseconds);
                }
                else if (pArgs.MobMakerMode == MobMakerMode.Wait)
                {
                    Console.WriteLine("Press any key to exit..Press Enter to process again.");
                    if (Console.ReadKey().Key != ConsoleKey.Enter)
                    {
                        break;
                    }
                }

                mm.Convert(pArgs.GameDat, pArgs.Force);

                if (pArgs.MobMakerMode == MobMakerMode.RunOnce)
                {
                    break;
                }
            }

        }
    }
    public enum ConvertInfoStatus { BlendFileNotFound, NoChanges, Error, Success }
    public class ConvertInfo
    {
        int t_start;
        public string Name { get; set; } = "";
        public float Time { get; set; }
        public int Errors { get; set; }
        public string FileName { get; set; }
        public int ConvertIndex { get; set; }
        public string MobPathOut { get; set; }
        public string MobBinPathOut { get; set; }
        public ConvertInfoStatus ConvertInfoStatus { get; set; }
        public string BlendFileName { get; set; } = "";
        public void Print()
        {


            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.Write("  " + Name);
            Console.ForegroundColor = ConsoleColor.Magenta;

            if (ConvertInfoStatus == ConvertInfoStatus.NoChanges)
            {
                Console.WriteLine("..No Changes Detected");
            }
            else if (ConvertInfoStatus == ConvertInfoStatus.BlendFileNotFound)
            {
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.Write(".. " + BlendFileName);
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine(" file Not Found.");
            }
            else
            {
                Console.Write(", " + Time.ToString() + " seconds. ");
                if (ConvertInfoStatus == ConvertInfoStatus.Success)
                {

                }
                else if (ConvertInfoStatus == ConvertInfoStatus.Error)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                }
                Console.WriteLine("  " + Errors.ToString() + " Errors");
                Console.WriteLine("    MOB: " + MobPathOut);
                Console.WriteLine("    MBI: " + MobBinPathOut);
            }
            Console.ForegroundColor = ConsoleColor.White;
        }
        public void Start()
        {
            t_start = System.Environment.TickCount;
        }
        public void End()
        {
            Time = ((float)System.Environment.TickCount - (float)t_start) / 1000.0f;
        }
    }
    public class MobMaker
    {
        string localPath;
        List<string> _lstMobs;
        string _strModelsDir = "";
        string _strMobDir = "";
        string _strMobBinDir = "";
        string _strBlenderExePath = "";
        string _strPythonScriptPath = "";

        public bool Convert(string gameDatFile, bool bForce)
        {
            List<ConvertInfo> lstConvertInfos = new List<ConvertInfo>();//just an easy way to tell what we did.
            //log("Loading '" + gameDatFile + "'");
            if (LoadFiles(gameDatFile))
            {
                //log("Converting " + _lstMobs.Count + " mobs..");
                for (int i = 0; i < _lstMobs.Count; ++i)
                {
                    ConvertInfo inf = new ConvertInfo();
                    inf.Name = System.IO.Path.GetFileName(_lstMobs[i]);
                    inf.FileName = _lstMobs[i];
                    inf.ConvertIndex = i;

                    lstConvertInfos.Add(inf);

                   // log(" " + i.ToString() + " " + _lstMobs[i]);
                }
                foreach (ConvertInfo inf in lstConvertInfos)
                {
                    inf.Start();
                    try
                    {
                        inf.MobPathOut = ConvertToMob(inf, bForce);
                        inf.MobBinPathOut = ConvertToMobBin(inf.FileName);
                    }
                    catch (Exception ex)
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine(ex.ToString());
                        Console.ForegroundColor = ConsoleColor.White;
                        inf.Errors++;
                    }
                    inf.End();
                }
            }
            //log("Done.");

            bool bPrint = false;
            foreach (ConvertInfo inf in lstConvertInfos)
            {
                if(inf.ConvertInfoStatus != ConvertInfoStatus.NoChanges)
                {
                    bPrint = true;
                }
            }
            if (bPrint)
            {
                foreach (ConvertInfo inf in lstConvertInfos)
                {
                    inf.Print();
                }
                log("\nDone..\nPolling for changes..");
            }

            return true;
        }
        private string ConvertToMob(ConvertInfo inf, bool bForce)
        {
            string fn = StripDoubleQuotes(inf.FileName);
            fn = System.IO.Path.GetFileNameWithoutExtension(fn);
            fn = fn + ".blend";

            string strBlenderFullPath = "";
            string strMobPathOut = "";
            string strOutputPath = _strMobDir;

            
            //File name is also the directory of the model.
            strBlenderFullPath = System.IO.Path.Combine(System.IO.Path.Combine(_strModelsDir, inf.FileName), fn);
            inf.BlendFileName = strBlenderFullPath;
            if (System.IO.File.Exists(strBlenderFullPath))
            {
                string strFullPathForBlender = System.IO.Path.GetFullPath(strOutputPath);
                strMobPathOut = ConvertFile(strBlenderFullPath, strFullPathForBlender, inf, bForce);
            }
            else
            {
                inf.ConvertInfoStatus = ConvertInfoStatus.BlendFileNotFound;
            }

            return strMobPathOut;
        }
        private string ConvertToMobBin(string file)
        {
            string strMobBinPath = "";
            string strMobPath = System.IO.Path.Combine(_strMobDir, StripDoubleQuotes(file), StripDoubleQuotes(file) + ".MOB");
            if (System.IO.File.Exists(strMobPath))
            {
                if (!System.IO.Directory.Exists(_strMobBinDir))
                {
                    System.IO.Directory.CreateDirectory(_strMobBinDir);
                }

                string strMobFolder = System.IO.Path.Combine(_strMobBinDir, StripDoubleQuotes(file));
                if (!System.IO.Directory.Exists(strMobFolder))
                {
                    System.IO.Directory.CreateDirectory(strMobFolder);
                }

                strMobBinPath = System.IO.Path.Combine(strMobFolder, StripDoubleQuotes(file) + ".mbi");

                //log("Converting \"" + strMobPath + "\" to Bin \"" + strMobBinPath + "\"");
                //log("Loading Mob..");
                //MobFile mb = new MobFile(strMobPath);
                //mb.loadAndParse();

                //log("Writing..");
                //MobBin mbin = new MobBin();
                //mbin.WriteMobBin(mb.Models, strMobPath, strMobBinPath);
                //log("..Done");
            }

            return strMobBinPath;
        }

        private void log(string str)
        {
            System.Console.WriteLine(str);
        }
        private void logError(string str)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            log(str);
            Console.ForegroundColor = ConsoleColor.White;
        }
        private bool LoadFiles(string file)
        {
            ObFile ob = new ObFile(file);
            ob.loadAndParse();
            if (ob.getErrorString().Length > 0)
            {
                logError(ob.getErrorString());
            }
            _lstMobs = ob._lstMobs;

            _strModelsDir = ob.BlenderModelDir;
            _strMobDir = ob.MobDir;
            _strMobBinDir = ob.MobBinDir;
            _strBlenderExePath = ob.BlenderExePath;
            _strPythonScriptPath = ob.PythonScriptPath;

            return ob.Errors.Count == 0;
        }
        string StripDoubleQuotes(string s)
        {
            return s.Replace("\"", "");
        }
        bool ShouldConvert(string strBlendFile, string strOutputPath, string strModelName)
        {
            string blendDir = System.IO.Path.GetDirectoryName(strBlendFile);
            if (!System.IO.Directory.Exists(blendDir))
            {
                return true;
            }

            string outFile = System.IO.Path.Combine(strOutputPath, strModelName, strModelName + ".mob");
            if (!System.IO.File.Exists(outFile))
            {
                return true;
            }

            DateTime dFile = new System.IO.FileInfo(outFile).LastWriteTime;
            string[] files = System.IO.Directory.GetFiles(blendDir);
            foreach (string fileName in files)
            {
                if (new System.IO.FileInfo(fileName).LastWriteTime > dFile)
                {
                    return true;
                }
            }

            return false;
        }
        string ConvertFile(string strBlendFile, string strOutputPath, ConvertInfo inf, bool bForce)
        {
            //file is blend file.
            //string file
            //search blend directory

            if(bForce==false && ShouldConvert(strBlendFile, strOutputPath, inf.FileName) == false)
            {
                inf.ConvertInfoStatus = ConvertInfoStatus.NoChanges;
                return "";
            }
            log("Converting \"" + strBlendFile + "\"");

            string strMobPathOut = "";
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.FileName = "cmd.exe";
            startInfo.RedirectStandardInput = true;
            startInfo.RedirectStandardOutput = true;
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = false;
            //startInfo.Arguments = "/c \"" + _strBlenderExePath + " /b --background --python \"\"" + _strPythonScriptPath + "\"\" \"";
            log("Executing " + startInfo.Arguments);
            process.StartInfo = startInfo;
            process.Start();

            //https://blender.stackexchange.com/questions/6817/how-to-pass-command-line-arguments-to-a-blender-python-script
            //https://blender.stackexchange.com/questions/1365/how-can-i-run-blender-from-command-line-or-a-python-script-without-opening-a-gui
            //**NOTE THE space after the --
            string fileArg = "\"" + _strBlenderExePath + "\" \"" + strBlendFile + "\" --background " +
                "--python \"" + _strPythonScriptPath + "\" -- -outpath \"" + strOutputPath + "\"";

            process.StandardInput.WriteLine(fileArg);
            process.StandardInput.Flush();
            process.StandardInput.Close();

            //set to success, set to error later.
            inf.ConvertInfoStatus = ConvertInfoStatus.Success;

            string output = process.StandardOutput.ReadToEnd();
            int a, b;
            a = output.IndexOf("$1");
            b = output.LastIndexOf("$1");
            if (a > 0 && b > 0)
            {
                strMobPathOut = output.Substring(a + 2, b - (a + 2));
                strMobPathOut = strMobPathOut.Replace("\"", "");
            }

            //Write RED if there was an error.
            Console.ForegroundColor = ConsoleColor.Yellow;
            a = output.IndexOf("$2");
            b = output.LastIndexOf("$2");
            if (a > 0 && b > 0)
            {
                string strError = output.Substring(a + 2, b - (a + 2));
                if (strError.ToLower() == "error")
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    inf.Errors++;
                    inf.ConvertInfoStatus = ConvertInfoStatus.Error;
                }
            }


            Console.WriteLine(output);
            Console.ForegroundColor = ConsoleColor.White;
            return strMobPathOut;
        }
    }
}

//#region MOBBIN
//
//
//public class MobBin
//{
//    static float FileVersion = 0.2f;
//    public void WriteMobBin(List<Model> models, string mobPath, string mobBinPath)
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteChar('M', _final);
//        _final = BinUtils.WriteChar('B', _final);
//        _final = BinUtils.WriteChar('I', _final);
//        _final = BinUtils.WriteChar('1', _final);
//        _final = BinUtils.WriteFloat(FileVersion, _final);
//        _final = BinUtils.WriteInt32(models.Count, _final);
//        foreach (Model mod in models)
//        {
//            _final = BinUtils.BufferCombine(_final, mod.Serialize());
//        }

//        //**Write all PNG textures onto file.
//        List<string> texs = new List<string>();
//        foreach (Model mod in models)
//        {
//            foreach (Mesh m in mod.Meshes)
//            {
//                if (m.Material != null)
//                {
//                    foreach (Texture t in m.Material.Textures)
//                    {
//                        string texdir = System.IO.Path.GetDirectoryName(mobPath);
//                        string texfn = t.FileName.Replace("\"", "");
//                        texs.Add(System.IO.Path.Combine(texdir, texfn));
//                    }
//                }
//            }
//        }
//        _final = BinUtils.WriteInt32(texs.Count(), _final);
//        foreach (string str in texs)
//        {
//            byte[] texBytes = System.IO.File.ReadAllBytes(str);
//            _final = BinUtils.WriteInt32(texBytes.Length, _final);
//            _final = BinUtils.BufferCombine(_final, texBytes);
//        }

//        //This is too slow
//        // Build the Huffman tree
//        //  MobMaker.HuffmanTree huffmanTree = new MobMaker.HuffmanTree();
//        //  int beforeCompress = _final.Length;
//        //  _final = huffmanTree.EncodeAsBytes(_final);
//        //  int afterCompress = _final.Length;
//        //  Console.WriteLine("Compressed " + beforeCompress + "B to " + afterCompress + "B.");


//        BinUtils.WriteBytesViaStream(mobBinPath, _final);
//    }

//}
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


//#endregion
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//#region MOB_STRUCTS
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
//public enum ParentType
//{
//    None, Bone, Object, Armature
//}
//public class Bone
//{
//    public ParentType ParentType { get; set; }
//    public string Name { get; set; }
//    public string ParentName { get; set; }
//    public int Ordinal { get; set; }
//    public Mat4 BindWorld { get; set; } = new Mat4();
//    public Vec3 Head { get; set; } = new Vec3();
//    public Vec3 Tail { get; set; } = new Vec3();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteString(Name, _final);
//        _final = BinUtils.WriteString(ParentName, _final);
//        _final = BinUtils.WriteInt32(Ordinal, _final);
//        _final = BinUtils.WriteMat4(BindWorld, _final);
//        _final = BinUtils.WriteVec3(Head, _final);
//        _final = BinUtils.WriteVec3(Tail, _final);

//        return _final;
//    }
//}
//public class ActionGroup
//{
//    public string Name { get; set; }
//    public List<Action> Actions { get; set; } = new List<Action>();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteString(Name, _final);
//        _final = BinUtils.WriteInt32(Actions.Count, _final);
//        foreach (Action a in Actions)
//        {
//            _final = BinUtils.BufferCombine(_final, a.Serialize());
//        }

//        return _final;
//    }
//}
//public class Action
//{
//    //  public string Name { get; set; }
//    public string ObjectName { get; set; }
//    public List<PRSKeyframe> Keyframes = new List<PRSKeyframe>();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        // _final = BinUtils.WriteString(Name, _final);
//        _final = BinUtils.WriteString(ObjectName, _final);
//        _final = BinUtils.WriteInt32(Keyframes.Count, _final);
//        foreach (PRSKeyframe kf in Keyframes)
//        {
//            _final = BinUtils.BufferCombine(_final, kf.Serialize());
//        }

//        return _final;
//    }
//}
//public class Armature
//{
//    public string Name { get; set; }
//    public string ParentName { get; set; }
//    public ParentType ParentType { get; set; }
//    public int ArmatureId { get; set; }//not part of mob file
//    public Mat4 World { get; set; } = new Mat4();
//    public Mat4 ParentInverse { get; set; } = new Mat4();
//    public List<Bone> Bones { get; set; } = new List<Bone>();

//    private static int _id = 0; //Gives armatures arbitrary ids.

//    public Armature()
//    {
//        ArmatureId = _id++;
//    }

//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteString(Name, _final);
//        _final = BinUtils.WriteString(ParentName, _final);
//        _final = BinUtils.WriteInt32(ArmatureId, _final);
//        _final = BinUtils.WriteMat4(World, _final);
//        _final = BinUtils.WriteMat4(ParentInverse, _final);
//        _final = BinUtils.WriteInt32(Bones.Count, _final);
//        foreach (Bone b in Bones)
//        {
//            _final = BinUtils.BufferCombine(_final, b.Serialize());
//        }

//        return _final;
//    }
//}
//public class Texture
//{
//    public string FileName { get; set; }
//    public float Amount { get; set; }
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteString(FileName, _final);
//        _final = BinUtils.WriteFloat(Amount, _final);

//        return _final;
//    }
//}
//public class Material
//{
//    public string Name { get; set; }
//    public List<Texture> Textures = new List<Texture>();
//    public Vec4 Ambient = new Vec4();
//    public Vec4 Specular = new Vec4();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteString(Name, _final);
//        _final = BinUtils.WriteInt32(Textures.Count, _final);
//        foreach (Texture t in Textures)
//        {
//            _final = BinUtils.BufferCombine(_final, t.Serialize());
//        }
//        _final = BinUtils.WriteVec4(Ambient, _final);
//        _final = BinUtils.WriteVec4(Specular, _final);

//        return _final;
//    }
//}
//public class v_v3n3x2
//{
//    public Vec3 v = new Vec3();
//    public Vec3 n = new Vec3();
//    public Vec2 x = new Vec2();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteVec3(v, _final);
//        _final = BinUtils.WriteVec3(n, _final);
//        _final = BinUtils.WriteVec2(x, _final);
//        return _final;
//    }
//}
//public class v_v3x2
//{
//    public Vec3 v = new Vec3();
//    public Vec2 x = new Vec2();

//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteVec3(v, _final);
//        _final = BinUtils.WriteVec2(x, _final);
//        return _final;
//    }
//}
//public class v_v3n3
//{
//    public Vec3 v = new Vec3();
//    public Vec3 n = new Vec3();

//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteVec3(v, _final);
//        _final = BinUtils.WriteVec3(n, _final);
//        return _final;
//    }
//}
//public class v_v3
//{
//    public Vec3 v = new Vec3();

//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteVec3(v, _final);
//        return _final;
//    }
//}
//public class GpuAnimatedMeshWeightData
//{
//    public int Offset { get; set; }    // offset into weight buffer (GpuAnimatedMeshWeight)
//    public int Count { get; set; }    // count of weights
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteInt32(Offset, _final);
//        _final = BinUtils.WriteInt32(Count, _final);
//        return _final;
//    }
//};
//public class GpuAnimatedMeshWeight
//{
//    public int ArmJointOffset { get; set; }    // - Offset into joint matrix buffer 12/2017 - sorted by armature! (GpuAnimatedMeshBindMatrix / GpuAnimatedMeshSkinMatrix)
//    public float Weight { get; set; }        // weight of joint
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteInt32(ArmJointOffset, _final);
//        _final = BinUtils.WriteFloat(Weight, _final);
//        return _final;
//    }
//};
//public class VertexWeightGroup
//{
//    public List<VertexArmWeight> Weights { get; set; } = new List<VertexArmWeight>();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteInt32(Weights.Count, _final);
//        foreach (VertexArmWeight w in Weights)
//        {
//            _final = BinUtils.BufferCombine(_final, w.Serialize());
//        }
//        return _final;
//    }
//}
//public class VertexArmWeight
//{
//    public int ArmatureId { get; set; }
//    public List<VertexBoneWeight> Weights { get; set; } = new List<VertexBoneWeight>();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteInt32(ArmatureId, _final);
//        _final = BinUtils.WriteInt32(Weights.Count, _final);
//        foreach (VertexBoneWeight w in Weights)
//        {
//            _final = BinUtils.BufferCombine(_final, w.Serialize());
//        }
//        return _final;
//    }
//}
//public class VertexBoneWeight
//{
//    public int BoneId { get; set; }
//    public float Weight { get; set; }
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];
//        _final = BinUtils.WriteInt32(BoneId, _final);
//        _final = BinUtils.WriteFloat(Weight, _final);
//        return _final;
//    }
//}
//public enum PhysicsShapeType
//{
//    None, Hull, Sphere, AABox
//}
//public class PhysicsShape
//{
//    public PhysicsShapeType Type { get; set; }
//    public bool Kinematic { get; set; }
//    public bool Dynamic { get; set; }
//}
//public class Mesh
//{
//    public List<Armature> Armatures { get; set; } = new List<Armature>();
//    public PhysicsShape PhysicsShape { get; set; } = null;
//    public bool HideRender { get; set; } = false;
//    public string Name { get; set; }
//    public string ParentName { get; set; }
//    public ParentType ParentType { get; set; }//TODO
//    public Mat4 Basis { get; set; } = new Mat4();
//    public Mat4 ParentInverse { get; set; } = new Mat4();
//    // public List<v_v3x2n3> TempFragments = new List<v_v3x2n3>();
//    public List<v_v3n3x2> Fragments_v3n3x2 = new List<v_v3n3x2>();
//    public List<v_v3x2> Fragments_v3x2 = new List<v_v3x2>();
//    public List<v_v3n3> Fragments_v3n3 = new List<v_v3n3>();
//    public List<v_v3> Fragments_v3 = new List<v_v3>();
//    public List<VertexWeightGroup> Weights = new List<VertexWeightGroup>();//Should equal the number of frags.
//    public List<GpuAnimatedMeshWeight> GpuWeights = new List<GpuAnimatedMeshWeight>();//Should equal the number of frags.
//    public List<GpuAnimatedMeshWeightData> GpuWeightData = new List<GpuAnimatedMeshWeightData>();//Should equal the number of frags.
//    public List<int> Indexes = new List<int>();
//    public bool FlipTris = false;
//    public bool SwapUvs = false;
//    public Material Material { get; set; }
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteString(Name, _final);
//        _final = BinUtils.WriteString(ParentName, _final);
//        _final = BinUtils.WriteMat4(Basis, _final);
//        _final = BinUtils.WriteMat4(ParentInverse, _final);
//        _final = BinUtils.WriteBool(HideRender, _final);
//        //if physics shape is not null then 12 bytes follow
//        _final = BinUtils.WriteBool(PhysicsShape == null, _final);
//        if (PhysicsShape != null)
//        {
//            _final = BinUtils.WriteInt32((Int32)PhysicsShape.Type, _final);
//            _final = BinUtils.WriteBool(PhysicsShape.Kinematic, _final);
//            _final = BinUtils.WriteBool(PhysicsShape.Dynamic, _final);
//        }
//        if (Fragments_v3n3x2.Count > 0)
//        {
//            _final = BinUtils.WriteInt32(0, _final);
//            _final = BinUtils.WriteInt32(Fragments_v3n3x2.Count, _final);
//            foreach (v_v3n3x2 frag in Fragments_v3n3x2)
//            {
//                _final = BinUtils.BufferCombine(_final, frag.Serialize());
//            }
//        }
//        else if (Fragments_v3x2.Count > 0)
//        {
//            _final = BinUtils.WriteInt32(1, _final);
//            _final = BinUtils.WriteInt32(Fragments_v3x2.Count, _final);
//            foreach (v_v3x2 frag in Fragments_v3x2)
//            {
//                _final = BinUtils.BufferCombine(_final, frag.Serialize());
//            }
//        }
//        else if (Fragments_v3n3.Count > 0)
//        {
//            _final = BinUtils.WriteInt32(1, _final);
//            _final = BinUtils.WriteInt32(Fragments_v3n3.Count, _final);
//            foreach (v_v3n3 frag in Fragments_v3n3)
//            {
//                _final = BinUtils.BufferCombine(_final, frag.Serialize());
//            }
//        }
//        else if (Fragments_v3.Count > 0)
//        {
//            _final = BinUtils.WriteInt32(1, _final);
//            _final = BinUtils.WriteInt32(Fragments_v3.Count, _final);
//            foreach (v_v3 frag in Fragments_v3)
//            {
//                _final = BinUtils.BufferCombine(_final, frag.Serialize());
//            }
//        }
//        _final = BinUtils.WriteInt32(GpuWeights.Count, _final);
//        foreach (GpuAnimatedMeshWeight w in GpuWeights)
//        {
//            _final = BinUtils.BufferCombine(_final, w.Serialize());

//        }
//        _final = BinUtils.WriteInt32(GpuWeightData.Count, _final);
//        foreach (GpuAnimatedMeshWeightData wd in GpuWeightData)
//        {
//            _final = BinUtils.BufferCombine(_final, wd.Serialize());

//        }
//        //_final = BinUtils.WriteInt32(Weights.Count, _final);
//        //foreach (VertexWeightGroup group in Weights)
//        //{
//        //    _final = BinUtils.BufferCombine(_final, group.Serialize());
//        //}

//        _final = BinUtils.WriteInt32(Indexes.Count, _final);
//        foreach (int ind in Indexes)
//        {
//            _final = BinUtils.WriteInt32(ind, _final);
//        }
//        //*These should be preprocessed
//        //_final = BinUtils.WriteBool(FlipTris, _final);
//        //_final = BinUtils.WriteBool(SwapUvs, _final);
//        _final = BinUtils.WriteBool(Material != null, _final);
//        if (Material != null)
//        {
//            _final = BinUtils.BufferCombine(_final, Material.Serialize());
//        }
//        return _final;
//    }
//}
//public class Model
//{
//    public Int32 FrameRate { get; set; }
//    public List<Armature> Armatures = new List<Armature>();
//    public List<ActionGroup> ActionGroups = new List<ActionGroup>();
//    public List<Mesh> Meshes = new List<Mesh>();
//    public byte[] Serialize()
//    {
//        byte[] _final = new byte[0];

//        _final = BinUtils.WriteInt32(FrameRate, _final);

//        _final = BinUtils.WriteInt32(Armatures.Count, _final);
//        foreach (Armature arm in Armatures)
//        {
//            _final = BinUtils.BufferCombine(_final, arm.Serialize());

//        }
//        _final = BinUtils.WriteInt32(ActionGroups.Count, _final);
//        foreach (ActionGroup act in ActionGroups)
//        {
//            _final = BinUtils.BufferCombine(_final, act.Serialize());
//        }
//        _final = BinUtils.WriteInt32(Meshes.Count, _final);
//        foreach (Mesh mesh in Meshes)
//        {
//            _final = BinUtils.BufferCombine(_final, mesh.Serialize());
//        }

//        return _final;
//    }
//}

//public class ivec3
//{
//    public int x { get; set; }
//    public int y { get; set; }
//    public int z { get; set; }
//}
//#endregion


#region FILES 

//public class MobFile : PoundFile
//{
//    public List<Model> Models = new List<Model>();

//    Model _pCurModel = null;
//    Armature _pCurArm = null;
//    Mesh _pCurMesh = null;
//    Action _pCurAction = null;
//    Bone _pCurBone = null;
//    Material _pCurMaterial;

//    Dictionary<ivec3, int> _vertexCache = new Dictionary<ivec3, int>();
//    List<Vec3> _verts = new List<Vec3>();
//    List<Vec2> _texs = new List<Vec2>();
//    List<Vec3> _normals = new List<Vec3>();
//    List<VertexWeightGroup> _weights = new List<VertexWeightGroup>();

//    protected override void PostLoad()
//    {
//        //So 1/14/18 we are abandongin loading from the parser.
//        throw new NotImplementedException();
//        //public int Offset { get; set; }    // offset into weight buffer (GpuAnimatedMeshWeight)
//        //public int Count { get; set; }    // count of weights
//        //
//        //public int ArmJointOffset { get; set; }    // - Offset into joint matrix buffer 12/2017 - sorted by armature! (GpuAnimatedMeshBindMatrix / GpuAnimatedMeshSkinMatrix)
//        //public float Weight { get; set; }        // weight of joint
//        //process vertex weights
//        foreach (Model mod in Models)
//        {
//            foreach (Mesh mesh in mod.Meshes)
//            {
//                //  CollectArmatures(mesh,mod);
//                int iOff = 0;
//                for (int iVert = 0; iVert < mesh.Fragments_v3n3x2.Count; iVert++)
//                {
//                    // ProcessVertexWeight(mod, mesh, iVert, ref iOff);
//                }
//                for (int iVert = 0; iVert < mesh.Fragments_v3n3.Count; iVert++)
//                {

//                }
//                for (int iVert = 0; iVert < mesh.Fragments_v3x2.Count; iVert++)
//                {

//                }
//                for (int iVert = 0; iVert < mesh.Fragments_v3.Count; iVert++)
//                {

//                }


//            }
//        }
//    }
//    //private void CollectArmatures(Mesh mesh, Model mod)
//    //{
//    //    foreach (VertexWeightGroup vwg in mesh.Weights)
//    //    {
//    //        foreach (VertexArmWeight aw in vwg.Weights)
//    //        {
//    //            aw.ArmatureId
//    //        }
//    //    }
//    //}
//    //private int ProcessVertexWeight(Model model, Mesh mesh, int iVert, ref int iOff)
//    //{
//    //    VertexWeightGroup vwg = mesh.Weights[iVert];

//    //    for (int iArm = 0; iArm < vwg.Weights.Count; ++iArm)
//    //    {
//    //        VertexArmWeight vw = vwg.Weights[iArm];
//    //        for(int iBone=0; iBone < vw.Weights.Count; ++iBone)
//    //        {
//    //            VertexBoneWeight bw = vw.Weights[iBone];

//    //            GpuAnimatedMeshWeight wt = new GpuAnimatedMeshWeight();
//    //            wt.ArmJointOffset = GetGpuJointOrdinal(model, iArm, iBone);
//    //            mesh.GpuWeights.Add(wt);
//    //        }
//    //    }
//    //    GpuAnimatedMeshWeightData wd = new GpuAnimatedMeshWeightData();
//    //    wd.Offset = iOff;
//    //    wd.Count = vwg.Weights.Count;
//    //    mesh.GpuWeightData.Add(wd);
//    //}
//    //private int GetGpuJointOrdinal(Mesh mesh, int arm, int joint)
//    //{

//    //}
//    private int FindCachedVertex(int vi, int xi, int ni)
//    {
//        foreach (KeyValuePair<ivec3, int> p in _vertexCache)
//        {
//            if (p.Key.x == vi && p.Key.y == xi && p.Key.z == ni)
//            {
//                return p.Value;
//            }
//        }
//        return -1;
//    }
//    private void AddCachedVertex(int newInd, int vi, int xi, int ni)
//    {
//        ivec3 v = new ivec3();
//        v.x = vi;
//        v.y = xi;
//        v.z = ni;
//        _vertexCache.Add(v, newInd);
//    }
//    public MobFile(string file) : base(file)
//    {
//    }
//    private ParentType ParseParentType(string str)
//    {
//        str = str.Replace("\"", "");
//        if (str.ToLower() == "none") return ParentType.None;
//        else if (str.ToLower() == "armature") return ParentType.Armature;
//        else if (str.ToLower() == "bone") return ParentType.Bone;
//        else if (str.ToLower() == "object") return ParentType.Object;
//        else
//            throw new NotImplementedException();
//    }
//    private PhysicsShapeType ParsePhysicsShapeType(string str)
//    {
//        str = str.Replace("\"", "");
//        if (str.ToLower() == "convex_hull") return PhysicsShapeType.Hull;
//        else if (str.ToLower() == "sphere") return PhysicsShapeType.Sphere;
//        else if (str.ToLower() == "box") return PhysicsShapeType.AABox;
//        else
//            throw new NotImplementedException();
//    }
//    protected override void pkp(List<string> tokens)
//    {
//        if (lcmp(tokens[0], "MOD_BEG", 2))
//        {
//            _pCurModel = new Model();
//        }
//        else if (lcmp(tokens[0], "MOD_END", 2))
//        {
//            Models.Add(_pCurModel);
//            _pCurModel = null;
//            _verts.Clear();
//            _texs.Clear();
//            _normals.Clear();
//            _vertexCache.Clear();
//        }
//        else if (lcmp(tokens[0], "FRR", 2))
//        {
//            _pCurModel.FrameRate = StrToInt(tokens[1]);
//        }
//        else if (lcmp(tokens[0], "ARM_BEG", 5))
//        {
//            _pCurArm = new Armature();
//            _pCurArm.Name = tokens[1].Replace("\"", "");
//            _pCurArm.ParentName = tokens[2].Replace("\"", "");
//            _pCurArm.ParentType = ParseParentType(tokens[3]);
//            _pCurArm.ArmatureId = StrToInt(tokens[4]);
//        }
//        else if (lcmp(tokens[0], "ARM_END", 2))
//        {
//            _pCurModel.Armatures.Add(_pCurArm);
//            _pCurArm = null;
//        }
//        else if (lcmp(tokens[0], "ARM_WORLD", 2))
//        {
//            _pCurArm.World = ParseMat4(tokens[1]);
//        }
//        else if (lcmp(tokens[0], "arm_parent_inverse", 2))
//        {
//            _pCurArm.ParentInverse = ParseMat4(tokens[1]);
//        }
//        else if (lcmp(tokens[0], "bones_beg", 1))
//        {
//        }
//        else if (lcmp(tokens[0], "bones_end", 1))
//        {
//        }
//        else if (lcmp(tokens[0], "bone_beg", 5))
//        {
//            _pCurBone = new Bone();
//            _pCurBone.Name = tokens[1].Replace("\"", "");
//            _pCurBone.Ordinal = StrToInt(tokens[2]);
//            _pCurBone.ParentName = tokens[3].Replace("\"", "");
//            _pCurBone.ParentType = ParseParentType(tokens[4]);
//        }
//        else if (lcmp(tokens[0], "bone_end", 2))
//        {
//            _pCurArm.Bones.Add(_pCurBone);
//            _pCurBone = null;
//        }
//        else if (lcmp(tokens[0], "bone_head", 4))
//        {
//            _pCurBone.Head = ParseVec3(tokens[1], tokens[2], tokens[3]);
//        }
//        else if (lcmp(tokens[0], "bone_tail", 4))
//        {
//            _pCurBone.Tail = ParseVec3(tokens[1], tokens[2], tokens[3]);
//        }
//        else if (lcmp(tokens[0], "bone_bind_world", 3))
//        {
//            _pCurBone.BindWorld = ParseMat4(tokens[2]);
//        }
//        else if (lcmp(tokens[0], "act_beg", 3))
//        {
//            string name = tokens[1].Replace("\"", "");
//            ActionGroup found = null;
//            foreach (ActionGroup ag in _pCurModel.ActionGroups)
//            {
//                if (ag.Name.Equals(name))
//                {
//                    found = ag;
//                    break;
//                }
//            }
//            if (found == null)
//            {
//                //Create new group
//                found = new ActionGroup();
//                found.Name = name;
//                _pCurModel.ActionGroups.Add(found);
//            }

//            _pCurAction = new Action();
//            _pCurAction.ObjectName = tokens[2].Replace("\"", "");
//            found.Actions.Add(_pCurAction);
//        }
//        else if (lcmp(tokens[0], "act_end", 3))
//        {
//            // _pCurModel.Actions.Add(_pCurAction);
//            _pCurAction = null;
//        }
//        else if (lcmp(tokens[0], "kf", 4))
//        {
//            PRSKeyframe kf = ParsePRSKeyframe(tokens[1], tokens[2], tokens[3]);
//            _pCurAction.Keyframes.Add(kf);
//        }
//        else if (lcmp(tokens[0], "mpt_beg", 4))
//        {
//            _pCurMesh = new Mesh();
//            _pCurMesh.Name = tokens[1].Replace("\"", "");
//            _pCurMesh.ParentName = tokens[2].Replace("\"", "");
//            _pCurMesh.ParentType = ParseParentType(tokens[3]);
//        }
//        else if (lcmp(tokens[0], "mpt_end", 2))
//        {

//            if (_pCurMesh.FlipTris)
//            {
//                Console.WriteLine("Flipping Mesh Tris..");
//                for (int i = 0; i < _pCurMesh.Indexes.Count; i += 3)
//                {
//                    int tmp = _pCurMesh.Indexes[i + 0];
//                    _pCurMesh.Indexes[i + 0] = _pCurMesh.Indexes[i + 1];
//                    _pCurMesh.Indexes[i + 1] = tmp;
//                }
//            }
//            _pCurMesh.SwapUvs = tokens[1].Trim().ToLower() == "true";
//            if (_pCurMesh.SwapUvs)
//            {
//                Console.WriteLine("Swapping Mesh UVs..");
//                foreach (v_v3n3x2 vert in _pCurMesh.Fragments_v3n3x2)
//                {
//                    float t = vert.x.x;
//                    vert.x.x = vert.x.y;
//                    vert.x.y = t;
//                }
//                foreach (v_v3x2 vert in _pCurMesh.Fragments_v3x2)
//                {
//                    float t = vert.x.x;
//                    vert.x.x = vert.x.y;
//                    vert.x.y = t;
//                }
//            }

//            _pCurModel.Meshes.Add(_pCurMesh);
//            _pCurMesh = null;
//        }
//        else if (lcmp(tokens[0], "mpt_basis", 2))
//        {
//            _pCurMesh.Basis = ParseMat4(tokens[1]);
//        }
//        else if (lcmp(tokens[0], "mpt_parent_inverse", 2))
//        {
//            _pCurMesh.ParentInverse = ParseMat4(tokens[1]);
//        }
//        else if (lcmp(tokens[0], "mat_beg", 2))
//        {
//            _pCurMaterial = new Material();
//            _pCurMaterial.Name = tokens[1].Replace("\"", "");
//        }
//        else if (lcmp(tokens[0], "mat_end", 2))
//        {
//            _pCurMesh.Material = _pCurMaterial;
//            _pCurMaterial = null;
//        }
//        else if (lcmp(tokens[0], "mat_tex_diffuse", 3))
//        {
//            Texture t = new Texture();
//            t.FileName = tokens[1].Replace("\"", "");
//            t.Amount = StrToFloat(tokens[2]);
//            _pCurMaterial.Textures.Add(t);
//        }
//        else if (lcmp(tokens[0], "physics_shape", 4))
//        {
//            _pCurMesh.PhysicsShape = new PhysicsShape();
//            _pCurMesh.PhysicsShape.Type = ParsePhysicsShapeType(tokens[1]);
//            _pCurMesh.PhysicsShape.Kinematic = StrToBool(tokens[2]);
//            _pCurMesh.PhysicsShape.Dynamic = StrToBool(tokens[3]);
//        }
//        else if (lcmp(tokens[0], "mpt_hide_render", 2))
//        {
//            _pCurMesh.HideRender = StrToBool(tokens[1]);
//        }
//        else if (lcmp(tokens[0], "mat_ambient", 5))
//        {
//            _pCurMaterial.Ambient = ParseVec4(tokens[1], tokens[2], tokens[3], tokens[4]);
//        }
//        else if (lcmp(tokens[0], "mat_spec", 5))
//        {
//            _pCurMaterial.Specular = ParseVec4(tokens[1], tokens[2], tokens[3], tokens[4]);
//        }
//        else if (lcmp(tokens[0], "v", 4))
//        {
//            _verts.Add(ParseVec3(tokens[1], tokens[2], tokens[3]));
//        }
//        else if (lcmp(tokens[0], "vn", 4))
//        {
//            _normals.Add(ParseVec3(tokens[1], tokens[2], tokens[3]));
//        }
//        else if (lcmp(tokens[0], "vt", 3))
//        {
//            _texs.Add(ParseVec2(tokens[1], tokens[2]));
//        }
//        else if (lcmp(tokens[0], "vw"))
//        {
//            //TODO:
//            //_texs.Add(ParseVec2(tokens[1], tokens[2]));
//            int nGroups = StrToInt(tokens[1]);
//            VertexWeightGroup g = new VertexWeightGroup();
//            for (int iGroup = 0; iGroup < nGroups; iGroup++)
//            {
//                int armId = StrToInt(tokens[2 + iGroup * 3 + 0]);
//                // armName = armName.Replace("\"", "");
//                int iWeightCount = StrToInt(tokens[2 + iGroup * 3 + 1]);
//                string strWeights = tokens[2 + iGroup * 3 + 2];
//                VertexArmWeight aw = ParseArmWeight(armId, strWeights);
//                g.Weights.Add(aw);
//            }


//        }
//        else if (lcmp(tokens[0], "f", 4))
//        {
//            ParseObjFaceVert(tokens[1]);
//            ParseObjFaceVert(tokens[2]);
//            ParseObjFaceVert(tokens[3]);

//        }
//        else if (lcmp(tokens[0], "mpt_flip_tris", 2))
//        {
//            _pCurMesh.FlipTris = tokens[1].Trim().ToLower() == "true";
//        }
//        else if (lcmp(tokens[0], "mpt_swap_uvs", 2))
//        {
//            _pCurMesh.SwapUvs = tokens[1].Trim().ToLower() == "true";
//        }

//    }
//    private void ParseObjFaceVert(string face)
//    {
//        const int cMissingIdx = -999999999;

//        string[] inds = face.Split(new char[] { '/' }, StringSplitOptions.None);
//        if (inds.Length != 3)
//        {
//            parseErr("Failed to parse face indexes.. Did not have 3 components.", true);
//        }
//        //v, x , n
//        int indV = (inds[0].Length == 0) ? cMissingIdx : StrToInt(inds[0]) - 1;
//        int indX = (inds[1].Length == 0) ? cMissingIdx : StrToInt(inds[1]) - 1;
//        int indN = (inds[2].Length == 0) ? cMissingIdx : StrToInt(inds[2]) - 1;

//        //ind0 = ind0 - 1;
//        //ind1 = ind1 - 1;
//        //ind2 = ind2 - 1;

//        int ind = FindCachedVertex(indV, indX, indN);
//        if (ind == -1)
//        {
//            //Parse the vertex format
//            if (indX == cMissingIdx && indN == cMissingIdx)
//            {
//                v_v3 frag = new v_v3();
//                frag.v = _verts[indV];
//                _pCurMesh.Fragments_v3.Add(frag);
//            }
//            else if (indX == cMissingIdx)
//            {
//                v_v3n3 frag = new v_v3n3();
//                frag.v = _verts[indV];
//                frag.n = _normals[indN];
//                _pCurMesh.Fragments_v3n3.Add(frag);
//            }
//            else if (indN == cMissingIdx)
//            {
//                v_v3x2 frag = new v_v3x2();
//                frag.v = _verts[indV];
//                frag.x = _texs[indX];
//                _pCurMesh.Fragments_v3x2.Add(frag);
//            }
//            else
//            {
//                v_v3n3x2 frag = new v_v3n3x2();
//                frag.v = _verts[indV];
//                frag.x = _texs[indX];
//                frag.n = _normals[indN];
//                _pCurMesh.Fragments_v3n3x2.Add(frag);
//            }

//            ind = _verts.Count() - 1;
//            AddCachedVertex(ind, indV, indX, indN);
//        }

//        _pCurMesh.Indexes.Add(ind);
//    }
//    private VertexArmWeight ParseArmWeight(int armId, string strWeights)
//    {
//        VertexArmWeight aw = new VertexArmWeight();
//        aw.ArmatureId = armId;// GetArmatureId(armName);

//        string[] weights = strWeights.Split(',');
//        for (int iw = 0; iw < weights.Length; iw += 2)
//        {
//            VertexBoneWeight bw = new VertexBoneWeight();
//            bw.BoneId = StrToInt(weights[iw + 0]);
//            bw.Weight = StrToFloat(weights[iw + 1]);
//            aw.Weights.Add(bw);
//        }
//        return aw;
//    }
//    //private int GetArmatureId(string armName)
//    //{
//    //    foreach (Armature arm in _pCurModel.Armatures)
//    //    {
//    //        if (arm.Name == armName)
//    //        {
//    //            return arm.ArmatureId;
//    //        }
//    //    }
//    //    parseErr("Could not Find armature by name " + armName + " - the armatures may not have come prior in the file.", true);
//    //    return -1;
//    //}

//}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
public class ObFile : PoundFile
{
    public List<string> _lstMobs = new List<string>();
    public string BlenderExePath = "";
    public string PythonScriptPath = "";
    public string BlenderModelDir = "";
    public string MobDir = "";
    public string MobBinDir = "";
    public ObFile(string file) : base(file)
    {
    }
    protected override void PostLoad() { }
    protected override void pkp(List<string> tokens)
    {
        if (lcmp(tokens[0], "MOB", 7))
        {
            _lstMobs.Add(tokens[1].Replace("\"", ""));
        }
        else if (lcmp(tokens[0], "MOB_BPATH", 2))
        {
            BlenderExePath = tokens[1].Replace("\"", "");
        }
        else if (lcmp(tokens[0], "MOB_PYCON", 2))
        {
            PythonScriptPath = tokens[1].Replace("\"", "");
        }
        else if (lcmp(tokens[0], "MOB_BMDIR", 2))
        {
            BlenderModelDir = tokens[1].Replace("\"", "");
        }
        else if (lcmp(tokens[0], "MOB_MOBDIR", 2))
        {
            MobDir = tokens[1].Replace("\"", "");
        }
        else if (lcmp(tokens[0], "MOB_MOBBINDIR", 2))
        {
            MobBinDir = tokens[1].Replace("\"", "");
        }
    }
}
////////////////////////////////////////////////////////////////////v
////////////////////////////////////////////////////////////////////
public abstract class PoundFile
{
    public List<string> Errors { get; } = new List<string>();
    List<string> lines;
    List<string> _vecTokens;
    string _strFile;
    // string _curLine;
    int _iLine = 1;
    protected Vec2 ParseVec2(string v0, string v1)
    {
        Vec2 v = new Vec2();
        v.x = StrToFloat(v0);
        v.y = StrToFloat(v1);

        return v;
    }
    protected Vec3 ParseVec3(string v0, string v1, string v2)
    {
        Vec3 v = new Vec3();
        v.x = StrToFloat(v0);
        v.y = StrToFloat(v1);
        v.z = StrToFloat(v2);
        return v;
    }
    protected Vec4 ParseVec4(string v0, string v1, string v2, string v3)
    {
        Vec4 v = new Vec4();
        v.x = StrToFloat(v0);
        v.y = StrToFloat(v1);
        v.z = StrToFloat(v2);
        v.w = StrToFloat(v3);
        return v;
    }
    protected Mat4 ParseMat4(string str)
    {
        Mat4 m = new Mat4();
        string[] strs = str.Split(',');
        if (strs.Length != 16)
        {
            parseErr("Error parsing matrix, matrix was not 16 elements", true);
        }
        for (int i = 0; i < 16; ++i)
        {
            m.m[i] = StrToFloat(strs[i]);
        }

        return m;
    }
    protected float StrToFloat(string str)
    {
        float ret = 0;
        if (float.TryParse(str, out ret) == false)
        {
            parseErr("Failed to parse float.", true);
        }
        return ret;
    }
    protected int StrToInt(string str)
    {
        int ret = 0;
        if (Int32.TryParse(str, out ret) == false)
        {
            parseErr("Failed to parse int.", true);
        }
        return ret;
    }
    protected bool StrToBool(string str)
    {
        bool ret = false;
        if (Boolean.TryParse(str, out ret) == false)
        {
            parseErr("Failed to parse bool.", true);
        }
        return ret;
    }
    protected void ParsePRSString(string str, out Vec3 p, out Vec4 r, out Vec3 s)
    {
        string[] vals = str.Split(',');
        if (vals.Length != 10)
        {
            parseErr("Failed to parse PRS string", true);
        }
        p = new Vec3();
        r = new Vec4();
        s = new Vec3();

        p.x = StrToFloat(vals[0]);
        p.y = StrToFloat(vals[1]);
        p.z = StrToFloat(vals[2]);

        r.x = StrToFloat(vals[3]);
        r.y = StrToFloat(vals[4]);
        r.z = StrToFloat(vals[5]);
        r.w = StrToFloat(vals[6]);

        s.x = StrToFloat(vals[7]);
        s.y = StrToFloat(vals[8]);
        s.z = StrToFloat(vals[9]);
    }
    protected PRSKeyframe ParsePRSKeyframe(string type, string ind, string prs)
    {
        Vec3 p = null; Vec4 r = null; Vec3 s = null;
        PRSKeyframe kf = new PRSKeyframe();

        ParsePRSString(prs, out p, out r, out s);

        kf.Index = StrToInt(ind);
        kf.Pos = p;
        kf.Rot = r;
        kf.Scale = s;

        return kf;
    }
    public PoundFile(string file)
    {
        _strFile = file;
    }

    public string getErrorString()
    {
        string ret = "";
        foreach (string err in Errors)
        {
            ret += err + "\r\n";
        }
        return ret;
    }
    private bool isEscapeCharacter(char c)
    {
        return (c == '"')
            || (c == '\'')
            || (c == '[')
            || (c == ']')
            || (c == '(')
            || (c == ')')
            || (c == '#')
            ;
    }
    private List<string> parseLine(string line)
    {
        List<string> tokens = new List<string>();
        string token = "";
        bool b_eof = false;
        char c;
        //ptr = buf;

        bool _nStringProfile = false;
        int _nParenProfile = 0;
        int _nBracketProfile = 0;

        //_iCurrentParseLine = 1;

        bool _bEscape = false;
        int iChar = 0;
        while (iChar < line.Length)
        {
            c = line[iChar];
            if (c == '\0')
            {
                b_eof = true;
            }
            if (b_eof)
            {
                //if we're EOF then parse dumb token
                if (token.Length > 0)
                {
                    tokens.Add(token);
                }
                token = "";

                break;//return tokens;
                      //if (tokens.Count > 0)
                      //    pkp(_vecTokens);// - Parse the given token buffer

                //break;
            }
            else if (c == '"')
            {
                _nStringProfile = !_nStringProfile;

                token += c;
                iChar++;//inc()
            }
            else if (_nStringProfile)
            {
                //Inside string - do not parse other delimiters
                token += c;
                iChar++;//inc();
            }
            else if (_bEscape && !(_nStringProfile))
            {
                if (!isEscapeCharacter(c))
                    parseErr("The given character '" + c + "' is not a valid pound file escape character.");
                token += c;
                _bEscape = false;
                iChar++;//inc();
            }
            else if (c == '\\' && !(_nStringProfile))
            {
                _bEscape = true;
                iChar++;//inc();
            }
            else if (c == '#')
            {
                if (token.Length > 0)
                    tokens.Add(token);
                token = "";

                break;// return tokens;
                      //if (tokens.Count > 0)
                      //    pkp(_vecTokens);// - Parse the given token buffer

                //_vecTokens.Clear();
                //return tokens;// eatLine(_iCurrentParseLine);
            }
            else if (c == '(')
            {
                _nParenProfile++;

                token += c;
                iChar++;//inc();
            }
            else if (c == ')')
            {
                _nParenProfile--;
                if (_nParenProfile < 0)
                    parseErr("Parentheses profile is invalid. You're missing a parentheses somewhere.");

                token += c;
                iChar++;//inc();
            }
            else if (c == '[')
            {
                _nBracketProfile++;

                token += c;
                iChar++;//inc();
            }
            else if (c == ']')
            {
                _nBracketProfile--;
                if (_nBracketProfile < 0)
                    parseErr("Bracket profile is invalid. You're missing a Bracket somewhere.");

                token += c;
                iChar++;//inc();
            }
            //else if (c == '\n' || c == '\r')
            //{
            //    if (_nStringProfile || _nParenProfile || _nBracketProfile)
            //        parseErr("Parser error - you can't have multi-line strings \"\", braces [] or parentheses ()");
            //    if (token.Length > 0)
            //        _vecTokens.Add(token);
            //    token = "";
            //    if (_vecTokens.Count > 0)
            //        pkp(_vecTokens);// - Parse the given token buffer
            //    _vecTokens.clear();
            //    return;//eatLine(_iCurrentParseLine);
            //    //_iCurrentParseLine++;
            //}
            // - If we're whitespace but we also are within a string or bracket then do not skip it, add it to
            // the argument.
            else if (String.IsNullOrWhiteSpace(c.ToString()) && !(_nStringProfile || _nParenProfile > 0 || _nBracketProfile > 0))
            {
                if (token.Length > 0)
                    tokens.Add(token);
                token = "";
                iChar++;// inc();
            }
            else
            {
                token += c;
                iChar++;//inc();
            }

        }

        if (token.Length > 0)
        {
            tokens.Add(token);
        }
        token = "";

        return tokens;
    }
    public void loadAndParse()
    {
        if (System.IO.File.Exists(_strFile) == false)
        {
            parseErr("File " + _strFile + " does not exist.");
            return;
        }
        lines = System.IO.File.ReadAllLines(_strFile).ToList();
        foreach (string line in lines)
        {
            _vecTokens = parseLine(line);
            if (_vecTokens.Count > 0)
            {
                pkp(_vecTokens);
            }
            _iLine++;
        }
    }
    protected void parseErr(string str, bool bFatal = false)
    {
        str = "  Parse Error: " + str + " : line " + _iLine;
        Errors.Add(str);
        System.Console.ForegroundColor = ConsoleColor.Red;
        System.Console.WriteLine(str);
        System.Console.ForegroundColor = ConsoleColor.White;
        if (bFatal)
        {
            throw new Exception("Fatal - " + str + " - parse error.");
        }
    }
    protected bool lcmp(string tok, string str, int argCount = -1)
    {
        if (tok.ToLower() == str.ToLower())
        {
            if ((argCount != -1) && (_vecTokens.Count != argCount))
            {
                parseErr("Invalid number of arguments for '" + tok + "'");

                Debugger.Break();
                return false;
            }
            else
            {
                return true;

            }
        }
        return false;
    }
    protected abstract void pkp(List<string> tokens);
    protected abstract void PostLoad();
}

#endregion
