#include "../base/GrowBuffer.h"
#include "../model/UtilMeshInline.h"
#include "../model/IndexBufferData.h"
#include "../model/FragmentBufferData.h"

namespace BR2 {
UtilMeshInline::UtilMeshInline(std::shared_ptr<GLContext> ctx) :
  UtilMesh(ctx, v_v3c4::getVertexFormat(), nullptr, GL_NONE) //The prim type will begin as invalid.
{
  _vDefaultColor.construct(1, 0, 0, 1);
}
UtilMeshInline::~UtilMeshInline() {
  _pBuf.resize(0);
}
void UtilMeshInline::begin(GLenum type) {
  _pBuf.resize(0);
  setDrawMode(type);
}
void UtilMeshInline::vt1(v_v3c4& v1) {
  _curVert = v1;
  pushCurVert();
}
void UtilMeshInline::vt2(v_v3c4& v1, v_v3c4& v2) {
  vt1(v1);
  vt1(v2);
}
void UtilMeshInline::vt2(vec3& v1, vec3& v2, vec4* color) {
  v_v3c4 a, b;
  a.v = v1;
  a.c = color ? *color : _vDefaultColor;
  b.v = v2;
  b.c = color ? *color : _vDefaultColor;

  vt1(a);
  vt1(b);
}
void UtilMeshInline::pushCurVert() {
  _pBuf.push_back(_curVert);
  _curVert.c.zero();
  _curVert.v.zero();
}
void UtilMeshInline::generate() {
  allocData((int)_pBuf.size(), (int)_pBuf.size(), v_v3c4::getVertexFormat());

  for (size_t i = 0; i < _pBuf.size(); ++i) {
    getVerts()->v3f(i) = _pBuf.at(i).v;
    getVerts()->c4f(i) = _pBuf.at(i).c;

    getIndexes()->i32(i) = (v_index32)i;
  }

  copyBuffersToVao();
}
void UtilMeshInline::end(std::shared_ptr<CameraNode> cam) {
  init();
  draw(cam);
}
void UtilMeshInline::addBox(Box3f* b, vec4* color) {
  vec3 p[8];
  b->getPoints(p, 8);
  addBox(p, color);
}
void UtilMeshInline::addBox(vec3* points, vec4* color) {
  vec3* v = points; //shorthand

  vt2(v[0], v[1], color); //Back
  vt2(v[2], v[3], color);
  vt2(v[0], v[2], color);
  vt2(v[1], v[3], color);

  vt2(v[4], v[5], color); // Front
  vt2(v[6], v[7], color);
  vt2(v[4], v[6], color);
  vt2(v[5], v[7], color);

  vt2(v[0], v[4], color); //Connectors
  vt2(v[1], v[5], color);
  vt2(v[2], v[6], color);
  vt2(v[3], v[7], color);
}

//////////////////////////////////////////////////////////////////////////
UtilMeshInline2d::UtilMeshInline2d(std::shared_ptr<GLContext> ctx) :
  UtilMesh(ctx, v_v2c4::getVertexFormat(), nullptr, GL_NONE) //The prim type will begin as invalid.
{
  _vDefaultColor.construct(1, 0, 0, 1);
}
UtilMeshInline2d::~UtilMeshInline2d() {
  _pBuf.resize(0);
}
///////////////////////////////////////////////////////////////////
void UtilMeshInline2d::begin(GLenum type) {
  _pBuf.resize(0);
  setDrawMode(type);
}
void UtilMeshInline2d::vt1(v_v2c4& v1) {
  _curVert = v1;
  pushCurVert();
}
void UtilMeshInline2d::vt2(vec2& v1, vec2& v2, vec4* color) {
  v_v2c4 a, b;
  a.v = v1;
  a.c = color ? *color : _vDefaultColor;
  b.v = v2;
  b.c = color ? *color : _vDefaultColor;

  vt1(a);
  vt1(b);
}
void UtilMeshInline2d::pushCurVert() {
  _pBuf.push_back(_curVert);
  _curVert.c.zero();
  _curVert.v.zero();
}
void UtilMeshInline2d::generate() {
  allocData((int)_pBuf.size(), (int)_pBuf.size(), v_v2c4::getVertexFormat());

  for (size_t i = 0; i < _pBuf.size(); ++i) {
    getVerts()->v2f(i) = _pBuf.at(i).v;
    getVerts()->c4f(i) = _pBuf.at(i).c;

    getIndexes()->i32(i) = (v_index32)i;
  }

  copyBuffersToVao();
}
void UtilMeshInline2d::end(std::shared_ptr<CameraNode> cam) {
  init();
  draw(cam);
}





}//ns Game
