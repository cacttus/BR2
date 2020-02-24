#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../model/UtilMeshSphere.h"
#include "../model/FragmentBufferData.h"
#include "../model/MeshUtils.h"
#include "../model/MeshSpec.h"

namespace BR2 {
UtilMeshSphere::UtilMeshSphere(std::shared_ptr<GLContext> ctx, float radius, vec3& vOffset, vec4& vColor, int32_t nSlices, int32_t nStacks) :
  UtilMesh(ctx, MeshUtils::MeshMakerVert::getVertexFormat(), nullptr, GL_TRIANGLES)
  , _fRadius(radius)
  , _vOffset(vOffset)
  , _vColor(vColor)
  , _blnWireFrame(true)
  , _nSlices(nSlices)
  , _nStacks(nStacks) {
  //wireframe isn't getting unloaded. 
}
UtilMeshSphere::~UtilMeshSphere() {

}
void UtilMeshSphere::generate() {
  //Note: to copy everything is too slow.  We will just keep the spec here and delete it when done
  _pSpec = MeshUtils::makeSphere(_fRadius, _nSlices, _nStacks, &_vColor, &_vOffset, !_blnWireFrame);
  copyFromSpec(_pSpec);
  _pSpec = nullptr;
  //  DEL_MEM(_pSpec);
}
void UtilMeshSphere::preDraw() {
  if (_blnWireFrame == true) {
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    glLineWidth(1.0f);
    //Gu::getGraphicsContext()->setLineWidth(1.0f);
  }
  //Gd::pushPolygonMode(GfxPolygonMode::PolygonModeWireframe);

}
void UtilMeshSphere::postDraw() {
  if (_blnWireFrame == true) {
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
  }
}

}//ns game
