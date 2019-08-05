using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace GameHost 
{
    public static class Globals
    {

        private static MainWindow _objMainWindow = null;
        public static MainWindow MainWindow { get { return _objMainWindow; } }
        public const string ProgramVersion = "0.02";
        public static double GetProgramVersion()
        {
            double d = 0;
            Double.TryParse(ProgramVersion, out d);
            return d;
        }
        public const string ApplicationTitle = "Indieworks";

        #region Network
        public static class Network {
            public const int GameServerPort = 44244;

#if DEBUG
            public const int GameServerConnectionAttemptTimeoutMS = 60000;
            public const int GameHostDisconnectAttemptTimeoutMs = 60000;
            public const int SendAndRecvTimeoutMs = 60000;
#else
            public const int GameServerConnectionAttemptTimeoutMS = 30000;
            public const int GameHostDisconnectAttemptTimeoutMs = 2000;
            public const int SendAndRecvTimeoutMs = 2000;
#endif
        }
        #endregion


        public static string SupportedLoadSpriteImageFilter = "Image Files (*.png;*.bmp;*.gif;*.jpg)|*.png;*.bmp;*.gif;*.jpg|All files (*.*)|*.*";
        public static string GifFilter = "Gif Files (*.gif)|*.gif;|All files (*.*)|*.*";
        public static string ExeFilter = "Exe Files (*.exe)|*.exe|All files (*.*)|*.*";
        public static string ProjectExt = ".iwp";
        public static string ProjectFilter = "Project Files (*" + ProjectExt + ")|*" + ProjectExt + "";
        public static string AllFilter = "All files (*.*)|*.*";


        public static void Initialize(MainWindow w)
        {
            _objMainWindow = w;
            Proteus.Globals.InitializeGlobals("GameHostLog.txt");
        }
        private static Action EmptyDelegate = delegate () { };
        public static void UpdateLayout(DependencyObject ob)
        {
            //Immediately updates the layout of a content control.  Kind of a hack.
            ob.Dispatcher.Invoke(DispatcherPriority.Render, EmptyDelegate);
        }
        public static T FindParent<T>(DependencyObject child) where T : DependencyObject
        {
            //get parent item
            DependencyObject parentObject = VisualTreeHelper.GetParent(child);

            //we've reached the end of the tree
            if (parentObject == null) return null;

            //check if the parent matches the type we're looking for
            T parent = parentObject as T;
            if (parent != null)
                return parent;
            else
                return FindParent<T>(parentObject);
        }
        public static List<DependencyObject> GetSiblings(DependencyObject ob)
        {
            List<DependencyObject> ret = new List<DependencyObject>();
            DependencyObject parent = VisualTreeHelper.GetParent(ob);
            if (parent == null)
            {
                return ret;
            }

            int count = VisualTreeHelper.GetChildrenCount(parent);
            for(int idx=0; idx<count; ++idx)
            {
                ret.Add(VisualTreeHelper.GetChild(parent, idx));
            }

            return ret;
        }
        public static T FindAncestor<T>(DependencyObject child) where T : DependencyObject
        {
            List<DependencyObject> sibs = GetSiblings(child);
            foreach (DependencyObject sib in sibs)
            {
                T tsib = sib as T;
                if (tsib != null)
                {
                    return sib as T;
                }
            }

            //get parent item
            DependencyObject parentObject = VisualTreeHelper.GetParent(child);

            //we've reached the end of the tree
            if (parentObject == null)
            {
                return null;
            }

            return FindAncestor<T>(parentObject);
        }
        public static DependencyObject FindAncestor(DependencyObject child, string sibname)
        {
            List<DependencyObject> sibs = GetSiblings(child);
            foreach(DependencyObject sib in sibs)
            {
                if ((sib as FrameworkElement) != null && (sib as FrameworkElement).Name.Equals(sibname))
                {
                    return sib;
                }
            }

            //get parent item
            DependencyObject parentObject = VisualTreeHelper.GetParent(child);

            //we've reached the end of the tree
            if (parentObject == null)
            {
                return null;
            }

            return FindAncestor(parentObject, sibname);
        }
        public static string GetEnumDescription(Enum value)
        {
            FieldInfo fi = value.GetType().GetField(value.ToString());

            DescriptionAttribute[] attributes =
                (DescriptionAttribute[])fi.GetCustomAttributes(
                typeof(DescriptionAttribute),
                false);

            if (attributes != null &&
                attributes.Length > 0)
            {
                return attributes[0].Description;
            }

            return value.ToString();

        }

        private static Random Random = new Random();
        public static double RandomDouble(double min, double max)
        {
            if (min > max)
            {
                double tmp = max;
                max = min;
                min = tmp;
            }
            return min + (max - min) * Random.NextDouble();
        }


        #region File & Path IO
        public static bool PathIsDirectory(string path)
        {
            if ((File.GetAttributes(path) & FileAttributes.Directory) == FileAttributes.Directory)
            {
                return true;
            }
            return false;
        }
        public static void CountItemsInDirectoryTree(string path, out int out_files, out int out_dirs)
        {
            out_files = 0;
            out_dirs = 0;

            if (!Globals.PathIsDirectory(path))
            {
                path = System.IO.Path.GetDirectoryName(path);
            }

            List<string> pathsToSearch = new List<string>();
            pathsToSearch.Add(path);

            while (pathsToSearch.Count > 0)
            {
                string curpath = pathsToSearch[0];
                pathsToSearch.RemoveAt(0);

                out_files += System.IO.Directory.GetFiles(curpath).Length;

                string[] dirs = System.IO.Directory.GetDirectories(curpath);

                out_dirs += dirs.Length;
                foreach (string dir in dirs)
                {
                    pathsToSearch.Add(dir);
                }
            }

        }
        public static void WipeDirectory(string path, bool deleteFileStructure = true, bool swallowErrors = true)
        {
            //Deletes all files from a directory tree and optionally wipes the whole thing.
            ProgressWindow pr = new ProgressWindow();
            pr.Execute((token) =>
            {
                if (!Globals.PathIsDirectory(path))
                {
                    path = System.IO.Path.GetDirectoryName(path);
                }

                int fileCount = 0, dirCount = 0;
                Globals.CountItemsInDirectoryTree(path, out fileCount, out dirCount);

                int maxFileCount = fileCount;

                List<string> pathsToSearch = new List<string>();
                pathsToSearch.Add(path);

                while (pathsToSearch.Count > 0)
                {
                    token.ThrowIfCancellationRequested();

                    string curpath = pathsToSearch[0];
                    pathsToSearch.RemoveAt(0);

                    string[] files = System.IO.Directory.GetFiles(curpath);
                    foreach (string file in files)
                    {
                        try
                        {
                            File.Delete(file);
                        }
                        catch (Exception ex)
                        {
                            if (swallowErrors == false)
                            {
                                throw ex;
                            }
                        }
                        fileCount--;
                        pr.UpdateProgressAsync(1.0f - ((float)fileCount / (float)maxFileCount));
                    }
                    string[] dirs = System.IO.Directory.GetDirectories(curpath);
                    foreach (string dir in dirs)
                    {
                        pathsToSearch.Add(dir);
                    }
                }
                if (deleteFileStructure)
                {
                    try
                    {
                        Directory.Delete(path, true);
                    }
                    catch (Exception ex)
                    {
                        if (swallowErrors == false)
                        {
                            throw ex;
                        }
                    }
                }
            });
        }
        public static bool PathsAreEqual(string a, string b)
        {
            return string.Equals(System.IO.Path.GetFullPath(a),
                            System.IO.Path.GetFullPath(b), StringComparison.OrdinalIgnoreCase);
        }
        public static string GetRelativePath(string fileOrPath, string folder)
        {
            Uri pathUri = new Uri(fileOrPath);
            // Folders must end in a slash
            if (!folder.EndsWith(Path.DirectorySeparatorChar.ToString()))
            {
                folder += Path.DirectorySeparatorChar;
            }
            Uri folderUri = new Uri(folder);
            return Uri.UnescapeDataString(folderUri.MakeRelativeUri(pathUri).ToString().Replace('/', Path.DirectorySeparatorChar));
        }
        public static string GetDocumentsFolderPath()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }
        public static string GetDesktopFolderPath()
        {
            return Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
        }
        public static string GetLocalSettingsFilePath()
        {
            string loc = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonDocuments), "GameHost");
            loc = System.IO.Path.Combine(loc, "localsettings." + Globals.ProgramVersion + ".dat");
            return loc;
        }
        public static string ShortenFilePath(string ts, int maxlen = 16)
        {
            string ret = ts;

            if (ts.Length > maxlen)
            {
                ret = ts.Substring(0, maxlen) + "..." + ts.Substring((ts.Length - maxlen), maxlen);
            }

            return ret;
        }
        public static string NormalizePath(string path)
        {
            string p = "";
            if (string.IsNullOrEmpty(path) == false)
            {
                try
                {
                    p = Path.GetFullPath(new Uri(path).LocalPath)
                              .TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar)
                              .ToUpperInvariant();
                }
                catch (Exception ex)
                {
                    //blah
                }

                return p;
            }

            return p;

        }
        public static void SafeWriteFile(string fn, string data)
        {
            string dir = System.IO.Path.GetDirectoryName(fn);
            if (!System.IO.Directory.Exists(dir))
            {
                System.IO.Directory.CreateDirectory(dir);
            }
            using (FileStream file = new FileStream(fn, FileMode.Create, FileAccess.Write, FileShare.Read))
            {
                using (StreamWriter writer = new StreamWriter(file, Encoding.Unicode))
                {
                    writer.Write(data);
                }
            }
        }
        public static string SafeReadFile(string fn)
        {
            string dir = System.IO.Path.GetDirectoryName(fn);
            if (!System.IO.Directory.Exists(dir))
            {
                System.IO.Directory.CreateDirectory(dir);
            }
            using (FileStream file = new FileStream(fn, FileMode.OpenOrCreate, FileAccess.Read, FileShare.Read))
            {
                using (StreamReader reader = new StreamReader(file, Encoding.Unicode))
                {
                    return reader.ReadToEnd();
                }
            }
        }
        #endregion

        #region Registry (Unused)
        public static string RecentFilesRegKey = "RecentFiles";
        private static string GetLocalSettingsRegKey(string Keyname)
        {
            return "Software" + "/" + ApplicationTitle + "/" + ProgramVersion + "/" + Keyname;
        }
        public static void SaveRegistryValue(string skey, string value)
        {

            RegistryKey key;
            key = Registry.LocalMachine.OpenSubKey(GetLocalSettingsRegKey(skey), true);

            if (key == null)
            {
                key = Registry.LocalMachine.OpenSubKey("Software", true);

                key.CreateSubKey(ApplicationTitle);
                key = key.OpenSubKey(ApplicationTitle, true);

                key.CreateSubKey(ProgramVersion);
                key = key.OpenSubKey(ProgramVersion, true);
            }

            key.SetValue(skey, value);

        }
        public static string LoadRegistryValue(string skey)
        {
            RegistryKey key;
            key = Registry.LocalMachine.OpenSubKey(GetLocalSettingsRegKey(skey), true);

            if (key == null)
            {
                return "";
            }
            else
            {
                return key.GetValue(skey) as string;
            }

        }
        #endregion

        public static string TimeSpanToString(TimeSpan t)
        {
            string shortForm = "";
            if (t.Hours > 0)
            {
                shortForm += string.Format("{0}h", t.Hours.ToString());
            }
            if (t.Minutes > 0)
            {
                shortForm += string.Format("{0}m", t.Minutes.ToString());
            }
            if (t.Seconds > 0)
            {
                shortForm += string.Format("{0}s", t.Seconds.ToString());
            }
            if (t.Milliseconds > 0)
            {
                shortForm += string.Format("{0}ms", t.Milliseconds.ToString());
            }
            return shortForm;
        }

        public static void ShowError(string s, string caption = "Error", System.Windows.Window owner = null)
        {
            CustomMessageBox.Show(s, caption, System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Error);
        }
        public static void ShowInfo(string s, string caption = "Info", System.Windows.Window owner = null)
        {

            CustomMessageBox.Show(s, caption, System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Information);
        }
        public static string ResourceDirectory = "Resources/";
        public static Bitmap DefaultImage = null;
        public static Bitmap GetDefaultXImage()
        {
            if (DefaultImage == null)
            {
                try
                {
                    DefaultImage = new Bitmap("Resources/invalid_file.png");
                }
                catch (Exception ex)
                {
                    DefaultImage = new Bitmap(64, 64);
                }
            }
            return DefaultImage;
        }
        public static Bitmap BitmapImage2Bitmap(BitmapImage bitmapImage)
        {
            // BitmapImage bitmapImage = new BitmapImage(new Uri("../Images/test.png", UriKind.Relative));

            using (MemoryStream outStream = new MemoryStream())
            {
                BitmapEncoder enc = new BmpBitmapEncoder();
                enc.Frames.Add(BitmapFrame.Create(bitmapImage));
                enc.Save(outStream);
                System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(outStream);

                return new Bitmap(bitmap);
            }
        }
        public static BitmapImage Bitmap2BitmapImage(Bitmap bitmap)
        {
            using (var memory = new MemoryStream())
            {
                bitmap.Save(memory, ImageFormat.Png);
                memory.Position = 0;

                var bitmapImage = new BitmapImage();
                bitmapImage.BeginInit();
                bitmapImage.StreamSource = memory;
                bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                bitmapImage.EndInit();
                bitmapImage.Freeze();

                return bitmapImage;
            }
        }
        public static RenderTargetBitmap GetRenderTargetBitmap(Action<DrawingContext> drawfunc)
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            DrawingContext drawingContext = drawingVisual.RenderOpen();
            drawfunc(drawingContext);

            drawingContext.Close();

            RenderTargetBitmap bmp = new RenderTargetBitmap(180, 180, 120, 96, PixelFormats.Pbgra32);
            bmp.Render(drawingVisual);

            return bmp;
        }

        public static void CopyRegionIntoImage(Bitmap srcBitmap, Rectangle srcRegion,
            ref Bitmap destBitmap, Rectangle destRegion)
        {
            using (Graphics graphics = Graphics.FromImage((Image)destBitmap))
            {
                graphics.DrawImage((Image)srcBitmap, destRegion, srcRegion, GraphicsUnit.Pixel);
            }
        }

        public static byte[] Combine(byte[] a, byte[] b)
        {
            byte[] numArray = new byte[a.Length + b.Length];
            Buffer.BlockCopy((Array)a, 0, (Array)numArray, 0, a.Length);
            Buffer.BlockCopy((Array)b, 0, (Array)numArray, a.Length, b.Length);
            return numArray;
        }

        public static string[] GetValidUserFolder(string initialdir, bool multiple)
        {
            using (FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog())
            {
                folderBrowserDialog.Description = "Select a folder";
                if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
                    return new string[1]
                    {
                        folderBrowserDialog.SelectedPath
                    };
            }
            return new string[0];
        }

        public static string[] GetValidOpenSaveUserFile(bool bSave, string saveFilter,
            string loadFilter, string defaultext, string initialdir, bool multiple = false, string defaultName = "")
        {
            int num = 0;
            Microsoft.Win32.FileDialog fileDialog;
            string str;
            if (!bSave)
            {
                fileDialog = (Microsoft.Win32.FileDialog)new Microsoft.Win32.OpenFileDialog();
                str = loadFilter;
                (fileDialog as Microsoft.Win32.OpenFileDialog).Multiselect = multiple;
            }
            else
            {
                fileDialog = (Microsoft.Win32.FileDialog)new Microsoft.Win32.SaveFileDialog();
                str = saveFilter;
            }
            string fullPath = Path.GetFullPath(initialdir);
            fileDialog.InitialDirectory = fullPath;
            fileDialog.DefaultExt = defaultext;
            fileDialog.Filter = str;
            fileDialog.FilterIndex = num;
            if (string.IsNullOrEmpty(defaultName) == false)
            {
                fileDialog.FileName = defaultName + defaultext;
            }
            if (fileDialog.ShowDialog().Value == false)
            {
                return new string[0];
            }
            if (!bSave && !File.Exists(fileDialog.FileName))
            {
                return new string[0];
            }
            return fileDialog.FileNames;
        }



    }
}




