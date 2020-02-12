#include "../base/GLContext.h"
#include "../model/UtilMeshGrid.h"
#include "../model/FragmentBufferData.h"
#include "../model/IndexBufferData.h"
#include "../model/VaoDataGeneric.h"

namespace Game {
UtilMeshGrid::UtilMeshGrid(std::shared_ptr<GLContext> pc,
  float r, float g, float b,
  int32_t nSlices,
  float fSliceWidth,
  Vector3& center
) :
  UtilMesh(pc, v_v3c4::getVertexFormat(), nullptr),
  _nSlices(nSlices),
  _fSliceWidth(fSliceWidth),
  _center(center),
  _color(r, g, b, 1.0f),
  _lineWidth(1.0f) {
}
UtilMeshGrid::~UtilMeshGrid() {
}
void UtilMeshGrid::generate() {
  float gridWidth_2 = _nSlices * _fSliceWidth / 2.0f;

  //Allcate and generate verts
  int n = (_nSlices + 1) * 2 * 2;

  allocData(n, n, v_v3c4::getVertexFormat());

  int32_t curInd = 0;
  for (int32_t i = 0; i < _nSlices + 1; ++i) {
    getVerts()->v3f(curInd).x = _center.x - (gridWidth_2);
    getVerts()->v3f(curInd).y = _center.y - 0;
    getVerts()->v3f(curInd).z = _center.z - (gridWidth_2)+(_fSliceWidth * (float)i);
    getVerts()->c4f(curInd) = _color;
    getIndexes()->i32(curInd) = curInd;
    curInd++;

    getVerts()->v3f(curInd).x = _center.x + (gridWidth_2);
    getVerts()->v3f(curInd).y = _center.y - 0;
    getVerts()->v3f(curInd).z = _center.z - (gridWidth_2)+(_fSliceWidth * (float)i);
    getVerts()->c4f(curInd) = _color;
    getIndexes()->i32(curInd) = curInd;
    curInd++;
  }
  for (int32_t i = 0; i < _nSlices + 1; ++i) {
    getVerts()->v3f(curInd).x = _center.x - (gridWidth_2)+(_fSliceWidth * (float)i);
    getVerts()->v3f(curInd).y = _center.y - 0;
    getVerts()->v3f(curInd).z = _center.z - (gridWidth_2);
    getVerts()->c4f(curInd) = _color;
    getIndexes()->i32(curInd) = curInd;
    curInd++;

    getVerts()->v3f(curInd).x = _center.x - (gridWidth_2)+(_fSliceWidth * (float)i);
    getVerts()->v3f(curInd).y = _center.y - 0;
    getVerts()->v3f(curInd).z = _center.z + (gridWidth_2);
    getVerts()->c4f(curInd) = _color;
    getIndexes()->i32(curInd) = curInd;
    curInd++;
  }

  copyBuffersToVao();
}
void UtilMeshGrid::preDraw() {
  getContext()->setLineWidth(_lineWidth);
}
void UtilMeshGrid::postDraw() {
}


}//ns game
