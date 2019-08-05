using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    public class MtTex
    {
        public Frame Frame { get; set; } = (Frame)null;
        public MtNode Node { get; set; } = (MtNode)null;
        public Bitmap Image { get; set; } = (Bitmap)null;
        public MtTex(Bitmap bmp, Frame fr)
        {
            Image = bmp;
            Frame = fr;
        }
        public MtTex CreateCopy(Frame newFrame)
        {
            return new MtTex(new Bitmap((System.Drawing.Image)Image), newFrame);
        }
    }
    public class MtNode
    {
        public MtTex Tex { get; set; } = (MtTex)null;
        public MtNode[] Child { get; set; } = new MtNode[2];
        public Box2i Rect { get; set; } = new Box2i();

        /// <summary>
        /// Splits the MtNode for a new texture region
        /// </summary>
        /// <param name="tex"></param>
        /// <returns></returns>
        public MtNode Plop(MtTex tex)
        {
            if (Child[0] != null && Child[1] != null)
            {
                MtNode mtNode = Child[0].Plop(tex);
                if (mtNode != null)
                {
                    return mtNode;
                }
                return Child[1].Plop(tex);
            }
            if (Tex != null)
            {
                return null;
            }
            int num1 = Rect.Width();
            int num2 = Rect.Height();
            if (tex.Image.Height > num2 || tex.Image.Width > num1)
            {
                return (MtNode)null;
            }
            if (tex.Image.Width == num1 && tex.Image.Height == num2)
            {
                if (Rect.p0.x == 0 && Rect.p0.y == 0)
                {
                    int num3 = 0 + 1;
                }
                return this;
            }
            Child[0] = new MtNode();
            Child[1] = new MtNode();
            if (num1 - tex.Image.Width > num2 - tex.Image.Height)
            {
                Child[0].Rect.Construct(Rect.Left(), Rect.Top(), Rect.Left() + tex.Image.Width, Rect.Bottom());
                Child[1].Rect.Construct(Rect.Left() + tex.Image.Width, Rect.Top(), Rect.Right(), Rect.Bottom());
            }
            else
            {
                Child[0].Rect.Construct(Rect.Left(), Rect.Top(), Rect.Right(), Rect.Top() + tex.Image.Height);
                Child[1].Rect.Construct(Rect.Left(), Rect.Top() + tex.Image.Height, Rect.Right(), Rect.Bottom());
            }
            return Child[0].Plop(tex);
        }
    }
    public class MegaTex
    {
        private MtNode _pRoot = (MtNode)null;
        public Bitmap MasterImage = (Bitmap)null;

        public int Id { get; set; } = -1;

        public string Name { get; set; }

        public MegaTex()
        {
        }

        public MegaTex(string name, int id)
        {
            Name = name;
            Id = id;
        }

        public void Serialize(BinaryWriter stream, bool packImages)
        {
            if (MasterImage == null)
            {
                throw new Exception("The master image was null for the given MegaTex " + Name);
            }
            ImageConverter imageConverter = new ImageConverter();
            BinUtils.WriteString(Name, stream);
            BinUtils.WriteInt32(Id, stream);
            if (packImages)
            {
                byte[] b = (byte[])imageConverter.ConvertTo((object)MasterImage, typeof(byte[]));
                BinUtils.WriteInt32(b.Length, stream);
                BinUtils.WriteBlock(b, stream);
            }
            else
            {
                BinUtils.WriteInt32(0, stream);
            }
        }

        public void Deserialize(BinaryReader stream, bool packedTextures)
        {
            Name = BinUtils.ReadString(stream);
            Id = BinUtils.ReadInt32(stream);
            int count = BinUtils.ReadInt32(stream);
            if (count <= 0)
            {
                return;
            }

            using (MemoryStream memoryStream = new MemoryStream(BinUtils.ReadBlock(count, stream)))
            {
                MasterImage = new Bitmap((Stream)memoryStream);
            }
        }

        public void Compile(List<MtTex> Texs, int iMinTexSize, int iMaxTexSize, int iGrowBy)
        {
            if (iMinTexSize > iMaxTexSize)
            {
                throw new Exception("Min texture size is greater than max texture size.");
            }
            Texs.Sort((Comparison<MtTex>)((a, b) =>
            {
                int num1 = a.Image.Width * a.Image.Height;
                int num2 = b.Image.Width * b.Image.Height;
                if (num1 > num2)
                    return 1;
                return num1 < num2 ? -1 : 0;
            }));
            int num3 = iMinTexSize;
            int num4 = 0;
            List<MtTex> mtTexList = new List<MtTex>();
            while (num3 <= iMaxTexSize)
            {
                _pRoot = new MtNode();
                _pRoot.Rect.p0.Construct(0, 0);
                _pRoot.Rect.p1.Construct(num3, num3);
                bool flag = true;
                mtTexList.Clear();
                foreach (MtTex mtTex in Texs) {
                    mtTex.Node = (MtNode)null;
                }
                foreach (MtTex tex in Texs)
                {
                    if (tex.Image != null)
                    {
                        MtNode mtNode = _pRoot.Plop(tex);
                        if (mtNode != null)
                        {
                            if (mtNode.Tex != null)
                            {
                                int num1 = 0 + 1;
                            }
                            else {
                                mtNode.Tex = tex;
                            }
                            tex.Node = mtNode;
                            mtTexList.Add(tex);
                        }
                        else
                        {
                            num3 += iGrowBy;
                            ++num4;
                            flag = false;
                            break;
                        }
                    }
                    else
                    {
                        int num2 = 0 + 1;
                    }
                }
                if (flag)
                {
                    break;
                }
            }
            foreach (MtTex mtTex in mtTexList) { 
            Texs.Remove(mtTex); }
            //if (num3 <= iMaxTexSize)
            //    ;
            MasterImage = new Bitmap(num3, num3);
            foreach (MtTex mtTex1 in mtTexList)
            {
                foreach (MtTex mtTex2 in mtTexList)
                {
                    if (mtTex1 != mtTex2)
                    {
                        if (mtTex1.Node == mtTex2.Node)
                        {
                            int num1 = 0 + 1;
                        }
                        if (mtTex1.Node.Rect.p0.x == mtTex2.Node.Rect.p0.x && mtTex1.Node.Rect.p0.y == mtTex2.Node.Rect.p0.y)
                        {
                            int num2 = 0 + 1;
                        }
                    }
                }
            }
            foreach (MtTex mtTex in mtTexList)
            {
                GraphicsUnit pageUnit = GraphicsUnit.Pixel;
                Rectangle destRegion = new Rectangle(mtTex.Node.Rect.p0.x, mtTex.Node.Rect.p0.y, mtTex.Node.Rect.Width(), mtTex.Node.Rect.Height());
                Rectangle srcRegion = Rectangle.Round(mtTex.Image.GetBounds(ref pageUnit));
                Globals.CopyRegionIntoImage(mtTex.Image, srcRegion, ref MasterImage, destRegion);
                mtTex.Frame.x = mtTex.Node.Rect.p0.x;
                mtTex.Frame.y = mtTex.Node.Rect.p0.y;
                mtTex.Frame.w = mtTex.Node.Rect.Width();
                mtTex.Frame.h = mtTex.Node.Rect.Height();
                mtTex.Frame.texid = Id;
            }
        }
    }
}
