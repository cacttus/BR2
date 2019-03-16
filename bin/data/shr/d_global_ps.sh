//20151126
#include "d_hdr_base.sh"

// Global deferred MRT targets
layout(location = 0) out vec4 _gPositionOut; 
layout(location = 1) out vec4 _gColorOut;
layout(location = 2) out vec4 _gNormalOut;
layout(location = 3) out vec4 _gPlaneOut;
layout(location = 4) out uint _gPickOut;

//*Pick Id
uniform uint _ufPickId;

#include "df_material.sh"

//void setBloom()
//{
//    //Apply a default bloom on a per color basis/
//    //this is incorrect, but demonstrates global bloom
//    float fBloomLevel = 0.2195;
//    
//    //transform color back to linar space.
//    vec4 tmColor = toneMapLightFinal(_gColorOut);
//    
//    float sum = (tmColor.r + tmColor.g + tmColor.b) / 3.0f;
//    if(sum > fBloomLevel)
//    {
//        _gBloomOut = tmColor;
//        _gBloomOut.a = 1.0f;
//    }
//    else
//    {
//        _gBloomOut = vec4(0,0,0,0);
//    }
//}