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
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GameHost
{
    public enum DockLocation
    {
        Top, Left, Right, Bottom, Center
    }
    public class DropZone
    {
        public bool CanDock = true;
        public DockLocation DockLocation;
        public double DefaultWH { get; set; } = -1; //In percentage
        public DropZone(DockLocation type, double wh, bool canDock)
        {
            CanDock = canDock;
            DockLocation = type;
            DefaultWH = wh;
        }
    }
    /// <summary>
    /// Interaction logic for ExpandoPane.xaml
    /// Basically the logic of this is we have a tab pane and we split it in 2 parts (h/v) then we add a new pane
    /// into both parts and continue this forever
    /// </summary>
    public partial class ExpandoTabPane : UserControl
    {
        public List<DropZone> AvailableDockLocations { get; set; } = new List<DropZone>();
        public DockLocation? PendingDock { get; set; } = null;
        private bool _bIsParentDock = false;
        public bool IsParentDock
        {
            //True if this pane is split and serves as a parent.
            get
            {
                return _bIsParentDock;
            }
            private set
            {
                _bIsParentDock = value;

                if (ParentPane == null)
                {
                    //If we are root, then don't show the popout button
                    _btnPopOut.Visibility = Visibility.Collapsed;
                }
                else
                {
                    _btnPopOut.Visibility = _bIsParentDock ? Visibility.Collapsed : Visibility.Visible;
                }
            }
        }
        public ExpandoTabPane ParentPane { get; set; } = null;
        public DockLocation? ParentDockMode { get; private set; } = null;//Curent dock mode of us(as parent) after we've set children panes.

        Rectangle indicator = new Rectangle();
        public ExpandoTabPane ChildTL { get; private set; } = null;
        public ExpandoTabPane ChildRB { get; private set; } = null;
        private const double GridSplitterWH = 2;

        public ExpandoTabPane()
        {
            InitializeComponent();
            double pctWH = 0.5;  //0.5 for all windows that are not main window.

            //For every thing excep tmainwindow, 0.5
            AvailableDockLocations = new List<DropZone>(){
               new DropZone(DockLocation.Left, pctWH, true)
                ,new DropZone(DockLocation.Top, pctWH, true)
               ,new DropZone(DockLocation.Right, pctWH, true)
               ,new DropZone(DockLocation.Bottom, pctWH, true)
               ,new DropZone(DockLocation.Center, 0, false)
            };
        }
        private TabItem CreateTab(ToolWindowBase b, TabControl tcParent)
        {
            TabItem ti = new TabItem();
            if (string.IsNullOrEmpty(b.Title))
            {
                //This hides the tab if we have no header (e.g. on the tool window)
                //https://stackoverflow.com/questions/1465443/hide-the-tabcontrol-header
                ti.Visibility = Visibility.Collapsed;

                //However we need to add space for the popout button up top, so make a margin with a top margin fo the button width
                tcParent.Margin = new Thickness(0, _btnPopOut.Height + 4, 0, 0);
            }
            else
            {
                ti.Header = b.Title;
            }
            ti.Content = b;
            return ti;
        }
        public void DropToolWindow(FloatingWindowHost host)
        {
            if (IsParentDock)
            {
                ChildTL.DropToolWindow(host);
                ChildRB.DropToolWindow(host);
            }
            else if (PendingDock != null)
            {
                //Close host window and unset content (must come first)
                ToolWindowBase uc = host.InnerControl;
                host.Content = null;
                host.InnerControl = null;
                Globals.MainWindow.WindowManager.CloseToolWindow(host);

                //We hovered and st to drop window in this guy
                if (PendingDock == DockLocation.Center)
                {
                    //Add a new tab to the current tab control
                    _objTabControl.Items.Add(CreateTab(uc, _objTabControl));
                }
                else
                {
                    //Remove contents and create 2 new expando panels
                    _objMainGrid.Children.Remove(indicator);
                    _objMainGrid.Children.Remove(_objTabContainer);

                    //Pane 1 = current content
                    ExpandoTabPane pane1 = new ExpandoTabPane();
                    pane1._objTabContainer = _objTabContainer;
                    pane1.ParentPane = this;
                    //Pane 2  = new split contet
                    ExpandoTabPane pane2 = new ExpandoTabPane();
                    pane2._objTabControl.Items.Add(CreateTab(uc, pane2._objTabControl));
                    pane2.ParentPane = this;

                    GridSplitter gp = new GridSplitter();
                    gp.VerticalAlignment = VerticalAlignment.Stretch;
                    gp.HorizontalAlignment = HorizontalAlignment.Stretch;
                    gp.ResizeBehavior = GridResizeBehavior.PreviousAndNext;
                    gp.Background = new SolidColorBrush(Colors.Gainsboro);
                    gp.ShowsPreview = true;
                    // gp.SetResourceReference(FrameworkElement.StyleProperty, "HorizontalGridSplitterStyle");

                    if (PendingDock == DockLocation.Right)
                    {
                        Grid.SetColumn(pane1, 0);
                        Grid.SetColumn(gp, 1);
                        Grid.SetColumn(pane2, 2);
                        gp.Width = GridSplitterWH;
                    }
                    else if (PendingDock == DockLocation.Left)
                    {
                        Grid.SetColumn(pane1, 2);
                        Grid.SetColumn(gp, 1);
                        Grid.SetColumn(pane2, 0);
                        gp.Width = GridSplitterWH;
                    }
                    else if (PendingDock == DockLocation.Top)
                    {
                        Grid.SetRow(pane1, 2);
                        Grid.SetRow(gp, 1);
                        Grid.SetRow(pane2, 0);
                        gp.Height = GridSplitterWH;
                    }
                    else if (PendingDock == DockLocation.Bottom)
                    {
                        Grid.SetRow(pane1, 0);
                        Grid.SetRow(gp, 1);
                        Grid.SetRow(pane2, 2);
                        gp.Height = GridSplitterWH;
                    }

                    _objMainGrid.Children.Add(pane1);
                    _objMainGrid.Children.Add(pane2);
                    _objMainGrid.Children.Add(gp);

                    ChildTL = pane1;
                    ChildRB = pane2;

                    IsParentDock = true;

                }

                ParentDockMode = PendingDock;
                PendingDock = null;
            }
        }
        public bool InBounds(Point p)
        {
            return (p.X >= 0 && p.Y >= 7 && p.X <= ActualWidth && p.Y <= ActualHeight);
        }
        public void UpdateDropZoneHover(FloatingWindowHost host)
        {
            if (IsParentDock)
            {
                if (ChildTL != null && ChildRB != null)
                {
                    ChildTL.UpdateDropZoneHover(host);
                    ChildRB.UpdateDropZoneHover(host);
                }
                else
                {
                    throw new Exception("Data error in dock - if we are parent we need to have child docks.");
                }
            }
            else
            {
                ToolWindowBase tb = host.Content as ToolWindowBase;
                if (tb != null)
                {

                    Point p = Mouse.GetPosition(_objMainGrid);

                    double xmargin = _objMainGrid.ActualWidth * 0.1;
                    double ymargin = _objMainGrid.ActualHeight * 0.1;

                    if (xmargin > 50) { xmargin = 50; }
                    if (ymargin > 50) { ymargin = 50; }

                    if (xmargin < 10) { xmargin = 10; }
                    if (ymargin < 10) { ymargin = 10; }

                    if (p.X > ActualWidth - xmargin && p.X < ActualWidth && InBounds(p))
                    {
                        AnimateDropZone(DockLocation.Right, tb);
                    }
                    else if (p.Y > ActualHeight - ymargin && p.Y < ActualHeight && InBounds(p))
                    {
                        AnimateDropZone(DockLocation.Bottom, tb);
                    }
                    else if (p.X <= xmargin && p.X >= 0 && InBounds(p))
                    {
                        AnimateDropZone(DockLocation.Left, tb);
                    }
                    else if (p.Y <= ymargin && p.Y >= 0 && InBounds(p))
                    {
                        AnimateDropZone(DockLocation.Top, tb);
                    }
                    else if (p.X > xmargin && p.Y > ymargin && p.X < ActualWidth - xmargin && p.Y < ActualHeight - ymargin && InBounds(p))
                    {
                        AnimateDropZone(DockLocation.Center, tb);
                    }
                    else
                    {
                        AnimateDropZone(null, tb);
                    }
                }

            }
        }
        private void CollapseGrid()
        {
            //Removes the child grid and replaces it with the basic tab container.
            _objMainGrid.Children.Clear();
            _objMainGrid.Children.Add(_objTabContainer);//Remove and add back to make sure we added only once
            _objMainGrid.ColumnDefinitions.Clear();
            _objMainGrid.RowDefinitions.Clear();
            Grid.SetColumn(_objTabContainer, 0);
            Grid.SetRow(_objTabContainer, 0);
            PendingDock = null;
        }
        private void AnimateDropZone(DockLocation? location, ToolWindowBase tb)
        {
            if (PendingDock != null)
            {
                if (location == PendingDock)
                {
                    //Do nothing - we're already animating
                }
                else
                {
                    //Location is null - we cancleled dock, or we are animating a different zone - reset everything
                    CollapseGrid();
                }
            }

            if (PendingDock == null)
            {
                DropZone zone = AvailableDockLocations.Where(x => x.DockLocation == location).FirstOrDefault();
                if (zone != null)
                {
                    PendingDock = location;
                    indicator.Fill = new SolidColorBrush(Color.FromScRgb(0.5f, 0, 0, 1));
                    indicator.HorizontalAlignment = HorizontalAlignment.Stretch;
                    indicator.VerticalAlignment = VerticalAlignment.Stretch;

                    //So we add a column or row basd on the dock location
                    if (location == DockLocation.Right || location == DockLocation.Left)
                    {
                        ColumnDefinition gridCol1 = new ColumnDefinition();
                        ColumnDefinition gridCol2 = new ColumnDefinition();//Col2 is for the gridsplitter
                        ColumnDefinition gridCol3 = new ColumnDefinition();
                        gridCol2.Width = new GridLength(GridSplitterWH);

                        if (location == DockLocation.Left)
                        {
                            gridCol1.Width = new GridLength(tb.ActualWidth);
                            Grid.SetColumn(indicator, 0);
                            gridCol3.Width = new GridLength(1, GridUnitType.Star);
                            Grid.SetColumn(_objTabContainer, 2);
                            Globals.UpdateLayout(_objMainGrid);
                            //gridCol1.Width = new GridLength(0);
                            //Globals.UpdateLayout(_objMainGrid);

                            //TODO: *test *
                          //  NameScope.SetNameScope(this, new NameScope());
                           // gridCol3.Name = "_AnimatingGridCol";
                            //RegisterName(gridCol3.Name, gridCol3);


                            storyBoard = new Storyboard();

                            //DoubleAnimation da = new DoubleAnimation();
                            //da.From = 0;
                            //da.To = tb.ActualWidth;
                            //da.Completed += (x, y) => {
                            //    int n = 0;
                            //    n++;
                            //};
                            //da.Duration = new Duration(new TimeSpan(0, 0, 0, 0, 500));
                            //storyBoard.Children.Add(da);
                            //Storyboard.SetTarget(da, gridCol1);
                            //Storyboard.SetTargetProperty(da, new PropertyPath(WidthProperty));
                            //storyBoard.Children.Add(da);
                            //storyBoard.Begin();
                        }
                        else if (location == DockLocation.Right)
                        {
                            gridCol1.Width = new GridLength(1, GridUnitType.Star);
                            Grid.SetColumn(_objTabContainer, 0);
                            gridCol3.Width = new GridLength(tb.ActualWidth);
                            Grid.SetColumn(indicator, 2);
                        }
                        _objMainGrid.ColumnDefinitions.Add(gridCol1);
                        _objMainGrid.ColumnDefinitions.Add(gridCol2);
                        _objMainGrid.ColumnDefinitions.Add(gridCol3);

                    }
                    if (location == DockLocation.Top || location == DockLocation.Bottom)
                    {
                        RowDefinition r1 = new RowDefinition();
                        RowDefinition r2 = new RowDefinition();
                        RowDefinition r3 = new RowDefinition();

                        r2.Height = new GridLength(GridSplitterWH);

                        if (location == DockLocation.Top)
                        {
                            r1.Height = new GridLength(tb.ActualHeight);
                            Grid.SetRow(indicator, 0);
                            r3.Height = new GridLength(1, GridUnitType.Star);
                            Grid.SetRow(_objTabContainer, 2);
                        }
                        else if (location == DockLocation.Bottom)
                        {
                            r1.Height = new GridLength(1, GridUnitType.Star);
                            Grid.SetRow(_objTabContainer, 0);
                            r3.Height = new GridLength(tb.ActualHeight);
                            Grid.SetRow(indicator, 2);
                        }
                        _objMainGrid.RowDefinitions.Add(r1);
                        _objMainGrid.RowDefinitions.Add(r2);
                        _objMainGrid.RowDefinitions.Add(r3);

                    }
                    _objMainGrid.Children.Add(indicator);
                }
            }

        }
        Storyboard storyBoard = null;
        public void CollapseChildPane(ExpandoTabPane p)
        {
            //Pass TabContainer up to the parent, and remove all the grid nonsense
            if (p == ChildTL)
            {
                _objTabContainer = ChildRB._objTabContainer;

                CollapseGrid();
            }
            else if (p == ChildRB)
            {
                _objTabContainer = ChildTL._objTabContainer;

                CollapseGrid();
            }
            else
            {
                Globals.MainWindow.LogError("Could not match the given child expando pane to one owened by the parent.");
            }
            IsParentDock = false;
            ParentDockMode = null;
            PendingDock = null;
        }

        private void _btnPopOut_Click(object sender, RoutedEventArgs e)
        {
            //Popout button - delete tool window tab and show it in a popup.
            TabItem ti = _objTabControl.SelectedItem as TabItem;
            if (ti != null)
            {
                ToolWindowBase tw = ti.Content as ToolWindowBase;
                if (tw != null)
                {
                    _objTabControl.Items.Remove(ti);
                    Globals.MainWindow.WindowManager.CreatePopoutWindow(tw);

                    if (_objTabControl.Items.Count == 0)
                    {
                        if (ParentPane != null)
                        {
                            ParentPane.CollapseChildPane(this);
                        }
                    }
                }
            }
        }

        private void _btnClose_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
////Animate the col
///                        //gridCol2.Name = "GridCol2Name";
//NameScope.SetNameScope(this, new NameScope());
//RegisterName(gridCol2.Name, gridCol2);
//GridLengthAnimation anim = new GridLengthAnimation();
//anim.From = new GridLength(0, GridUnitType.Pixel);
//anim.To = new GridLength(targetWidth, GridUnitType.Pixel);
//anim.Duration = new Duration(new TimeSpan(0, 0, 0, 0, 500));
//anim.ReverseValue = 5;
//DoubleAnimation da = new DoubleAnimation();
//da.From = 0;
//da.To = targetWidth;
//da.Duration = new Duration(new TimeSpan(0, 0, 0, 2, 500));
//Storyboard.SetTargetName(da, gridCol2.Name);
//Storyboard.SetTargetProperty(da, new PropertyPath(WidthProperty));

///TODO:
///In the child window we create a draggable thumb bar on the edge that
////we added it/

