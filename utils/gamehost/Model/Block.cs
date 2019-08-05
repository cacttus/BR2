using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    /// <summary>
    /// Don't know what this is for
    /// </summary>
    public class Block
    {
        public int Size { get; set; } = 0;
        public BinaryWriter OutStream { get; set; } = (BinaryWriter)null;
        public BinaryReader InStream { get; set; } = (BinaryReader)null;

        public Block(BinaryWriter o)
        {
            this.OutStream = o;
        }

        public Block(BinaryReader i)
        {
            this.InStream = i;
        }
    }
}
