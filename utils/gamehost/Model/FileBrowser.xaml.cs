using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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

namespace GameHost
{
    public enum FileBrowserBrowseMode
    {
        OpenFile,
        SaveFile,
        OpenFolder
    }
    /// <summary>
    /// Interaction logic for FileBrowser.xaml
    /// </summary>
    public partial class FileBrowser : UserControl
    {

        //public static readonly DependencyProperty TextInternal = DependencyProperty.Register(@"Text", typeof(string), typeof(FileBrowser));

        //public string Text
        //{
        //    get
        //    {
        //        return (double)GetValue(TextInternal);
        //    }
        //    set
        //    {
        //        SetValue(TextInternal, value);
        //    }
        //}

        public FileBrowser()
        {
            InitializeComponent();
        }

        public delegate void FileSelectHandler(FileBrowser b, string[] filesOrFolders);
        public event FileSelectHandler FileSelect;
        public event EventHandler TextChanged = null;
        public new event EventHandler KeyUp = null;

        string _selectedPath = "";
        public string SelectedPath
        {
            get
            {
                return _selectedPath;
            }
            set
            {
                _selectedPath = value;
                _txtPath.Text = value;
            }
        }//The path the user selected
        public string DisplayPath { get { return _txtPath.Text; } set { _txtPath.Text = value; } } //Path shown in the textbox

        public string Root { get; set; } = "C:\\";
        public string Filter { get; set; } = "png | *.png";
        public string DefaultExt { get; set; } = "png";
        public FileBrowserBrowseMode BrowseMode { get; set; } = FileBrowserBrowseMode.OpenFile;
        public bool Multiple { get; set; } = false;
        public Regex InputFilterRegex { get; set; } = null;

        public void SetBoxColor(System.Windows.Media.Brush c)
        {
            _txtPath.BorderBrush = c;
        }

        private void btnSelect_Click(object sender, RoutedEventArgs e)
        {
            string[] selected = new string[0];
            if (BrowseMode == FileBrowserBrowseMode.OpenFolder)
            {
                selected = Globals.GetValidUserFolder(Root, Multiple);
            }
            else
            {
                selected = Globals.GetValidOpenSaveUserFile(BrowseMode == FileBrowserBrowseMode.SaveFile, Filter,
                    Filter, DefaultExt, Root, Multiple);
            }
            if (selected.Length > 0)
            {
                SelectedPath = selected.Aggregate((x, y) => x + ";" + y);
                _txtPath.Text = SelectedPath;
                FileSelect?.Invoke(this, selected);
            }

        }

        private void _txtPath_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (InputFilterRegex != null)
            {
                e.Handled = InputFilterRegex.IsMatch(e.Text);
            }
        }

        private void _txtPath_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextChanged?.Invoke(sender, e);
        }

        private void _txtPath_KeyUp(object sender, KeyEventArgs e)
        {
            KeyUp?.Invoke(sender, e);

        }
    }
}
