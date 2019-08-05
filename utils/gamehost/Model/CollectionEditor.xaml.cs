using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Reflection;
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
    /// Interaction logic for being able to edit collections of crap
    /// </summary>
    public partial class CollectionEditor : ToolWindowBase
    {

        private ObservableCollection<ResourceBase> Items { get; set; }
        public const string DefaultFilter = "filter";

        Border SelectedItem = null;
        Color HighlightColor = Color.FromArgb(190, 17, 17, 212);
        Color BaseColor = Colors.LightGray;

        public bool ShowControls
        {
            set
            {
                _objAddEditControlsPanel.Visibility = value ? Visibility.Visible : Visibility.Hidden;
            }
        }

        public event EventHandler AddItemClicked = null;
        public event EventHandler RemoveItemClicked = null;
        public event EventHandler EditItemClicked = null;

        public Func<List<ResourceBase>> GetObjects { get; set; } = null;

        public ResourceBase GetSelectedItem()
        {
            if (SelectedItem != null)
            {
                try
                {
                    ResourceBase r = SelectedItem.Tag as ResourceBase;
                    return r;
                    //Int64 id = (Int64)SelectedItem.Tag;
                  //  return Globals.MainWindow.ProjectFile?.GetResourceById(id);
                }
                catch(Exception ex)
                {
                    Globals.MainWindow.LogError(ex.ToString());
                }

            }
            return null;
        }

        public CollectionEditor( string title, Func<List<ResourceBase>> getObjs = null)
        {
            GetObjects = getObjs;
            Title = title;
            InitializeComponent();
            RefreshEverything();
        }

        public override bool OnClose()
        {
            Hide();
            return false;
        }
        public void RefreshEverything()
        {
            ClearFilter();

            if (Globals.MainWindow.ProjectFile != null)
            {
                Filter(true);
            }
            else
            {
                DataContext = null;
                Items = null;
            }
        }

        private void TextBox_GotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
        {
            if (_txtFilter.Text.Equals(DefaultFilter))
            {
                _txtFilter.Text = "";
            }
        }

        private void _txtFilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            Filter(false);
        }

        //private BitmapImage TryGetBitmapImageValue(object o)
        //{
        //    Type myType = o.GetType();
        //    PropertyInfo inf = myType.GetProperties().Where(x => x.Name == "BitmapImage").FirstOrDefault();
        //    if (inf != null)
        //    {
        //        BitmapImage propValue = inf.GetValue(o, null) as BitmapImage;

        //        return propValue;
        //    }
        //    return null;
        //}

        //private string TryGetNameValue(object o)
        //{
        //    Type myType = o.GetType();
        //    PropertyInfo inf = myType.GetProperties().Where(x => x.Name == "Name").FirstOrDefault();
        //    if (inf != null)
        //    {
        //        string propValue = inf.GetValue(o, null) as string;

        //        return propValue;
        //    }
        //    return null;
        //}

        private void Filter(bool forceReload)
        {
            Items = new ObservableCollection<ResourceBase>();
            string filter = (_txtFilter.Text == DefaultFilter) ? "" : _txtFilter.Text.ToLower();

            if (Globals.MainWindow.ProjectFile != null)
            {
                List<ResourceBase> items = GetObjects();
                foreach(ResourceBase o in items)
                {
                    if (string.IsNullOrEmpty(filter) || o.Name.ToLower().Contains(filter))
                    {
                        if (forceReload)
                        {
                            o.Refresh();
                        }
                        Items.Add(o);
                    }
                }
            }

            DataContext = Items;
        }
        private void _btnClearFilter_Click(object sender, RoutedEventArgs e)
        {
            ClearFilter();
        }
        void ClearFilter()
        {
            Keyboard.ClearFocus();
            _txtFilter.Text = DefaultFilter;
        }
        private void _txtFilter_LostFocus(object sender, RoutedEventArgs e)
        {
            if (_txtFilter.Text == "")
            {
                _txtFilter.Text = DefaultFilter;
            }
        }
        private void btnAdd_Click(object sender, RoutedEventArgs e)
        {
            AddItemClicked(sender, e);

            RefreshEverything();
        }
        private void _btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            RefreshEverything();
        }
        private void btnRemove_Click(object sender, RoutedEventArgs e)
        {
            RemoveItemClicked(sender, e);

            RefreshEverything();
        }


        long lastClick = 0;

        private void Grid_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Grid fe = sender as Grid;
            if (fe != null)
            {
                //Get border
                Border b = fe.Parent as Border;
                if (b != null)
                {
                    if (SelectedItem != null)
                    {
                        //Check for double-click to edit
                        if (lastClick != 0)
                        {
                            if(Environment.TickCount - lastClick < 500)
                            {
                                EditItemClicked(this, new RoutedEventArgs());
                            }
                        }
                        lastClick = System.Environment.TickCount;


                        SelectedItem.Background = new SolidColorBrush(BaseColor);
                    }
                    SelectedItem = b;
                    b.Background = new SolidColorBrush(HighlightColor);
                }
            }

        }

        private void btnEdit_Click(object sender, RoutedEventArgs e)
        {
            EditItemClicked(sender, e);
        }
    }
}
