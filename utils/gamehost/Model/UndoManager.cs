using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    public enum UndoAction
    {
        [Description("Add Image")]
        AddImage,
        [Description("Delete Image")]
        DeleteImage,
        [Description("Edit Image")]
        EditImage,

        [Description("Add Sprite")]
        AddSprite,
        [Description("Delete Sprite ")]
        DeleteSprite,
        [Description("Edit Sprite ")]
        EditSprite,

        [Description("Add Map")]
        AddMap,
        [Description("Delete Map ")]
        DeleteMap,
        [Description("Edit Map ")]
        EditMap,

        [Description("Add Frame")]
        AddFrame,
        [Description("Delete Frame ")]
        DeleteFrame,
        [Description("Edit Frame ")]
        EditFrame

    }
    public class UndoItem
    {
        public object OldDataCopy { get; set; }
        public object NewDataReference { get; set; }
        public UndoAction UndoAction { get; set; }
        public UndoItem(UndoAction act, object old_copy, object new_reference)
        {
            UndoAction = act;
            OldDataCopy = old_copy;
            NewDataReference = new_reference;
        }
    }

    public class UndoManager
    {
        private ProjectFile ProjectFile { get { return Globals.MainWindow.ProjectFile; }  }
        private List<UndoItem> UndoItems { get; set; } = new List<UndoItem>();
        private List<UndoItem> RedoItems { get; set; } = new List<UndoItem>();

        public UndoManager()
        {
        }


        public void AddUndoItem(UndoItem newUndoItem)
        {
            UndoItems.Add(newUndoItem);
            if (UndoItems.Count > ProjectFile.MaxUndo)
            {
                UndoItems.RemoveAt(0);
            }

            Globals.MainWindow.UndoMenuItem.IsEnabled = true;
            Globals.MainWindow.UndoMenuItem.Header = "Undo " + Globals.GetEnumDescription((UndoAction)newUndoItem.UndoAction);
        }
        public void PerformUndo()
        {
            if (UndoItems.Count > 0)
            {
                UndoItem item = UndoItems.Last();
                UndoItems.RemoveAt(UndoItems.Count - 1);
                RedoItems.Add(item);
                if (RedoItems.Count > ProjectFile.MaxUndo)
                {
                    RedoItems.RemoveAt(0);
                }
                Globals.MainWindow.RedoMenuItem.IsEnabled = true;
                Globals.MainWindow.RedoMenuItem.Header = "Redo " + Globals.GetEnumDescription((UndoAction)item.UndoAction);

                if (UndoItems.Count == 0)
                {
                    Globals.MainWindow.UndoMenuItem.IsEnabled = false;
                    Globals.MainWindow.UndoMenuItem.Header = "Undo";
                }

                if (item.UndoAction == UndoAction.AddImage)
                {
                    ImageResource data = item.NewDataReference as ImageResource;
                    if (data != null)
                    {
                        if (!ProjectFile.Images.Contains(data as ImageResource))
                        {
                            Globals.MainWindow.LogError("Image Resource was not found for undoing!");
                        }
                        ProjectFile.Images.Remove(data as ImageResource);
                        Globals.MainWindow.WindowManager.RefreshAllWindows();
                    }
                }
                else if (item.UndoAction == UndoAction.DeleteImage)
                {
                    ImageResource data = item.OldDataCopy as ImageResource;
                    if (data != null)
                    {
                        if (ProjectFile.Images.Contains(data as ImageResource))
                        {
                            //Error
                            System.Diagnostics.Debugger.Break();
                            Globals.MainWindow.LogError("Image Resource was already found for undoing!");
                        }
                        ProjectFile.Images.Add(data as ImageResource);
                        Globals.MainWindow.WindowManager.RefreshAllWindows();
                    }
                }
                else if (item.UndoAction == UndoAction.EditImage)
                {
                    ImageResource dataold = item.OldDataCopy as ImageResource;
                    ImageResource datanew = item.NewDataReference as ImageResource;
                    if (dataold != null && datanew != null)
                    {
                        ProjectFile.Images.Remove(datanew);
                        ProjectFile.Images.Add(dataold);
                        Globals.MainWindow.WindowManager.RefreshAllWindows();
                    }
                }
                else
                {
                    throw new NotImplementedException();
                }

            }
        }
        public void PerformRedo()
        {

        }
        private void UndoSwapData(object old, object newob)
        {

        }
    }
}
