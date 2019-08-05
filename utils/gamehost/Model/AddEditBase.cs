using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost
{
    public enum AddEditMode
    {
        Add, Edit
    }
    public class AddEditBase : ToolWindowBase
    {
        private string _strBaseTitle = "";
        private AddEditMode _mode = AddEditMode.Add;
        public AddEditMode AddEditMode
        {
            get
            {
                return _mode;
            }
            set
            {
                _mode = value;
                UpdateTitle();
            }
        }
        public AddEditBase(string title)
        {
            _strBaseTitle = title;
            Title = title;
        }
        private void UpdateTitle()
        {
            if (AddEditMode == AddEditMode.Add)
            {
                Title = "Add " + _strBaseTitle;
            }
            else
            {
                Title = "Edit " + _strBaseTitle;
            }

        }
    }
}
