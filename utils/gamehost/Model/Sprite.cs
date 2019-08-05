using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    public class Sprite : ResourceBase
    {
        // public int Direction { get; set; } = -1;
        [JsonProperty]
        public bool Loop { get; set; } = true;
        [JsonProperty]
        public List<Frame> Frames { get; set; } = new List<Frame>();
        [JsonProperty]
        public string FileLocation { get; set; } = "";
        [JsonProperty]
        public float Origin_X { get; set; } = 0.0f;
        [JsonProperty]
        public float Origin_Y { get; set; } = 0.0f;

        public Sprite() { }
        public Sprite(Int64 id) : base(id) { }

        public override void Refresh()
        {
            throw new NotImplementedException();
        }

        public void SortFrames()
        {
            Frames.Sort((Comparison<Frame>)((x, y) => x.FrameId.CompareTo(y.FrameId)));
        }
        public override void Serialize(BinaryWriter stream, string version)
        {
            //BinUtils.WriteInt32(Direction, stream);
            base.Serialize(stream, version);
            BinUtils.WriteBool(Loop, stream);
            BinUtils.WriteString(FileLocation, stream);
            BinUtils.WriteFloat(Origin_X, stream);
            BinUtils.WriteFloat(Origin_Y, stream);
            BinUtils.WriteInt32(Frames.Count, stream);
            foreach (Frame frame in Frames)
            {
                frame.Serialize(stream, version);
            }
        }

        public override void Deserialize(BinaryReader stream, string version)
        {
            //  Direction = BinUtils.ReadInt32(stream);
            base.Deserialize(stream, version);
            Loop = BinUtils.ReadBool(stream);
            FileLocation = BinUtils.ReadString(stream);
            Origin_X = BinUtils.ReadFloat(stream);
            Origin_Y = BinUtils.ReadFloat(stream);
            int num = BinUtils.ReadInt32(stream);
            for (int index = 0; index < num; ++index)
            {
                Frame frame = new Frame(-1);
                frame.Deserialize(stream, version);
                frame.Sprite = this;
                Frames.Add(frame);
            }
        }
    }
}
