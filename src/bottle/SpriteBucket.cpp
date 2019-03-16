#include "../base/Hash.h"
#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/GLContext.h"
#include "../base/RoomBase.h"
#include "../base/PoundFile.h"
#include "../base/Img32.h"

#include "../display/Atlas.h"

#include "../bottle/SpriteBucket.h"
#include "../bottle/ObFile.h"
#include "../bottle/Tile25Spec.h"

namespace Game {

///////////////////////////////////////////////////////////////////
SpriteBucket::SpriteBucket() {}
SpriteBucket::~SpriteBucket() {
    SpriteSpecMap::iterator itSpec = _mapSpecs.begin();
    //for (; itSpec != _mapSpecs.end(); itSpec++) {
    //    std::shared_ptr<SpriteSpec> sp = itSpec->second;
    //    DEL_MEM(sp);
    //}
    _mapSpecs.clear();
    for (std::pair<uint8_t, Tile25Spec*> p : _mapTiles) {
        Tile25Spec* sp = p.second;
        DEL_MEM(sp);
    }
    _mapTiles.clear();
}
void SpriteBucket::getUniqueFrameMap(std::map<Hash32, SpriteFrame*>& __out_ frameMap){
    frameMap.clear();
    for (std::pair<Hash32, std::shared_ptr<SpriteSpec>> p : _mapSpecs) {
        std::shared_ptr<SpriteSpec> ps = p.second;

        for (SpriteFrame* pf : ps->getFrames()) {
            frameMap.insert(std::make_pair(pf->getHash(), pf));
        }
    }
}
void SpriteBucket::addToAtlas(std::shared_ptr<Atlas> atlas) {
    //Build unique frames first
    std::map<Hash32, SpriteFrame*> frameMap;
    getUniqueFrameMap(frameMap);

    //Add unique frames to the atlas.
    std::map<Hash32, SpriteFrame*>::iterator it = frameMap.begin();
    for (; it != frameMap.end(); it++) {
        //These should match.  Dumb check but whevetver.
        AssertOrThrow2(it->first == it->second->getHash());

        if(it->second->getGenerated() != nullptr) { 
            atlas->addImage(it->second->getHash(), it->second->getImageName(), it->second->getGenerated());
        }
        else {
            atlas->addImage(it->second->getHash(), it->second->getImageName());
        }
    }
}
int32_t SpriteBucket::getNumUniqueFrames() {
    std::map<Hash32, SpriteFrame*> frameMap;
    getUniqueFrameMap(frameMap);

    return (int32_t)frameMap.size();
}
void SpriteBucket::addSpec(std::shared_ptr<SpriteSpec> sp)
{
    Hash32 hash = sp->getNameHashed();
    _mapSpecs.insert(std::make_pair(hash, sp));

}
std::shared_ptr<SpriteSpec> SpriteBucket::getSpecByName(t_string name)
{
    Hash32 h = STRHASH(name);
    return getSpecByName(h);
}
std::shared_ptr<SpriteSpec> SpriteBucket::getSpecByName(Hash32 h)
{
    SpriteSpecMap::iterator it = _mapSpecs.find(h);
    if (it != _mapSpecs.end()) {
        return it->second;
    }
    return nullptr;
}
void SpriteBucket::loadMotions(std::shared_ptr<ObFile> obFile) {
    BroLogInfo("Loading Sprites and Motions.");

    for(std::shared_ptr<SpriteSpec> ps : obFile->getMotions()){
        addSpec(ps);
    }

    for (Tile25Spec* ps : obFile->getTiles()) {
        TileMap::iterator it = _mapTiles.find(ps->getTileIndex());
        if(it!=_mapTiles.end()){
            BroLogError("Duplicate tile Index", ps->getTileIndex()," tried to be added to motion bucket.");
                Gu::debugBreak();
        }
        else{
            _mapTiles.insert(std::make_pair(ps->getTileIndex(), ps));
        }
    }

    //Process liquid motions.


    //Update masks
    SpriteSpecMap::iterator itSpec = _mapSpecs.begin();
    for (; itSpec != _mapSpecs.end(); itSpec++) {
        std::shared_ptr<SpriteSpec> sp = itSpec->second;
    //    if(sp->getMaskIndex() != TILE_MASK_NONE) {
            if(StringUtil::equalsi(sp->getName(), "tx-blank")) {
                if(_pBlank != nullptr){
                    BroLogWarn("Multiple blank tile IDs specified.");
                    Gu::debugBreak();
                }
                _pBlank = sp;
            }
    //        else if(sp->getSolidTileIndex() != TILE_MASK_NONE) {
    //            // If we are a base tile we MUST have an associatd mask
    //            MotionSpecMap::iterator itSpec2 = _mapSpecs.begin();
    //            for (; itSpec2 != _mapSpecs.end(); itSpec2++) {
    //                Motion25Spec* sp2 = itSpec2->second;
    //                if(sp2->getSolidTileIndex() == 0 && sp2->getLiquidTileIndex() == 0 &&
    //                    sp2->getMaskIndex() == sp->getMaskIndex()) {
    //                    if(sp->getMaskSpec() != nullptr){
    //                        BroLogWarn("Multiple Masks specified for tile.");
    //                        Gu::debugBreak();
    //                    }
    //                    sp->setMask(sp2);
    //                }
    //            }

    //            if(sp->getMaskSpec() == nullptr){
    //                BroThrowException("Base tile ",sp->getName()," did not have a mask.  Please specify a mask ID for the base tile.");
    //            }
    //        }
    //    }
    }

    if(_pBlank == nullptr){
        BroThrowException("Blank tile was not specified.  Please specify the name as 'tx-blank' to indicate the blank tile.");
    }

}
//Motion25Spec* Motion25Bucket::getSolidTileById(TileId8 id) {
//    if(id== NO_CELL_SPRITE) {
//        return nullptr;
//    }
//    MotionSpecMap::iterator itSpec = _mapSpecs.begin();
//    for (; itSpec != _mapSpecs.end(); itSpec++) {
//        Motion25Spec* sp = itSpec->second;
//        if(sp->getSolidTileIndex() == id){
//            return sp;
//        }
//    }
//    return nullptr;
//}
//Motion25Spec* Motion25Bucket::getLiquidTileById(TileId8 id) {
//    if (id == NO_CELL_SPRITE) {
//        return nullptr;
//    }
//    MotionSpecMap::iterator itSpec = _mapSpecs.begin();
//    for (; itSpec != _mapSpecs.end(); itSpec++) {
//        Motion25Spec* sp = itSpec->second;
//        if (sp->getLiquidTileIndex() == id) {
//            return sp;
//        }
//    }
//    return nullptr;
//}
//Motion25Spec* Motion25Bucket::getNineTileById(TileId8 id) {
//    if (id == NO_CELL_SPRITE) {
//        return nullptr;
//    }
//    SpecMap::iterator itSpec = _mapSpecs.begin();
//    for (; itSpec != _mapSpecs.end(); itSpec++) {
//        Motion25Spec* sp = itSpec->second;
//        if (sp->getNineTileIndex() == id) {
//            return sp;
//        }
//    }
//    return nullptr;
//}
void SpriteBucket::update(float dt) {
    for (std::pair<uint8_t, Tile25Spec*> p : _mapTiles) {
        p.second->getTop()->update(dt);
        p.second->getSide()->update(dt);
        p.second->getBot()->update(dt);
    }
}

void SpriteBucket::getGpuTiles(std::shared_ptr<Atlas> pa, GpuTile __out_ (&gpuTiles)[W25_MAX_GPU_SPRITES]) {
    for(int i=0; i<W25_MAX_GPU_SPRITES; ++i){
        gpuTiles[i]._iOff = -1;
    }
    for(std::pair<uint8_t, Tile25Spec*> p : _mapTiles) {
        Sprite* m0 = p.second->getTop();
        Sprite* m1 = p.second->getSide();
        Sprite* m2 = p.second->getBot();

        setBufferTileMotion(pa, m0, gpuTiles);
        setBufferTileMotion(pa, m1, gpuTiles);
        setBufferTileMotion(pa, m2, gpuTiles);
    }
}
void SpriteBucket::setBufferTileMotion(std::shared_ptr<Atlas> pa, Sprite* mot, GpuTile __out_(&gpuTiles)[W25_MAX_GPU_SPRITES]){
    int32_t ind = mot->getGpuIndex();
    Hash32 frameId = mot->getFrameAtlasIndex();
    int32_t tileOff = pa->getLinearTileOffset(frameId);

    //This tells us whether we're overwriting stuff.
    AssertOrThrow2(gpuTiles[ind]._iOff == -1);

    AssertOrThrow2(ind >= 0 && ind < W25_MAX_GPU_SPRITES);
    gpuTiles[ind]._iOff = tileOff;

}
Tile25Spec* SpriteBucket::getDefaultTile(GridMeshLayer::e mode) {
    for (std::pair<uint8_t, Tile25Spec*> p : _mapTiles) {
        if(p.second->getMatterMode() == mode){
            return p.second;
        }
    }
    return nullptr;
}
Tile25Spec* SpriteBucket::getTileById(uint8_t id) {
    TileMap::iterator ite = _mapTiles.find(id);
    if (ite != _mapTiles.end()) {
        return ite->second;
    }
    return nullptr;
}
Tile25Spec* SpriteBucket::getTileByName(t_string name) {
    Hash32 h = STRHASH(name);
    for (TileMap::iterator ite = _mapTiles.begin(); ite != _mapTiles.end(); ite++) {
        if (ite->second->getHashName() == h) { return ite->second; }
    }
    return nullptr;
}
////////////////////////1///////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
SpriteSpec::SpriteSpec(std::shared_ptr<SpriteBucket> pBucket, t_string strName, bool bRandomSelect) :
    _strName(strName), _bRandomSelect(bRandomSelect), _pBucket(pBucket) {
    _iNameHashed = STRHASH(_strName);
}
SpriteSpec::SpriteSpec(std::shared_ptr<SpriteBucket> pBucket, t_string strName,
    std::vector<t_string>& frames, float fDefaultDuration, bool bDefaultLoop, bool bRandomSelect) :
    SpriteSpec(pBucket, strName, bRandomSelect) {

    _fDefaultDuration = fDefaultDuration;
    _bDefaultLoop = bDefaultLoop;

    for(size_t i=0; i<frames.size(); ++i){
        SpriteFrame* pFrame = new SpriteFrame(frames[i]);
        _vecFrames.push_back(pFrame);
    }


}

SpriteSpec::~SpriteSpec() {
    for (size_t i = 0; i<_vecFrames.size(); ++i) {
        DEL_MEM(_vecFrames[i]);
    }
}
void SpriteSpec::addGeneratedFrame(t_string strName, std::shared_ptr<Img32> bi){
    SpriteFrame* pFrame = new SpriteFrame(strName, bi);
    _vecFrames.push_back(pFrame);
}
t_string SpriteSpec::getFirstFrameFileName() { 
    AssertOrThrow2(_vecFrames.size() > 0);
    return _vecFrames[0]->getImageName();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
SpriteFrame::SpriteFrame(t_string img) {
    _iHash = STRHASH(img);
    _strImage = img;
}
SpriteFrame::SpriteFrame(t_string img, std::shared_ptr<Img32> pGen) : SpriteFrame(img) {
    _pGenerated = pGen;
}
SpriteFrame::~SpriteFrame() {
    _pGenerated=nullptr;
    //if(_pGenerated != nullptr){
    //    //DEL_MEM(_pGenerated);
    //}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite(std::shared_ptr<SpriteSpec> bu) : Sprite(bu, bu->getDefaultDuration(), bu->getDefaultLoop()) {
}
Sprite::Sprite(std::shared_ptr<SpriteSpec> bu, float durationSeconds, bool bLoop) :
    _fDurationSeconds(durationSeconds),
    _bLoop(bLoop),
    _pSpec(bu)
{

}
Sprite::~Sprite() {
}
void Sprite::update(float delta) {
    _fCurTime += delta;


    if(_pSpec->getRandomSelect() == true) {
        if (_fCurTime >= _fDurationSeconds) {
            _fCurTime = fmodf(_fCurTime, _fDurationSeconds);
            //Randomly select the next frame not including the current one.
            if(_pSpec->getFrames().size() > 1){
                std::vector<size_t> vi;
                for(size_t iFrame=0; iFrame < _pSpec->getFrames().size(); ++iFrame) {
                    if(_iCurrentFrameIndex != iFrame){
                        vi.push_back(iFrame);
                    }
                }
                size_t ind = rand() % vi.size();
                _iCurrentFrameIndex = vi.at(ind);
            }
            else
            {
                _iCurrentFrameIndex = 0;
            }
        }
    }
    else {

        if (_pSpec->numFrames() <= 1 ) {
            _iCurrentFrameIndex = 0;
        }
        else if(_fDurationSeconds == 0){
            //fDurationseconds can't be zero  * we end up with a divide by zero.
            _iCurrentFrameIndex = 0;
        }
        else {
            if (_fCurTime >= _fDurationSeconds) {
                if (_bLoop) {
                    _fCurTime = fmodf(_fCurTime, _fDurationSeconds);
                }
                else {
                    _fCurTime = _fDurationSeconds;
                }
            }

            float secondsPerFrame = _fDurationSeconds / (float)_pSpec->numFrames();
            if(secondsPerFrame == 0.0f){
                secondsPerFrame = 1;
                //error
                Gu::debugBreak();

            }
            _iCurrentFrameIndex = (int)(_fCurTime / secondsPerFrame);

            if(_bLoop==false){
                if(_iCurrentFrameIndex == _pSpec->getFrames().size()){
                    _iCurrentFrameIndex = _pSpec->getFrames().size() - 1;
                }
            }
        }
    }
}
Hash32 Sprite::getFrameAtlasIndex() {
    if (_iCurrentFrameIndex < (int32_t)_pSpec->getFrames().size() && _iCurrentFrameIndex>=0) {
        return _pSpec->getFrames()[_iCurrentFrameIndex]->getHash();
    }
    else {
        //Error
        Gu::debugBreak();
    }

    return 0;
}




}//ns Game
