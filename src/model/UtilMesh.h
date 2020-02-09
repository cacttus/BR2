/**
*
*    @file UtilMesh.h
*    @date November 21, 2015
*    @author MetalMario971
*
*    © 2015 
*
*
*/
#pragma once
#ifndef __UTILMESH_12100900221286185268356_H__
#define __UTILMESH_12100900221286185268356_H__


#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
/**
*    @class UtilMesh
*    @brief
*
*/
class UtilMesh : public VirtualMemory {
private:
    std::shared_ptr<GLContext> _pContext = nullptr;
    std::shared_ptr<VertexFormat> _pVertexFormat = nullptr;
    GLuint bdVerts, bdIndexes, vaoIndexes;
    bool _bInitialized = false;

    GLenum _eDrawMode = GL_LINES;

    mat4 _m4ModelMatrix;
    std::shared_ptr<ShaderBase> _pShader = nullptr;
    std::shared_ptr<FragmentBufferData> _pVerts = nullptr; //Note if using a spec make sure to set vertexes and indexes to NULL in post draw so you don't delete the spec data.
    std::shared_ptr<IndexBufferData> _pIndexes = nullptr;
    std::shared_ptr<VaoDataGeneric> _pVaoData = nullptr;
    std::shared_ptr<VaoDataGeneric> getVaoData() { return _pVaoData; }
protected:
    void copyBuffersToVao();
    void copyFromSpec(std::shared_ptr<MeshData> sp);
    std::shared_ptr<IndexBufferData> getIndexes() { return _pIndexes; }
    std::shared_ptr<FragmentBufferData> getVerts() { return _pVerts; }
    void setDrawMode(GLenum e) { _eDrawMode = e; }
    //void allocVerts(int count, std::shared_ptr<VertexFormat> fmt);
    //void allocIndexes(int count);
    void allocData(int nVerts, int nIndexes, std::shared_ptr<VertexFormat> fmt);
    void cleanup();
    std::shared_ptr<GLContext> getContext() { return _pContext; }
public:
    UtilMesh(std::shared_ptr<GLContext> ctx, std::shared_ptr<VertexFormat>, std::shared_ptr<ShaderBase> pShader, GLenum drawMode = GL_LINES);
    virtual ~UtilMesh() override;

    void setModelMatrix(mat4& m) { _m4ModelMatrix = m; }

    MUST_OVERRIDE void generate()=0;
    MUST_OVERRIDE void preDraw()=0;
    MUST_OVERRIDE void postDraw()=0;
    virtual std::shared_ptr<ShaderBase> getShader();

    void init();
    void draw();

    //+-- CTOR/DTOR --+
;
};

}//ns game



#endif
