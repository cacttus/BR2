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
using MahApps.Metro.Controls;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for ImagePreview.xaml
    /// </summary>
    public partial class ImagePreview : ContentControl
    {
        private MapEditor _myImageViewer = null;

        private ImageResource _objImageResource = null;
        public ImageResource ImageResource
        {
            get
            {
                return _objImageResource;
            }
            set
            {
                _objImageResource = value;
                Refresh();
            }
        }

        public ImagePreview()
        {
            InitializeComponent();

            Unloaded += (x, y) =>
            {
                _myImageViewer?.Close();
            };

            Refresh();
        }
        public void Refresh()
        {
            try
            {
                if (ImageResource != null)
                {
                    if (String.IsNullOrEmpty(ImageResource.Path) == false)
                    {
                        ImageResource.LoadIfNecessary(true);//force
                        _imgImagePreview.Source = ImageResource.BitmapImage;
                    }
                }
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError(ex.ToString(), false);
            }
        }
        private void _btnImagePreview_Click(object sender, RoutedEventArgs e)
        {
            if (ImageResource != null)
            {
                _myImageViewer = new MapEditor("Image Viewer", null, ImageResource, MapEditorMode.ImageViewer);
                _myImageViewer.ShowDialog(() => { });
                _myImageViewer = null;
            }

        }

    }
}
