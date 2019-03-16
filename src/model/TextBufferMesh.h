/**
*
*    @file TextBufferMesh.h
*    @date April 14, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __TEXTBUFFERMESH_1492206006611577427_H__
#define __TEXTBUFFERMESH_1492206006611577427_H__


#include "../base/BaseHeader.h"
#include "../display/DisplayHeader.h"
#include "../model/ModelHeader.h"
#include "../math/MathAll.h"
namespace Game {;
class VaoDataGeneric;
class TextBufferMesh : public VirtualMemory {
protected:
    std::vector<v_v2c4x2> _verts;
    std::vector<v_index32> _indexes;
    std::shared_ptr<GLContext> _pContext = nullptr;
    std::shared_ptr<VaoDataGeneric> _pVaoData = nullptr;

    int _iCurrentQuadIndex = 0;
    int32_t _iMaxQuads = 0;

    void assignIndexes();
    void freeData();
public:
    std::shared_ptr<VaoDataGeneric> getVao() {
        return _pVaoData;
    }
    TextBufferMesh(
        std::shared_ptr<GLContext> pContext,
        int32_t count
    );
    virtual ~TextBufferMesh() override;
    //void setTexture(std::shared_ptr<Texture2DSpec> tex);
    FORCE_INLINE int32_t getMaxQuads() { return _iMaxQuads; }
    FORCE_INLINE int32_t getQuadCount() { return _iCurrentQuadIndex; }
    FORCE_INLINE void resetCopy() { _iCurrentQuadIndex = 0; }
    FORCE_INLINE void backupQuad()
    {
        AssertOrThrow2(_iCurrentQuadIndex>0);
        _iCurrentQuadIndex--;
    }
    FORCE_INLINE void nextQuad()
    {
        if (_iCurrentQuadIndex < _iMaxQuads) {
            _iCurrentQuadIndex++;
        }
    }
    bool isFull() {
        return _iCurrentQuadIndex >= _iMaxQuads;
    }

    void getQuad(v_v2c4x2*& v0, v_v2c4x2*& v1, v_v2c4x2*& v2, v_v2c4x2*& v3);    // Gets the next client side GPU quad.  After setting the quad call nextQuad().  then copyToGpu() then draw() then resetCopy()
    void allocateQuads(int32_t count);    //Allocates count quads
                                          //  void createShader(t_string& srcName);
    void copyToGpu(bool bInit = false);// don't unnecessarily copy the indexes.  leave count -1 to copy all quads.

    void draw(RenderParams& rp);

};
}//ns Game



#endif
