using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Dragablz;
using MahApps.Metro;

namespace GameHost
{
    public enum EditStatus
    {
        Unchanged,
        Modified
    }

    public class ToolWindowBase : MahApps.Metro.Controls.MetroContentControl
    {
        // public string Title = "";
        public bool? DialogResult=null;

        public EditStatus EditStatus { get; set; }

        public Action AfterShowDialog = null;
        public int WindowId { get; set; }
        static int WindowIdGen = 1;

        //TODO: implement these things
        //public SizeToContent SizeToContent { get; set; } = SizeToContent.Manual;
        //public ResizeMode ResizeMode { get; set; } = ResizeMode.CanResize;
        public ToolWindowBase()
        {
            WindowId = WindowIdGen++;

            //**Must be set to true
            AllowDrop = true;
            this.DragEnter += (x, e) =>
            {
                int n = 0;
                n++;

            };
            this.Drop += (x, e) =>
            {
                int n = 0;
                n++;
            };
        }


        #region XAML Editor (Visible Properties)
        public static readonly DependencyProperty _dTitle = DependencyProperty.Register(@"Title", typeof(string), typeof(ToolWindowBase));
        public static readonly DependencyProperty _dSizeToContent = DependencyProperty.Register(@"SizeToContent", typeof(SizeToContent), typeof(ToolWindowBase));
        public static readonly DependencyProperty _dResizeMode = DependencyProperty.Register(@"ResizeMode", typeof(ResizeMode), typeof(ToolWindowBase));
       // public static readonly DependencyProperty _dIcon = DependencyProperty.Register(@"Icon", typeof(Image), typeof(ToolWindowBase));

        public string Title
        {
            get { return (string)GetValue(_dTitle); }
            set { SetValue(_dTitle, value); }
        }
        public SizeToContent SizeToContent
        {
            get { return (SizeToContent)GetValue(_dSizeToContent); }
            set { SetValue(_dSizeToContent, value); }
        }
        public ResizeMode ResizeMode
        {
            get { return (ResizeMode)GetValue(_dResizeMode); }
            set { SetValue(_dResizeMode, value); }
        }
        //public ResizeMode Icon
        //{
        //    get { return (ResizeMode)GetValue(_dIcon); }
        //    set { SetValue(_dIcon, value); }
        //}
        #endregion

        public virtual bool OkButtonClicked()
        {
            DialogResult = true;
            Close();
            return true;
        }
        public virtual bool CancelButtonClicked()
        {
            DialogResult = false;
            Close();
            return true;
        }

        public virtual bool OnClose()
        {
            //Return false to prevent hte window from closing, and doing some other logic
            return true;
        }

        public void Show()
        {
            Globals.MainWindow.WindowManager.ShowToolWindow(Title, this, false);
        }
        public void ShowDialog(Action afterShowDialog)
        {
            AfterShowDialog = afterShowDialog;
            Globals.MainWindow.WindowManager.ShowToolWindow(Title, this, true, afterShowDialog);
        }
        public void Hide()
        {
            Globals.MainWindow.WindowManager.HideToolWindow(this.WindowId);
        }
        public void Close()
        {
            Globals.MainWindow.WindowManager.CloseToolWindow(this.WindowId);
        }
        protected virtual List<string> Validate() { return new List<string>(); }

        protected bool SaveOnly(Func<bool> postValidate)
        {
            try
            {
                List<string> results = Validate();
                if (results.Count > 0)
                {
                    string msg = "Please correct the following errors: " + Environment.NewLine + results.Aggregate((x, y) => x + Environment.NewLine + y) + Environment.NewLine;
                    Globals.ShowInfo(msg);
                    return false;
                }
                else
                {
                    return postValidate();
                }
            }
            catch (Exception ex)
            {
                Globals.MainWindow.LogError(ex.ToString());
                return false;
            }
        }

        protected void SaveAndClose(Func<bool> postValidate)
        {
            if (SaveOnly(postValidate))
            {
                Close();
            }
        }


    }
}
