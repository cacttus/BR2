using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for FolderBrowserWindow.xaml
    /// </summary>
    public partial class FolderBrowserWindow : ToolWindowBase
    {
        public FolderBrowserWindow(string title)
        {
            InitializeComponent();
            Title = title;
            RefreshEverything();
        }

        void CreateNode(TreeViewItem tvParent, string fullPath)
        {
            TreeViewItem item = new TreeViewItem();
            item.Tag = fullPath;
            item.FontWeight = FontWeights.Normal;

            bool isFile = System.IO.File.Exists(fullPath);

            StackPanel pan = new StackPanel();
            pan.Orientation = Orientation.Horizontal;
            //Try set the icon
            try
            {
                Icon icon = null;
                if (isFile)
                {
                    icon = NativeMethods.GetFileIcon(fullPath, NativeMethods.IconSize.Small);
                }
                else
                {
                    icon = NativeMethods.GetFolderIcon(NativeMethods.IconSize.Small, NativeMethods.FolderType.Closed);
                }
                System.Windows.Controls.Image image = new System.Windows.Controls.Image();
                image.Height = 16;
                image.Source = NativeMethods.ToImageSource(icon);
                pan.Children.Add(image);
                pan.Children.Add(new TextBlock(new Run(System.IO.Path.GetFileName(fullPath))));
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError(ex.ToString());
            }
            item.Header = pan;

            //Create Open Context Menu
            string contextMenuText = "Open Folder In Explorer";
            if (isFile)
            {
                contextMenuText = "Open File";
            }
            item.ContextMenu = new ContextMenu();
            MenuItem openFileMenuItem = new MenuItem();
            openFileMenuItem.Header = contextMenuText;
            openFileMenuItem.Click += (x, e) =>
            {
                System.Diagnostics.Process.Start(fullPath);
            };
            item.ContextMenu.Items.Add(openFileMenuItem);
            item.MouseRightButtonUp += (x, e) =>
            {
                item.ContextMenu.Visibility = Visibility.Visible;
            };

            // Create Expand code
            item.Collapsed += (x, e) =>
            {
                TreeViewItem tvItem = x as TreeViewItem;
                if (x != null)
                {
                    //Make sure to clear before refresh
                    tvItem.Items.Clear();
                }
            };
            item.Expanded += (x, e) =>
            {
                TreeViewItem tvItem = x as TreeViewItem;
                if (x != null)
                {
                    //Make sure to clear before refresh
                   // tvItem.Items.Clear();

                    string dirtag = (string)tvItem.Tag;

                    if (System.IO.Directory.Exists(dirtag))
                    {
                        string[] dirs = System.IO.Directory.GetDirectories(dirtag);
                        foreach (string dir in dirs)
                        {
                            CreateNode(tvItem, dir);
                        }
                        string[] files = System.IO.Directory.GetFiles(dirtag);
                        foreach (string file in files)
                        {
                            CreateNode(tvItem, file);
                        }
                    }
                    else if (System.IO.File.Exists(dirtag))
                    {
                        //itsa file
                        System.Diagnostics.Process.Start(dirtag);
                    }
                }

            };
            if (tvParent == null)
            {
                _tvFolders.Items.Add(item);
            }
            else
            {
                tvParent.Items.Add(item);
            }
        }
        public void RefreshEverything()
        {
            _tvFolders.Items.Clear();
            if (Globals.MainWindow.ProjectFile != null)
            {
                string rootDir = System.IO.Path.GetDirectoryName(Globals.MainWindow.ProjectFile.LoadedOrSavedProjectFileName);
                CreateNode(null, rootDir);
            }
            else
            {
                _lblInfo.Content = "No project file is loaded.";
            }
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            RefreshEverything();

        }

  
        private void _btnCollapseAll_Click(object sender, RoutedEventArgs e)
        {

            CollapseTree(null);
        }
        private void CollapseTree(TreeViewItem parent)
        {
            if (parent == null)
            {
                foreach (TreeViewItem child in _tvFolders.Items)
                {
                    if (child != null)
                    {
                        CollapseTree(child);
                        child.IsExpanded = false;
                    }
                }
            }
            else
            {
                foreach (TreeViewItem child in parent.Items)
                {
                    if (child != null)
                    {
                        CollapseTree(child);
                        child.IsExpanded = false;
                    }
                }
            }
            if (parent != null)
            {
                parent.IsExpanded = false;
            }
        }
    }
}
