using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsErrorDialog
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            string a, b, c;

            a = "Unspecified Title";
            b = "Unspecified Error";
            c = "Unspecified Log Dir";

            if (args.Length > 0)
                a = args[0];
            if (args.Length > 1) 
                b = args[1];
            if (args.Length > 2)
                c = args[2];

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm(a, b, c));
        }
    }
}
