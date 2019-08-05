using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
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
using MahApps.Metro.Controls;
using Dragablz;
using Dragablz.Dockablz;
using System.Diagnostics;
using System.Windows.Controls.Primitives;
using System.Windows.Threading;

namespace GameHost
{
    public class WindowManager
    {
        #region Private:Members
        private List<MahApps.Metro.SimpleChildWindow.ChildWindow> _lstModals = new List<MahApps.Metro.SimpleChildWindow.ChildWindow>();
        private List<ToolWindowBase> AllWindows { get; set; } = new List<ToolWindowBase>();
        private List<FloatingWindowHost> All_Window_Windows { get; set; } = new List<FloatingWindowHost>();
        private FolderBrowserWindow _objFolderBrowserWindow = null;
        private EditToolbarWindow _objEditToolbarWindow = null;
        private CollectionEditor _objImagesWindow = null;
        private CollectionEditor _objSpritesWindow = null;
        private CollectionEditor _objMapsWindow = null;
        private PreferencesWindow _objPreferencesWindow = null;

        private List<FloatingWindowHost> _lstFloatingWindows = new List<FloatingWindowHost>();

        private Dragablz.Dockablz.Layout FloatLayout { get { return Globals.MainWindow.FloatLayout; } }
        #endregion

        #region Public: Members
        public FolderBrowserWindow FolderBrowserWindow
        {
            get
            {
                CreateFolderBrowserWindow();
                return _objFolderBrowserWindow;
            }
        }
        public EditToolbarWindow EditToolbarWindow
        {
            get
            {
                CreateToolbarWindow();
                return _objEditToolbarWindow;
            }
        }
        public CollectionEditor ImagesWindow
        {
            get
            {
                CreateImagesWindow();
                return _objImagesWindow;
            }
        }
        public CollectionEditor SpritesWindow
        {
            get
            {
                CreateSpritesWindow();
                return _objSpritesWindow;
            }
        }
        public CollectionEditor MapsWindow
        {
            get
            {
                CreateMapsWindow();
                return _objMapsWindow;
            }
        }
        public PreferencesWindow PreferencesWindow
        {
            get
            {
                CreatePreferencesWindow();
                return _objPreferencesWindow;
            }
        }
        #endregion

        public void RefreshAllWindows()
        {
            ImagesWindow?.RefreshEverything();
            SpritesWindow?.RefreshEverything();
            MapsWindow?.RefreshEverything();
        }
        public void ClearPreferences()
        {
            _objPreferencesWindow = new PreferencesWindow("Preferences");
        }
        private void CreatePreferencesWindow()
        {
            if (_objPreferencesWindow == null)
            {
                _objPreferencesWindow = new PreferencesWindow("Preferences");
            }
        }
        public void ShowToolWindow(string header, UIElement content, bool bModal = false, Action afterShow = null)
        {
            if (bModal)
            {
                MahApps.Metro.SimpleChildWindow.ChildWindow modal = new MahApps.Metro.SimpleChildWindow.ChildWindow();

                modal.Title = header;
                modal.Content = content;
                modal.ShowCloseButton = true;
                modal.IsModal = true;
                Grid.SetRow(modal, 0);
                Grid.SetColumn(modal, 0);

                //Execute an action when the dialog closes.
                modal.Closing += (x, e) =>
                {
                    afterShow?.Invoke();

                    ToolWindowBase tb = modal.Content as ToolWindowBase;
                    if (tb != null)
                    {
                        if (tb.OnClose() == false)
                        {
                            Globals.MainWindow.LogError("tried to cancel close action of a modal window - can't do that.");
                            ///throw new NotImplementedException();//Shouldn't call this
                        }
                    }

                    if (_lstModals.Contains(modal))
                    {

                        _lstModals.Add(modal);
                    }
                    else
                    {
                        Globals.MainWindow.LogError("Couldn't find modal window!", true);
                    }
                };

                MahApps.Metro.SimpleChildWindow.ChildWindowManager.ShowChildWindowAsync(Globals.MainWindow, modal);
                _lstModals.Add(modal);
            }
            else
            {

                    ToolWindowBase tb = content as ToolWindowBase;
                // CreateDockablzWindow(header, content);
                if (tb != null)
                {
                    tb.Title = header;
                    CreatePopoutWindow(tb);
                }
                else
                {
                    Globals.MainWindow.LogError("Content window was not a toolwindow");
                }

            }
        }
        public void CreatePopoutWindow(ToolWindowBase content)
        {
            GameHost.FloatingWindowHost w = new FloatingWindowHost();
            w.Content = content;
            w.InnerControl = content;
            w.IsWindowDraggable = true;
            w.LocationChanged += (x, e) =>
            {
                Globals.MainWindow.CheckDropZoneDrag(w);
            };
            w.MouseUp += (x, e) =>
            {
                Globals.MainWindow.CheckDropZoneDrop(w);
            };
            ToolWindowBase uc = content as ToolWindowBase;
            if (uc != null)
            {
                w.Width = uc.MinWidth;
                w.Height = uc.MinHeight;
            }
            w.Title = content.Title;
            w.Show();

            All_Window_Windows.Add(w);
        }
        private void CreateDockablzWindow(string header, UIElement content)
        {
            Label label = new Label();
            label.Style = Application.Current.Resources["IW_ToolWindowTitlebarTextStyle"] as Style;
            label.Content = header;

            foreach (var x in _lstFloatingWindows)
            {
                if (x.Title == header)
                {
                    x.BringIntoView();
                    return;
                }
            }


            //**Old way
            HeaderedItemViewModel viewmodel = new HeaderedItemViewModel(label, content, true);
            Globals.MainWindow.FloatLayout.FloatingItems.Add(viewmodel);
            Globals.UpdateLayout(Globals.MainWindow);


            //Add event for thumb drag.
            DependencyObject thumb = Globals.FindAncestor(content, "PART_Thumb");
            if (thumb != null)
            {
                //Add event for when we drag a window
                (thumb as Thumb).DragDelta += (x, e) =>
                {
                    //We're dragging
                    CheckDock();

                };
            }

        }
        void CheckDock()
        {

        }

        public bool FindToolWindow(int iWindowId, out HeaderedItemViewModel foundVm, out ToolWindowBase foundWindow)
        {
            foundVm = null;
            foundWindow = null;
            foreach (HeaderedItemViewModel vm in FloatLayout.FloatingItems)
            {
                ToolWindowBase tb = vm.Content as ToolWindowBase;
                if (tb != null)
                {
                    if (tb.WindowId == iWindowId)
                    {
                        foundVm = vm;
                        foundWindow = tb;
                        break;
                    }
                }
            }

            if (foundVm != null)
            {
                return true;
            }
            return false;
        }
        public void HideToolWindow(int iWindowId)
        {
            HeaderedItemViewModel foundVM = null;
            ToolWindowBase foundWindow = null;

            if (FindToolWindow(iWindowId, out foundVM, out foundWindow))
            {
                //This won't work but meh.
                foundWindow.Visibility = Visibility.Hidden;
            }
        }
        public void CloseToolWindow(FloatingWindowHost host)
        {
            this.All_Window_Windows.Remove(host);
            host.Close();

        }
        public void CloseToolWindow(int iWindowId)
        {
            //Closes a tool window with the given windowId
            HeaderedItemViewModel foundVM = null;
            ToolWindowBase foundWindow = null;
            ToolWindowBase tb = null;
            MahApps.Metro.SimpleChildWindow.ChildWindow cw = null;

            if (GetModal(iWindowId, out tb, out cw))
            {
                //Don't actuallly "close" the modal just hide it
                if (tb != null)
                {
                    if (tb.OnClose())
                    {
                        cw.Close();
                        _lstModals.Remove(cw);
                    }
                }
                else
                {
                    cw.Close();
                    _lstModals.Remove(cw);
                }
            }
            else if (FindToolWindow(iWindowId, out foundVM, out foundWindow))
            {
                //Close + Run the OnClose for the window
                if (foundWindow.OnClose() == false)
                {
                    FloatLayout.FloatingItems.Remove(foundVM);
                }
            }
            else
            {
                Globals.MainWindow.LogError("Couldn't close window - widnow not found.", true);
            }
        }
        public bool GetModal(int iWindowId, out ToolWindowBase tb, out MahApps.Metro.SimpleChildWindow.ChildWindow cw)
        {
            tb = null;
            cw = null;
            foreach (MahApps.Metro.SimpleChildWindow.ChildWindow w in _lstModals)
            {
                ToolWindowBase tw = w.Content as ToolWindowBase;
                if (tw != null)
                {
                    if (tw.WindowId == iWindowId)
                    {
                        cw = w;
                        tb = tw;
                        return true;
                    }
                }
            }
            return false;
        }

        private void CreateImagesWindow()
        {
            if (_objImagesWindow == null)
            {
                _objImagesWindow = new CollectionEditor("Images", () => { return Globals.MainWindow.ProjectFile?.Images.ToList<ResourceBase>(); });
                _objImagesWindow.AddItemClicked += (x, y) =>
                {
                    AddEditImage d = new AddEditImage("Image", null);
                    d.ShowDialog(() =>
                    {
                        if (d.DialogResult == true)
                        {
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.AddImage, null, d.ImageResource));
                            _objImagesWindow.RefreshEverything();
                        }
                    });
                };
                _objImagesWindow.EditItemClicked += (x, y) =>
                {
                    ResourceBase r = _objImagesWindow.GetSelectedItem();

                    if (r as ImageResource != null)
                    {
                        ImageResource OldImageResourceCopy = IdItemBase.CreateCopy(r as ImageResource);

                        AddEditImage d = new AddEditImage("Resource", r as ImageResource);
                        d.ShowDialog(() =>
                        {
                            if (d.DialogResult == true)
                            {
                                Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.EditImage, OldImageResourceCopy, d.ImageResource));
                                _objImagesWindow.RefreshEverything();
                            }
                        });
                    }
                };
                _objImagesWindow.RemoveItemClicked += (x, y) =>
                {
                    ResourceBase r = _objImagesWindow.GetSelectedItem();
                    if (r != null)
                    {
                        if (r as ImageResource != null)
                        {
                            Globals.MainWindow.ProjectFile?.Images.Remove(r as ImageResource);
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.DeleteImage, r as ImageResource, null));
                            _objImagesWindow.RefreshEverything();
                        }
                    }
                };
            }
            _objImagesWindow.Show();
        }
        private void CreateFolderBrowserWindow()
        {
            _objFolderBrowserWindow = new FolderBrowserWindow("Folder Explorer");
        }


        private void CreateSpritesWindow()
        {
            if (_objSpritesWindow == null)
            {
                string title = "Sprite";
                _objSpritesWindow = new CollectionEditor(title, () => { return Globals.MainWindow.ProjectFile?.Sprites.ToList<ResourceBase>(); });
                _objSpritesWindow.AddItemClicked += (x, y) =>
                {
                    AddEditSprite d = new AddEditSprite(title, null);
                    d.ShowDialog(() =>
                    {
                        if (d.DialogResult == true)
                        {
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.AddSprite, null, d.Sprite));
                            _objSpritesWindow.RefreshEverything();
                        }
                    });
                };
                _objSpritesWindow.EditItemClicked += (x, y) =>
                {
                    ResourceBase r = _objSpritesWindow.GetSelectedItem();

                    if (r as Sprite != null)
                    {
                        Sprite OldSpriteCopy = IdItemBase.CreateCopy(r as Sprite);

                        AddEditSprite d = new AddEditSprite(title, r as Sprite);
                        d.ShowDialog(() =>
                        {
                            if (d.DialogResult == true)
                            {
                                Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.EditSprite, OldSpriteCopy, d.Sprite));
                                _objSpritesWindow.RefreshEverything();
                            }
                        });
                    }
                };
                _objSpritesWindow.RemoveItemClicked += (x, y) =>
                {
                    ResourceBase r = _objSpritesWindow.GetSelectedItem();
                    if (r != null)
                    {
                        if (r as Sprite != null)
                        {
                            Globals.MainWindow.ProjectFile?.Sprites.Remove(r as Sprite);
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.DeleteSprite, r as Sprite, null));
                            _objSpritesWindow.RefreshEverything();
                        }
                    }
                };
            }
            _objSpritesWindow.Show();
        }

        private void CreateMapsWindow()
        {
            if (_objMapsWindow == null)
            {
                string title = "Map";
                _objMapsWindow = new CollectionEditor(title, () => { return Globals.MainWindow.ProjectFile?.Maps.ToList<ResourceBase>(); });
                _objMapsWindow.AddItemClicked += (x, y) =>
                {
                    MapEditor d = new MapEditor(title, null);
                    d.ShowDialog(() =>
                    {
                        if (d.DialogResult == true)
                        {
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.AddMap, null, d.TileMap));
                            _objMapsWindow.RefreshEverything();
                        }
                    });
                };
                _objMapsWindow.EditItemClicked += (x, y) =>
                {
                    ResourceBase r = _objMapsWindow.GetSelectedItem();

                    if (r as Sprite != null)
                    {
                        TileMap OldCopy = IdItemBase.CreateCopy(r as TileMap);

                        MapEditor d = new MapEditor(title, r as TileMap);
                        d.ShowDialog(() =>
                        {
                            if (d.DialogResult == true)
                            {
                                Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.EditMap, OldCopy, d.TileMap));
                                _objMapsWindow.RefreshEverything();
                            }
                        });
                    }
                };
                _objMapsWindow.RemoveItemClicked += (x, y) =>
                {
                    ResourceBase r = _objMapsWindow.GetSelectedItem();
                    if (r != null)
                    {
                        if (r as Sprite != null)
                        {
                            Globals.MainWindow.ProjectFile?.Maps.Remove(r as TileMap);
                            Globals.MainWindow.UndoManager.AddUndoItem(new UndoItem(UndoAction.DeleteMap, r as TileMap, null));
                            _objMapsWindow.RefreshEverything();
                        }
                    }
                };
            }
            _objMapsWindow.Show();
        }
        private void CreateToolbarWindow()
        {
            if (_objEditToolbarWindow == null)
            {
                _objEditToolbarWindow = new EditToolbarWindow();
            }

            //Hide menu buttons
            ControlTemplate template = _objEditToolbarWindow.Template;
            object myControl = (object)template.FindName("ToolWindow_CloseButton", _objEditToolbarWindow);
            // myControl.Visibility = Visibility.Hidden;
            int n = 0
                    ;
            n++;
        }




    }
}
