using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;
using Newtonsoft.Json;

namespace GameHost
{
    /// <summary>
    /// A single sprite frame for frame-by-frame animation.
    /// Sprites have at least one frame, or multiple frames!
    /// </summary>
    public class Frame : ResourceBase
    {
        [JsonProperty]
        public int Delay = 41;
        [JsonProperty]
        public int texid = -1;
        [JsonProperty]
        public string Location = "";
        [JsonProperty]
        public string Name = "";
        [JsonProperty]
        public int FrameId = -1;
        [JsonProperty]
        public int x;
        [JsonProperty]
        public int y;
        [JsonProperty]
        public int w;
        [JsonProperty]
        public int h;

        [JsonIgnore]
        public MtTex ImageTemp { get; set; } = (MtTex)null;
        [JsonIgnore]
        public Sprite Sprite { get; set; } = (Sprite)null;

        public Frame() { }
        public Frame(Int64 id) : base(id) { }

        public override void Refresh()
        {
            throw new NotImplementedException();
        }

        public override void Serialize(BinaryWriter stream, string version)
        {
            BinUtils.WriteInt32(x, stream);
            BinUtils.WriteInt32(y, stream);
            BinUtils.WriteInt32(w, stream);
            BinUtils.WriteInt32(h, stream);
            BinUtils.WriteInt32(texid, stream);
            BinUtils.WriteInt32(Delay, stream);
            BinUtils.WriteString(Location, stream);
            BinUtils.WriteString(Name, stream);
            BinUtils.WriteInt32(FrameId, stream);
        }

        public override void Deserialize(BinaryReader stream, string version)
        {
            Block block = new Block(stream);
            x = BinUtils.ReadInt32(stream);
            y = BinUtils.ReadInt32(stream);
            w = BinUtils.ReadInt32(stream);
            h = BinUtils.ReadInt32(stream);
            texid = BinUtils.ReadInt32(stream);
            Delay = BinUtils.ReadInt32(stream);
            Location = BinUtils.ReadString(stream);
            Name = BinUtils.ReadString(stream);
            FrameId = BinUtils.ReadInt32(stream);
        }

        //public Frame CreateCopy()
        //{
        //    Frame newFrame = new Frame(this._objProjectFile, this.Id)
        //    {
        //        Delay = Delay,
        //        x = x,
        //        y = y,
        //        w = w,
        //        h = h,
        //        texid = texid
        //    };
        //    newFrame.ImageTemp = ImageTemp.CreateCopy(newFrame);
        //    newFrame.Location = Location;
        //    newFrame.Name = Name;
        //    newFrame.Sprite = Sprite;
        //    newFrame.FrameId = FrameId;
        //    return newFrame;
        //}

        public void LoadFrameImage(string file, MainWindow mf)
        {
            Bitmap bmp;
            if (!File.Exists(file))
            {
                bmp = Globals.GetDefaultXImage();
            }
            else
            {
                bmp = new Bitmap(file);
            }
            ImageTemp = new MtTex(bmp, this);
        }
    }
}
