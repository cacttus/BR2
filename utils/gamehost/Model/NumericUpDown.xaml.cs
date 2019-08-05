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
    /// <summary>
    /// Interaction logic for NumericUpDown.xaml
    /// </summary>
    public partial class NumericUpDown : UserControl
    {
        public static readonly DependencyProperty MaxValueInternal = DependencyProperty.Register(@"MaxValue", typeof(double), typeof(NumericUpDown));
        public static readonly DependencyProperty MinValueInternal = DependencyProperty.Register(@"MinValue", typeof(double), typeof(NumericUpDown));
        public static readonly DependencyProperty IncrementInternal = DependencyProperty.Register(@"Increment", typeof(double), typeof(NumericUpDown));
        public static readonly DependencyProperty PrecisionInternal = DependencyProperty.Register(@"Decimals", typeof(int), typeof(NumericUpDown));
        public static readonly DependencyProperty ValueChangeInternal = DependencyProperty.Register(@"ValueChange", typeof(EventHandler), typeof(NumericUpDown));

        public double MaxValue
        {
            get
            {
                return (double)GetValue(MaxValueInternal);
            }
            set
            {
                SetValue(MaxValueInternal, (value < (double)GetValue(MinValueInternal) ? (double)GetValue(MinValueInternal) : value));
            }
        }
        public double MinValue
        {
            get
            {
                return (double)GetValue(MinValueInternal);
            }
            set
            {
                SetValue(MinValueInternal, (value > (double)GetValue(MaxValueInternal) ? (double)GetValue(MaxValueInternal) : value));
            }
        }
        public double Increment
        {
            get
            {
                return (double)GetValue(IncrementInternal);
            }
            set
            {
                SetValue(IncrementInternal, value);
            }
        }
        public int Decimals
        {
            get
            {
                return (int)GetValue(PrecisionInternal);
            }
            set
            {
                SetValue(PrecisionInternal, value);
            }
        }
        public double Value
        {
            get
            {
                double res;
                if (Double.TryParse(_txtValue.Text, out res))
                {
                    return res;
                }
                else
                {
                    return 0;
                };
            }
            set
            {
                _txtValue.Text = value.ToString();
            }
        }
        private Object _myLock = new Object();
      //  public event EventHandler ValueChange = null;
        public event EventHandler ValueChange
        {
            add
            {
                lock (_myLock)
                {
                    SetValue(ValueChangeInternal, value);
                }
            }
            remove
            {
                lock (_myLock)
                {
                    SetValue(ValueChangeInternal, value);
                }
            }

        }
        public NumericUpDown()
        {
            InitializeComponent();

            MaxValue = 100;
            MinValue = 1;
            Increment = 1;

        }
        private void NumericUpDown_UpButton(object o, EventArgs e)
        {
            Value += Increment;
            FormatText();
            CheckBounds();
             ((EventHandler)GetValue(ValueChangeInternal))?.Invoke(o,e);
        }
        private void NumericUpDown_DownButton(object o, EventArgs e)
        {
            Value -= Increment;
            FormatText();
            CheckBounds();
            ((EventHandler)GetValue(ValueChangeInternal))?.Invoke(o, e);
        }
        void CheckBounds()
        {
            if (Value > MaxValue)
            {
                Value = MaxValue;
            }
            if (Value < MinValue)
            {
                Value = MinValue;
            }
        }
        private void TextBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Regex _regex = new Regex("[^0-9.-]+");
            e.Handled = _regex.IsMatch(e.Text);
        }
        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            FormatText();
        }
        private void FormatText()
        {
            try
            {
                //format desired decimal places
                double d;
                if (double.TryParse(_txtValue.Text, out d))
                {
                    string fmt = String.Format("{0:n" + Decimals + "}", d);
                    _txtValue.Text = fmt;
                }
            }
            catch (Exception ex)
            {

            }
        }
        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            FormatText();
            CheckBounds();
        }
        private void NumericUpDown_ValueChange(object sender, TextChangedEventArgs e)
        {
            FormatText();
            CheckBounds();
            ((EventHandler)GetValue(ValueChangeInternal))?.Invoke(sender, e);
        }
    }
}
