/**
*  @file PhysicsGrid.h
*  @date January 4, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __CELLNODE_15150857494221655763_H__
#define __CELLNODE_15150857494221655763_H__

#include "../world/WorldHeader.h"

namespace BR2 {
/**
*  @class PhysicsGrid
*  @brief This is a cell in a cell grid managed by the physics system. A box in 3D space which is a "cell" that physics uses to optimize collisions.
*/
class PhysicsGrid : public VirtualMemoryShared<PhysicsGrid> {
public:
  PhysicsGrid(std::shared_ptr<PhysicsWorld> pw, const ivec3& viPos, float fNodeWidth, float fNodeHeight, bool bEmpty);
  virtual ~PhysicsGrid() override;

  std::shared_ptr<PhysicsGrid> getNeighbor(int iSide);
  Box3f* getBoundBox() { return _pBoundBox; }
  std::unique_ptr<NodeManifold>& getManifold() { return _pManifold; }
  const ivec3& getGridPos() { return _viPos; }
  std::shared_ptr<PhysicsGrid> getNeighborOffset(float off_x, float off_y, float off_z);
  bool canDelete();
  virtual bool getIsGenerating() { return false; }
  vec3 getNodeCenterR3();
  vec3 getOriginR3();
  bool isEmpty() { return _bEmpty; }

  virtual std::shared_ptr<MeshNode> getMesh() { return nullptr; }
  static PhysicsGridSide::e getOppNeighborIndex(PhysicsGridSide::e iNeighbor);
  static const int c_nNeighbors = 6;

private:
  //std::shared_ptr<PhysicsGrid> getThis() { return shared_from_this(); }
  ivec3 _viPos;
  Box3f* _pBoundBox = nullptr;
  std::unique_ptr<NodeManifold> _pManifold = nullptr;
  float _fNodeWidth, _fNodeHeight;
  std::shared_ptr<PhysicsWorld> _pPhysicsWorld = nullptr;
  std::shared_ptr<PhysicsGrid> _pNeighbor[c_nNeighbors];
  bool _bEmpty = false;

  void linkToGrid(std::shared_ptr<PhysicsGrid> pNeighbor, PhysicsGridSide::e eNeighbor);
  void unsetGridNeighbor(PhysicsGridSide::e eSide);
  void setGridNeighbor(std::shared_ptr<PhysicsGrid> pNeighbor, PhysicsGridSide::e eNeighbor);

protected:
  void linkGrids();
  void unlinkGrids();
};

}//ns Game



#endif
