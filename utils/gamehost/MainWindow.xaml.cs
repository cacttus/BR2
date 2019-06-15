using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MahApps.Metro.Controls;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        private System.Windows.Forms.Panel _panel;
        private Process _process;
        private GameServer _objGameServer = new GameServer(); // Proteus.PacketMakerTcp _objPacketMakerTcp = new Proteus.PacketMakerTcp();
        private System.Windows.Forms.Timer _objServerPollTimer;

        public MainWindow()
        {
            InitializeComponent();
            _panel = new System.Windows.Forms.Panel();
            windowsFormsHost1.Child = _panel;

        }
        public override void BeginInit()
        {
            base.BeginInit();
        }
        [DllImport("user32.dll")]
        private static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32")]
        private static extern IntPtr SetParent(IntPtr hWnd, IntPtr hWndParent);

        [DllImport("user32")]
        private static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, int uFlags);

        private const int SWP_NOZORDER = 0x0004;
        private const int SWP_NOACTIVATE = 0x0010;
        private const int GWL_STYLE = -16;
        private const int WS_CAPTION = 0x00C00000;
        private const int WS_THICKFRAME = 0x00040000;

        private void Grid_Loaded(object sender, RoutedEventArgs e)
        {
            Proteus.Proteus.Initialize("GameHostLog");

            BeginHost();

        }

        private void BeginHost()
        {
            try
            {
                string file = "";
#if DEBUG
                file =  @".\bottle_x64_d.exe";
#else
                file =  @".\bottle_x64_r.exe";
#endif


                _process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = file,
                        UseShellExecute = false,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        WorkingDirectory = @".\",
                        Arguments = "--show-console=false --game-host=true"
                    }
                };

                _process.OutputDataReceived += (sender, args) =>
                {
                    Dispatcher.BeginInvoke((Action)(() =>
                    {
                        _txtOutput.AppendText(args.Data + Environment.NewLine);
                    }));
                };
                _process.ErrorDataReceived += (sender, args) =>
                {
                    Dispatcher.BeginInvoke((Action)(() =>
                    {
                        _txtOutput.AppendText(args.Data + Environment.NewLine);
                    }));
                };

                //The window errors out probably because of the DC 
                //https://www.codeproject.com/Articles/23736/Creating-OpenGL-Windows-in-WPF
                _process.Start();
                _process.BeginOutputReadLine();

                //See updateWindowHandleForGamehost() in AppRunner
                while (_process.MainWindowHandle == IntPtr.Zero)
                {
                    System.Threading.Thread.Sleep(1);
                }

                MainWindow that = this;
                bool? c = _objGameServer.Connect(true, ()=> {

                    //Run on UI thread
                    that.Dispatcher.BeginInvoke((Action)(() => {
                        //Per MSDN "You must use the Refresh method to refresh the Process object to get the current main window handle if it has changed."
                        _process.Refresh();
                        SetParent(_process.MainWindowHandle, _panel.Handle);
                        // remove control box
                        int style = GetWindowLong(_process.MainWindowHandle, GWL_STYLE);
                        style = style & ~WS_CAPTION & ~WS_THICKFRAME;
                        SetWindowLong(_process.MainWindowHandle, GWL_STYLE, style);

                        // resize embedded application & refresh
                        ResizeEmbeddedApp();
                    }));
 
                });


 

            }
            catch (Exception ex)
            {
                LogError(ex.ToString());
            }
        }
        private void LogError(string x)
        {
            Proteus.Globals.Logger.LogError(x);
            _txtOutput.AppendText(x + Environment.NewLine);
        }

        private void PollServer()
        {
            if (_objGameServer != null)
            {
                if (_objGameServer.IsConnected())
                {
                    string data = _objGameServer.Recv();
                    if (String.IsNullOrEmpty(data) == false)
                    {
                        int n = 0;
                        n++;
                        //            PacketType pt = SpartanGlobals.GetPacketType(ref data);
                        //            HandlePacketData(objAgent, pt, ref data);
                    }

                }
            }
            //for (int iAgent = 0; iAgent < Agents.Count; iAgent++)
            //{
            //    objAgent = Agents[iAgent];
            //    try
            //    {
            //        if (!objAgent.IsConnected())
            //            continue;

            //        originaldata = objAgent.Recv();
            //        data = originaldata;

            //        if (String.IsNullOrEmpty(data) == false)
            //        {
            //            //Do stuff
            //            PacketType pt = SpartanGlobals.GetPacketType(ref data);
            //            HandlePacketData(objAgent, pt, ref data);
            //        }
            //    }
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            base.OnClosing(e);
            if (_process != null)
            {
                _process.Refresh();
                _process.Close();
            }
        }

        private void ResizeEmbeddedApp()
        {
            if (_process == null)
                return;

            SetWindowPos(_process.MainWindowHandle, IntPtr.Zero, 0, 0, (int)_panel.ClientSize.Width, (int)_panel.ClientSize.Height, SWP_NOZORDER | SWP_NOACTIVATE);
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            Size size = base.MeasureOverride(availableSize);
            ResizeEmbeddedApp();
            return size;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _process.Kill();
            _process.WaitForExit();
            _process.Dispose();
        }
    }
}
