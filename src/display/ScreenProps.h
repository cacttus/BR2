/**
*
*    @file ScreenProps.h
*    @date March 5, 2012
*    @author Derek Page
*
*    © 2012 
*
*    Render properties of the window.
*/
#pragma once
#ifndef __SCREENPROPS_751578542347261859456_H__
#define __SCREENPROPS_751578542347261859456_H__


#include "../display/DisplayHeader.h"

namespace Game {

// - This enum tells the window what to do with the rendering viewport
//    when the size changes.
typedef enum {
    VP_DONOTCHANGE,    // - The viewport will stay the same as it did from startup
    VP_MAINTAIN_ASPECT_RATIO,    // - The aspect ratio will maintain, but the render target will change size.
    VP_FILL_WINDOW,    // - The aspect ratio changes to fill the entire window. (this will distort objects in the scene)
}ViewportConstraint;

// - Location of the viewport when window is resized
// This does not apply to WR_FILL_WINDOW
typedef enum {
    // val            aspect ratio
    VIEWPORT_TOPLEFTCORNER,
    VIEWPORT_CENTER_V,    // - Centers the viewport vertically
    VIEWPORT_CENTER_H,    // - Centers the viewport horizontally
    VIEWPORT_CENTER_HV,    // - Centers the viewport horizontally and vertically
    VIEWPORT_CENTER_NONE,
}VIEWPORT_LOCATION;

// - Screen Resolution
typedef enum {
    // val            aspect ratio
    res_800x600,    //4:3
    res_1024x768,    //4:3
    res_1280x1024,    //5:4

    res_1152x864,    //4:3
    res_1280x960,    //4:3

    res_1600x900,    //16:9

    res_1680x1050,    //8:5

    res_custom,        // custom
    
    screen_res_enum_limit
}t_screen_resolution;

}//ns game



#endif
