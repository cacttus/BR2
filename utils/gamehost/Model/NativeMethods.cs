using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace GameHost
{
    public static class NativeMethods
    {
        private const int FILE_ATTRIBUTE_NORMAL = 0x80;
        private const int SHGFI_TYPENAME = 0x400;
        private const int SHGFI_USEFILEATTRIBUTES = 0x000000010;
        [DllImport("shell32.dll", CharSet = CharSet.Unicode)]
        private static extern IntPtr SHGetFileInfo(
            string pszPath,
            int dwFileAttributes,
            ref SHFILEINFO shinfo,
            uint cbfileInfo,
            int uFlags);


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        private struct SHFILEINFO
        {
            public SHFILEINFO(bool b)
            {
                hIcon = IntPtr.Zero;
                iIcon = 0;
                dwAttributes = 0;
                szDisplayName = "";
                szTypeName = "";
            }

            public IntPtr hIcon;
            public int iIcon;
            public uint dwAttributes;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string szDisplayName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
            public string szTypeName;
        };


        public static string GetShellFileType(string fileName)
        {
            var shinfo = new SHFILEINFO(true);
            const int flags = SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES;

            if (SHGetFileInfo(fileName, FILE_ATTRIBUTE_NORMAL, ref shinfo, (uint)Marshal.SizeOf(shinfo), flags) == IntPtr.Zero)
            {
                return "File";
            }

            return shinfo.szTypeName;
        }


        public enum FolderType
        {
            Closed,
            Open
        }

        public enum IconSize
        {
            Large,
            Small
        }

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DestroyIcon(IntPtr hIcon);

        public const int SHGFI_ICON = 0x000000100;
        // public const uint SHGFI_USEFILEATTRIBUTES = 0x000000010;
        public const int SHGFI_OPENICON = 0x000000002;
        public const int SHGFI_SMALLICON = 0x000000001;
        public const int SHGFI_LARGEICON = 0x000000000;
        public const int FILE_ATTRIBUTE_DIRECTORY = 0x00000010;
        public static Icon GetFolderIcon(IconSize size, FolderType folderType)
        {
            // Need to add size check, although errors generated at present!    
            int flags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

            if (FolderType.Open == folderType)
            {
                flags += SHGFI_OPENICON;
            }
            if (IconSize.Small == size)
            {
                flags += SHGFI_SMALLICON;
            }
            else
            {
                flags += SHGFI_LARGEICON;
            }
            // Get the folder icon    
            var shfi = new SHFILEINFO();

            var res = SHGetFileInfo(@"C:\Windows",
                FILE_ATTRIBUTE_DIRECTORY,
                ref shfi,
                (uint)Marshal.SizeOf(shfi),
                flags);

            if (res == IntPtr.Zero)
                throw Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error());

            // Load the icon from an HICON handle  
            Icon.FromHandle(shfi.hIcon);

            // Now clone the icon, so that it can be successfully stored in an ImageList
            var icon = (Icon)Icon.FromHandle(shfi.hIcon).Clone();

            DestroyIcon(shfi.hIcon);        // Cleanup    

            return icon;
        }
        public static Icon GetFileIcon(string fullpath, IconSize size)
        {
            var shfi = new SHFILEINFO();
            int flags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;
            if (IconSize.Small == size)
            {
                flags += SHGFI_SMALLICON;
            }
            else
            {
                flags += SHGFI_LARGEICON;
            }
            var res = SHGetFileInfo(fullpath,
                FILE_ATTRIBUTE_NORMAL,
                ref shfi,
                (uint)Marshal.SizeOf(shfi),
                flags);

            if (res == IntPtr.Zero)
            {
                throw Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error());
            }

            // Load the icon from an HICON handle  
            Icon.FromHandle(shfi.hIcon);
            var icon = (Icon)Icon.FromHandle(shfi.hIcon).Clone();
            DestroyIcon(shfi.hIcon);        // Cleanup    

            return icon;
        }

        [DllImport("gdi32.dll", SetLastError = true)]
        private static extern bool DeleteObject(IntPtr hObject);

        public static ImageSource ToImageSource(this Icon icon)
        {
            Bitmap bitmap = icon.ToBitmap();
            IntPtr hBitmap = bitmap.GetHbitmap();

            ImageSource wpfBitmap = Imaging.CreateBitmapSourceFromHBitmap(
                hBitmap,
                IntPtr.Zero,
                Int32Rect.Empty,
                BitmapSizeOptions.FromEmptyOptions());

            if (!DeleteObject(hBitmap))
            {
                throw new Win32Exception();
            }

            return wpfBitmap;
        }








        //    namespace IconExtractor
        //{
        //[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        //public struct SHFILEINFO
        //{
        //    public IntPtr hIcon;
        //    public int iIcon;
        //    public uint dwAttributes;
        //    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
        //    public string szDisplayName;
        //    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
        //    public string szTypeName;
        //};



        //    [DllImport("shell32.dll", CharSet = CharSet.Auto)]
        //    public static extern IntPtr SHGetFileInfo(string pszPath, uint dwFileAttributes, out SHFILEINFO psfi, uint cbFileInfo, uint uFlags);

        //    [DllImport("user32.dll", SetLastError = true)]
        //    [return: MarshalAs(UnmanagedType.Bool)]
        //    static extern bool DestroyIcon(IntPtr hIcon);

        //    public const uint SHGFI_ICON = 0x000000100;
        //    public const uint SHGFI_USEFILEATTRIBUTES = 0x000000010;
        //    public const uint SHGFI_OPENICON = 0x000000002;
        //    public const uint SHGFI_SMALLICON = 0x000000001;
        //    public const uint SHGFI_LARGEICON = 0x000000000;
        //    public const uint FILE_ATTRIBUTE_DIRECTORY = 0x00000010;

        //    public static Icon GetFolderIcon(IconSize size, FolderType folderType)
        //    {
        //        // Need to add size check, although errors generated at present!    
        //        uint flags = SHGFI_ICON | SHGFI_USEFILEATTRIBUTES;

        //        if (FolderType.Open == folderType)
        //        {
        //            flags += SHGFI_OPENICON;
        //        }
        //        if (IconSize.Small == size)
        //        {
        //            flags += SHGFI_SMALLICON;
        //        }
        //        else
        //        {
        //            flags += SHGFI_LARGEICON;
        //        }
        //        // Get the folder icon    
        //        var shfi = new SHFILEINFO();

        //        var res = SHGetFileInfo(@"C:\Windows",
        //            FILE_ATTRIBUTE_DIRECTORY,
        //            out shfi,
        //            (uint)Marshal.SizeOf(shfi),
        //            flags);

        //        if (res == IntPtr.Zero)
        //            throw Marshal.GetExceptionForHR(Marshal.GetHRForLastWin32Error());

        //        // Load the icon from an HICON handle  
        //        Icon.FromHandle(shfi.hIcon);

        //        // Now clone the icon, so that it can be successfully stored in an ImageList
        //        var icon = (Icon)Icon.FromHandle(shfi.hIcon).Clone();

        //        DestroyIcon(shfi.hIcon);        // Cleanup    

        //        return icon;
        //    }

        //public Form1()
        //{
        //    InitializeComponent();
        //}

        //private void Form1_Load(object sender, EventArgs e)
        //{
        //    try
        //    {

        //        Icon icon = GetFolderIcon(IconSize.Large, FolderType.Open);
        //        pictureBox1.Image = icon.ToBitmap();
        //        // Note: The image actually should be disposed somewhere
        //    }
        //    catch (Exception ex)
        //    {
        //        MessageBox.Show(ex.Message);
        //    }
        //}

    }


}
