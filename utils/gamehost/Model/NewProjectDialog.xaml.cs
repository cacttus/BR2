using System;
using System.Collections.Generic;
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
    /// Interaction logic for NewProjectDialog.xaml
    /// </summary>
    public partial class NewProjectDialog : ToolWindowBase
    {
        public ProjectFile ProjectFile { get; set; }

        public NewProjectDialog( string sTitle)
        {
            InitializeComponent();
            Title = sTitle;

            _objProjectRootFileBrowser.SelectedPath = Globals.GetDesktopFolderPath();
                
        }
        bool _bDoNotAppendDir_UserHasModifiedManually = false;
        private void _frmNewProjectDialog_Loaded(object sender, RoutedEventArgs e)
        {
            _objProjectRootFileBrowser.BrowseMode = FileBrowserBrowseMode.OpenFolder;
            _objProjectRootFileBrowser.Root = Globals.GetDocumentsFolderPath();
            _objProjectRootFileBrowser.FileSelect += (x, filesOrFolders) =>
            {
                _bDoNotAppendDir_UserHasModifiedManually = false;
                UpdateAppendNewProjectDir();
            };
            _objProjectRootFileBrowser.KeyUp += (x, y) =>
            {
                _bDoNotAppendDir_UserHasModifiedManually = true;
            };

            _txtProjectName.Text = "My Project";
        }

        protected override List<string> Validate()
        {
            List<string> results = new List<string>();
            _txtProjectName.BorderBrush = Brushes.Black;
            _objProjectRootFileBrowser.SetBoxColor(Brushes.Black);

            if (String.IsNullOrEmpty(_txtProjectName.Text))
            {
                results.Add("Please enter project name.");
                _txtProjectName.BorderBrush = Brushes.Red;
            }

            if (String.IsNullOrEmpty(_objProjectRootFileBrowser.SelectedPath))
            {
                results.Add("Please set project path.");
                _objProjectRootFileBrowser.SetBoxColor(Brushes.Red);
            }

            return results;
        }

        public override bool OkButtonClicked()
        {
            SaveAndClose(() =>
            {
                try
                {
                    ProjectFile = new ProjectFile();
                    ProjectFile.ProjectName = _txtProjectName.Text;
                    ProjectFile.LoadedOrSavedProjectFileName =  System.IO.Path.Combine(_objProjectRootFileBrowser.DisplayPath, ProjectFile.ProjectName);
                    ProjectFile.LoadedOrSavedProjectFileName += Globals.ProjectExt;

                    if (CheckProjectDoesNotAlreadyExistAtSelectedLocation() == false)
                    {
                        return false;
                    }
                }
                catch (Exception ex)
                {
                    Globals.MainWindow.LogError("Failed to create project: " + ex);
                }

                return true;
            });
            return false;//Prevent OK from closing dialog, we manually call close on the validation
        }
        private bool CheckProjectDoesNotAlreadyExistAtSelectedLocation()
        {
            if (System.IO.Directory.Exists(System.IO.Path.GetDirectoryName(ProjectFile.LoadedOrSavedProjectFileName)))
            {
                string msg = "The project directory '" + ProjectFile.LoadedOrSavedProjectFileName + "' already exists ";

                if (System.IO.File.Exists(ProjectFile.LoadedOrSavedProjectFileName))
                {
                    msg += " and it already contains a project file ";
                }
                msg += ". What would you like to do?";

                MessageBoxResult r = CustomMessageBox.Show(msg, "Warning", MessageBoxButton.YesNoCancel, MessageBoxImage.Asterisk,
                    new List<string>() { "Delete entire project (including images)", "Just Delete Project File", "Cancel" });

                if (r == MessageBoxResult.Yes)
                {
                    //delete everything
                    Globals.WipeDirectory(ProjectFile.LoadedOrSavedProjectFileName);
                }
                else if (r == MessageBoxResult.No)
                {
                    System.IO.File.Delete(ProjectFile.LoadedOrSavedProjectFileName);
                }
                else if (r == MessageBoxResult.Cancel)
                {
                    return false;
                }
            }
            return true;
        }
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            ProjectFile = null;
            Close();
        }

        private void _chkCreateDirectory_Checked(object sender, RoutedEventArgs e)
        {
            UpdateAppendNewProjectDir();
        }

        private void UpdateAppendNewProjectDir()
        {
            if (_chkCreateDirectory != null && _objProjectRootFileBrowser != null)
            {
                if (_bDoNotAppendDir_UserHasModifiedManually == false)
                {
                    if (_chkCreateDirectory.IsChecked == true)
                    {
                        _objProjectRootFileBrowser.DisplayPath = System.IO.Path.Combine(_objProjectRootFileBrowser.SelectedPath, _txtProjectName.Text);
                    }
                    else
                    {
                        _objProjectRootFileBrowser.DisplayPath = _objProjectRootFileBrowser.SelectedPath;
                    }
                }

            }
        }

        private void _objProjectRootFileBrowser_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void _txtProjectName_TextChanged(object sender, TextChangedEventArgs e)
        {
            UpdateAppendNewProjectDir();
        }
    }
}
