using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using MahApps.Metro.Controls;
using Dragablz.Dockablz;
using System.Diagnostics;

namespace GameHost
{

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        //Main project file for whole application!
        public ProjectFile ProjectFile { get; set; }
        public Layout FloatLayout { get { return _objFloatLayout; } }
        public WindowManager WindowManager { get; set; }
        public UndoManager UndoManager { get; set; } 
        public MenuItem UndoMenuItem { get { return _mnuUndo; } }
        public MenuItem RedoMenuItem { get { return _mnuRedo; } }
        public bool _bShown = false;

        public MainWindow()
        {
            InitializeComponent();

            _objExpandoPaneRoot._btnPopOut.Visibility = Visibility.Collapsed;
        }
        private void MetroWindow_ContentRendered(object sender, EventArgs e)
        {
            //Need to do this in order to set owner property of child windows
            if (_bShown == false)
            {
                _bShown = true;
                AfterShow();
            }
        }

        public void CheckDropZoneDrop(FloatingWindowHost host)
        {
            _objExpandoPaneRoot.DropToolWindow(host);

        }
        public void CheckDropZoneDrag(FloatingWindowHost host)
        {
            //Check to see if the grid is still present in the thing
            _objExpandoPaneRoot.UpdateDropZoneHover(host);
            Globals.UpdateLayout(_objExpandoPaneRoot);
        }
        private void AfterShow()
        {
            Globals.Initialize(this);

            WindowManager = new WindowManager();
            UndoManager = new UndoManager();

            UnloadAndRefresh();

            _objFloatLayout.AllowDrop = true;
            _objFloatLayout.IsFloatDropZoneEnabled = true;
            _objFloatLayout.SizeChanged += (x, e) =>
            {
                int n = 0;
                n++;
            };
            //_objFloatLayout.drag
            _objFloatLayout.ClosingFloatingItemCallback = (x) =>
            {
                //When closed.
                int n = 0;
                n++;
            };
            _objFloatLayout.PreviewDragEnter += (x, e) =>
            {
                int n = 0;
                n++;
            };
            _objFloatLayout.TargetUpdated += (x, e) =>
            {
                int n = 0;
                n++;
            };

            WindowManager.EditToolbarWindow.Show();
        }

        #region Private: UI Methods
        private void Prefs_Click(object sender, RoutedEventArgs e)
        {
            EditShowPrefs();
        }

        private void mnuNew_Click(object sender, RoutedEventArgs e)
        {
            FileCreateNew();
        }

        private void _mnuOpen_Click(object sender, RoutedEventArgs e)
        {
            FileOpen();
        }

        private void _mnuExit_Click(object sender, RoutedEventArgs e)
        {
            if (ProjectFile != null && ProjectFile.DataChanged)
            {
                MessageBoxResult r = CustomMessageBox.Show(
                    "The current file has unsaved changes. Would you like to save them?",
                    "Warning", MessageBoxButton.YesNoCancel, MessageBoxImage.Exclamation);
                if (r == MessageBoxResult.Yes)
                {
                    try
                    {
                        FileSaveProject();
                        Environment.Exit(0);
                    }
                    catch (Exception ex)
                    {
                        LogError(ex.ToString());
                    }
                }
                else if (r == MessageBoxResult.No)
                {
                    Environment.Exit(0);
                }
                else if (r == MessageBoxResult.Cancel)
                {
                    //Changed his mind.
                }
            }
            else
            {
                Environment.Exit(0);
            }
        }

        private void _mnuImages_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.ImagesWindow.Show();
 
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            Globals.ShowError("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
        }

        private void MenuItem_Click_1(object sender, RoutedEventArgs e)
        {
        }

        private void _mnuAbout_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.ShowToolWindow("About", new About());
        }

        private void _mnuOpenInExplorer_Click(object sender, RoutedEventArgs e)
        {
            OpenProjectExplorer();
        }

        private void _mnuTileEditor_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.ShowToolWindow("Tile Editor", new TileEditor(), false);
        }

        private void _mnuFolderBrowser_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.FolderBrowserWindow.Show();
        }

        private void MenuItem_Click_2(object sender, RoutedEventArgs e)
        {
            ProgressWindow pr = new ProgressWindow();
            pr.Execute((x) =>
            {
                int max = 10009000;
                for (int n = 0; n < max; ++n)
                {
                    x.ThrowIfCancellationRequested();

                    double f = Globals.RandomDouble(0.0, 1.0);
                    f = Math.Sqrt(f);
                    pr.UpdateProgressAsync(((double)n / (double)max), n.ToString(), 50);
                }
            });
        }

        private void _mnuUndo_Click(object sender, RoutedEventArgs e)
        {
            UndoManager.PerformUndo();
        }

        private void _mnuRedo_Click(object sender, RoutedEventArgs e)
        {
            UndoManager.PerformRedo();
        }

        private void _mnuSprites_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.SpritesWindow.Show();

        }

        private void _mnuMaps_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.MapsWindow.Show();
        }

        private void _btnSave_Click(object sender, RoutedEventArgs e)
        {
            FileSaveProject();
        }

        private void _btnSaveAll_Click(object sender, RoutedEventArgs e)
        {

        }

        private void _btnNew_Click(object sender, RoutedEventArgs e)
        {
            FileCreateNew();
        }

        private void _btnOpen_Click(object sender, RoutedEventArgs e)
        {
            FileOpen();
        }

        private void _btnUndo_Click(object sender, RoutedEventArgs e)
        {
            UndoManager.PerformUndo();
        }

        private void _btnRedo_Click(object sender, RoutedEventArgs e)
        {
            UndoManager.PerformRedo();
        }

        private void _btnPreferences_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.PreferencesWindow.ShowDialog(() =>
            {
                if (ProjectFile != null)
                {
                    ProjectFile.MarkChanged();
                }
            });
        }

        private void _mnuShowToolbar_Click(object sender, RoutedEventArgs e)
        {
            WindowManager.EditToolbarWindow.Show();

        }

        #endregion

        #region Public:Methods

        public void LogError(string e, bool messagebox = false)
        {
            SetStatus(e);
            if (messagebox)
            {
                Globals.ShowError(e, "Error", this);
            }
        }
        public void SetStatus(string stat)
        {
            Dispatcher.BeginInvoke((Action)(() =>
            {
                _txtStatus.Text = stat;
            }));
        }


        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);

            Application.Current.Shutdown();
        }

        #endregion

        #region Private: File MEthods

        private void FileOpen()
        {
            try
            {
                bool keepgoing = CheckDiscardChanges();
                if (keepgoing)
                {
                    string[] files = Globals.GetValidOpenSaveUserFile(false, Globals.ProjectFilter, "",
                        Globals.ProjectExt, Globals.GetDocumentsFolderPath(), false, ProjectFile != null ? ProjectFile.ProjectName : Globals.GetDocumentsFolderPath());

                    if (files.Length == 1)
                    {
                        LoadProjectFile(files[0]);
                    }
                }

            }
            catch (Exception ex)
            {
                LogError(ex.ToString());
            }
        }
        private void SetAutoLoadFile(string l)
        {
            //This is for automatically loading a file... for preferences window... 
            string full = Globals.NormalizePath(l);
            WindowManager.PreferencesWindow.AutoLoadFilePath = full;
            WindowManager.PreferencesWindow.SavePrefs();
        }
        public void Save(object sender, RoutedEventArgs e)
        {
            FileSaveProject();
        }
        private void SaveAs(object sender, RoutedEventArgs e)
        {
            PromptSaveAs();
        }
        private void FileSaveProject()
        {
            //User file -> save - save to last path
            if (string.IsNullOrEmpty(ProjectFile.LoadedOrSavedProjectFileName) == false)
            {
                //if (File.Exists(ProjectFile.ProjectFileName))
                {
                    ProjectFile.Save(ProjectFile.LoadedOrSavedProjectFileName, true);
                    SaveRecentFile(ProjectFile.LoadedOrSavedProjectFileName);

                    SetAutoLoadFile(ProjectFile.LoadedOrSavedProjectFileName);
                }
            }
            else
            {
                //If no last path set save as
                PromptSaveAs();
            }
        }
        private void PromptSaveAs()
        {
            if (ProjectFile == null)
            {
                LogError("Project file wasn't set.");
            }
            else
            {
                string[] files = Globals.GetValidOpenSaveUserFile(true, Globals.ProjectFilter, "",
                    Globals.ProjectExt, Globals.GetDocumentsFolderPath(), false, ProjectFile.ProjectName);

                if (files.Length == 1)
                {
                    ProjectFile.Save(files[0], true);
                    SaveRecentFile(files[0]);
                    SetAutoLoadFile(files[0]);

                    AfterProjectFileLoaded(true);
                }
            }
        }

        private void CloseCurrentProject(object sender, RoutedEventArgs e)
        {
            UnloadAndRefresh();
        }
        private void LoadProjectFile(string fn)
        {
            try
            {
                ProjectFile = new ProjectFile();
                ProjectFile.Load(fn);
                SaveRecentFile(fn);
                SetAutoLoadFile(fn);

                AfterProjectFileLoaded(true);
            }
            catch (Exception ex)
            {
                LogError(ex.ToString());
            }
        }
        private void AfterProjectFileLoaded(bool loaded)
        {
            //Enable UI when we have a project loaded.
            _mnuOpenInExplorer.IsEnabled =
            _mnuSave.IsEnabled =
            _mnuImages.IsEnabled =
            _mnuSprites.IsEnabled =
            _mnuMaps.IsEnabled =
            _mnuSaveAs.IsEnabled = loaded;

            Button b = this.FindChild<Button>("_btnOpenInExplorer");
            if (b != null)
            {
                b.Visibility = loaded ? Visibility.Visible : Visibility.Hidden; 
            }

            if (loaded && ProjectFile != null)
            {
                Title = Globals.ApplicationTitle + " - " + ProjectFile.ProjectName;
                //Set CWD
                System.IO.Directory.SetCurrentDirectory(ProjectFile.GetProjectRoot());
            }
            else
            {
                Title = Globals.ApplicationTitle;
            }

            WindowManager.RefreshAllWindows();

        }

        public void MarkChanged(bool changed)
        {
            if (this.ProjectFile != null)
            {
                this.Title = Globals.ApplicationTitle + " - " + ProjectFile.ProjectName + (changed ? "*" : "");
            }
        }

        public void SaveRecentFile(string path)
        {
            try
            {
                WindowManager.PreferencesWindow.LoadPrefs();
                string full = Globals.NormalizePath(path);
                if (!WindowManager.PreferencesWindow.RecentFiles.Contains(full))
                {
                    WindowManager.PreferencesWindow.RecentFiles.Add(System.IO.Path.GetFullPath(path).Trim());

                    if (WindowManager.PreferencesWindow.RecentFiles.Count > 10)
                    {
                        WindowManager.PreferencesWindow.RecentFiles.RemoveAt(0);
                    }
                }
                LoadRecentFiles();
            }
            catch (Exception ex)
            {
                //log silently
                LogError(ex.ToString(), false);
            }
        }
        public void LoadRecentFiles()
        {
            try
            {
                WindowManager.PreferencesWindow.LoadPrefs();

                _mnuRecentFiles.Items.Clear();
                foreach (string mnuitem in WindowManager.PreferencesWindow.RecentFiles)
                {
                    MenuItem item = new MenuItem();
                    item.Header = Globals.ShortenFilePath(mnuitem, 16);
                    item.Tag = mnuitem;
                    item.Click += (x, y) =>
                    {
                        LoadProjectFile(mnuitem);
                    };
                    _mnuRecentFiles.Items.Add(item);
                }
            }
            catch (Exception ex)
            {
                //log silently
                LogError(ex.ToString(), false);
            }
        }

        private void EditShowPrefs()
        {
            WindowManager.PreferencesWindow.ShowDialog(() =>
            {
                if (WindowManager.PreferencesWindow.EditStatus == EditStatus.Modified)
                {
                    Globals.MainWindow.ProjectFile?.MarkChanged();
                }
            });
        }

        private void FileCreateNew()
        {
            try
            {
                if (CheckDiscardChanges())
                {
                    NewProjectDialog d = new NewProjectDialog("New Project");
                    d.ShowDialog(() =>
                    {
                        if (d.ProjectFile != null)
                        {
                            ProjectFile = d.ProjectFile;
                            ProjectFile.MarkChanged();

                            //Run initial save so we have it.
                            FileSaveProject();

                            AfterProjectFileLoaded(true);
                        }

                    });


                }
            }
            catch (Exception ex)
            {
                LogError(ex.ToString(), true);
            }
        }

        private void UnloadAndRefresh()
        {
            ProjectFile = null;
            GC.Collect();
            //Disable UI until user loads something or makes a new
            AfterProjectFileLoaded(false);

            //Load Prefs, autoload file.
            WindowManager.ClearPreferences();
            if (WindowManager.PreferencesWindow.AutoLoadFileEnabled)
            {
                if (System.IO.File.Exists(WindowManager.PreferencesWindow.AutoLoadFilePath))
                {
                    LoadProjectFile(WindowManager.PreferencesWindow.AutoLoadFilePath);
                }
                else
                {
                    WindowManager.PreferencesWindow.AutoLoadFilePath = "";
                    WindowManager.PreferencesWindow.SavePrefs();
                }
            }

            //Add blank recent file if none exists
            MenuItem bla = new MenuItem();
            bla.Header = "";
            _mnuRecentFiles.Items.Add(bla);

            LoadRecentFiles();

            RefreshUI();

            GC.Collect();
        }
        private void RefreshUI()
        {
            //If bound, I wonder if this is needed?
            if (ProjectFile != null)
            {
                //lvImageResources.ItemsSource = ProjectFile.Resources;
            }

        }
        private bool CheckDiscardChanges()
        {
            bool keepgoing = true;
            if (ProjectFile != null && ProjectFile.DataChanged)
            {
                MessageBoxResult r = CustomMessageBox.Show("The current file has unsaved changes.  These will be lost.  Continue?",
                    "Warning", MessageBoxButton.YesNo, MessageBoxImage.Exclamation);
                keepgoing = (r == MessageBoxResult.Yes);
            }
            return keepgoing;
        }

        #endregion

        private void _obj_PreviewQueryContinueDrag(object sender, QueryContinueDragEventArgs e)
        {
        }

        private void _btnOpenInExplorer_Click(object sender, RoutedEventArgs e)
        {
            OpenProjectExplorer();
        }

        private void OpenProjectExplorer()
        {
            try
            {
                if (ProjectFile != null)
                {
                    string dir = System.IO.Path.GetDirectoryName(ProjectFile.LoadedOrSavedProjectFileName);
                    if (Directory.Exists(dir))
                    {
                        Process.Start(dir);
                    }
                }
            }
            catch (Exception ex)
            {
                LogError(ex.ToString(), false);
            }
        }

    }
}
