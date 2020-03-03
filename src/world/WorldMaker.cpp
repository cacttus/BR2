#include "../base/Exception.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../world/WorldMaker.h"
#include "../world/PhysicsWorld.h"
#include "../world/WorldMaker.h"
#include "../world/Lair.h"
//#include "../world/World25.h"
#include "../world/W25Config.h"
//#include "../world/BottleRoom.h"
#include "../world/Lair.h"
#include "../world/SpriteBucket.h"
#include "../world/Tile25Spec.h"
#include "../world/WorldCellFile.h"
#include "../world/WorldGrid.h"
#include "../world/WipGrid.h"


namespace BR2 {
WorldMaker::WorldMaker(std::shared_ptr<PhysicsWorld> pw, std::vector<LairSpec*>& mapLairs, std::vector<WalkerSpec*>& mapWalkers) {
  _pMotionBucket = pb;
  _pWorld25 = pw;

  if (mapLairs.size() == 0) {
    BRThrowException("There were no lairs.");
  }
  if (mapWalkers.size() == 0) {
    BRThrowException("There were no Walkers.");
  }
  for (LairSpec* sp : mapLairs) {
    _mapLairSpecs.insert(std::make_pair(sp->getLairId(), sp));

    if (sp->getDefault() == true) {
      _pDefaultLair = sp;
    }
  }
  for (WalkerSpec* sp : mapWalkers) {
    _mapWalkerSpecs.insert(sp);
  }

  if (_pDefaultLair == nullptr) {
    BRThrowException("Default lair (LA_DEFAULT) was not set..");
  }

}
WorldMaker::~WorldMaker() {
  std::map<uint32_t, LairSpec*>::iterator ite = _mapLairSpecs.begin();
  for (; ite != _mapLairSpecs.end(); ite++) {
    LairSpec* s = ite->second;
    delete s;
  }
}
void WorldMaker::initialize() {
}
LairSpec* WorldMaker::getLairSpecById(Hash32 id) {
  std::map<Hash32, LairSpec*>::iterator it = _mapLairSpecs.find(id);
  if (it != _mapLairSpecs.end()) {
    return it->second;
  }
  return nullptr;
}
void WorldMaker::selectRandomLair() {
  //select a default lair
  std::vector<LairSpec*> vecs;
  for (std::pair<Hash32, LairSpec*> p : getLairSpecs()) {
    vecs.push_back(p.second);
  }

  size_t rnd = (size_t)Random::nextUint32(0, (uint32_t)vecs.size());
  _pCurrentLair = vecs[rnd];

  vecs.resize(0);
}
string_t WorldMaker::makeGridFileName(string_t strGameName, string_t strWorldName, int32_t ix, int32_t iy, int32_t iz) {

  char buf[256];
  memset(buf, 0, 256);


  //int32_t x = _viPos.x;
  //int32_t y = _viPos.y;
  //int32_t z = _viPos.z;

  //TEST:
  string_t fmt = Stz
    "%+0" + c_iGlobDigits + "d" +
    "%+0" + c_iGlobDigits + "d" +
    "%+0" + c_iGlobDigits + "d"
    ;
  //"%+09d%+09d%+09d.g"

  snprintf(buf, 256, fmt.c_str(), ix, iy, iz);

#ifdef __APPLE__
  //TODO: - apple won't let us save here
  BRThrowException("Save dir for apple wasn't set.  Not an error, just put this in.");
#elif __WINDOWS__
  //    saveDir = "./data-dc";
#endif

  string_t ddir = getWorldGridsDir(strGameName, strWorldName);



  FileSystem::createDirectoryRecursive(ddir);

  fmt = ddir + "/" + buf + ".g";

  return fmt;

}
string_t WorldMaker::getWorldGridsDir(string_t strGameName, string_t strWorldName) {
  if (StringUtil::isEmpty(strGameName)) {
    BRThrowException("Game name was not selected.");
  }
  if (StringUtil::isEmpty(strWorldName)) {
    BRThrowException("World name was not selected.");
  }

  string_t ddir =
    _pWorld25->getRoom()->getWorldDirectory(
      strGameName,
      strWorldName);

  ddir = FileSystem::combinePath(ddir, "/g");
  return ddir;
}
std::shared_ptr<PhysicsGrid> WorldMaker::loadGrid(ivec3(v)) {
  string_t strFileName = makeGridFileName(_pWorld25->getRoom()->getGameName(), _pWorld25->getWorldName(), v.x, v.y, v.z);

  return loadGridFromFile(strFileName);
}
std::shared_ptr<PhysicsGrid> WorldMaker::loadGridFromFile(string_t strFileNamePath) {
  ivec3 vPos;
  std::shared_ptr<PhysicsGrid> pg = nullptr;
  if (FileSystem::fileExists(strFileNamePath)) {
    WorldCellFile wFile(strFileNamePath);

    if (wFile.loadAndParse() != 0) {
      return nullptr;
    }
    string_t strFile = FileSystem::getFileNameFromPath(strFileNamePath);
    string_t i0 = strFile.substr(0, c_iGlobDigits);
    string_t i1 = strFile.substr(c_iGlobDigits, c_iGlobDigits);
    string_t i2 = strFile.substr(c_iGlobDigits * 2, c_iGlobDigits);
    vPos.construct(TypeConv::strToInt(i0), TypeConv::strToInt(i1), TypeConv::strToInt(i2));

    pg = std::make_shared<PhysicsGrid>(_pWorld25, vPos, false);
    pg->initSync();
    pg->expand();
    pg->createAllCells(&wFile, World25GridGen::e::Load, nullptr);

    for (World25ObjectData* obData : wFile.getObjects()) {
      _pWorld25->createObjFromFile(obData);//, pCell, iObj);
    }
  }

  return pg;
}
void WorldMaker::saveGrid(std::shared_ptr<PhysicsGrid> g) {
  //BRLogDebugBeg("Saving..");
  //t_timeval tv = Gu::getMicroSeconds();
  //{
  string_t st = makeGridFileName(
    _pWorld25->getRoom()->getGameName(),
    _pWorld25->getWorldName(),
    g->getGridPos().x,
    g->getGridPos().y,
    g->getGridPos().z);

  WorldCellFile wFile(st);

  wFile.clearObjects();
  wFile.clearCells();

  //if (bConsolidateAndExpand) {
  g->expand(); // grid MUST be expanded.
  //}
  g->getCellData(nullptr, &wFile);
  g->getObjData(&wFile);

  AssertOrThrow2(_pCurrentLair != nullptr);//We must have a lair.

  wFile.getGlobData()->setLairId(_pCurrentLair->getLairId());
  wFile.save();

  //if (bConsolidateAndExpand) {
  g->consolidate();
  //}
// }
// _tvSave = (Gu::getMicroSeconds() - tv) / 1000;
}

void WorldMaker::makeNewWorld() {
  //std::vector<ivec3> vv{ ivec3(0,-1,0), ivec3(-1,-1,0), ivec3(0,-1,-1), ivec3(-1,-1,-1) };
  std::vector<ivec3> vv;//{ ivec3(0,-1,0), ivec3(-1,-1,0), ivec3(0,-1,-1), ivec3(-1,-1,-1) };
/*  int xzSize = 1;
  int ySize = 1;
  for (int xx = -xzSize; xx <= xzSize*2; xx++)
  for (int yy = -ySize;  yy <0         ; yy++)
  for (int zz = -xzSize; zz <= xzSize*2; zz++)
  {
      vv.push_back(ivec3(xx,yy,zz));
  }*/

  vv.push_back(ivec3(0, 0, 0));

  std::shared_ptr<PhysicsGrid> pg;

  _pCurrentLair = _mapLairSpecs.begin()->second;

  AssertOrThrow2(_pCurrentLair != nullptr);

  Tile25Spec* tileGrass = _pWorld25->getSpriteBucket()->getTileByName("t-grass");
  if (tileGrass == nullptr) {
    //not found
    Gu::debugBreak();
  }
  for (ivec3 v : vv) {
    WipGrid* wg = new WipGrid(v, this->_pCurrentLair, 0);
    wg->clear(tileGrass);

    pg = std::make_shared<PhysicsGrid>(_pWorld25, v, false);
    pg->initSync();
    pg->expand();

    pg->createAllCells(nullptr, World25GridGen::e::GenAndSave, wg);

    saveGrid(pg);

    pg = nullptr;
    //delete pg;
    //grids.insert(pg);
  }


}
void WorldMaker::loadAllGrids(std::set<std::shared_ptr<PhysicsGrid>>& __out_ grids) {
  string_t gridsDir = getWorldGridsDir(_pWorld25->getRoom()->getGameName(), _pWorld25->getWorldName());
  std::vector<string_t> files;

  FileSystem::getAllFiles(gridsDir, files);
  for (string_t file : files) {
    std::shared_ptr<PhysicsGrid> pg = loadGridFromFile(file);
    grids.insert(pg);
  }

}


}//ns game

