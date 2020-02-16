/**
*
*    @file BottleGui.h
*    @date February 2, 2018
*    @author MetalMario971
*
*    © 2018 
*
*
*/
#pragma once
#ifndef __BOTTLEGUI_15176010892956417418_H__
#define __BOTTLEGUI_15176010892956417418_H__

#include "../gfx/Gui2d.h"
#include "../bottle/BottleHeader.h"

namespace Game {
/**
*    @class BottleGui
*    @brief
*
*/
class WorldEditState : public VirtualMemoryShared<WorldEditState> {
    int _iHoverVertex = 0;
    //GameMode::e _eGameMode = GameMode::e::Play; //Duplicate data - this is set in room.
    BlockMode::e _eBlockMode = BlockMode::e::Single;
    EditTool::e _eEditTool = EditTool::e::Pencil;
    EditMode::e _eEditMode = EditMode::e::Block;
    RegionSize::e _eRegionSize = RegionSize::e::Region_1x1;
    ProcMode::e _eProcMode = ProcMode::e::Block;
    GridMeshLayer::e _eGridMeshLayer = GridMeshLayer::e::Opaque;

    WorldObj* _pSelectedPixObj = nullptr;
    W25Tile _pSelectedBaseTile = W25TILE_EMPTY;
    W25Tile _pSelectedLiquidTile = W25TILE_EMPTY;
    bool _bUndoPressed = false;

    //Selection
    NodeId _pSelectedObj = 0;
    NodeId _pHoveredObj = 0;
    WorldCell* _pSelectedTile = nullptr; // So we don't have a selected tile.  Select mode is for objjects onlyh.
    WorldCell* _pHoveredTile = nullptr;
    vec3 _vHoverPt;
    vec3 _vHoverNorm;
    Box3f* _pHoveredBox = nullptr;
    bool _bIsHovering = false;
    std::vector<ivec3> _vecHoveredNodes;
    std::vector<Box3f*> _vecHoverBoxes;
  //  void buildTiles(std::map<Hash32, WorldObjectSpec*>& specs);
    //void updateUi();
    //TileButton* createTileButton(Motion25Spec* motion, PixObjSpec* ps, std::shared_ptr<EditButtonGroup> pGroup);
//    void buildGameWidgets();
    void setSelectedTile(TileButton* b) {  /*_pSelected = b;*/ }
    //void drawEditText();


    //void addGroupLine(std::shared_ptr<EditButtonGroup> ebg, float dy, float& baseX, float& baseY);
    //void addTileGroups(float dy, float& baseX, float& baseY);


    bool _bInputCaptured = false;
    Box3f* addHoverBox(Box3f* b);
    Box3f* addHoverBox(Box3f b);

    std::shared_ptr<World25> _pWorld25 = nullptr;
public:
    WorldEditState();
    virtual ~WorldEditState() override;

    NodeId getSelectedObject();
    void setSelectedObject(NodeId ob);
    NodeId getHoveredObject();

    void setHoverVertex(WorldCell* pc, vec3& normal, vec3& point, int i);
    void setHoverTile(WorldCell* pc, vec3& normal, vec3& point);
    void setHoverObject(std::shared_ptr<ModelNode> ob);
    EditMode::e getEditMode() { return _eEditMode; }
    void setEditMode(EditMode::e em);
    EditTool::e getEditTool() { return _eEditTool; }
    void setEditTool(EditTool::e et) { _eEditTool = et; }
    BlockMode::e getBlockMode() { return _eBlockMode; }
    void setBlockMode(BlockMode::e ee){ _eBlockMode = ee;}
    ProcMode::e getProcMode() { return _eProcMode; }
    void setProcMode(ProcMode::e ee) { _eProcMode = ee; }   
    void setRegionSize(RegionSize::e sz) { _eRegionSize = sz; }
    RegionSize::e getRegionSize(){ return _eRegionSize; }
    GridMeshLayer::e getGridMeshLayer() { return _eGridMeshLayer; }
    void setMatterMode(GridMeshLayer::e ee) { _eGridMeshLayer = ee; }

    void setSelectedBaseTile(W25Tile bs) { _pSelectedBaseTile = bs; }
    void setSelectedLiquidTile(W25Tile bs) { _pSelectedLiquidTile = bs; }
    void setSelectedSpec(WorldObj* ps) { _pSelectedPixObj = ps; }

    bool getIsHovering() { return _bIsHovering; }
    WorldCell* getHoveredTile() { return _pHoveredTile; }
    const vec3& getHoveredPoint() { return _vHoverPt; }
    const vec3& getHoveredNormal() { return _vHoverNorm; }
    int getHoveredVertex() { return _iHoverVertex; }
    //const Box3f* getHoverBox() { return _pHoveredBox; }
    std::vector<Box3f*>& getHoverBoxes() { return _vecHoverBoxes; }
    WorldCell* getHoverVertex() { return _pHoveredTile; }
  //  void addHoverNode(const ivec3& n);
    std::vector<ivec3>& getHoverNodes() { return _vecHoveredNodes; }
    //void addHoverNode(World25Grid* pg);
    void clearHover();

    void setUndoPressed() { _bUndoPressed = true; }
    bool getUndoPressed() { return _bUndoPressed; }
    void clearUndoPressed() { _bUndoPressed = false; }
    W25Tile getSelectedTile();
   // WorldObjectSpec* getSelectedSpec();
   // virtual bool updateTouch(std::shared_ptr<Fingers> pFingers) override;
    void gameModeChanged(GameMode::e eMode);
    
    void setInputCaptured(bool b) { _bInputCaptured = b;}

};

}//ns Game



#endif
