#include "../base/Gu.h"
#include "../base/GLContext.h"

#include "../math/MathAll.h"

#include "../display/FrustumBase.h"
#include "../display/CameraNode.h"
#include "../display/Atlas.h"
#include "../display/Texture2DSpec.h"
#include "../display/RenderUtils.h"
#include "../display/ShaderBase.h"

#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"
#include "../model/IboData.h"

#include "../bottle/W25GridMesh.h"
#include "../bottle/World25.h"
#include "../bottle/WorldGrid.h"
#include "../bottle/WorldCell.h"
#include "../bottle/BlockNode.h"
#include "../bottle/BottleUtils.h"
#include "../bottle/W25MeshMaker.h"


namespace Game {

W25GridMesh::W25GridMesh(std::shared_ptr<WorldGrid> pGrid, GridMeshLayer::e eMatter) :
    _eMatterMode(eMatter), _pGrid(pGrid)
{

}
W25GridMesh::~W25GridMesh() {
    _pMesh = nullptr;
    _verts.clear();
    _indexes.clear();

}
void W25GridMesh::updateTopology() {
    std::lock_guard<std::mutex> guard(_mtTopoMtx);

    if(_setTopoMod.size() > 0) {
        t_timeval tv = Gu::getMicroSeconds();
        {
            if (getMustRedoWholeMesh() == true) {
                makeMeshImmediately(false);
            }
            else {

                std::shared_ptr<W25MeshMaker> pMeshMaker = _pGrid->getWorld25()->getMeshMaker();
                std::shared_ptr<Atlas> pAtlas = _pGrid->getWorld25()->getWorldAtlas();//dynamic_cast<std::shared_ptr<Atlas>>(_pWorld25->getGlContext()->getTexCache()->getOrLoad(Utils::WorldAtlasName, true));
                AssertOrThrow2(pAtlas != nullptr);//Atlas must be loaded.
                AssertOrThrow2(pMeshMaker != nullptr);//Atlas must be loaded.
                float fWidth = BottleUtils::getCellWidth();
                float fHeight = BottleUtils::getCellHeight();
                //std::set<WorldCell*>& setCells = _pGrid->getCells();
                std::shared_ptr<SpriteBucket> pBucket = _pGrid->getWorld25()->getSpriteBucket();//dynamic_cast<std::shared_ptr<Atlas>>(_pWorld25->getGlContext()->getTexCache()->getOrLoad(Utils::WorldAtlasName, true));

                for (WorldCell* gc : _setTopoMod) {
                    //  WorldCell* gc = tt.getCell();

                    // MatterMode::e eMode = tt.getMode();
             //       if(gc->getGeom(_eMatterMode) != W25GEOM_EMPTY) {
                        pMeshMaker->redoMeshForCellNoGpuCopy(
                            getThis<W25GridMesh>(), pAtlas, gc, pBucket, _eMatterMode,
                            fWidth, fHeight,    
                            _pGrid->getWorld25()->getConfig(),
                            _pGrid,
                            false
                        );
             //       }
                }


            }

            //We can just copy the whole thing to the GPU for now.  That's quick.  It's the freaking fillMesh() that is slow.
            sendMeshToGpu();
        }
#ifdef _DEBUG
        BroLogDebug(_pGrid->getGridPos().toString() + " -> Redo " + _setTopoMod.size() +
           ( (_setTopoMod.size() == 1 && (*_setTopoMod.begin()==nullptr)) ? " grids " : " cells: " )+ (Gu::getMicroSeconds() - tv) / 1000 + "ms");
#endif
        _setTopoMod.clear();
    }
}
void W25GridMesh::sendMeshToGpu() {
    BottleUtils::makeWorldMesh(Gu::getContext(), _pMesh, &_verts, &_indexes);
}
void W25GridMesh::draw(RenderParams& rp, int& iDbgNumTrisDrawn) {
    if (getMesh() != nullptr) {
        if (false) {
            std::shared_ptr<VaoDataGeneric> vao = getMesh()->getMeshSpec()->getVaoData();
            RenderUtils::debugGetRenderState(false, true);
            W25MeshVert* test_read = new W25MeshVert[vao->getVbo()->getNumElements()];
            vao->getVbo()->copyDataServerClient(vao->getVbo()->getNumElements(), test_read);
            delete[] test_read;
        }
            
        uint32_t pid = getMesh()->getPickId();
        rp.getShader()->setUf("_ufPickId", (void*)&pid);
        rp.setMesh(getMesh());
        rp.draw();
        //pShader->draw(getMesh(), -1, GL_TRIANGLES, ps);
        iDbgNumTrisDrawn += getMesh()->getMeshSpec()->indexCount() / 3;
#ifdef _DEBUG
         std::shared_ptr<FrustumBase> pf = Gu::getContext()->getCamera()->getFrustum();
       if (!pf->hasBox(getMesh()->getBoundBoxObject())) {
           static int nnnn=0;
           if(nnnn==0){
               //So this should never hit because we collect nodes..
               Gu::debugBreak();
           }
       }
#endif
    }
}

void W25GridMesh::updateRedoMesh() {
    std::lock_guard<std::mutex> guard(_mtTopoMtx);

    //Queue an update to redo the whole thing
    _setTopoMod.clear();
    _setTopoMod.insert(nullptr);
}
bool W25GridMesh::getMustRedoWholeMesh(){

    if(_setTopoMod.size() == 0){
        return false;
    }
    else {
        //This find is fast because we easy out, and we removed all elements from the set except nullptr.
        return (_setTopoMod.size()==1) && (_setTopoMod.find(nullptr) != _setTopoMod.end());
    }
}
void W25GridMesh::updateRedoMeshForCell(WorldCell* pc, bool bNeighbors) {
    std::lock_guard<std::mutex> guard(_mtTopoMtx);

    AssertOrThrow2(pc->getGrid() == _pGrid);
    if (getMustRedoWholeMesh() == false) {
        //*We can't call another grids methods asynchronously.

        //if(bNeighbors == true) {
        //    //It MIGHT be faster just to redo the adjacent sides, however
        //    //we would still need to update all the index offsets, etc
        //    for (int i = 0; i< WorldCell::c_nNeighbors; ++i) {
        //        WorldCell* pn = pc->getNeighbor((World25Side::e)i);
        //        pc->getGrid()->updateRedoMeshForCell(pc, _eMatterMode, false);
        //    }
        //}

        _setTopoMod.insert(pc);
    }
}


void W25GridMesh::makeMeshImmediately(bool bAsync) {
    //This doesn't seem to make it faster.
    _verts.resize(0);
    _indexes.resize(0);

    if(_iMaxVertexCount > 0) {
        makeMeshImmediately_r(_pGrid->getRoot());

        if (bAsync == false) {
            Gu::checkErrorsRt();
            sendMeshToGpu();
        }

      //  propogateMeshData();
    }
}
void W25GridMesh::makeMeshForCell(WorldCell* gc) {

    std::shared_ptr<W25MeshMaker> pMeshMaker = _pGrid->getWorld25()->getMeshMaker();
    std::shared_ptr<Atlas> pAtlas = _pGrid->getWorld25()->getWorldAtlas();//dynamic_cast<std::shared_ptr<Atlas>>(_pWorld25->getGlContext()->getTexCache()->getOrLoad(Utils::WorldAtlasName, true));
    AssertOrThrow2(pAtlas != nullptr);//Atlas must be loaded.
    AssertOrThrow2(pMeshMaker != nullptr);//Atlas must be loaded.
    float fWidth = BottleUtils::getCellWidth();
    float fHeight = BottleUtils::getCellHeight();
    std::shared_ptr<SpriteBucket> pBucket = _pGrid->getWorld25()->getSpriteBucket();


    W25Geom geom = gc ->getGeom(_eMatterMode);
    //Adding an extra check here.
    //It's just TOO SLOW
    WorldCell* gcn;
    bool bVisible = false;

    if (geom != W25GEOM_EMPTY) {
        bVisible = true;
    }
    else {
        for (int i = 0; i<6; ++i) {
            gcn = gc->getNeighbor((PhysicsGridSide::e)i);
            if (gcn != nullptr){
                W25Geom nGeom = gcn->getGeom(_eMatterMode);
                if(nGeom != W25GEOM_EMPTY) {
                    bVisible = true;
                    break;
                }
            }
        }
    }

    gc->getGrid()->checkKilled();


    if (bVisible) {
        pMeshMaker->fillMeshByConfig(
            _verts,
            _indexes,
            pBucket,
            pAtlas, gc, _eMatterMode,
            fWidth, fHeight,
            _pGrid->getWorld25()->getConfig(),
            nullptr
        );
    }
}
void W25GridMesh::makeMeshImmediately_r(BlockNode* pParent) {
    if (pParent->getIsLeaf()) {
        for(int i=0; i<BlockNode::c_nCells; ++i){
            if(pParent->getCell(i) != nullptr) {
                makeMeshForCell(pParent->getCell(i));
            }
        }
    }
    else {
        if (pParent->getChildren() != nullptr) {
            for (size_t i = 0; i<pParent->getChildren()->size(); ++i) {
                if(pParent->getChildren()->at(i) != nullptr) {
                    makeMeshImmediately_r(pParent->getChildren()->at(i));
                }
            }
        }
    }
}
/*

//Do we need "propogatemeshdata"
// This is for physics, but we delete empty nodes anyway.


void W25GridMesh::propogateMeshData() {
   // t_timeval tv = Gu::getMicroSeconds();
    int32_t dbgTot = 0;
    propogateMeshData_r(_pGrid->getRoot(), dbgTot);
  //  _tvProp = (Gu::getMicroSeconds() - tv) / 1000;
}
void W25GridMesh::propogateMeshData_r(BlockNode* pParent, int32_t& dbg_tot) {

    if (pParent->getCell() != nullptr) {
        if(pParent->getCell()->getVisibleCell() != nullptr) {
            //Final parents only have 1 cell just copy the cell data.
            for (int iMatter = 0; iMatter < MatterMode::e::MaxMatters; ++iMatter) {
                int32_t indCount = pParent->getCell()->getVisibleCell()->getMeshIndexCount((MatterMode::e)iMatter);
                dbg_tot += indCount;
                if (indCount > 0) {
                    pParent->setIndexCount((MatterMode::e)iMatter, indCount);
                }
            }
        }
    }
    else {
        if (pParent->getChildren() != nullptr) {
            int32_t indTotal[MatterMode::e::MaxMatters];
            for (int iMatter = 0; iMatter < MatterMode::e::MaxMatters; ++iMatter) {
                indTotal[iMatter] = 0;
            }

            for (size_t i = 0; i<pParent->getChildren()->size(); ++i) {
                if(pParent->getChildren()->at(i) != nullptr) {
                    propogateMeshData_r(pParent->getChildren()->at(i), dbg_tot);

                    //Copy the propogated data.
                    for (int iMatter = 0; iMatter < MatterMode::e::MaxMatters; ++iMatter) {
                        int32_t indCount = pParent->getChildren()->at(i)->getIndexCount((MatterMode::e)iMatter);
                        if (indCount > 0) {
                            indTotal[iMatter] += indCount;
                        }
                    }
                }
            }

            for (int iMatter = 0; iMatter < MatterMode::e::MaxMatters; ++iMatter) {
                pParent->setIndexCount((MatterMode::e)iMatter, indTotal[iMatter]);
            }
        }
    }

}


*/





}//ns Game
