using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace CodeShark
{
    /*
     */
    public class Rep
    {
        public string _pat;
        public string _rep;
        public Rep(string pat, string rep)
        {
            _pat = pat;
            _rep = rep;
        }
    }
    public class Program
    {
        static float _fVersion = 0.02f;
        static List<string> extensions = new List<string> { ".h", ".cpp" };

        static void LogMsg(string str)
        {
            Console.WriteLine(DateTime.Now.ToShortDateString() + ": " + str);
        }
        static List<string> getFiles()
        {
            List<string> files = new List<string>();
            List<string> localFiles = System.IO.Directory.GetFiles(System.Environment.CurrentDirectory).ToList();

            foreach (string file in localFiles)
            {
                foreach (string ext in extensions)
                {
                    if (System.IO.Path.GetExtension(file).ToLower() == ext.ToLower())
                    {
                        files.Add(file);
                        break;
                    }
                }
            }
            return files;
        }
        static void Main(string[] args)
        {
            string headstr = "";
            headstr += "****************************************\r\n";
            headstr += "* Code Shark v" + _fVersion + "\r\n";
            headstr += "*   MetalMario971 12/06/2017\r\n";
            headstr += "*   Replaces old BRO code with standard C++\r\n";
            headstr += "****************************************\r\n";
            headstr += "\r\n";

            headstr += "Found Files:";
            List<string> files = getFiles();
            string  app = "";
            foreach (string file in files)
            {
                headstr += app + System.IO.Path.GetFileName(file);
                app = ", ";
            }

            headstr += "\r\n\r\n";
            headstr += "Press ENTER to process files.  Press ESC to exit." + "\r\n";
            Console.Write(headstr);
            if (System.Console.ReadKey().Key != ConsoleKey.Enter)
            {
                System.Environment.Exit(0);
            }

            //**Put all regex replacements here.
            List<Rep> reps = new List<Rep>();
            reps.Add(new Rep("t_bool(\\s)", "bool$1"));
            reps.Add(new Rep("t_int32(\\s)", "int32_t$1"));
            reps.Add(new Rep("t_int(\\s)", "int32_t$1"));
            reps.Add(new Rep("t_int64(\\s)", "int64_t$1"));
            reps.Add(new Rep("t_long(\\s)", "int32_t$1"));
            reps.Add(new Rep("VectorEx", "std::vector"));

            reps.Add(new Rep("t_vecsize(\\s)", "size_t$1"));
            reps.Add(new Rep("t_memsize(\\s)", "size_t$1"));

            reps.Add(new Rep("t_float32(\\s)", "float$1"));
            reps.Add(new Rep("t_float(\\s)", "float$1"));
            reps.Add(new Rep("t_double(\\s)", "double$1"));
            reps.Add(new Rep("MapEx", "std::map"));
            reps.Add(new Rep("Cube3f", "Box3f"));
            reps.Add(new Rep("Matrix4x4", "mat4"));
            reps.Add(new Rep("Matrix3x3", "mat3"));
            reps.Add(new Rep("Vec2f", "vec2"));
            reps.Add(new Rep("Vec3f", "vec3"));
            reps.Add(new Rep("NULL", "nullptr"));

            reps.Add(new Rep("\t", "    "));

            reps.Add(new Rep("TRef\\(([a-zA-Z]+)\\)", "$1*"));
            reps.Add(new Rep("([a-zA-Z0-9_]+)\\.safeCast<([a-zA-Z0-9_]+)>\\(\\)", "($2*)$1"));
            reps.Add(new Rep("([a-zA-Z0-9_]+)\\.isNull\\(\\)", "$1 == nullptr"));
            reps.Add(new Rep("([a-zA-Z0-9_]+)\\.isNotNull\\(\\)", "$1 != nullptr"));

            List <string> newFileText = new List<string>();
            foreach (string file in files)
            {
                try
                {
                    string str = System.IO.File.ReadAllText(file);
                    LogMsg(System.IO.Path.GetFileName(file));
                    int rep_count = 0;

                    foreach (Rep r in reps)
                    {
                        Regex rgx = new Regex(r._pat);
                        foreach (Match m in rgx.Matches(str))
                        {
                            string strPreview = m.ToString();

                            LogMsg("    " + strPreview + " => " + rgx.Replace(strPreview, r._rep));
                        }
                        rep_count += rgx.Matches(str).Count;
                        str = rgx.Replace(str, r._rep);
                    }

                    LogMsg("    " + System.IO.Path.GetFileName(file) + ", replaced: " + rep_count + " tokens.");
                    newFileText.Add(str);
                }
                catch (Exception ex)
                {
                    LogMsg("Failed to process file " + file +" -> " + ex.ToString());
                    newFileText.Add("");
                }

            }

            //Console.Write("Press ENTER to save files.  Press ESC to exit." + "\r\n");
            //if (System.Console.ReadKey().Key != ConsoleKey.Enter)
            //{
            //    System.Environment.Exit(0);
            //}

            for(int i=0; i<files.Count; ++i)
            {
                try
                {
                    string backText = System.IO.File.ReadAllText(files[i]);
                    LogMsg("Saving backup " + files[i]);
                    System.IO.File.WriteAllText(files[i] + ".backup", backText);
                    System.IO.File.WriteAllText(files[i], newFileText[i]);
                }
                catch (Exception ex)
                {
                    LogMsg("Failed to save file or backup " + files[i] + " -> " + ex.ToString());
                }
            }

            Console.Write("Press ENTER to exit.\r\n");
            Console.ReadKey();
        }
    }
}
