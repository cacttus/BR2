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
    public enum MapEditorMode
    {
        ImageViewer,
        TileEditor
    }
    /// <summary>
    /// Interaction logic for Imageviewer.xaml
    /// </summary>
    public partial class MapEditor : ToolWindowBase
    {
        public TileMap TileMap = null;
        public ImageResource ImageResource = null;
        public MapEditorMode MapEditorMode;
        const float MaxScale = 10.0f;
        const float MinScale = 0.1f;
        Point _ptDragStart;
        Point _ptDragOrigin;

        vec2 basePos = new vec2(0, 0);

        public MapEditor(string title, TileMap tm = null, ImageResource ir = null, MapEditorMode mode= MapEditorMode.ImageViewer)
        {
            Title = title;
            InitializeComponent();
            MapEditorMode = mode;

            if(mode == MapEditorMode.ImageViewer)
            {
                ImageResource = ir;
            }
            else if (mode == MapEditorMode.TileEditor)
            {
                if (tm == null)
                {
                    TileMap = new TileMap(Globals.MainWindow.ProjectFile, Globals.MainWindow.ProjectFile.GenId());
                }
                else
                {
                    TileMap = tm;
                }
            }

            TranslateTransform tt = (TranslateTransform)((TransformGroup)_imgRenderImage.RenderTransform).Children.First(tr => tr is TranslateTransform);
            basePos.x = (float)tt.X;
            basePos.y = (float)tt.Y;

            Reset();
        }

        #region UI Cbks
        private void _btnClose_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
        private void _optFilterNearestNeighbor_Checked(object sender, RoutedEventArgs e)
        {
            if (_imgRenderImage != null)
            {
                RenderOptions.SetBitmapScalingMode(_imgRenderImage, BitmapScalingMode.NearestNeighbor);
            }
        }

        private void _optFilterLinear_Checked(object sender, RoutedEventArgs e)
        {
            if (_imgRenderImage != null)
            {
                RenderOptions.SetBitmapScalingMode(_imgRenderImage, BitmapScalingMode.Linear);
            }
        }

        public override bool OnClose()
        {
            Hide();
            return false;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
        }
        #endregion

        #region Zoom
        private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            _tbZoom.Value += e.Delta > 0 ? 10 : -10;
        }

        private void _tbZoom_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Zoom((float)_tbZoom.Value / 100.0f);
        }

        private void Zoom(float zoom)
        {
            try
            {
                if (zoom < 0.0f)
                {
                    zoom = 0.0f;
                }
                if (zoom > 1.0f)
                {
                    zoom = 1.0f;
                }

                ScaleTransform st = (ScaleTransform)((TransformGroup)_imgRenderImage.RenderTransform).Children.First(tr => tr is ScaleTransform); ;

                float minScale = 0.1f;

                st.ScaleX = MinScale + (MaxScale - MinScale) * zoom;
                st.ScaleY = MinScale + (MaxScale - MinScale) * zoom;
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError(ex.ToString(), false);
            }
        }

        #endregion

        #region Pan

        private void _imgRenderImage_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            _imgRenderImage.CaptureMouse();
            TranslateTransform tt = (TranslateTransform)((TransformGroup)_imgRenderImage.RenderTransform).Children.First(tr => tr is TranslateTransform);
            _ptDragStart = e.GetPosition(border);
            _ptDragOrigin = new Point(tt.X, tt.Y);
        }
        private void _imgRenderImage_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _imgRenderImage.ReleaseMouseCapture();
        }
        private void _imgRenderImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (_imgRenderImage.IsMouseCaptured)
            {
                TranslateTransform tt = (TranslateTransform)((TransformGroup)_imgRenderImage.RenderTransform).Children.First(tr => tr is TranslateTransform);
                Vector v = _ptDragStart - e.GetPosition(border);
                tt.X = _ptDragOrigin.X - v.X;
                tt.Y = _ptDragOrigin.Y - v.Y;
            }
        }
        #endregion

        private void Reset()
        {
            try
            {
                UpdateSource();

                _imgRenderImage.RenderTransformOrigin = new Point(0.5f, 0.5f);

                _tbZoom.Value = 1 * 100.0f / MaxScale;

                TranslateTransform tt = (TranslateTransform)((TransformGroup)
                    _imgRenderImage.RenderTransform).Children.First(tr => tr is TranslateTransform);
                tt.X = basePos.x;
                tt.Y = basePos.y;
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError(ex.ToString(), false);
            }
        }

        public void UpdateSource()
        {
            if (ImageResource != null)
            {
                ImageResource.LoadIfNecessary(true);//force
                _imgRenderImage.Source = ImageResource.BitmapImage;
            }
            else
            {
                _imgRenderImage.Source = Globals.Bitmap2BitmapImage(Globals.GetDefaultXImage());
            }
        }

        private void _btnReset_Click(object sender, RoutedEventArgs e)
        {
            Reset();
        }
    }
}


