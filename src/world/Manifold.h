/**
*  @file Manifold.h
*  @date January 4, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __MANIFOLD_15150861112662815887_H__
#define __MANIFOLD_15150861112662815887_H__

#include "../world/WorldHeader.h"

namespace Game {
//GridManifold
//Simply a set of grids that an PhysicsNode intersects
//Used by physics to make collision checking faster
class GridManifold : public VirtualMemory {
    //std::set<WorldCell*> _setCells;
    std::set<std::shared_ptr<PhysicsGrid>> _setGrids;
public:
    //std::set<WorldCell*>* getCells() { return &_setCells; }
    std::set<std::shared_ptr<PhysicsGrid>>* getGrids() { return &_setGrids; }
    void addGrid(std::shared_ptr<PhysicsGrid> pc);
    //void addCell(WorldCell* pc);

    void clear();
};
//NodeManifold
// Set of PhysicsNodes intersecting in a BaseGrid 
//Used by physics to make collision checking faster
class NodeManifold : public VirtualMemory {
    std::set<std::shared_ptr<PhysicsNode>> _setAll;
    //  std::set<PixObj*> _setStatic;
    std::set<std::shared_ptr<PhysicsNode>> _setActive;
    std::set<std::shared_ptr<PhysicsNode>> _setResting;
public:
    std::set<std::shared_ptr<PhysicsNode>>* getAll() { return &_setAll; }
    // std::set<PixObj*>* getStatic() { return &_setStatic; }
    std::set<std::shared_ptr<PhysicsNode>>* getActive() { return &_setActive; }
    std::set<std::shared_ptr<PhysicsNode>>* getResting() { return &_setResting; }

    void remove(std::shared_ptr<PhysicsNode> ob);
    void add(std::shared_ptr<PhysicsNode> ob);
};

//W25ObjCellManfiold - for cells
//class ObjCellManfiold : public VirtualMemory {
//    //Cells just have objects for the light data.
//    std::multimap<float, std::shared_ptr<BaseNode>> _setAll;
//public:
//    std::multimap<float, std::shared_ptr<BaseNode>>* getObjects() { return &_setAll; }
//
//    void add(std::shared_ptr<BaseNode> ob);
//    void remove(std::shared_ptr<BaseNode> ob);
//};

}//ns Game



#endif
