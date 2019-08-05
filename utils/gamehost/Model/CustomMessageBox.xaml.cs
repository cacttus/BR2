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
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for CustomMessageBox.xaml
    /// </summary>
    public partial class CustomMessageBox : Window
    {
        List<string> CustomButtonOptions = null;
        public CustomMessageBox()
        {
            InitializeComponent();
        }

        MessageBoxButton Buttons;
        MessageBoxResult result = MessageBoxResult.OK;

        private void SetIcon(System.Drawing.Icon icon)
        {
            Icon = Imaging.CreateBitmapSourceFromHIcon(icon.Handle, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());

            _imgIcon.Source = Icon;
        }

        public CustomMessageBox(string contents, string caption, MessageBoxButton button, MessageBoxImage img, List<string> customButtonOptions = null)
        {
            CustomButtonOptions = customButtonOptions;
            InitializeComponent();

            Title = caption;
            _txtMessage.Text = contents;
            Buttons = button;

            if (button == MessageBoxButton.OK)
            {
                _btnOkYes.Content = "OK";
                _btnOkYes.Visibility = Visibility.Visible;
                _btnCancelNo.Visibility = Visibility.Hidden;
                _btnCancel.Visibility = Visibility.Hidden;
                _grdButtonGrid.Columns = 1;
                _grdButtonGrid.Margin = new Thickness(80, 30, 80, 0);
            }
            else if (button == MessageBoxButton.OKCancel)
            {
                _btnOkYes.Content = "OK";
                _btnOkYes.Visibility = Visibility.Visible;
                _btnCancelNo.Content = "Cancel";
                _btnCancelNo.Visibility = Visibility.Visible;
                _btnCancel.Visibility = Visibility.Hidden;
                _grdButtonGrid.Columns = 2;
                _grdButtonGrid.Margin = new Thickness(60, 30, 60, 0);
            }
            else if (button == MessageBoxButton.YesNo)
            {
                _btnOkYes.Content = "Yes";
                _btnOkYes.Visibility = Visibility.Visible;
                _btnCancelNo.Content = "No";
                _btnCancelNo.Visibility = Visibility.Visible;
                _btnCancel.Visibility = Visibility.Hidden;
                _grdButtonGrid.Columns = 2;
                _grdButtonGrid.Margin = new Thickness(60, 30, 60, 0);
            }
            else if (button == MessageBoxButton.YesNoCancel)
            {
                _btnOkYes.Content = "Yes";
                _btnOkYes.Visibility = Visibility.Visible;
                _btnCancelNo.Content = "No";
                _btnCancelNo.Visibility = Visibility.Visible;
                _btnCancel.Content = "Cancel";
                _btnCancel.Visibility = Visibility.Visible;
                _grdButtonGrid.Columns = 3;
                _grdButtonGrid.Margin = new Thickness(30, 30, 30, 0);
            }

            //Change the button text to use pref.
            if (customButtonOptions != null)
            {
                if (button == MessageBoxButton.OK && customButtonOptions.Count == 1)
                {
                    _btnOkYes.Content = SetTb(customButtonOptions[0]);
                }
                if ((button == MessageBoxButton.YesNo || button == MessageBoxButton.OKCancel) && customButtonOptions.Count == 2)
                {
                    _btnOkYes.Content = SetTb(customButtonOptions[0]);
                    _btnCancelNo.Content = SetTb(customButtonOptions[1]);
                }
                if (button == MessageBoxButton.YesNoCancel && customButtonOptions.Count == 3)
                {
                    _btnOkYes.Content = SetTb(customButtonOptions[0]);
                    _btnCancelNo.Content = SetTb(customButtonOptions[1]);
                    _btnCancel.Content = SetTb(customButtonOptions[2]);
                }
            }

            if (img == MessageBoxImage.Asterisk) { SetIcon(SystemIcons.Asterisk); }
            if (img == MessageBoxImage.Error) { SetIcon(SystemIcons.Error); }
            if (img == MessageBoxImage.Exclamation) { SetIcon(SystemIcons.Exclamation); }
            if (img == MessageBoxImage.Hand) { SetIcon(SystemIcons.Hand); }
            if (img == MessageBoxImage.Information) { SetIcon(SystemIcons.Information); }
            if (img == MessageBoxImage.None) { }
            if (img == MessageBoxImage.Question) { SetIcon(SystemIcons.Question); }
            //if(img == MessageBoxImage.Stop) { SetIcon(SystemIcons.Stop); }
            if (img == MessageBoxImage.Warning) { SetIcon(SystemIcons.Warning); }


            MessageBoxResult r = MessageBoxResult.Cancel;
            if (button == MessageBoxButton.OK)
            {
                _btnCancel.Visibility = Visibility.Hidden;
            }


        }
        private TextBlock SetTb(string msg)
        {
            TextBlock b = new TextBlock();
            b.Text = msg;
            b.TextWrapping = TextWrapping.Wrap;
            b.Margin = new Thickness(5, 5, 5, 5);

            _grdButtonGrid.Columns = 1;
            _grdButtonGrid.Rows = 3;
            return b;
        }
        public static MessageBoxResult Show(string contents, string caption, MessageBoxButton button, MessageBoxImage img,
            List<string> customButtonOptions = null)
        {
            CustomMessageBox b = new CustomMessageBox(contents, caption, button, img, customButtonOptions);

            b.ShowDialog();

            return b.result;
        }

        private void _btnCancelNo_Click(object sender, RoutedEventArgs e)
        {
            if (Buttons == MessageBoxButton.OK || Buttons == MessageBoxButton.OKCancel)
            {
                result = MessageBoxResult.OK;
            }
            else if (Buttons == MessageBoxButton.YesNo || Buttons == MessageBoxButton.YesNoCancel)
            {
                result = MessageBoxResult.No;
            }

            Hide();
        }

        private void _btnOkYes_Click(object sender, RoutedEventArgs e)
        {
            if (Buttons == MessageBoxButton.OKCancel)
            {
                result = MessageBoxResult.OK;
            }
            else if (Buttons == MessageBoxButton.YesNo || Buttons == MessageBoxButton.YesNoCancel)
            {
                result = MessageBoxResult.Yes;
            }
            Hide();
        }

        private void _btnCancel_Click(object sender, RoutedEventArgs e)
        {
            if (Buttons == MessageBoxButton.YesNoCancel)
            {
                result = MessageBoxResult.Cancel;
            }
            Hide();
        }
    }
}
