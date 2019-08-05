using System;
using System.IO;
using System.Windows.Media.Imaging;
using GameHost.Constants;

namespace GameHost
{
    /// <summary>
    /// an image, texture, or sprite atlas
    /// </summary>
    public class ImageResource : ResourceBase
    {
        public string Path { get; set; }
        public ImageType ImageType { get; set; }
        public Int16 TileCountX { get; set; }
        public Int16 TileCountY { get; set; }
        public Int16 TileWidth { get; set; }
        public Int16 TileHeight { get; set; }
        public Int16 PaddingX { get; set; }
        public Int16 PaddingY { get; set; }
        public Int16 SpacingX { get; set; }
        public Int16 SpacingY { get; set; }

        public ImageResource(){}
        public ImageResource(Int64 id) : base(id){}

        public override void Refresh()
        {
            LoadIfNecessary(true);
        }

        public void LoadIfNecessary(bool force)
        {
            if (BitmapImage == null || force)
            {
                if (BitmapImage != null)
                {
                    BitmapImage = null;
                    GC.Collect();
                }
                if (File.Exists(Path))
                {
                    string dir = System.IO.Path.GetFullPath(Path);

                    BitmapImage = new BitmapImage(new Uri(dir));
                }
                else
                {
                    BitmapImage = Globals.Bitmap2BitmapImage(Globals.GetDefaultXImage());// new BitmapImage(new Uri("Resources/invalid_file.png", UriKind.Absolute));
                }
            }
        }

        public bool hasValues()
        {
            if (string.IsNullOrEmpty(Name) || string.IsNullOrEmpty(Path))
            {
                return false;
            }
            return true;
        }

        public override void Serialize(BinaryWriter w, string version)
        {
            base.Serialize(w, version);
            BinUtils.WriteString(Path, w);
            BinUtils.WriteInt16(TileCountX, w);
            BinUtils.WriteInt16(TileCountY, w);
            BinUtils.WriteInt16(TileWidth, w);
            BinUtils.WriteInt16(TileHeight, w);
            BinUtils.WriteInt16(PaddingX, w);
            BinUtils.WriteInt16(PaddingY, w);
            BinUtils.WriteInt16(SpacingX, w);
            BinUtils.WriteInt16(SpacingY, w);
        }

        public override void Deserialize(BinaryReader r, string version)
        {
            base.Deserialize(r, version);
            Path = BinUtils.ReadString(r);
            TileCountX = BinUtils.ReadInt16(r);
            TileCountY = BinUtils.ReadInt16(r);
            TileWidth = BinUtils.ReadInt16(r);
            TileHeight = BinUtils.ReadInt16(r);
            PaddingX = BinUtils.ReadInt16(r);
            PaddingY = BinUtils.ReadInt16(r);
            SpacingX = BinUtils.ReadInt16(r);
            SpacingY = BinUtils.ReadInt16(r);
        }

    }
}
