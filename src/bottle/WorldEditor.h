/**
*  @file WorldEditor.h
*  @date May 3, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLDEDITOR_14937876123321759296_H__
#define __WORLDEDITOR_14937876123321759296_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
class WorldEditor;
class WorldEdit;
class GridEditHistoryItem;
class GridEditHistoryClick;
#define W25_NO_HISTORY (-1)
class GridEditHistoryItem {
  std::shared_ptr<WorldEditor> _pWorldEditor = nullptr;
public:
  WorldCell* _pCell = nullptr;
  //World25Grid* _pGrid = nullptr;
  int _iPreviousMaterial = -1;
  int _iPreviousGeom = -1;
  GridMeshLayer::e _eMatterMode;
  void doUndoItem();

  GridEditHistoryItem(std::shared_ptr<WorldEditor> cr) : _pWorldEditor(cr) {}
};
class GridEditHistoryClick {
  std::shared_ptr<WorldEditor> _pEditor = nullptr;
public:
  std::vector<std::shared_ptr<GridEditHistoryItem>> _vecHistories;
  void doUndoClick();

  GridEditHistoryClick(std::shared_ptr<WorldEditor> cr) : _pEditor(cr) {}
};
class WorldEdit : public VirtualMemory {
public:
  WorldCell* _pCell;
  GridMeshLayer::e _eMatterMode;

  WorldEdit(WorldCell* pc,
    GridMeshLayer::e em) : _pCell(pc), _eMatterMode(em) {
  }
  virtual ~WorldEdit() override {}
};
class WorldEditor : public VirtualMemoryShared<WorldEditor> {
  typedef std::set<WorldCell*> CellMap;
  typedef std::map<std::shared_ptr<WorldGrid>, CellMap> GridMap;
  typedef std::map<GridMeshLayer::e, GridMap> EditMap;
public:

  WorldEditor(std::shared_ptr<World25> pWorld);
  virtual ~WorldEditor() override;
  void init();
  void editWorld(std::shared_ptr<InputManager> pFingers);
  void drawForward();

  void performUndo();
  void placeObject(std::shared_ptr<ModelSpec> ms);
  void editWorldUpdateGrid(WorldCell* pc, std::shared_ptr<WorldGrid> pg, bool bMakeMeshRightAway, bool bSaveOnly, GridMeshLayer::e eMatter);
  void editWorldEndEdit();
private:
  std::shared_ptr<ShaderBase> _pHoverShader = nullptr;
  std::shared_ptr<World25> _pWorld25 = nullptr;
  WorldCell* _pLastEditedCell = nullptr;
  bool _bBeginEdit = false;
  EditMap _mapWorldEdits;// Map of grids to mesh levels that need to be redone.
  std::set<std::shared_ptr<WorldGrid>> _setGridsToSave;
  std::map<std::shared_ptr<WorldGrid>, std::set<WorldCell*>> _mapDirtyCells;

  std::shared_ptr<ModelNode> _pSelector = nullptr;

  vec3 _vShiftEditNormal; //Last normal we selected for shift editing.
  ivec3 _viShiftEditPos; //Last position we selected for shift editing.
  bool _bShiftEditBegin = false;

  std::shared_ptr<GridEditHistoryClick> _pCurrentClickHistory = nullptr;
  std::vector<std::shared_ptr<GridEditHistoryClick>> _vecEditHistory;

  std::shared_ptr<WorldEditState> _pWorldEditState = nullptr;

  std::shared_ptr<ModelNode> _pPlacedObject = nullptr;

  void addEditedGrid(std::shared_ptr<WorldGrid> savegrid, WorldCell*, GridMeshLayer::e eMatter);
  void addHistory(WorldCell* pc, int iPreviousGeom, int iPreviousTile, GridMeshLayer::e eMatterMode);
  bool updateShiftEdit(std::shared_ptr<InputManager> pFingers, WorldCell* pc, vec3& n_out);
  void editWorldMouseRelease(std::shared_ptr<InputManager> pFingers);
  void playEditSound(GridMeshLayer::e eMatterMode, bool bDelete);
  void editBlockVertex(EditTool::e eEditTool, GridMeshLayer::e eMatterMode, WorldCell* pn, int nv, W25Tile tms, std::set<WorldCell*>& editedCells);
  void editTileVert(WorldCell* pc, EditTool::e eEditTool, EditMode::e eEditMode, GridMeshLayer::e eMatterMode);
  void editWorldMouseDown(std::shared_ptr<InputManager> pFingers);
  void editWorldUpdateHover(std::shared_ptr<InputManager> pFingers);
  void addDrityCell(std::shared_ptr<WorldGrid> pg, WorldCell* pc);
  void makeAllEditedMeshes();
  void setLastEditedCell(WorldCell* pc) { _pLastEditedCell = pc; }
  WorldCell* getLastEditedCell() { return _pLastEditedCell; }

  std::shared_ptr<MeshNode> _pHoverDot = nullptr;

};
}//ns Game



#endif
