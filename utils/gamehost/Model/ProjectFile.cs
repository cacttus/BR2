using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Xml;
using System.Xml.Serialization;

namespace GameHost
{
    //**We need something besides xmlserializer
    public class ProjectFile
    {
        #region Private:Members
        [JsonIgnore]
        private string _strProjectName = "MyProject--unsaved";
        [JsonIgnore]
        private string _strLoadedOrSavedProjectFileName = "MyProject.iwp";
        [JsonIgnore]
        private List<ImageResource> _lstImages = new List<ImageResource>();
        [JsonIgnore]
        private List<TileMap> _lstTileMaps = new List<TileMap>();
        [JsonIgnore]
        private List<Sprite> _lstSprites = new List<Sprite>();
        [JsonProperty]
        private Int64 IdGen = 1000;
        [JsonProperty]
        public int MaxUndo { get; set; } = 100;

        public Int64 GenId()
        {
            return IdGen++;
        }
        #endregion

        #region Properties
        [JsonProperty]
        public string ProjectName
        {
            get
            {
                return _strProjectName;
            }
            set
            {
                _strProjectName = value;
                MarkChanged();
            }
        }
        //This path chanes when we load
        [JsonIgnore]
        public string LoadedOrSavedProjectFileName
        {
            get
            {
                return _strLoadedOrSavedProjectFileName;
            }
            set
            {
                _strLoadedOrSavedProjectFileName = value; MarkChanged();
            }
        }
        [JsonProperty]
        public List<ImageResource> Images
        {
            get
            {
                return _lstImages;
            }
            set
            {
                _lstImages = value;
                MarkChanged();
            }
        }
        [JsonProperty]
        public List<Sprite> Sprites
        {
            get
            {
                return _lstSprites;
            }
            set
            {
                _lstSprites = value;
                MarkChanged();
            }
        }
        [JsonProperty]
        public List<TileMap> Maps
        {
            get
            {
                return _lstTileMaps;
            }
            set
            {
                _lstTileMaps = value;
                MarkChanged();
            }
        }

        [JsonIgnore]
        public bool DataChanged { get; private set; } = false;//do not serialize
        #endregion

        public ProjectFile()
        {
        }

        public ResourceBase GetResourceById(Int64 id)
        {
            ResourceBase r;
            r = _lstImages.Where(x => x.Id == id).FirstOrDefault();
            if (r != null) { return r; }
            r = _lstTileMaps.Where(x => x.Id == id).FirstOrDefault();
            if (r != null) { return r; }
            r = _lstSprites.Where(x => x.Id == id).FirstOrDefault();
            if (r != null) { return r; }

            foreach (Sprite s in _lstSprites)
            {
                r = s.Frames.Where(x => x.Id == id).FirstOrDefault();
                if (r != null) { return r; }
            }
            //TODO: more here

            return null;
        }

        public void SetCwdToRoot()
        {
            System.IO.Directory.SetCurrentDirectory(GetProjectRoot());
        }

        public string GetProjectRoot()
        {
            return System.IO.Path.GetDirectoryName(LoadedOrSavedProjectFileName);
        }

        public string GetImageResourcePath()
        {
            string bp = GetProjectRoot();
            string imagesPath = System.IO.Path.Combine(bp, "images");

            if (!Directory.Exists(imagesPath))
            {
                Directory.CreateDirectory(imagesPath);
            }

            return imagesPath;
        }

        public void MarkChanged()
        {
            DataChanged = true;
            Globals.MainWindow.MarkChanged(true);
        }

        public void ClearChanged()
        {
            DataChanged = false;
            Globals.MainWindow.MarkChanged(false);
        }

        private void ValidateFileWaypoint(string s, BinaryReader r, BinaryWriter w)
        {
            //this method reads or writes a small string value to make sure file isn't jacked up.
            if (r != null)
            {
                string test = BinUtils.ReadString(r);

                if (s.Equals(test) == false)
                {
                    //Error*!
                    Globals.MainWindow.LogError("File load failed: Validation failed at '" + s + " (validation value was " + test + ").");
                }
            }
            else if (w != null)
            {
                BinUtils.WriteString(s, w);
            }
        }
        public void Save(string loc, bool textures)
        {
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            {
                LoadedOrSavedProjectFileName = loc;
                SaveBinary(loc, textures);

                //string xmlLoc = Path.Combine(Path.GetDirectoryName(loc), Path.GetFileNameWithoutExtension(loc) + ".xml");
                //SaveXML(xmlLoc, true, this);

                string jsonLoc = Path.Combine(Path.GetDirectoryName(loc), Path.GetFileNameWithoutExtension(loc) + ".json");
                SaveJson(jsonLoc, true, this);
            }
            sw.Stop();
            Globals.MainWindow.SetStatus("Saved '" + loc + "' in " + Globals.TimeSpanToString(sw.Elapsed));
        }
        public void Load(string loc)
        {
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            {
                LoadedOrSavedProjectFileName = loc;
                if (LoadBinary(loc) == false)
                {
                    //LoadXML(loc);
                    LoadJson(loc);
                }
            }
            sw.Stop();
            Globals.MainWindow.SetStatus("Loaded '" + loc + "' in " + Globals.TimeSpanToString(sw.Elapsed));
        }
        public static void SaveXML(string loc, bool textures, ProjectFile pf)
        {

            //Problem is we can't serialize private properties and we want that.
            //Eiteher implement custom xmlserializer OR
            //Use theother approach (which is..?)
            try
            {
                //using (var ms = new MemoryStream())
                //{
                //    var ser = new BinaryFormatter();
                //    System.Console.WriteLine("> serializing");
                //    ser.Serialize(ms, pf);
                //    System.Console.WriteLine("< serializing");
                //    ms.Position = 0;
                //    System.Console.WriteLine("> deserializing");
                //    ser.Deserialize(ms);
                //    System.Console.WriteLine("< deserializing");
                //}

                XmlSerializer xsSubmit = new XmlSerializer(typeof(ProjectFile));
                var xml = "";
                using (var sww = new StringWriter())
                {
                    using (XmlWriter writer = XmlWriter.Create(sww))
                    {
                        xsSubmit.Serialize(writer, pf);
                        xml = sww.ToString(); // Your XML
                    }
                }

                File.WriteAllText(loc, xml);
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to save XML File: " + ex.ToString());
            }
        }

        public static ProjectFile LoadXML(string loc)
        {
            try
            {
                using (var stream = System.IO.File.OpenRead(loc))
                {
                    var serializer = new XmlSerializer(typeof(ProjectFile));
                    return serializer.Deserialize(stream) as ProjectFile;
                }
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to Load XML File: " + ex.ToString());
            }
            return null;
        }
        public static void SaveJson(string loc, bool textures, ProjectFile pf)
        {
            try
            {
                string output = JsonConvert.SerializeObject(pf);


                File.WriteAllText(loc, output);
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to save XML File: " + ex.ToString());
            }
        }
        public static ProjectFile LoadJson(string loc)
        {
            try
            {
                string text = System.IO.File.ReadAllText(loc);
                return JsonConvert.DeserializeObject<ProjectFile>(text);
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to save XML File: " + ex.ToString());
            }
            return null;
        }
        private bool SaveBinary(string loc, bool textures)
        {
            bool success = false;
            try
            {
                string dirname = System.IO.Path.GetDirectoryName(loc);
                if (!System.IO.Directory.Exists(loc))
                {
                    System.IO.Directory.CreateDirectory(dirname);
                }

                using (BinaryWriter w = new BinaryWriter((Stream)File.Open(loc, FileMode.Create)))
                {
                    BinUtils.WriteString(Globals.ProgramVersion, w);
                    BinUtils.WriteInt32(80085, w);
                    BinUtils.WriteInt16((short)8080, w);
                    BinUtils.WriteBlock(new byte[1] { (byte)1 }, w);
                    BinUtils.WriteFloat(3.14f, w);
                    BinUtils.WriteBool(true, w);
                    BinUtils.WriteBlock(new byte[5] { 5, 4, 3, 2, 1 }, w);

                    BinUtils.WriteInt64(IdGen, w);
                    BinUtils.WriteInt32(MaxUndo, w);

                    BinUtils.WriteString(ProjectName, w);

                    string version = Globals.ProgramVersion;

                    WriteGroup<ImageResource>("ImageResources", Images, w, version);
                    WriteGroup<Sprite>("Sprites", Sprites, w, version);
                    WriteGroup<TileMap>("TileMaps", Maps, w, version);
                    //**Other file stuff

                    ValidateFileWaypoint("EndOfFile", null, w);
                    ClearChanged();
                }

                success = true;
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to load IWP Project file binary: " + ex.ToString());
            }
            return success;
        }
        private bool LoadBinary(string loc)
        {
            bool success = true;
            try
            {
                using (BinaryReader r = new BinaryReader((Stream)File.Open(loc, FileMode.Open)))
                {
                    string version = BinUtils.ReadString(r);

                    if (version != Globals.ProgramVersion)
                    {
                        string str = "Note: This was created with an older version. " +
                            version + ".  Press 'OK' to upgrade to version " +
                            Globals.ProgramVersion + ".";

                        Globals.MainWindow.LogError(str);

                        Globals.ShowError(str, "Upgrade Notification");
                    }
                    BinUtils.ReadInt32(r);
                    BinUtils.ReadInt16(r);
                    BinUtils.ReadBlock(1, r);
                    BinUtils.ReadFloat(r);
                    BinUtils.ReadBool(r);
                    BinUtils.ReadBlock(5, r);

                    if (Convert.ToDouble(version) > 0.01)
                    {
                        IdGen = BinUtils.ReadInt64(r);
                        MaxUndo = BinUtils.ReadInt32(r);
                    }

                    ProjectName = BinUtils.ReadString(r);

                    ReadGroup<ImageResource>("ImageResources", Images, r, version);
                    ReadGroup<Sprite>("Sprites", Sprites, r, version);
                    ReadGroup<TileMap>("TileMaps", Maps, r, version);

                    ValidateFileWaypoint("EndOfFile", r, null);
                    success = true;
                }
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError("Failed to load file binary " + ex.ToString());
                success = false;
            }
            return success;
        }
        private void ReadGroup<T>(string groupname, List<T> group, BinaryReader r, string version) where T : ResourceBase
        {
            ValidateFileWaypoint(groupname, r, null);

            group = new List<T>();
            int count = BinUtils.ReadInt32(r);
            for (int i = 0; i < count; ++i)
            {
                T x = (T)Activator.CreateInstance(typeof(T), new object[] { this, -1 });

                x.Deserialize( r, version);
                group.Add(x);
            }
        }
        private void WriteGroup<T>(string groupname, List<T> group, BinaryWriter w, string version) where T : ResourceBase
        {
            ValidateFileWaypoint(groupname, null, w);

            BinUtils.WriteInt32(group.Count, w);
            foreach (T x in group)
            {
                x.Serialize(w, version);
            }
        }
    }
}
