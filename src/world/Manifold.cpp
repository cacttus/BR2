#include "../base/Gu.h"
#include "../world/Manifold.h"


namespace Game {
void NodeManifold::remove(std::shared_ptr<PhysicsNode> ob) {
  std::set<std::shared_ptr<PhysicsNode>>::iterator obit;
  obit = getAll()->find(ob);
  if (obit != getAll()->end()) {
    getAll()->erase(obit);
  }
  else {
    Gu::debugBreak();
  }

  //This is a possible optimization for later.
//  if (ob->getPhy()->getIsAtRest()) {
//      obit = getResting()->find(ob);
//      if (obit != getResting()->end()) {
//          getResting()->erase(obit);
//      }
//      else {
//          Gu::debugBreak();
//      }
//  }
//  else {
//      obit = getActive()->find(ob);
//      if (obit != getActive()->end()) {
//          getActive()->erase(obit);
//      }
//      else {
//          Gu::debugBreak();
//      }
//  }

}
void NodeManifold::add(std::shared_ptr<PhysicsNode> ob) {
  std::set<std::shared_ptr<PhysicsNode>>::iterator obit;
  //#ifdef _DEBUG
  obit = getAll()->find(ob);
  if (obit == getAll()->end()) {
    //#endif
    getAll()->insert(ob);
    //#ifdef _DEBUG
  }
  else {
    Gu::debugBreak();
    //Error
  }
  //#endif

  //This is a possible optimization for later.
    //  if (ob->getPhy()->getIsAtRest()) {
    //      obit = getResting()->find(ob);
    //      if (obit == getResting()->end()) {
    //          getResting()->insert(ob);
    //      }
    //      else {
    //          Gu::debugBreak();
    //      }
    //  }
    //  else {
    //      obit = getActive()->find(ob);
    //      if (obit == getActive()->end()) {
    //          getActive()->insert(ob);
    //      }
    //      else {
    //          Gu::debugBreak();
    //      }
    //  }

}
//////////////////////////////////////////////////////////////////////////
//void ObjCellManfiold::add(std::shared_ptr<PhysicsNode> ob) {
//    //#ifdef _DEBUG
//    bool bFound = false;
//    std::multimap<float, std::shared_ptr<PhysicsNode>>::iterator ite = getObjects()->begin();
//    for (; ite != getObjects()->end(); ite++) {
//        if (ite->second == ob) {
//            bFound = true;
//            break;
//        }
//    }
//
//    if (bFound == true) {
//        Gu::debugBreak();
//    }
//    else {
//        //#endif
//
//        //Store objects by their Lowest Z on the plane.  
//        //Thus, the beginning of the std::map will be the objects closest to the ground.
//      //  float zInd = fabsf(ob->getZ());
//     //   getObjects()->insert(std::make_pair(zInd, ob));
//        BroThrowNotImplementedException();
//        //#ifdef _DEBUG
//    }
//    //#endif
//
//
//}
//void ObjCellManfiold::remove(std::shared_ptr<PhysicsNode> ob) {
//    std::multimap<float, std::shared_ptr<PhysicsNode>>::iterator ite = getObjects()->begin();
//    for (; ite != getObjects()->end(); ite++) {
//        if (ite->second == ob) {
//            getObjects()->erase(ite);
//            break;
//        }
//    }
//}
//////////////////////////////////////////////////////////////////////////
//void CellManifold::addCell(WorldCell* pc) {
//    //#ifdef _DEBUG
//    std::set<WorldCell*>::iterator it = getCells()->find(pc);
//    if (it == getCells()->end()) {
//        getCells()->insert(pc);
//    }
//    else {
//        Gu::debugBreak();
//    }
//}
void GridManifold::addGrid(std::shared_ptr<PhysicsGrid> pc) {
  std::set<std::shared_ptr<PhysicsGrid>>::iterator it = getGrids()->find(pc);
  if (it == getGrids()->end()) {
    getGrids()->insert(pc);
  }
  else {
    Gu::debugBreak();
  }
}
void GridManifold::clear() {
  //getCells()->clear();
  getGrids()->clear();
}








}//ns Game
