/**
*
*    @file UtilMeshBox.h
*    @date December 13, 2015
*    @author MetalMario971
*
*    © 2015 
*
*
*/
#pragma once
#ifndef __UTILMESHBOX_2662422974268991781110246_H__
#define __UTILMESHBOX_2662422974268991781110246_H__

#include "../model/UtilMesh.h"

namespace Game {
/**
*    @class UtilMeshBox
*    @brief
*
*/
class UtilMeshBox : public UtilMesh {
private:
    float _fSize;
    vec3 _vOffset;
    vec4 _vColor;
    bool _blnWireFrame;
    Box3f* _pCube;
public:
    UtilMeshBox(std::shared_ptr<GLContext> ctx, Box3f* pCube, vec3& vOffset, Color4f& vColor);
    virtual ~UtilMeshBox() override ;
    void setWireFrame(bool blnWireFrame) { _blnWireFrame = blnWireFrame; }

    OVERRIDES void generate();
    OVERRIDES void preDraw();
    OVERRIDES void postDraw();
};

}//ns game



#endif
