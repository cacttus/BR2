#include "../model/OBB.h"
#include "../model/UtilMeshInline.h"


namespace Game {
OBB::OBB() {
}
OBB::~OBB() {
}
void OBB::calc(mat4& mat, const Box3f* base) {
  //calc transform.
  base->getPoints(_verts, 8);
  for (int i = 0; i < 8; ++i) {
    vec3 v = mat * vec4(_verts[i], 1);
    _verts[i] = v;
  }
  _bInvalid = false;
}
//void OBB::drawForward(UtilMeshInline& mi) {
//    vec3* v = _verts; //shorthand
//
//    mi.vt2(v[0], v[1]); //Back
//    mi.vt2(v[2], v[3]);
//    mi.vt2(v[0], v[2]);
//    mi.vt2(v[1], v[3]);
//
//    mi.vt2(v[4], v[5]); // Front
//    mi.vt2(v[6], v[7]);
//    mi.vt2(v[4], v[6]);
//    mi.vt2(v[5], v[7]);
//
//    mi.vt2(v[0], v[4]); //Connectors
//    mi.vt2(v[1], v[5]);
//    mi.vt2(v[2], v[6]);
//    mi.vt2(v[3], v[7]);
//
//}








}//ns Game
