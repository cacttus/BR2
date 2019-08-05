using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    /// <summary>
    /// Tiles in the tile grid
    /// </summary>
    public class Tile : IdItemBase
    {
        public Tile() { }
    }
    public class TileLayer : ResourceBase
    {
        List<List<Tile>> Tiles = new List<List<Tile>>();
        public TileLayer() { }
 
        public override void Serialize(BinaryWriter w, string version)
        {
            base.Serialize(w, version);
            BinUtils.WriteInt32(Tiles.Count, w);
            foreach (List<Tile> y in Tiles)
            {
                BinUtils.WriteInt32(y.Count, w);
                foreach (Tile x in y)
                {
                    x.Serialize(w, version);
                }
            }
        }
        public override void Deserialize(BinaryReader r, string version)
        {
            base.Deserialize(r, version);
            int ycount = BinUtils.ReadInt32(r);
            for (int y = 0; y < ycount; ++y)
            {
                int xcount = BinUtils.ReadInt32(r);
                for (int x = 0; x < xcount; ++x)
                {
                    Tile t = new Tile();
                    t.Deserialize(r, version);
                }
            }
        }

        public override void Refresh()
        {
            throw new NotImplementedException();
        }
    }
    /// <summary>
    /// Holds info about the tile grid.  Maps of tiles,  I.e  Worls.
    /// </summary>
    public class TileMap : ResourceBase
    {
        [JsonProperty]
        public List<TileLayer> Layers;
        
        public TileMap() { }
        public TileMap(ProjectFile pf, Int64 id)
        {

        }
        public override void Serialize(BinaryWriter w, string version)
        {
            base.Serialize(w, version);
            BinUtils.WriteInt32(Layers.Count, w);
            foreach(TileLayer layer in Layers) {
                layer.Serialize(w, version);
            }
        }
        public override void Deserialize(BinaryReader r, string version)
        {
            base.Deserialize(r, version);
            int count = BinUtils.ReadInt32(r);
            for(int x=0; x<count; x++)
            {
                TileLayer layer = new TileLayer();
                layer.Deserialize(r, version);
            }
        }
        public override void Refresh()
        {
            throw new NotImplementedException();
        }

    }
}
