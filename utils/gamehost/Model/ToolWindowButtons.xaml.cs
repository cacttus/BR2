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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for ToolWindowButtons.xaml
    /// </summary>
    public partial class ToolWindowButtons : UserControl
    {
        public ToolWindowButtons()
        {
            InitializeComponent();
        }

        public static readonly DependencyProperty _dShowCancelNo = DependencyProperty.Register(@"ShowCancelNo", typeof(bool), typeof(ToolWindowBase));
        public static readonly DependencyProperty _dShowOkYes = DependencyProperty.Register(@"ShowOkYes", typeof(bool), typeof(ToolWindowBase));

        private Button Cancel =null;
        private Button Ok =null;

        private void UpdateColumns()
        {
            if (Cancel == null)
            {
                Cancel = _btnCancel;
            }
            if (Ok == null)
            {
                Ok = _btnOk;
            }
            int n = 0;
            n += Cancel.Visibility == Visibility.Visible ? 1 : 0;
            n += Ok.Visibility == Visibility.Visible ? 1 : 0;
            _mainButtonGrid.Columns = n;
        }

        public bool ShowCancelNo
        {
            get
            {
                return (bool)GetValue(_dShowCancelNo);
            }
            set
            {
                UpdateColumns();

                _btnCancel.Visibility = value ? Visibility.Visible : Visibility.Hidden;

                UpdateColumns();

                SetValue(_dShowCancelNo, value);
            }
        }
        public bool ShowOkYes
        {
            get
            {
                return (bool)GetValue(_dShowOkYes);
            }
            set
            {
                UpdateColumns();
                _btnOk.Visibility = value ? Visibility.Visible : Visibility.Hidden;
                UpdateColumns();

                SetValue(_dShowOkYes, value);
            }
        }
        private ToolWindowBase GetParentWindow()
        {
            object parent = Parent;
            while ((parent != null) && (parent as ToolWindowBase == null))
            {
                if (parent as FrameworkElement != null)
                {
                    parent = (parent as FrameworkElement).Parent;
                }
            }
            if ((parent != null) && (parent as ToolWindowBase != null))
            {
                return parent as ToolWindowBase;
            }
            return null;
        }
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            GetParentWindow()?.CancelButtonClicked();
        }
        private void _btnOk_Click(object sender, RoutedEventArgs e)
        {
            GetParentWindow()?.OkButtonClicked();

        }
    }
}
