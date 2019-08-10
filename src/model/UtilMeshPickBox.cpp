//
////#include "../mem/mm_allocator.h"
////#include "../base/GlobalUtils.h"
////#include "../gfx/UtilMeshPickBox.h"
////#include "../topo/MeshCreator.h"
////#include "../topo/MeshCreator.h"
////#include "../topo/MeshManager.h"
////#include "../topo/MeshSpec.h"
////#include "../gpu/GLShaderManager.h"
////#include "../gpu/GlslShaderBase.h"
////#include "../gpu/GlslShaderUniformVariables.h"
////#include "../gpu/GlslUniformVariable.h"
////#include "../topo/BoxMesh.h"
//
//namespace Game {
//
//UtilMeshPickBox::UtilMeshPickBox(Cube3f* pCube, uint32_t uiColorId):
//UtilMesh(GL_TRIANGLES)
//, _blnWireFrame(true)
//, _pCube(pCube)
//, _uiColorId(uiColorId)
//{
//    //wireframe isn't getting unloaded. 
//}
//UtilMeshPickBox::~UtilMeshPickBox()
//{
//}
////////////////////////////////////////////////////////////////////////////
//TRef(GlslShaderBase) UtilMeshPickBox::getShader()
//{
//    return SafeCast(Gu::GetShaderManager(), GLShaderManager*)->getDefaultPickShader().safeCast<GlslShaderBase>();
//}
//void UtilMeshPickBox::generate()
//{
//    vec3 off(0,0,0);
//    
//    //Note: to copy everything is too slow.  We will just keep the spec here and delete it when done
//    _pSpec = Gu::GetMeshManager()->getMeshCreator()->generate_box(_pCube, &_vColor, &off).safeCast<MeshSpec>();
//    setVertexType(_pSpec->getVertexFormatType());
//    
//    // These are kind of a hack
//    _verts = _pSpec->getFrags(); 
//    _indexes = _pSpec->getIndices();
//}
//void UtilMeshPickBox::preDraw()
//{
//    _verts = _pSpec->getFrags();
//    _indexes = _pSpec->getIndices();
//    GlslUniformVariable* ufId = getShader()->getUniform(TStr("_ufInteger_i0"));
//    if(ufId!=NULL)
//        ufId->setData((void*)&_uiColorId);
//}
//void UtilMeshPickBox::postDraw()
//{
//    //CRITICAL we set these to null.
//    _verts = NULL;
//    _indexes = NULL;
//}
//
//}//ns game
