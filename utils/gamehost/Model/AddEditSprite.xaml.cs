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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GameHost
{
    /// <summary>
    /// Interaction logic for AddEditSprite.xaml
    /// </summary>
    public partial class AddEditSprite : AddEditBase
    {
        public Sprite Sprite = null;

        public CollectionEditor FrameEditor = null;

        public AddEditSprite(string tt, Sprite s = null) : base( tt)
        {
            InitializeComponent();

            if (s == null)
            {
                AddEditMode = AddEditMode.Add;
                Sprite = new Sprite(Globals.MainWindow.ProjectFile.GenId());
            }
            else
            {
                AddEditMode = AddEditMode.Edit;
                Sprite = s;
            }


            FrameEditor = new CollectionEditor("", () => { return Sprite.Frames.ToList<ResourceBase>(); });
            FrameEditor.HorizontalAlignment = HorizontalAlignment.Stretch;
            FrameEditor.VerticalAlignment = VerticalAlignment.Stretch;

            string title = "Frames";
            FrameEditor.AddItemClicked += (x, y) =>
            {
                AddEditFrame d = new AddEditFrame(title, null);
                d.ShowDialog(() =>
                {
                    if (d.DialogResult == true)
                    {
                        Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.AddSprite, null, d.Frame));
                        FrameEditor.RefreshEverything();
                    }
                });
            };
            FrameEditor.EditItemClicked += (x, y) =>
            {
                ResourceBase r = FrameEditor.GetSelectedItem();

                if (r as Sprite != null)
                {
                    Sprite OldSpriteCopy = IdItemBase.CreateCopy(r as Sprite);

                    AddEditFrame d = new AddEditFrame(title, r as Frame);
                    d.ShowDialog(() =>
                    {
                        if (d.DialogResult == true)
                        {
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.EditSprite, OldSpriteCopy, d.Frame));
                            FrameEditor.RefreshEverything();
                        }
                    });
                }
            };
            FrameEditor.RemoveItemClicked += (x, y) =>
            {
                ResourceBase r = FrameEditor.GetSelectedItem();
                if (r != null)
                {
                    if (r as Sprite != null)
                    {
                        Sprite.Frames.Remove(r as Frame);
                        Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.DeleteSprite, r as Frame, null));
                        FrameEditor.RefreshEverything();
                    }
                }
            };
            _objFrameEditorContainer.Children.Add(FrameEditor);


        }

        private void FramesWindowShow()
        {

        }
    }
}
