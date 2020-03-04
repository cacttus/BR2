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
#include "../world/PhysicsGrid.h"
#include "../world/GameFile.h"


namespace BR2 {
WorldMaker::WorldMaker(std::shared_ptr<PhysicsWorld> pw, std::vector<LairSpec*>& mapLairs, std::vector<WalkerSpec*>& mapWalkers) {
  BRLogInfo("Creating WorldMaker");

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

  string_t ddir = getWorldDirectory(strGameName, strWorldName);

  ddir = FileSystem::combinePath(ddir, "/g");
  return ddir;
}
std::shared_ptr<PhysicsGrid> WorldMaker::loadGrid(ivec3(v)) {
  string_t strFileName = makeGridFileName(getGameName(), getWorldName(), v.x, v.y, v.z);

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

    pg = std::make_shared<WorldGrid>(_pWorld25, vPos, false);
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
    getGameName(),
    getWorldName(),
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

  std::shared_ptr<WorldGrid> pg;

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

    pg = std::make_shared<WorldGrid>(_pWorld25, v, false);
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
  string_t gridsDir = getWorldGridsDir(getGameName(), getWorldName());
  std::vector<string_t> files;

  FileSystem::getAllFiles(gridsDir, files);
  for (string_t file : files) {
    std::shared_ptr<PhysicsGrid> pg = loadGridFromFile(file);
    grids.insert(pg);
  }
}


void WorldMaker::constructWorld() {
  //I don't think this method is necessary now that PhysicsWorld is persistent.
  BRLogInfo("Making world.");
  {
    t_timeval t0 = Gu::getMicroSeconds();
    /*_pWorld25 = std::make_shared<World25>();
    Gu::getGraphicsContext()->setPhysicsWorld(_pWorld25);
    _pWorld25->init(_pGameFile);*/

    //_pWorld25->createNewWorld();

    BRLogInfo("Finished.." + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");
  }
}

string_t WorldMaker::getGameDirectory(string_t gameName) {
  return FileSystem::combinePath(_strGameSaveDir, gameName);
}
string_t WorldMaker::getWorldDirectory(string_t gameName, string_t worldName) {
  return FileSystem::combinePath(getGameDirectory(gameName), worldName);
}
string_t WorldMaker::getNewWorldName(string_t gameName) {
  //Get highest world number
  string_t gameDir = getGameDirectory(gameName);
  std::vector<string_t> dirs;
  FileSystem::getAllDirs(gameDir, dirs);
  int32_t iHigh = 0;
  for (string_t dirPath : dirs) {
    int32_t i;
    string_t dir = FileSystem::getDirectoryFromPath(dirPath);
    if (dir.length() > 1) {
      dir = dir.substr(1);
      if (TypeConv::strToInt(dir, i)) {
        iHigh = MathUtils::brMax(i, iHigh);
      }
      else {
        BRLogWarn("Dir '" + dir + "' is not a valid world name");
      }
    }
    else {
      BRLogWarn("Dir '" + dir + "' is not a valid world name");

    }

  }

  iHigh++;//increment for next world.

  char buf[256];
  memset(buf, 0, 256);

  const int ciWorldDigits = 6;
  string_t fmt = Stz"w%0" + ciWorldDigits + "d";
  snprintf(buf, 256, fmt.c_str(), iHigh);

  string_t wn(buf);
  return wn;
}
bool WorldMaker::loadOrCreateGame(string_t gameName) {
  unloadGame();

  _strGameName = gameName;

  string_t gameDir = getGameDirectory(gameName);
  string_t gameFileDir = FileSystem::combinePath(gameDir, "game.dat");

  if (FileSystem::directoryExists(gameDir)) {
    //later
    BRLogInfo("Loading game " + gameName);

    if (FileSystem::fileExists(gameFileDir)) {

      GameFile gf;
      gf.loadAndParse(gameFileDir);
      if (StringUtil::isEmpty(gf.getWorldName())) {
        BRLogError("World name was empty.");
        Gu::debugBreak();
        return false;
      }
      else {
        _pWorld25->createNewWorld(gf.getWorldName());
      }
    }
    else {
      BRLogError("Game file '" + gameFileDir + "' not found.");
      Gu::debugBreak();
      return false;
    }
  }
  else {
    FileSystem::createDirectoryRecursive(gameDir);

    //Create initial world
    string_t worldName = getNewWorldName(gameName);
    string_t worldPath = getWorldDirectory(gameName, worldName);
    FileSystem::createDirectoryRecursive(worldPath);

    //create game & save file
    //THIS does nothing in bottle
    //_pWorld25->initWorld(worldName);
    
    _pWorld25->createNewWorld();

    GameFile gf;
    gf.save(gameFileDir, _pWorld25);
  }

  _pWorld25->loadWorld();
  // updateGameModeChanged();
  return true;
}
void WorldMaker::unloadGame() {
  _pWorld25->unloadWorld();
}
void WorldMaker::createGameSaveDir() {
  _strGameSaveDir = makeAssetPath("games");
  // string_t ddir = "";
  if (!FileSystem::createDirectoryRecursive(_strGameSaveDir)) {
    BRLogError("Failed to create game save directory " + _strGameSaveDir);
  }
}




}//ns game

