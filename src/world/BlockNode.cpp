#include "../base/Gu.h"
#include "../world/BlockNode.h"
#include "../world/BottleUtils.h"
#include "../world/PhysicsWorld.h"
#include "../world/WorldCell.h"
#include "../world/WorldGrid.h"
#include "../world/WorldCellFile.h"
#include "../world/WipGrid.h"
#include "../world/W25MeshMaker.h"

namespace BR2 {
BlockNode::BlockNode(std::shared_ptr<WorldGrid> pg) {
  _pGrid = pg;
  static int nNodes = 0;
  nNodes++;
  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    _iIndexCount[iMatter] = 0;
  }
}
BlockNode::~BlockNode() {
  if (_vecChildren != nullptr) {
    for (size_t i = 0; i < _vecChildren->size(); ++i) {
      if (_vecChildren->at(i) != nullptr) {
        delete _vecChildren->at(i);
      }
    }
    _vecChildren->resize(0);
    delete _vecChildren;
    _vecChildren = nullptr;
  }
  deleteCells();
}
void BlockNode::deleteCells() {
  if (_pCells != nullptr) {
    for (int i = 0; i < c_nCells; ++i) {
      if (_pCells[i] != nullptr) {
        delete _pCells[i];
      }
    }
    delete[] _pCells;
    _pCells = nullptr;
  }
}
void BlockNode::divide(t_timeval startTime, std::shared_ptr<WorldGrid> _pMyGlob, int32_t& nNodes, int32_t& nLeaves, int32_t treeLevel, Vec3f* ptPointToDivideTo, BlockNode** ptFoundNode, bool bCreate) {
  // This is an optional param
  if (ptPointToDivideTo != nullptr) {
    if (_box.containsInclusive(*ptPointToDivideTo) == false) {
      return;
    }
  }

  //cells x 2  = Prevents us from creating the final level  = 1 + 8 + 64 + 512 nodes.
  float divW = (BottleUtils::getCellWidth() * 2) + 0.001f;

  if (getBox()->getWidth() <= divW) {
    setIsLeaf();
    nLeaves++;
  }
  else {
    createChildren();

    unsigned int off;
    // - Divide again.
    for (int z = 0; z < 2; ++z)
    for (int y = 0; y < 2; ++y)
    for (int x = 0; x < 2; ++x) {
      // - Offset in the C8
      off = (unsigned int)((unsigned char)(x << 0) | (unsigned char)(y << 1) | (unsigned char)(z << 2));
      if (_vecChildren->at(off) != nullptr) {
        _vecChildren->at(off)->divide(startTime, _pMyGlob, nNodes, nLeaves, treeLevel + 1, ptPointToDivideTo, ptFoundNode, bCreate);
      }
    }

    nNodes += 8;
  }
}
uint32_t BlockNode::bitOff(uint32_t x, uint32_t y, uint32_t z) {
  return (uint32_t)(((uint32_t)x << 0) | ((uint32_t)y << 1) | ((uint32_t)z << 2));
}
void BlockNode::createChildren() {
  unsigned int off;

  if (_vecChildren == nullptr) {
    _vecChildren = new  std::vector<BlockNode*>();
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
    _vecChildren->push_back(nullptr);
  }

  vec3 parentOrigin = getBox()->getMin();
  vec3 childBoxSize;
  vec3 childOrigin;

  vec3 parentSize(getBox()->getWidth(), getBox()->getHeight(), getBox()->getDepth());
  childBoxSize.x = (parentSize.x * 0.5f);
  childBoxSize.y = (parentSize.y * 0.5f);
  childBoxSize.z = (parentSize.z * 0.5f);

  //otherwise, make children
  for (uint32_t z = 0; z < 2; ++z)
  for (uint32_t y = 0; y < 2; ++y)
  for (uint32_t x = 0; x < 2; ++x) {
    off = bitOff(x, y, z);

    if (_vecChildren->at(off) == nullptr) {
      _vecChildren->at(off) = new BlockNode(_pGrid);

      childOrigin.x = parentOrigin.x + childBoxSize.x * (float)x;
      childOrigin.y = parentOrigin.y + childBoxSize.y * (float)y;
      childOrigin.z = parentOrigin.z + childBoxSize.z * (float)z;

      _vecChildren->at(off)->getBox()->_min = childOrigin;
      _vecChildren->at(off)->getBox()->_max = childOrigin + childBoxSize;
    }
  }
}
void BlockNode::createCells(WorldCellFile* pFile, int32_t& iCell, World25GridGen::e eg,
  WipGrid* nf, std::shared_ptr<WorldGrid> pGrid, size_t(&nVerts)[GridMeshLayer::e::MaxMatters],
  int32_t& nCells) {

  if (getIsLeaf()) {

    deleteCells();
    _pCells = new WorldCell * [c_nCells];

    float cw2 = BottleUtils::getCellWidth() * 0.5f;
    float ch2 = BottleUtils::getCellHeight() * 0.5f;

    //Make all 8 cells.
    for (uint32_t zi = 0; zi < 2; ++zi)
    for (uint32_t yi = 0; yi < 2; ++yi)
    for (uint32_t xi = 0; xi < 2; ++xi) {
      int cOff = bitOff(xi, yi, zi);

      vec3 vp = getBox()->_min;

      vp.x += (float)(xi + 1) * cw2;
      vp.y += (float)(yi + 1) * ch2;
      vp.z += (float)(zi + 1) * cw2;

      vp = vp - pGrid->getOriginR3();
      ivec3 viPos = _pGrid->getWorld25()->v3Toi3CellLocal(vp);


      //#ifdef _DEBUG
      //            //Make sure no dupes
      //            for(WorldCell* ccc : cells) {
      //                if(ccc->getLocalPos() == viPos) {
      //                    Gu::debugBreak();
      //                }
      //            }
      //#endif

      _pCells[cOff] = makeCell(viPos.x, viPos.y, viPos.z, pFile, iCell, eg, nf, pGrid, nVerts);
      //cells.insert(_pCells[cOff]);
      nCells++;
    }

    //AssertOrThrow2(getCell() != nullptr);
  }
  else {
    if (getChildren() != nullptr) {
      for (size_t i = 0; i < getChildren()->size(); ++i) {
        if (getChildren()->at(i) != nullptr) {
          getChildren()->at(i)->createCells(pFile, iCell, eg, nf, pGrid, nVerts, nCells);
        }
        else {
          int nnn = 0;
          nnn++;
        }
      }
    }
  }

}

WorldCell* BlockNode::makeCell(int xx, int yy, int zz, WorldCellFile* pFile,
  int32_t& iCell, World25GridGen::e eg, WipGrid* nf, std::shared_ptr<WorldGrid> pGrid,
  size_t(&nVerts)[GridMeshLayer::e::MaxMatters]) {

  WorldCell* pRet = nullptr;

  if (eg == World25GridGen::e::GenAndSave) {
    pRet = new WorldCell(xx, yy, zz, this, 0, 0);
    nf->makeCell(xx, yy, zz, pRet);
  }
  else if (eg == World25GridGen::e::Load) {
    //Add to the post init data.
    WorldCellData* pd = pFile->getCells()[iCell];
    pRet = new WorldCell(xx, yy, zz, this, pd->_iSolid, pd->_iLiquid);
  }
  else {
    BroThrowNotImplementedException();
  }

  for (int iMatter = 0; iMatter < GridMeshLayer::e::MaxMatters; ++iMatter) {
    W25Geom cg = pRet->getGeom((GridMeshLayer::e)iMatter);
    nVerts[iMatter] += pGrid->getWorld25()->getMeshMaker()->getNumVertsForGeom(cg);
  }


  iCell++;

  return pRet;
}
void BlockNode::consolidate(bool& bEmpty, int32_t& nCells, int32_t& nDeleted, int32_t& nNodes) {
  if (getIsLeaf()) {
    if (_pCells != nullptr) {
      for (int ci = 0; ci < c_nCells; ++ci) {
        bool bCellEmpty = true;
        if (_pCells[ci] != nullptr) {
          for (int i = 0; i < GridMeshLayer::e::MaxMatters; ++i) {
            W25Geom g = _pCells[ci]->getGeom((GridMeshLayer::e)i);
            if (g != W25GEOM_EMPTY) {
              //We have data in this cell.
              bCellEmpty = false;
              break;
            }
          }

          if (bCellEmpty == false) {
            nCells++;
          }
          else {
            delete _pCells[ci];
            _pCells[ci] = nullptr;
          }
        }

        bEmpty = bEmpty && bCellEmpty;
      }
      if (bEmpty == true) {
        //delete the whole array.
        delete[] _pCells;
        _pCells = nullptr;
      }
    }
  }
  else {
    if (getChildren() != nullptr) {
      bool bAllChildrenEmpty = true;

      for (size_t iChild = 0; iChild < getChildren()->size(); ++iChild) {
        BlockNode* pChild = getChildren()->at(iChild);

        if (pChild != nullptr) {
          bool bChildEmpty = true;
          pChild->consolidate(bChildEmpty, nCells, nDeleted, nNodes);

          if (bChildEmpty == false) {
            bAllChildrenEmpty = false;
          }
          else {
            delete pChild;
            getChildren()->at(iChild) = nullptr;
            nDeleted++;
          }
        }
      }

      bEmpty = bAllChildrenEmpty;
      if (bEmpty == false) {
        nNodes++;
      }
    }
  }



}

void BlockNode::getCellForPoint(vec3& pt, WorldCell*& __out_ ret) {
  //**Note: this function could be optimzized by snapping PT to the cell grid and looking upt he index.
  //Loads faster
  if (ret != nullptr) {
    return;
  }
  if (getBox()->containsBottomLeftInclusive(pt)) {
    if (getIsLeaf()) {
      Box3f bx;
      if (_pCells != nullptr) {
        for (int ci = 0; ci < BlockNode::c_nCells; ++ci) {
          WorldCell* c = getCell(ci);
          if (c != nullptr) {
            c->getBoundBoxBvh(bx);
            if (bx.containsBottomLeftInclusive(pt)) {
              ret = c;
              break;
            }
          }
        }
      }
    }
    else {
      if (getChildren() != nullptr) {
        for (size_t iChild = 0; iChild < getChildren()->size(); ++iChild) {
          if (getChildren()->at(iChild) != nullptr) {
            getChildren()->at(iChild)->getCellForPoint(pt, ret);
            if (ret != nullptr) {
              return;
            }
            //getCellForPoint_r(pt, pParent->getChildren()->at(iChild), ret);
          }
        }
      }

    }
  }
}
void BlockNode::raycastCells(Ray_t* pr, std::multimap<float, WorldCell*>& outCells) {
  //return ALL cells that the ray hits * no easy out *
  if (getBox()->RayIntersect_EasyOut(pr)) {
    if (getIsLeaf()) {

      Box3f bx;
      for (int ci = 0; ci < BlockNode::c_nCells; ++ci) {
        WorldCell* c = getCell(ci);
        if (c != nullptr) {
          c->getBoundBoxBvh(bx);
          if (bx.RayIntersect_EasyOut(pr)) {
            outCells.insert(std::make_pair(pr->getTime(), c));
          }
        }
      }
    }
    else {
      if (getChildren() != nullptr) {
        for (size_t iChild = 0; iChild < getChildren()->size(); ++iChild) {
          if (getChildren()->at(iChild) != nullptr) {
            getChildren()->at(iChild)->raycastCells(pr, outCells);
          }
        }
      }
    }
  }
}


}//ns Game
