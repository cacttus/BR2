using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Proteus
{
    public class Globals
    {
        public static string ProgramName; // used by the logger.

        public const string DefaultLogDirectory = ".\\logs";

        public static void InitializeGlobals(string LogFileName, string LogFileDir = Globals.DefaultLogDirectory)
        {
            Logger.InitLogger(LogFileName,LogFileDir);

            NetworkSettings.Init();
        }

        public static void Throw(string str)
        {
            Logger.LogError(str, true);
        }
        public static void ThrowIf(bool condition, string str)
        {
            if (condition==true)
                Globals.Throw(str);
        }

    }
}
