using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameHost.Model
{
    //We are going to Mirror the UI seen in Krita
        public enum DockLocation
        {
            Top, Left, Right, Bottom, Center
        }
        public class DockTabContainer
        {
            //For the main grid container this is only on the edges (tabs) just like Krita. 
            public List<DockLocation> AvailableDockLocations { get; set; }

            //We have a tab,
            //Underneath tab we have a single dockpanel row that has 2 buttons
            // 1 button to float window, 1 button to close it
        }
    public class FloatingWindow
    {

    }


}
