#include "../base/Logger.h"
#include "../base/Fingers.h"
#include "../base/GLContext.h"
#include "../base/GLContext.h"
#include "../base/RoomBase.h"
#include "../base/SoundCache.h"
#include "../display/ShaderBase.h"
#include "../display/ShaderMaker.h"
#include "../display/RenderParams.h"
#include "../display/LightNode.h"
#include "../display/ShadowBox.h"
#include "../model/FragmentBufferData.h"
#include "../model/Model.h"
#include "../model/MeshNode.h"
#include "../model/ModelCache.h"
#include "../model/MeshSpec.h"
#include "../bottle/World25.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/WorldCell.h"
#include "../bottle/WorldEditor.h"
#include "../bottle/WorldMaker.h"
#include "../bottle/WorldEditState.h"
#include "../bottle/BottleRoom.h"
#include "../bottle/WorldObj.h"
#include "../bottle/W25Config.h"


namespace Game {

WorldEditor::WorldEditor(std::shared_ptr<World25> pWorld) {
    _pWorld25 = pWorld;
    //Must come after camera created AND world25 is created
    _pWorldEditState = std::make_shared<WorldEditState>();
}
WorldEditor::~WorldEditor() {

}
void WorldEditor::init(){
    /*_pHoverShader = Gu::getContext()->getShaderMaker()->makeShader(
        std::vector<t_string>{"d_v3n3_hover.vs", "d_v3n3_hover.gs", "d_v3n3_hover.ps"}
    );*/
    std::shared_ptr<ModelSpec> pSel = Gu::getContext()->getModelCache()->getOrLoadModel("hover_1");
    if (pSel != nullptr) {
        _pSelector = ModelNode::create(pSel);
        _pSelector->setPos(std::move(vec3(0, 0, 0)));
        _pSelector->playAction("hover_1.Rotate");

        _pSelector->setHidden(true);
        Gu::getContext()->getPhysicsWorld()->addObj(_pSelector, false, false);
        std::shared_ptr<LightNodePoint> pl = Gu::getContext()->getPhysicsWorld()->makePointLight(vec3(0,1,0), 10, vec4(1,1,0.5,1),"",true);
        pl->getShadowBox()->getSmallBoxSize() = 0.03f;
        _pSelector->attachChild(std::dynamic_pointer_cast<TreeNode>(pl));
    }


    //v_v3n3 frag;
    //frag.n = 0;
    //frag.v = 0;
    //int32_t ind = 0;
    //_pHoverDot = std::make_shared<MeshNode>(std::make_shared<MeshSpec>(
    //    (void*)&frag, 1, (void*)&ind, 1, v_v3n3::getVertexFormat(), nullptr
    //    ));
}

void WorldEditor::editWorld(std::shared_ptr<Fingers> pFingers) {
    ButtonState::e eLmb = pFingers->getLmbState();
    ButtonState::e eRmb = pFingers->getRmbState();
    vec2 vMouse = pFingers->getMousePos();
    vec2 vLast = pFingers->getLastMousePos();



    //Update shift edit
    if (pFingers->shiftHeld() == false) {
        _bShiftEditBegin = false;
    }

    if (_pWorldEditState->getUndoPressed()) {
        performUndo();
    }

    if (eLmb == ButtonState::Press || eLmb == ButtonState::Down || eRmb == ButtonState::Press || eRmb == ButtonState::Down) {
        editWorldMouseDown(pFingers);
    }
    else if (eLmb == ButtonState::Release) {
        editWorldMouseRelease(pFingers);
    }

    //Hovering - Set the selection.
    editWorldUpdateHover(pFingers);

    
    if (_pWorldEditState->getIsHovering()) {
        _pSelector->setPos(std::move(_pWorldEditState->getHoveredPoint()));
        _pSelector->setScale(vec3(0.8,0.8,0.8));
        vec3 n =_pWorldEditState->getHoveredNormal();
        if(n != vec3(0,1,0)) {
            //Rotate the selector
            vec3 rn = (vec3(0, 1, 0)).cross(n);
            float fn = acosf(n.dot(vec3(0,1,0)));
            _pSelector->setRot(vec4(rn, fn));

        }
        else {
            _pSelector->setRot(vec4(0,1,0, 0));

        }

        _pSelector->setHidden(false);
    }
    else {
         _pSelector->setHidden(true);
    }
}
void WorldEditor::drawForward(){
    //RenderParams rp;
    //rp.setShader(_pHoverShader);
    //rp.getShader()->setCameraUf(Gu::getContext()->getCamera());
    //rp.setMesh(_pHoverDot);
    //rp.setDrawMode(GL_POINTS);
    //rp.draw();
}
void WorldEditor::addEditedGrid(std::shared_ptr<WorldGrid> savegrid, WorldCell* pCell, GridMeshLayer::e eMatter) {
    EditMap::iterator eite;

    eite = _mapWorldEdits.find(eMatter);
    if (eite == _mapWorldEdits.end()) {
        _mapWorldEdits.insert(std::make_pair(eMatter, GridMap()));
        eite = _mapWorldEdits.find(eMatter);
    }
    GridMap& grMap = eite->second;

    GridMap::iterator gite = grMap.find(savegrid);
    if (gite == grMap.end()) {
        grMap.insert(std::make_pair(savegrid, std::set<WorldCell*>()));
        gite = grMap.find(savegrid);
    }

    CellMap& cMap = gite->second;

    if (pCell != nullptr) {
        if (cMap.find(nullptr) == cMap.end()) {
            cMap.insert(pCell);
        }
    }
    else {
        cMap.clear();
        cMap.insert(nullptr);
    }

    _setGridsToSave.insert(savegrid);
}
void WorldEditor::editWorldUpdateGrid(WorldCell* pc, std::shared_ptr<WorldGrid> pg, bool bMakeMeshRightAway,
    bool bSaveOnly, GridMeshLayer::e eMatter) {
    //Both pc and pg can be nullptr, in which case we're asking to just save the grid (objects got deleted or whatever)
    if (pg == nullptr && pc == nullptr) {
        BroThrowException("Error: both cell and grid can't be null.");
    }
    else if (pg != nullptr && pc != nullptr) {
        BroThrowException("Error: both cell and grid can't be set. ");
    }

    std::shared_ptr<WorldGrid> savegrid;
    if (pc != nullptr) {
        savegrid = pc->getGrid();
    }
    else {
        savegrid = pg;
    }

    _bBeginEdit = true;

    if (bSaveOnly == true) {
        _setGridsToSave.insert(savegrid);
    }
    else {
        if (pc != nullptr) {
            addEditedGrid(pc->getGrid(), pc, eMatter);
            addDrityCell(pc->getGrid(), pc);
            for (int i = 0; i<6; ++i) {
                WorldCell* pcn = pc->getNeighbor((PhysicsGridSide::e)i);
                if (pcn != nullptr) {
                    addEditedGrid(pcn->getGrid(), pcn, eMatter);
                    addDrityCell(pcn->getGrid(), pcn);
                }
            }
        }
        else if (pg != nullptr) {
            //Invalidate and just set the whole grid.
            addEditedGrid(pg, nullptr, eMatter);
            addDrityCell(pg, nullptr);
        }
    }
    if (bMakeMeshRightAway == true) {
        //Make the mesh right away.  Don't store the mesh data after the click is released.
        makeAllEditedMeshes();
    }
}
void WorldEditor::editWorldEndEdit() {
    if (_bBeginEdit == true) {
        _pLastEditedCell = nullptr;
        _bBeginEdit = false;

        //This won't fire if we specified "right away" in editWorldUpdateGrid
        makeAllEditedMeshes();

        for (std::shared_ptr<WorldGrid> pg : _setGridsToSave) {
            pg->getWorld25()->getWorldMaker()->saveGrid(pg);
            //pg->saveGrid();
        }

        BroLogInfo("Saved ", _setGridsToSave.size(), " grids.");
        _setGridsToSave.clear();
    }
}
void WorldEditor::makeAllEditedMeshes() {
    for(std::pair<GridMeshLayer::e, GridMap> p1 : _mapWorldEdits) {
        GridMeshLayer::e eMatter = p1.first;

        //Iterate over the map and add the edits
        for (std::pair<std::shared_ptr<WorldGrid>, std::set<WorldCell*>> p2 : p1.second) {
            if (p2.first != nullptr) {
                if (p2.second.size() == 1 && *(p2.second.begin()) == nullptr) {
                    p2.first->updateRedoMesh(eMatter);
                    for (int i = 0; i<6; ++i) {
                        std::shared_ptr<WorldGrid> pgn = std::dynamic_pointer_cast<WorldGrid>(p2.first->getNeighbor(i));
                        if (pgn != nullptr) {
                            pgn->updateRedoMesh(eMatter);
                        }
                    }
                }
                else {
                    //**TODO: - Make mesh level has been removed
                    //** Need to optimize the edit mode for "single blocks"
                    for (WorldCell* pcell : p2.second) {
                        p2.first->updateRedoMeshForCell(pcell, eMatter, true);
                    }
                }
            }
            else {
                Gu::debugBreak(); // Error
            }
        }
    }
    _mapWorldEdits.clear();

    //Update dirty cells.
    for (std::pair<std::shared_ptr<WorldGrid>, std::set<WorldCell*>> dp : _mapDirtyCells) {
        if (dp.first != nullptr) {
            if (dp.second.size() == 1 && *(dp.second.begin()) == nullptr) {
                dp.first->markAllCellsDirty();
            }
            else {
                for (WorldCell* pc : dp.second) {
                    dp.first->markCellDirty(pc);
                }
            }
        }
        else {
            Gu::debugBreak(); // Error
        }
    }
    _mapDirtyCells.clear();
}
void WorldEditor::addDrityCell(std::shared_ptr<WorldGrid> pg, WorldCell* pc) {
    std::map<std::shared_ptr<WorldGrid>, std::set<WorldCell*>>::iterator ite;

    ite = _mapDirtyCells.find(pg);
    if (ite == _mapDirtyCells.end()) {
        _mapDirtyCells.insert(std::make_pair(pg, std::set<WorldCell*>()));
        ite = _mapDirtyCells.find(pg);
    }
    if (ite != _mapDirtyCells.end()) {
        if (ite->second.size() == 1 && *(ite->second.begin()) == nullptr) {
            //This mean's we're to mark the whole grid as dirty.
        }
        else if (pc == nullptr) {
            //Mark whole grid as dirty.
            ite->second.clear();
            ite->second.insert(nullptr);
        }
        else {
            ite->second.insert(pc);
        }
    }
    else {
        Gu::debugBreak(); // Error
    }
}

void WorldEditor::editWorldUpdateHover(std::shared_ptr<Fingers> pFingers) {
    EditMode::e eEditMode = _pWorldEditState->getEditMode();
    ButtonState::e eLmb = pFingers->getLmbState();
    vec2 vMouse = pFingers->getMousePos();

    _pWorldEditState->clearHover();

    Ray_t pr = _pWorld25->getMouseRay(vMouse);

    if (eEditMode == EditMode::e::PlaceObject) {
        if (_pPlacedObject != nullptr) {

            WorldCell* out_cell;
            WorldCell* out_hover;
            vec3 out_n, out_pt;
            _pWorld25->getRayHoverBlock(&pr, out_cell, out_hover, out_n, out_pt);

            if (out_cell != nullptr) {
                _pPlacedObject->setPos(std::move(out_pt));
            }
        }
    }
    else if (eEditMode == EditMode::e::Tile) {
        WorldCell* cell_out = nullptr, *hover_out = nullptr;
        vec3 n_out;
        vec3 p_out;
        _pWorld25->getRayHoverBlock(&pr, cell_out, hover_out, n_out, p_out);
        _pWorldEditState->setHoverTile(cell_out, n_out, p_out);
    }
    else if (eEditMode == EditMode::e::Block) {
        WorldCell* cell_out = nullptr, *hover_out = nullptr;
        vec3 n_out;
        vec3 p_out;
        WorldCell* pHover = nullptr;
        _pWorld25->getRayHoverBlock(&pr, cell_out, hover_out, n_out, p_out);
        EditTool::e eEditTool = _pWorldEditState->getEditTool();

       pHover = cell_out;

        _pWorldEditState->setHoverTile(pHover, n_out, p_out);
    }
    else if (eEditMode == EditMode::e::Vertex || eEditMode == EditMode::e::VertexGroup) {
        WorldCell* cell_out = nullptr;
        int vert_out;
        vec3 n_out;
        vec3 p_out;
        _pWorld25->getRayHoverVertex(&pr, cell_out, vert_out, n_out, p_out);
        _pWorldEditState->setHoverVertex(cell_out, n_out, p_out, vert_out);
    }
    else if (eEditMode == EditMode::e::Proc) {
        ////            World25Grid* grid;
        //            //_pWorld25->getRaySelectNode(&pr, grid);
        //            //if(grid!=nullptr) {
        //            //    _pGui->addHoverNode(grid->getGridPos());
        //            //}

        //RegionSize::e eRegion = _pGui->getRegionSize();
        //float boxW2, boxH2;
        //int y = CongaUtils::getNodesYMax() - CongaUtils::getNodesYMin() + 1;

        //boxH2 = CongaUtils::getNodeHeight() * (float)y * 0.5f;
        //if (eRegion == RegionSize::e::Region_1x1) {
        //    boxW2 = CongaUtils::getNodeWidth() * 0.0f* 0.5f + 0.00001f;
        //}
        //else if (eRegion == RegionSize::e::Region_2x2) {
        //    boxW2 = CongaUtils::getNodeWidth() * 1.0f * 0.5f + 0.00001f;
        //}
        //else if (eRegion == RegionSize::e::Region_4x4) {
        //    boxW2 = CongaUtils::getNodeWidth() * 3.0f * 0.5f + 0.00001f;
        //}
        //else if (eRegion == RegionSize::e::Region_8x8) {
        //    boxW2 = CongaUtils::getNodeWidth() * 7.0f * 0.5f + 0.00001f;
        //}
        //std::vector<std::shared_ptr<WorldGrid>> outGrids;
        //_pWorld25->getAllNodesForRayBoxSLOW(&pr, boxW2, boxH2, outGrids, false);

        //for (std::shared_ptr<WorldGrid> gg : outGrids) {
        //    if (gg != nullptr) {
        //        _pGui->addHoverNode(gg->getGridPos());
        //    }
        //}



    }

    
}
void WorldEditor::editWorldMouseDown(std::shared_ptr<Fingers> pFingers) {
    ButtonState::e eLmb = pFingers->getLmbState();
    ButtonState::e eRmb = pFingers->getRmbState();
    EditTool::e eEditTool = _pWorldEditState->getEditTool();
    EditMode::e eEditMode = _pWorldEditState->getEditMode();
    BlockMode::e eBlockMode = _pWorldEditState->getBlockMode();
    GridMeshLayer::e eGridMeshLayer = _pWorldEditState->getGridMeshLayer();

    vec2 vMouse = pFingers->getMousePos();
    Ray_t pr = _pWorld25->getMouseRay(vMouse);

    if (eEditMode == EditMode::e::Tile) {
        W25Tile ms = _pWorldEditState->getSelectedTile();
        WorldCell* pc = _pWorld25->hitCellGroundUnderRay(&pr, true);

        if (pc != nullptr && (ms != W25TILE_EMPTY || eEditTool == EditTool::e::Eraser) && pc != getLastEditedCell()) {
            setLastEditedCell(pc);

            bool bEdited = false;
            if (eEditTool == EditTool::e::Pencil) {
                if (pc->getTile(eGridMeshLayer) != ms) {
                    addHistory(pc, W25_NO_HISTORY, pc->getTile(eGridMeshLayer),
                        eGridMeshLayer);
                    pc->setTile(eGridMeshLayer, ms);
                    bEdited = true;
                }
            }
            else if (eEditTool == EditTool::e::Eraser) {
                //Don't allow erasing of tile Materials.

            }
            if (bEdited == true) {
                editWorldUpdateGrid(pc, nullptr, true, false, eGridMeshLayer);
            }
        }
        else {
            //     BroLogWarn("cell not found or motion not selected.");
        }
    }
    else if (eEditMode == EditMode::e::Block) {
        if (((eBlockMode == BlockMode::e::Single) && (eLmb == ButtonState::Press || eRmb == ButtonState::Press)) || (eBlockMode == BlockMode::e::Paint)) {
            W25Tile tms = _pWorldEditState->getSelectedTile();
            WorldCell* cell_out = nullptr;
            WorldCell* hover_out = nullptr;
            WorldCell* pEditedCell = nullptr;
            vec3 n_out;
            vec3 p_out;
            _pWorld25->getRayHoverBlock(&pr, cell_out, hover_out, n_out, p_out);

            if (eLmb == ButtonState::Press) {
                if (hover_out != nullptr) {
                    if (updateShiftEdit(pFingers, hover_out, n_out)) {
                        addHistory(hover_out, hover_out->getGeom(eGridMeshLayer), W25_NO_HISTORY, eGridMeshLayer);
                        playEditSound(eGridMeshLayer, false);

                        hover_out->setTile(eGridMeshLayer, tms);
                        hover_out->setGeom(eGridMeshLayer, W25GEOM_SOLID);
                        pEditedCell = hover_out;
                    }
                }
            }
            else if (eRmb == ButtonState::Press) {
                if (cell_out != nullptr) {
                    if (updateShiftEdit(pFingers, cell_out, n_out)) {
                        addHistory(cell_out, cell_out->getGeom(eGridMeshLayer), W25_NO_HISTORY, eGridMeshLayer);
                        playEditSound(eGridMeshLayer, true);
                        cell_out->setGeom(eGridMeshLayer, W25GEOM_EMPTY);
                        pEditedCell = cell_out;
                    }
                }
            }
            if (pEditedCell != nullptr) {
                editWorldUpdateGrid(pEditedCell, nullptr, true, false, eGridMeshLayer);
            }
        }
    }
    else if (eEditMode == EditMode::e::PlaceObject) {
        if (_pPlacedObject != nullptr) {
            WorldCell* out_cell;
            WorldCell* out_hover;
            vec3 out_n, out_pt;
            _pWorld25->getRayHoverBlock(&pr, out_cell, out_hover, out_n, out_pt);

            if (out_cell != nullptr) {

                _pPlacedObject->setPos(std::move(out_pt));
                playEditSound(GridMeshLayer::e::Opaque, true);
                _pPlacedObject = nullptr;
            }
        }

    }
    else if (eEditMode == EditMode::e::Select) {
        std::shared_ptr<ModelNode> ob = _pWorld25->getClosestObjectUnderRay(&pr);
        if (ob != nullptr) {
            _pWorldEditState->setSelectedObject(ob->getId());
        }
        else {
            _pWorldEditState->setSelectedObject(NO_NODE_ID);
        }
    }
    else if (eEditMode == EditMode::e::Vertex || eEditMode == EditMode::e::VertexGroup) {
        if (((eBlockMode == BlockMode::e::Single) && (eLmb == ButtonState::Press)) || (eBlockMode == BlockMode::e::Paint)) {
            std::shared_ptr<WorldGrid> pEditedGrid = nullptr;
            WorldCell* pc = _pWorldEditState->getHoveredTile();

            editTileVert(pc, eEditTool, eEditMode, eGridMeshLayer);
        }

    }

}
void WorldEditor::editTileVert(WorldCell* pc, EditTool::e eEditTool, EditMode::e eEditMode, GridMeshLayer::e eMatterMode) {
    std::set<WorldCell*> editedCells;
    if (pc != nullptr) {
        WorldCell* pn = nullptr;
        W25Tile tms = _pWorldEditState->getSelectedTile();
        int hv = _pWorldEditState->getHoveredVertex();
        int nv;

        //Attempt to set geom, if we're already set then set neighbor above or bleow, unless we are ereasign
        W25Geom cg = pc->getGeom(eMatterMode);
        if (((cg & (1 << hv)) == 0) || (eEditTool == EditTool::e::Eraser)) {
            pn = pc;
            nv = hv;
        }
        else {

            //Check if the top or bottom vertex inside of the cube was set.  Otherwise try to check neighbors
#define V_ON(a,b) do { if((hv == a) && (cg & (1 << b)) == 0) { pn = pc; nv = b; } } while(0)

            V_ON(0, 2);
            V_ON(2, 0);
            V_ON(1, 3);
            V_ON(3, 1);
            V_ON(4, 6);
            V_ON(6, 4);
            V_ON(5, 7);
            V_ON(7, 5);

            //We ddin't set inside the cell, Get Cell above or below
            if (pn == nullptr) {
                if (hv == 0 || hv == 1 || hv == 4 || hv == 5) {
                    pn = pc->getNeighbor(PhysicsGridSide::e::gB);
                    if (hv == 0) nv = 2;
                    if (hv == 1) nv = 3;
                    if (hv == 4) nv = 6;
                    if (hv == 5) nv = 7;

                }
                else {
                    pn = pc->getNeighbor(PhysicsGridSide::e::gT);
                    if (hv == 2) nv = 0;
                    if (hv == 3) nv = 1;
                    if (hv == 6) nv = 4;
                    if (hv == 7) nv = 5;
                }
            }
        }
        if (pn != nullptr) {
            if (eEditMode == EditMode::e::Vertex) {
                editBlockVertex(eEditTool, eMatterMode, pn, nv, tms, editedCells);
            }
            else if (eEditMode == EditMode::e::VertexGroup) {
                int v4 = 0;
                WorldCell* cn[4];
                cn[0] = cn[1] = cn[2] = cn[3] = nullptr;
                int cv[4];
                cv[0] = cv[1] = cv[2] = cv[3] = -1;
                if (nv == 0 || nv == 2) v4 = 0;
                if (nv == 1 || nv == 3) v4 = 1;
                if (nv == 4 || nv == 6) v4 = 2;
                if (nv == 5 || nv == 7) v4 = 3;

                int _Gx[8], _Gz[8];
                _Gx[0] = 1; _Gx[1] = 0; _Gx[2] = 3; _Gx[3] = 2; _Gx[4] = 5; _Gx[5] = 4; _Gx[6] = 7; _Gx[7] = 6;
                _Gz[0] = 4; _Gz[4] = 0; _Gz[1] = 5; _Gz[5] = 1; _Gz[2] = 6; _Gz[6] = 2; _Gz[3] = 7; _Gz[7] = 3;


                if (v4 == 0) {
                    cn[0] = pn; cv[0] = nv;
                    if (cn[0]) { cn[1] = cn[0]->getNeighbor(PhysicsGridSide::e::gL);  cv[1] = _Gx[cv[0]]; }
                    if (cn[1]) { cn[2] = cn[1]->getNeighbor(PhysicsGridSide::e::gA);  cv[2] = _Gz[cv[1]]; }
                    if (cn[2]) { cn[3] = cn[2]->getNeighbor(PhysicsGridSide::e::gR);  cv[3] = _Gx[cv[2]]; }
                }
                if (v4 == 1) {
                    cn[0] = pn; cv[0] = nv;
                    if (cn[0]) { cn[1] = cn[0]->getNeighbor(PhysicsGridSide::e::gA);  cv[1] = _Gz[cv[0]]; }
                    if (cn[1]) { cn[2] = cn[1]->getNeighbor(PhysicsGridSide::e::gR);  cv[2] = _Gx[cv[1]]; }
                    if (cn[2]) { cn[3] = cn[2]->getNeighbor(PhysicsGridSide::e::gF);  cv[3] = _Gz[cv[2]]; }
                }
                if (v4 == 2) {
                    cn[0] = pn; cv[0] = nv;
                    if (cn[0]) { cn[1] = cn[0]->getNeighbor(PhysicsGridSide::e::gL);  cv[1] = _Gx[cv[0]]; }
                    if (cn[1]) { cn[2] = cn[1]->getNeighbor(PhysicsGridSide::e::gF);  cv[2] = _Gz[cv[1]]; }
                    if (cn[2]) { cn[3] = cn[2]->getNeighbor(PhysicsGridSide::e::gR);  cv[3] = _Gx[cv[2]]; }
                }
                if (v4 == 3) {
                    cn[0] = pn; cv[0] = nv;
                    if (cn[0]) { cn[1] = cn[0]->getNeighbor(PhysicsGridSide::e::gR);  cv[1] = _Gx[cv[0]]; }
                    if (cn[1]) { cn[2] = cn[1]->getNeighbor(PhysicsGridSide::e::gF);  cv[2] = _Gz[cv[1]]; }
                    if (cn[2]) { cn[3] = cn[2]->getNeighbor(PhysicsGridSide::e::gL);  cv[3] = _Gx[cv[2]]; }
                }
                for (int icc = 0; icc < 4; ++icc) {
                    editBlockVertex(eEditTool, eMatterMode, cn[icc], cv[icc], tms, editedCells);
                }


            }

            //Update and retopo the grids
            for (WorldCell* gc : editedCells) {
                editWorldUpdateGrid(gc, nullptr, false, false, eMatterMode);
            }
            makeAllEditedMeshes();
        }
    }
}
void WorldEditor::editBlockVertex(EditTool::e eEditTool, GridMeshLayer::e eMatterMode, WorldCell* pn,
    int nv, W25Tile tms, std::set<WorldCell*>& editedCells) {
    if (pn == nullptr || nv < 0 || nv >7) {
        return;
    }
    W25Geom cg = pn->getGeom(eMatterMode);
    W25Geom newG = cg;

    if (eEditTool == EditTool::e::Pencil) {
        newG |= (1 << nv);
    }
    else if (eEditTool == EditTool::e::Eraser) {
        newG &= ~(1 << nv);
    }

    if (newG != cg) {
        addHistory(pn, pn->getGeom(eMatterMode), W25_NO_HISTORY, eMatterMode);
        pn->setGeom(eMatterMode, newG);
        pn->setTile(eMatterMode, tms);
        playEditSound(eMatterMode, true);

        editedCells.insert(pn);
    }
}
void WorldEditor::playEditSound(GridMeshLayer::e eMatterMode, bool bDelete) {
    if(bDelete){
        Gu::getContext()->getSoundCache()->tryPlay(Gu::getContext()->getRoom()->makeAssetPath("snd", "s-delete.ogg"));
    }
    else if (eMatterMode == GridMeshLayer::e::Opaque) {
        Gu::getContext()->getSoundCache()->tryPlay(Gu::getContext()->getRoom()->makeAssetPath("snd", "s-block-low.ogg"));
    }
    else if (eMatterMode == GridMeshLayer::e::Transparent) {
        Gu::getContext()->getSoundCache()->tryPlay(Gu::getContext()->getRoom()->makeAssetPath("snd", "s-water-splash.ogg"));
    }
    //    }
    //    else if (eEditMode == EditMode::e::Vertex || eEditMode == EditMode::e::VertexGroup) {

    //        if (eMatterMode == GridMeshLayer::e::Opaque) {
    //            Gu::getContext()->getSoundCache()->tryPlay(Gu::getContext()->getRoom()->makeAssetPath("snd", "s-click.ogg"));
    //        }
    //        else if (eMatterMode == GridMeshLayer::e::Transparent) {
    //            Gu::getContext()->getSoundCache()->tryPlay(Gu::getContext()->getRoom()->makeAssetPath("snd", "s-water-splash.ogg"));
    //        }
    //    }
    //}
    //else if (eEditTool == EditTool::e::Eraser) {
    //    Gu::getContext()->getSoundCache()->tryPlay(Gu::getContext()->getRoom()->makeAssetPath("snd", "s-delete.ogg"));
    //}
}
void WorldEditor::editWorldMouseRelease(std::shared_ptr<Fingers> pFingers) {
    editWorldEndEdit();
    if (_pCurrentClickHistory != nullptr) {
        _vecEditHistory.push_back(_pCurrentClickHistory);
        _pCurrentClickHistory = nullptr;
    }
}
bool WorldEditor::updateShiftEdit(std::shared_ptr<Fingers> pFingers, WorldCell* pc, vec3& n_out) {
    if (pFingers == nullptr) {
        return false;
    }
    if (pc == nullptr) {
        return false;
    }
    //Return true if we can edit this axis (if shift is held).
    if (pFingers->shiftHeld()) {
        if (_bShiftEditBegin == false) {
            _bShiftEditBegin = true;
            _vShiftEditNormal = n_out;
            _viShiftEditPos = pc->getLocalPos();
        }

        //Start with the normal, if we're on the wong side return false.
        // if (n_out.dot(_vShiftEditNormal)) < 0.9f) {
        //     return false;
        // }

        //Check block position
        ivec3 vip = pc->getLocalPos();
        if ((fabsf(_vShiftEditNormal.y) > 0) && (vip.x != _viShiftEditPos.x || vip.z != _viShiftEditPos.z)) {
            return false;
        }
        else if ((fabsf(_vShiftEditNormal.x) > 0 || fabsf(_vShiftEditNormal.z) > 0) && (vip.y != _viShiftEditPos.y)) {
            return false;
        }

        return true;
    }
    else {
        return true;
    }
}
void WorldEditor::addHistory(WorldCell* pc, int iPreviousGeom, int iPreviousTile, GridMeshLayer::e eMatterMode) {
    AssertOrThrow2(iPreviousTile != W25_NO_HISTORY || iPreviousGeom != W25_NO_HISTORY);
    if (pc != nullptr) {

        if (_vecEditHistory.size() > 98) {
            _vecEditHistory.erase(_vecEditHistory.begin() + _vecEditHistory.size() - 1);
            BroLogWarn("Edit history exceeded maximum of 99.  Some history i8s lost");
        }

        if (_pCurrentClickHistory == nullptr) {
            _pCurrentClickHistory = std::make_shared<GridEditHistoryClick>(shared_from_this());
        }
        std::shared_ptr<GridEditHistoryItem> pg = std::make_shared<GridEditHistoryItem>(shared_from_this());
        pg->_pCell = pc;
        pg->_iPreviousGeom = iPreviousGeom;
        pg->_iPreviousMaterial = iPreviousTile;
        pg->_eMatterMode = eMatterMode;
        _pCurrentClickHistory->_vecHistories.push_back(pg);
    }
    else {
        Gu::debugBreak();
    }
}
void WorldEditor::performUndo() {
    if (_vecEditHistory.size() > 0) {
        std::shared_ptr<GridEditHistoryClick> pHistoryClick = *(_vecEditHistory.rbegin());

        pHistoryClick->doUndoClick();

        _vecEditHistory.erase(_vecEditHistory.begin() + _vecEditHistory.size() - 1);
    }
    _pWorldEditState->clearUndoPressed();
}
void GridEditHistoryClick::doUndoClick() {
    for (size_t i = 0; i < _vecHistories.size(); ++i) {
        _vecHistories[i]->doUndoItem();
    }
    for (size_t i = 0; i < _vecHistories.size(); ++i) {
        _vecHistories[i] = nullptr;
    }
   _pEditor->editWorldEndEdit();
}
void GridEditHistoryItem::doUndoItem() {
    if (_pCell != nullptr) {
        if (_iPreviousGeom != W25_NO_HISTORY) {
            //If we're blocks doing this will skip some configs because some of them are already "solid"
            std::shared_ptr<BottleRoom> br = std::dynamic_pointer_cast<BottleRoom>(Gu::getContext()->getRoom());
            bool bBlocks = false;
            if(br!=nullptr){ bBlocks = br->getWorld25()->getConfig()->getRenderBlocks(); }
            if (bBlocks == true || _pCell->getGeom(_eMatterMode) != _iPreviousGeom) {
                _pCell->setGeom(_eMatterMode, _iPreviousGeom);
                _pWorldEditor->editWorldUpdateGrid(_pCell, nullptr, false, false, _eMatterMode);
            }
        }
        if (_iPreviousMaterial != W25_NO_HISTORY) {
            W25Tile pBase;
            if (_iPreviousMaterial != -1) {
                pBase = _iPreviousMaterial;
                if (_pCell->getTile(_eMatterMode) != pBase) {
                    _pCell->setTile(_eMatterMode, pBase);
                    _pWorldEditor->editWorldUpdateGrid(_pCell, nullptr, false, false, _eMatterMode);
                }
            }
        }
    }
}
void WorldEditor::placeObject(std::shared_ptr<ModelSpec> ms){
    if (_pPlacedObject == nullptr) {

        _pPlacedObject = Gu::getContext()->getPhysicsWorld()->makeObj(ms, vec3(0, 0, 0), vec4(0, 0, 1, 0), vec3(1, 1, 1), "");
        _pWorldEditState->setEditMode(EditMode::e::PlaceObject);

    }
    else {
        //probably show a message stating that the player can't place right now because he'as already placing
    }

}





}//ns Game
