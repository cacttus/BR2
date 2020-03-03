#include "../base/GLContext.h"
#include "../model/UtilMeshAxis.h"
#include "../model/VertexFormat.h"
#include "../model/FragmentBufferData.h"
#include "../model/IndexBufferData.h"
namespace BR2 {
UtilMeshAxis::UtilMeshAxis(std::shared_ptr<GLContext> ctx, float scale, float lineWidth, mat4& xform) :
  UtilMesh(ctx, v_v3c4x2::getVertexFormat(), nullptr) {
  _scale = scale;
  _lineWidth = lineWidth;
  _mTransform = xform;
}
UtilMeshAxis::~UtilMeshAxis() {
}
void UtilMeshAxis::generate() {
  int n = 6;
  float s = _scale;

  allocData(n, n, v_v3c4x2::getVertexFormat());

  getVerts()->v3f(0) = _mTransform * vec4(0, 0, 0, 1);
  getVerts()->c4f(0) = Color4f(1, 0, 0, 1);
  getVerts()->v3f(1) = _mTransform * vec4(s, 0, 0, 1);
  getVerts()->c4f(1) = Color4f(1, 0, 0, 1);
  getVerts()->v3f(2) = _mTransform * vec4(0, 0, 0, 1);
  getVerts()->c4f(2) = Color4f(0, 1, 0, 1);
  getVerts()->v3f(3) = _mTransform * vec4(0, s, 0, 1);
  getVerts()->c4f(3) = Color4f(0, 1, 0, 1);
  getVerts()->v3f(4) = _mTransform * vec4(0, 0, 0, 1);
  getVerts()->c4f(4) = Color4f(0, 0, 1, 1);
  getVerts()->v3f(5) = _mTransform * vec4(0, 0, s, 1);
  getVerts()->c4f(5) = Color4f(0, 0, 1, 1);

  for (int i = 0; i < 6; ++i) {
    getIndexes()->i32(i) = i;
  }

  copyBuffersToVao();


}
void UtilMeshAxis::preDraw() {
  getContext()->setLineWidth(_lineWidth);

  //Gd::setLineWidth(_lineWidth);
}
void UtilMeshAxis::postDraw() {
}


}//ns game
