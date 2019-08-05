using MahApps.Metro.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for FloatingWindow.xaml
    /// </summary>
    public partial class FloatingWindowHost : MetroWindow
    {
        [DllImport("user32.dll")]
        private static extern int GetWindowLong(IntPtr hWnd, int nIndex);
        [DllImport("user32.dll")]
        private static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        private const int GWL_STYLE = -16;
        public const uint WS_MINIMIZEBOX = 0x00020000;
        public const uint WS_MAXIMIZEBOX = 0x00010000;
        private const int WS_SYSMENU = 0x80000;

        public ToolWindowBase InnerControl = null;

        public void HideCloseButton()
        {
            var hwnd = new WindowInteropHelper(this).Handle;
            var value = GetWindowLong(hwnd, GWL_STYLE);
            SetWindowLong(hwnd, GWL_STYLE, (int)(value & ~WS_SYSMENU));
        }
        public void HideMaximizeButton()
        {
            var hwnd = new WindowInteropHelper(this).Handle;
            var value = GetWindowLong(hwnd, GWL_STYLE);
            SetWindowLong(hwnd, GWL_STYLE, (int)(value & ~WS_MINIMIZEBOX));
        }


        public Grid GetGrid() { return HostGrid; }
        public FloatingWindowHost()
        {
            InitializeComponent();
            SizeToContent = SizeToContent.WidthAndHeight;
            ShowInTaskbar = false;
            ShowMaxRestoreButton = false;
            Owner = Globals.MainWindow;
        }


        private void MetroWindow_SourceInitialized(object sender, EventArgs e)
        {
        }

        private void MetroWindow_Loaded(object sender, RoutedEventArgs e)
        {
        }


    }
}
