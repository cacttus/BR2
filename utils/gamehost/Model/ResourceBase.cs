using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;
using System.Xml.Serialization;

namespace GameHost
{
    public class IdItemBase
    {
        [JsonProperty]
        public Int64 Id { get; set; } = -1;//Unique id of the resource

        public IdItemBase() { }
        public IdItemBase(Int64 id)
        {
            Id = id;
        }
        public virtual void Serialize(BinaryWriter w, string version)
        {
            BinUtils.WriteInt64(Id, w);
        }
        public virtual void Deserialize(BinaryReader r, string version)
        {
            Id = BinUtils.ReadInt64(r);
        }
        public static T CreateCopy<T>(T value) where T : IdItemBase
        {
            T ret = null;
            T x = value;
            using (MemoryStream m = new MemoryStream())
            {
                BinaryWriter w = new BinaryWriter(m);
                x.Serialize(w, Globals.ProgramVersion);

                m.Position = 0;

                BinaryReader r = new BinaryReader(m);
                //ret = new T(this._objProjectFile, -1);
                ret = (T)Activator.CreateInstance(typeof(T), new object[] { -1 });
                ret.Deserialize(r, Globals.ProgramVersion);

                r.Dispose();
                w.Dispose();
            }
            return ret;
        }
    }
    /// <summary>
    /// base class for all serializbale rersources
    /// </summary>
    public abstract class ResourceBase : IdItemBase
    {
        //Not Serialized
        [JsonIgnore]
        public BitmapImage BitmapImage { get; set; } = null;

        [JsonProperty]
        public string Name { get; set; }

        public ResourceBase() { }
        public ResourceBase(Int64 id) : base(id) { }

        // public abstract void Construct(ProjectFile pf, Int64 id);
        public abstract void Refresh();
        public override void Serialize(BinaryWriter w, string version)
        {
            base.Serialize(w, version);
            BinUtils.WriteString(Name, w);
        }
        public override void Deserialize(BinaryReader r, string version)
        {
            base.Deserialize(r, version);
            Name = BinUtils.ReadString(r);
        }
    }
}
