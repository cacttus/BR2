#include "../base/Gu.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../bottle/WorldCellFile.h"

namespace BR2 {
///////////////////////////////////////////////////////////////////
W25CompressedCellGroup::W25CompressedCellGroup() {
}
W25CompressedCellGroup::~W25CompressedCellGroup() {
}
WorldCellFile::WorldCellFile(string_t loc) : _strLoc(loc) {
  _pGlobData = new World25GlobData();
}
WorldCellFile::~WorldCellFile() {
  DEL_MEM(_pGlobData);
  clearCells();
  clearObjects();
}
WorldCellData::WorldCellData() {
}
WorldCellData::~WorldCellData() {
  int nnn = 0;
  nnn++;
}
World25ObjectData::World25ObjectData() {
}
World25ObjectData::~World25ObjectData() {
}
///////////////////////////////////////////////////////////////////
void WorldCellFile::clearObjects() {
  for (size_t iObj = 0; iObj < _vecObjects.size(); ++iObj) {
    World25ObjectData* ob = _vecObjects[iObj];
    delete ob;
  }
  _vecObjects.clear();
}
void WorldCellFile::clearCells() {
  //_CrtCheckMemory();

  for (size_t iCell = 0; iCell < _vecCells.size(); ++iCell) {
    WorldCellData* pc = _vecCells.at(iCell);
    //  _CrtCheckMemory();
    delete pc;
    //  _CrtCheckMemory();
  }
  _vecCells.clear();
}
int WorldCellFile::loadAndParse() {
  int64_t iReadSizeBytes;
  char* pOut;
  if (FileSystem::SDLFileRead(_strLoc, pOut, iReadSizeBytes, true) != 0) {
    return 1;
  }
  if (iReadSizeBytes <= 0) {
    return 1;
  }

  std::vector<char> cb(iReadSizeBytes);
  memcpy(cb.data(), pOut, iReadSizeBytes);

  parseFile(cb, iReadSizeBytes);

  FileSystem::SDLFileFree(pOut);

  return 0;
}
void WorldCellFile::testRW() {
  std::vector<char> buf;

  //TEST of read/write
  float f1 = 1987.0f, f2;
  writeFloat(buf, f1);
  uint32_t off = 0;
  readFloat(buf, off, f2);
  AssertOrThrow2(f1 == f2);

  buf.clear();
  off = 0;
  int16_t i161 = 1987, i162;
  writeInt16(buf, i161);
  readInt16(buf, off, i162);
  AssertOrThrow2(i161 == i162);

  buf.clear();
  off = 0;
  uint32_t i321 = 1987, i322;
  writeUInt32(buf, i321);
  readUInt32(buf, off, i322);
  AssertOrThrow2(i321 == i322);

  buf.clear();
  off = 0;
  int32_t si321 = 1987, si322;
  writeSInt32(buf, si321);
  readSInt32(buf, off, si322);
  AssertOrThrow2(si321 == si322);

  buf.clear();
  off = 0;
  uint8_t ui81 = 19, ui82;
  writeByte(buf, ui81);
  readByte(buf, off, ui82);
  AssertOrThrow2(ui81 == ui82);
}
string_t WorldCellFile::getFriendlyVersion(uint32_t iVersion) {
  char h1 = iVersion >> 24 & 0xFF;
  char h2 = iVersion >> 16 & 0xFF;
  char vA = iVersion >> 8 & 0xFF;
  char vI = iVersion >> 0 & 0xFF;

  string_t st;
  st += h2;
  st += h1;
  st += " v";
  st += TypeConv::intToStr((int)vA);
  st += ".";
  st += TypeConv::intToStr((int)vI);

  return st;
}
void WorldCellFile::parseFile(std::vector<char>& buf, uint32_t iDataSizeBytes) {
  uint32_t iOff = 0;
  decompress(buf);
  testRW();

  //Header
  readUInt32(buf, iOff, _iVersion);
  if (_iVersion != CONGA_GRID_FILE_VERSION) {
    BRLogError("Warning: Conga file version mismatch.");
    BRLogError("  Got: " + getFriendlyVersion(_iVersion));
    BRLogError("  Expected: " + getFriendlyVersion(CONGA_GRID_FILE_VERSION));
    Gu::debugBreak();
  }

  //GLOB DATA
  readUInt32(buf, iOff, _pGlobData->_iLairId);

  //Cell info
  readUInt32(buf, iOff, _nCellGroups);
  readUInt32(buf, iOff, _nCells);
  readUInt32(buf, iOff, _nObjects);

  //Read Groups
  parseGroups(buf, iOff);

  //Objects
  for (uint32_t i = 0; i < _nObjects; ++i) {
    World25ObjectData* ob = parseObject(buf, iDataSizeBytes, iOff);
    _vecObjects.push_back(ob);
  }

  //Make sure we didn't invalid offset somewhere
  AssertOrThrow2(iOff == iDataSizeBytes);
}
void WorldCellFile::parseGroups(std::vector<char>& buf, uint32_t& iOff) {
  std::vector<W25CompressedCellGroup*> groups;

  for (uint32_t icg = 0; icg < _nCellGroups; ++icg) {
    W25CompressedCellGroup* newg = new W25CompressedCellGroup();

    readUInt32(buf, iOff, newg->_iCount);
    readByte(buf, iOff, newg->_data._iBaseTileIdx);
    readByte(buf, iOff, newg->_data._iLiquidTileIdx);
    //  readByte(buf, iOff, newg->_data._iGasTileIdx);
    readByte(buf, iOff, newg->_data._iSolid);
    readByte(buf, iOff, newg->_data._iLiquid);
    //  readByte(buf, iOff, newg->_data._iGas);
    groups.push_back(newg);
  }

  decompressGroups(groups, _vecCells);

  //Cleanup
  for (size_t ig = 0; ig < groups.size(); ++ig) {
    delete groups[ig];
  }
  groups.clear();

  AssertOrThrow2((uint32_t)_vecCells.size() == _nCells);
}

World25ObjectData* WorldCellFile::parseObject(std::vector<char>& buf, uint32_t iDataSizeBytes, uint32_t& iOff) {
  AssertOrThrow2(iOff <= iDataSizeBytes);

  World25ObjectData* pObject = new World25ObjectData();

  readUInt32(buf, iOff, pObject->_iType);
  readFloat(buf, iOff, pObject->_vPos.x);
  readFloat(buf, iOff, pObject->_vPos.y);
  readFloat(buf, iOff, pObject->_vPos.z);

  //Swap - this is only for the grid conversion.
 // float tmp = pObject->_vPos.z;
 // pObject->_vPos.z = pObject->_vPos.y;
 // pObject->_vPos.y = -tmp;

  return pObject;
}
void WorldCellFile::save() {
  //Flatten the data to a char array
  std::vector<char> buf;
  uint32_t version = CONGA_GRID_FILE_VERSION;

  writeUInt32(buf, version);

  //GLOB DATA
  writeUInt32(buf, _pGlobData->_iLairId);

  std::vector<W25CompressedCellGroup*> groups;
  compressGroups(_vecCells, groups);

  //Cell data
  _nCellGroups = (uint32_t)groups.size();
  _nCells = (uint32_t)_vecCells.size();
  _nObjects = (uint32_t)_vecObjects.size();
  writeUInt32(buf, _nCellGroups);
  writeUInt32(buf, _nCells);
  writeUInt32(buf, _nObjects);

  //Write Groups
  for (W25CompressedCellGroup* cg : groups) {
    writeUInt32(buf, cg->_iCount);
    writeByte(buf, cg->_data._iBaseTileIdx);
    writeByte(buf, cg->_data._iLiquidTileIdx);
    //  writeByte(buf, cg->_data._iGasTileIdx);
    writeByte(buf, cg->_data._iSolid);
    writeByte(buf, cg->_data._iLiquid);
    //  writeByte(buf, cg->_data._iGas);
  }

  //Write Objects
  for (World25ObjectData* obd : _vecObjects) {
    writeUInt32(buf, obd->_iType);
    writeFloat(buf, obd->_vPos.x);
    writeFloat(buf, obd->_vPos.y);
    writeFloat(buf, obd->_vPos.z);
  }

  FileSystem::SDLFileWrite(_strLoc, buf.data(), buf.size());

  //Cleanup
  for (size_t i = 0; i < groups.size(); ++i) {
    W25CompressedCellGroup* cg = groups[i];
    delete cg;
  }
  groups.clear();
}
void WorldCellFile::writeBytes(std::vector<char>& buf, char* dat, size_t datSize) {
  char* valBuf = new char[datSize];
  memcpy(valBuf, (char*)dat, datSize);
  for (size_t i = 0; i < datSize; i++) {
    buf.push_back(valBuf[i]);
  }
  delete valBuf;
}
void WorldCellFile::readBytes(std::vector<char>& buf, uint32_t& __out_ iOff, char* __out_ ft, size_t outSize) {
  int rdSiz = outSize;
  memcpy(ft, buf.data() + iOff, rdSiz);
  iOff += rdSiz;
}
void WorldCellFile::writeFloat(std::vector<char>& buf, float& f) {
  writeBytes(buf, (char*)&f, sizeof(float));
}
void WorldCellFile::readFloat(std::vector<char>& buf, uint32_t& __out_ iOff, float& __out_ ft) {
  readBytes(buf, iOff, (char*)&ft, sizeof(float));
}
void WorldCellFile::writeByte(std::vector<char>& buf, uint8_t& f) {
  writeBytes(buf, (char*)&f, sizeof(uint8_t));
}
void WorldCellFile::readByte(std::vector<char>& buf, uint32_t& __out_ iOff, uint8_t& __out_ ft) {
  readBytes(buf, iOff, (char*)&ft, sizeof(uint8_t));
}
void WorldCellFile::writeSInt32(std::vector<char>& buf, int32_t& f) {
  writeBytes(buf, (char*)&f, sizeof(int32_t));
}
void WorldCellFile::readSInt32(std::vector<char>& buf, uint32_t& __out_ iOff, int32_t& __out_ ft) {
  readBytes(buf, iOff, (char*)&ft, sizeof(int32_t));
}
void WorldCellFile::writeUInt32(std::vector<char>& buf, uint32_t& f) {
  writeBytes(buf, (char*)&f, sizeof(uint32_t));
}
void WorldCellFile::readUInt32(std::vector<char>& buf, uint32_t& __out_ iOff, uint32_t& __out_ ft) {
  readBytes(buf, iOff, (char*)&ft, sizeof(uint32_t));
}
void WorldCellFile::writeInt16(std::vector<char>& buf, int16_t& f) {
  writeBytes(buf, (char*)&f, sizeof(int16_t));
}
void WorldCellFile::readInt16(std::vector<char>& buf, uint32_t& __out_ iOff, int16_t& __out_ ft) {
  readBytes(buf, iOff, (char*)&ft, sizeof(int16_t));
}

void WorldCellFile::compress(std::vector<char>& buf) {
}
void WorldCellFile::decompress(std::vector<char>& buf) {
}

void WorldCellFile::compressGroups(std::vector<WorldCellData*>& inCells, std::vector<W25CompressedCellGroup*>& outGroups) {
  size_t incpt = 0;
  while (incpt < inCells.size()) {
    W25CompressedCellGroup* group = new W25CompressedCellGroup();

    group->_data._iBaseTileIdx = inCells[incpt]->_iBaseTileIdx;
    group->_data._iLiquidTileIdx = inCells[incpt]->_iLiquidTileIdx;
    //  group->_data._iGasTileIdx = inCells[incpt]->_iGasTileIdx;
    group->_data._iSolid = inCells[incpt]->_iSolid;
    group->_data._iLiquid = inCells[incpt]->_iLiquid;
    // group->_data._iGas = inCells[incpt]->_iGas;

    size_t iCell;
    for (iCell = incpt + 1; iCell < inCells.size(); ++iCell) {
      if (inCells[iCell]->_iBaseTileIdx != group->_data._iBaseTileIdx ||
        inCells[iCell]->_iLiquidTileIdx != group->_data._iLiquidTileIdx ||
        // inCells[iCell]->_iGasTileIdx != group->_data._iGasTileIdx ||
        inCells[iCell]->_iSolid != group->_data._iSolid ||
        inCells[iCell]->_iLiquid != group->_data._iLiquid //||
     //   inCells[iCell]->_iGas != group->_data._iGas
        ) {
        break;
      }
    }

    group->_iCount = iCell - incpt;
    incpt = iCell;

    outGroups.push_back(group);
  }
}
void WorldCellFile::decompressGroups(std::vector<W25CompressedCellGroup*>& inGroups, std::vector<WorldCellData*>& outCells) {
  WorldCellData* pCellData;
  for (W25CompressedCellGroup* g : inGroups) {
    for (uint32_t ic = 0; ic < g->_iCount; ++ic) {
      pCellData = new WorldCellData();
      pCellData->_iBaseTileIdx = g->_data._iBaseTileIdx;
      pCellData->_iLiquidTileIdx = g->_data._iLiquidTileIdx;
      //   pCellData->_iGasTileIdx = g->_data._iGasTileIdx;
      pCellData->_iSolid = g->_data._iSolid;
      pCellData->_iLiquid = g->_data._iLiquid;
      //  pCellData->_iGas = g->_data._iGas;

      outCells.push_back(pCellData);
    }
  }

  // clearCells();
}
}//ns Game