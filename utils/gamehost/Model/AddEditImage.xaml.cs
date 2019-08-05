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
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for AddEditImage.xaml
    /// </summary>
    public partial class AddEditImage : AddEditBase
    {
        public ImageResource ImageResource { get; set; } = null;
        private static readonly Regex _regex = new Regex("[^0-9.-]+");

        public AddEditImage( string title, ImageResource ir) : base( title)
        {
            InitializeComponent();


            Label tooltip = new Label();
            tooltip.Content = "Check this to copy the image to the project directory.  " +
                "It is recommended you keep all images under /Images.  " +
                "If the file you select is alread in the /Images folder no copy is performed.";
            _chkCopyToResourceDir.ToolTip = tooltip;

            //Project file must be loaded.
            _objFileBrowser.Filter = Globals.SupportedLoadSpriteImageFilter;
            _objFileBrowser.BrowseMode = FileBrowserBrowseMode.OpenFile;
            _objFileBrowser.Root = System.IO.Path.GetDirectoryName(Globals.MainWindow.ProjectFile.LoadedOrSavedProjectFileName);
            _objFileBrowser.FileSelect += (x, filesOrFolders) =>
            {
                if (filesOrFolders.Length == 1)
                {
                    ImageResource.Path = filesOrFolders[0];

                    _objImagePreview.ImageResource = ImageResource;
                }
            };

            InitPopulate(ir);

        }

        private void InitPopulate(ImageResource ir)
        {
            //Populat Window
            if (ir == null)
            {
                ImageResource = new ImageResource(Globals.MainWindow.ProjectFile.GenId());
                _txtResourceName.Text = GenerateResourceName();
                AddEditMode = AddEditMode.Add;
            }
            else
            {
                //Edit*
                ImageResource = ir;
                _objFileBrowser.DisplayPath = ir.Path;
                _txtResourceName.Text = ir.Name;
                AddEditMode = AddEditMode.Edit;
            }

            _objImagePreview.ImageResource = ImageResource;
        }
        private string GenerateResourceName()
        {
            string rsc = "Image-";
            for (int i = 1; i < 100000; ++i)
            {
                bool found = false;
                string newname = rsc + i.ToString();
                foreach (ImageResource ir in Globals.MainWindow.ProjectFile.Images)
                {
                    if (ir.Name.Equals(newname))
                    {
                        found = true;
                        break;
                    }
                }
                if (found == false)
                {
                    return newname;
                }
            }
            return "";
        }
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            this.Close();
        }

        private void rbImageTypeAtlas_Checked(object sender, RoutedEventArgs e)
        {
            if (gdAtlasOptions != null)
            {
                gdAtlasOptions.Visibility = Visibility.Visible;
            }
        }

        private void rbImageTypeImage_Checked(object sender, RoutedEventArgs e)
        {
            if (gdAtlasOptions != null)
            {
                gdAtlasOptions.Visibility = Visibility.Collapsed;
            }
        }

        protected override List<String> Validate()
        {
            List<string> results = new List<string>();
            _txtResourceName.BorderBrush = System.Windows.Media.Brushes.Black;
            _objFileBrowser.SetBoxColor(System.Windows.Media.Brushes.Black);

            if (String.IsNullOrEmpty(ImageResource.Name))
            {
                results.Add("Please set the resource name.");
                _txtResourceName.BorderBrush = System.Windows.Media.Brushes.Red;
            }

            foreach (ImageResource ir in Globals.MainWindow.ProjectFile.Images)
            {
                if (ir.Name.Equals(ImageResource.Name))
                {
                    if (ir != ImageResource)
                    {
                        results.Add("Resource name '" + ir.Name + "' must be unique.");
                        _txtResourceName.BorderBrush = System.Windows.Media.Brushes.Red;
                        break;
                    }
                }
            }

            if (String.IsNullOrEmpty(ImageResource.Path))
            {
                results.Add("Please set image path.");
                _objFileBrowser.SetBoxColor(System.Windows.Media.Brushes.Red);
            }

            if (System.IO.File.Exists(ImageResource.Path) == false)
            {
                results.Add("Image file does not exist.");
                _objFileBrowser.SetBoxColor(System.Windows.Media.Brushes.Red);
            }

            return results;
        }

        public override bool OkButtonClicked()
        {
            SaveAndClose(() =>
            {
                //Most if not all resources should be copied to the project folder.
                if (_chkCopyToResourceDir.IsChecked == true)
                {
                    string p0 = System.IO.Path.GetFullPath(_objFileBrowser.DisplayPath);
                    string p1 = System.IO.Path.GetFullPath(ImageResource.Path);

                    if ((AddEditMode == AddEditMode.Edit && p0 != p1) || AddEditMode == AddEditMode.Add)
                    {
                        if (CopyResourceToLocalResourceFolder() == false)
                        {
                            return false;
                        }
                    }
                }

                //Run this when we validate successfully
                DialogResult = true;

                //Add resource to project file.
                if (AddEditMode == AddEditMode.Add)
                {
                    if (!Globals.MainWindow.ProjectFile.Images.Contains(ImageResource))
                    {
                        Globals.MainWindow.ProjectFile.Images.Add(ImageResource);
                    }
                    else
                    {
                        //test
                        System.Diagnostics.Debugger.Break();
                    }
                }
                else if (AddEditMode == AddEditMode.Edit)
                {

                }
                else
                {
                    throw new NotImplementedException();
                }

                return true;
            });
            return false;
        }
        private bool CopyResourceToLocalResourceFolder()
        {
            //Copy the given image to the ./{ProjectRoot}/images folder
            //Returns false if the user cancelled the operation
            try
            {
                Globals.MainWindow.ProjectFile.SetCwdToRoot();

                //Copy File.
                string imgCurFolder = System.IO.Path.GetDirectoryName(ImageResource.Path);
                string imgCurFilename = System.IO.Path.GetFileName(ImageResource.Path);
                string imgNewFilename = System.IO.Path.Combine(Globals.MainWindow.ProjectFile.GetImageResourcePath(), imgCurFilename);
                string imgNewFolder = System.IO.Path.GetDirectoryName(imgNewFilename);

                if (Globals.PathsAreEqual(imgCurFolder, imgNewFolder))
                {
                    Globals.MainWindow.SetStatus("Image " + imgNewFilename + " already located in /images folder, no copy performed.");
                }
                else
                {
                    //Check to see if the file exists.  If it does then prompt hte user that there is a conflict.
                    bool? continueCopy = CheckImageDoesntExistBeforeCopying(imgNewFilename);

                    if (continueCopy == null)
                    {
                        return false;
                    }
                    else if (continueCopy == true)
                    {
                        if (System.IO.File.Exists(imgNewFilename))
                        {
                            System.IO.File.Delete(imgNewFilename);
                        }
                        System.IO.File.Copy(ImageResource.Path, imgNewFilename);
                        ImageResource.Path = Globals.GetRelativePath(imgNewFilename, Globals.MainWindow.ProjectFile.GetProjectRoot());
                    }
                    //Otherwise wer're goign to use the image in that folder.

                    //Set path and reload.
                    ImageResource.LoadIfNecessary(true);
                }
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to copy image to ouput directory: " + ex.ToString(), true);
                throw ex;
            }
            return true;
        }
        private bool? CheckImageDoesntExistBeforeCopying(string imgNewFileName)
        {
            //Check to see if the file exists.  If it does then prompt hte user that there is a conflict.
            //returns true if teh copy continues
            //return null if the user cancelled the operation
            //return false to stop the copy.
            if (System.IO.File.Exists(imgNewFileName))
            {
                string msg = "The file '' already exists.";

                string newRelpath = Globals.GetRelativePath(imgNewFileName, Globals.MainWindow.ProjectFile.GetProjectRoot());

                List<ImageResource> rs = Globals.MainWindow.ProjectFile.Images.Where(x => Globals.PathsAreEqual(x.Path, newRelpath)).ToList();
                if (rs != null && rs.Count > 0)
                {
                    string tots = string.Join(", ", rs.Select(x => "  " + x.Name + "\r\n"));

                    msg += "It appears that the resource(s):\r\n '" + tots + "' \r\n is/are using this path.";
                }
                else
                {
                    msg += "No resources were found to use this resource. ";
                }
                msg += "Would you like to overwrite this resource? Note: Clicking 'No' will cause this new resource to use the " +
                "existing resource in the resource folder.  " +
                "To use a new resource click 'Cancel' , uncheck 'copy resource' above, and try saving again.";

                //file already exists! ask user about this.
                MessageBoxResult r = CustomMessageBox.Show(msg, "Warning", MessageBoxButton.YesNoCancel, MessageBoxImage.Asterisk);
                if (r == MessageBoxResult.Cancel)
                {
                    return null;
                }
                else if (r == MessageBoxResult.Yes)
                {
                    return true;
                }
                else if (r == MessageBoxResult.No)
                {
                    return false;
                }
            }

            return true;
        }
        private void TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            e.Handled = _regex.IsMatch(e.Text);
        }

        //private void _btnImagePreview_Click(object sender, RoutedEventArgs e)
        //{
        //    _myImageViewer = new MapEditor(Globals.MainWindow, "Image Viewer", null, ImageResource, MapEditorMode.ImageViewer);
        //    _myImageViewer.ShowDialog(()=> { });
        //}

        private void _txtSpriteName_TextChanged(object sender, TextChangedEventArgs e)
        {
            ImageResource.Name = _txtResourceName.Text;
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void CheckBox_Checked_1(object sender, RoutedEventArgs e)
        {

        }
        public override bool OnClose()
        {
            //_myImageViewer?.Close();
            return true;
        }
    }
}
