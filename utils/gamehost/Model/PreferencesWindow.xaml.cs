using MahApps.Metro.Controls;
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
    /// Interaction logic for PreferencesWindow.xaml
    /// 
    /// 
    /// Prefs window stores preferences in a file with the version name.
    /// </summary>
    public partial class PreferencesWindow : ToolWindowBase
    {
        public List<string> RecentFiles { get; set; } = new List<string>();

        public bool AutoLoadFileEnabled { get { return _chkLoadLastFile.IsChecked != null && _chkLoadLastFile.IsChecked == true; } private set { } }
        public string AutoLoadFilePath { get { return (string)_lblAutoLoadFilePath.Content; } set { _lblAutoLoadFilePath.Content = value; } }

        public bool MustBackupFiles { get { return _chkAutoSaveBackups.IsChecked == true; } }
        public string BackupLocation { get { return _objFilePicker.SelectedPath; } }
        public int MaxBackupCount { get { return (int)_nudBackupCount.Value; } }
        public DateTime LastBackupTime { get; set; }
        public TimeSpan BackupTimeMinutes { get { return new TimeSpan(0, (int)_nudBackupTimeMinutes.Value, 0); } }

        const char PrefsDelim = ';';
        const char FilesDelim = '|';

        public PreferencesWindow(string title)
        {
            InitializeComponent();
            Title = title;

            try
            {
                string backuppath = System.IO.Path.Combine(
                    System.IO.Path.GetDirectoryName(Globals.GetLocalSettingsFilePath()), "Backup");

                _objFilePicker.SelectedPath = _objFilePicker.DisplayPath = backuppath;

                if (!System.IO.Directory.Exists(backuppath))
                {
                    System.IO.Directory.CreateDirectory(backuppath);
                }
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError(ex.ToString(), false);
            }

            LoadPrefs();
        }

        public void ClearRecentFiles()
        {
            RecentFiles.Clear();
            SavePrefs();
        }

        public void SavePrefs()
        {
            List<string> values = new List<string>();
            values.Add(Globals.ProgramVersion);
            values.Add(RecentFiles.Count > 0 ? RecentFiles.Aggregate((x, y) => x + FilesDelim + y) : "");
            values.Add(_chkLoadLastFile.IsChecked.ToString());
            values.Add(AutoLoadFilePath);

            values.Add(_chkAutoSaveBackups.IsChecked.ToString());
            values.Add(_objFilePicker.DisplayPath.ToString());
            values.Add(_nudBackupCount.Value.ToString());

            //7
            string data = values.Aggregate((x, y) => x + PrefsDelim + y);

            Globals.SafeWriteFile(Globals.GetLocalSettingsFilePath(), data);
        }

        void SetControlValue(object o, List<string> vals, int index)
        {
        }

        public void LoadPrefs()
        {
            string strValues = Globals.SafeReadFile(Globals.GetLocalSettingsFilePath());

            List<string> values = strValues.Split(PrefsDelim).ToList();

            //Bool to load last file
            if (values.Count != 7)
            {
                Globals.MainWindow.LogError("Could not load settings, invalid number of values", false);
            }
            else
            {
                int iVal = 0;

                //Check version
                string version = values[iVal++];
                if (version != Globals.ProgramVersion)
                {
                    Globals.MainWindow.LogError("Could not load settings, invalid program version number wanted "
                        + Globals.ProgramVersion + " got " + version, false);
                }
                else
                {
                    //Files
                    RecentFiles = new List<string>();
                    List<string> files = values[iVal++].Split(FilesDelim).ToList();
                    foreach (string f in files)
                    {
                        string f2 = Globals.NormalizePath(f);

                        if (string.IsNullOrEmpty(f2) == false)
                        {
                            RecentFiles.Add(f2);
                        }
                    }

                    //Checkbox to load last file
                    bool val = false;
                    Boolean.TryParse(values[iVal++], out val);
                    _chkLoadLastFile.IsChecked = val;

                    //Last Load Path
                    AutoLoadFilePath = values[iVal++];

                    bool b;
                    if (Boolean.TryParse(values[iVal++], out b))
                    {
                        _chkAutoSaveBackups.IsChecked = b;
                    }

                    _objFilePicker.DisplayPath = _objFilePicker.SelectedPath = values[iVal++];

                    double i;
                    if (Double.TryParse(values[iVal++], out i))
                    {
                        _nudBackupCount.Value = i;
                    }
                }
            }
        }


        private void _btnClearRecentFiles_Click(object sender, RoutedEventArgs e)
        {
            ClearRecentFiles();
        }

        private void _btnOK_Click(object sender, RoutedEventArgs e)
        {
            Hide();
        }

        private void _chkLoadLastFile_Checked(object sender, RoutedEventArgs e)
        {
            SavePrefs();
        }

        private void _nudBackupCount_ValueChange(object sender, EventArgs e)
        {
            int n = 0;
            n++;
        }

        private void _chkAutoSaveBackups_Checked(object sender, RoutedEventArgs e)
        {
            _nudBackupCount.IsEnabled =
            _objFilePicker.IsEnabled =
            _lblBackupPath.IsEnabled =
            _lblBackupCount.IsEnabled =
            _nudBackupTimeMinutes.IsEnabled =
            _lblbackupminutes1.IsEnabled =
            _lblbackupminutes2.IsEnabled =
            (_chkAutoSaveBackups.IsChecked == true);
        }
    }
}
