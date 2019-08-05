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
    /// Interaction logic for AddEditFrame.xaml
    /// </summary>
    public partial class AddEditFrame : AddEditBase
    {
        public Frame Frame = null;
        public AddEditFrame(string title, Frame s = null) : base( title)
        {
            InitializeComponent();

            if (s == null)
            {
                AddEditMode = AddEditMode.Add;
                Frame = new Frame(Globals.MainWindow.ProjectFile.GenId());
            }
            else
            {
                AddEditMode = AddEditMode.Edit;
                Frame = s;
            }


        }
    }
}
