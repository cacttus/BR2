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
    /// Interaction logic for TileEditor.xaml
    /// </summary>
    public partial class TileEditor : ToolWindowBase
    {
        TileMap _objTileGrid; // The tilegrid being edited.

        /// <summary>
        /// sprites separate from objects
        /// ?
        /// </summary>
        public TileEditor()
        {

            InitializeComponent();

            Redraw();
        }
        public void Redraw()
        {
            _objCanvas.Children.Clear();
            int gw = 10, gh = 10;
            for(int y=0; y<gh; ++y)
            {
                Line line = new Line();
                line.Stroke = new SolidColorBrush(Colors.Red);
                line.StrokeThickness = 1;
                line.HorizontalAlignment = HorizontalAlignment.Left;
                line.VerticalAlignment = VerticalAlignment.Center;
                line.Y1 = y * 10;
                line.Y2 = y * 10;

                line.X1 = 0;
                line.X2 = (double)_objCanvas.ActualWidth;
                _objCanvas.Children.Add(line);
            }
            for (int x = 0; x < gw; ++x)
            {
                Line line = new Line();
                line.Stroke = new SolidColorBrush(Colors.Red);
                line.StrokeThickness = 2;
                line.HorizontalAlignment = HorizontalAlignment.Left;
                line.VerticalAlignment = VerticalAlignment.Center;
                line.X1 = x * 10;
                line.X2 = x * 10;

                line.Y1 = 0;
                line.Y2 = (double)_objCanvas.ActualHeight;
                _objCanvas.Children.Add(line);
            }
        }

        private void _objCanvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Point startPoint = e.GetPosition(_objCanvas);
            Rectangle rect = new Rectangle
            {
                Stroke = Brushes.LightBlue,
                Fill = Brushes.Red,
                StrokeThickness = 0,
                Width=200,
                Height=200
            };
            Canvas.SetLeft(rect, startPoint.X);
            Canvas.SetTop(rect, startPoint.Y);
            _objCanvas.Children.Add(rect);

            Redraw();
        }

        private void _cpBackground_SelectedColorChanged(object sender, RoutedPropertyChangedEventArgs<Color?> e)
        {
            _objCanvas.Background = new SolidColorBrush(_cpBackground.SelectedColor.Value);
        }

        private void ToolBar_Loaded(object sender, RoutedEventArgs e)
        {
            Redraw();
        }

        private void _objCanvas_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            Redraw();
        }
    }
}
