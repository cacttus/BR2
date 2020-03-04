/**
*  @file WorldCellFile.h
*  @date March 13, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLD25CELLFILE_14894217733518605026_H__
#define __WORLD25CELLFILE_14894217733518605026_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
class World25GlobData : public VirtualMemory {
  friend class WorldCellFile;
  Hash32 _iLairId = 0; ///The lair that this glob belongs to
public:
  Hash32 getLairId() { return _iLairId; }
  void setLairId(Hash32 h) { _iLairId = h; }
};

class World25ObjectData : public VirtualMemory {
public:
  uint32_t _iType; // This is a pix obj type serialized into 255 objects.
                  //Other props should probably be in a KVP  MUST BE 32 bits because we hash it
  vec3 _vPos;
  World25ObjectData();
  ~World25ObjectData();
};
class WorldCellData : public VirtualMemory {
public:
  //**NOTE: we perform a copy on this data so make sure it has a deep copy constructor!

  W25Tile _iBaseTileIdx;  // 1 byte
  W25Tile _iLiquidTileIdx;
  //  W25Tile _iGasTileIdx;

    //TODO: - side, bottom
    //8 - base, 8 - side, 8 - bot
  W25Geom _iSolid; //  1 byte
  W25Geom _iLiquid; //  1 byte
 // W25Geom _iGas;

  WorldCellData();
  ~WorldCellData();
};
class W25CompressedCellGroup : public VirtualMemory {
public:
  uint32_t _iCount;
  WorldCellData _data;
  W25CompressedCellGroup();
  ~W25CompressedCellGroup();
};
class WorldCellFile : public VirtualMemory {
  //WorldCellData* parseCell(std::vector<char>& buf, uint32_t iDataSizeBytes, uint32_t &iOff);

  static const int VersionSizeBytes = sizeof(int32_t);
  string_t _strLoc;

  uint32_t _iVersion;

  std::vector<WorldCellData*> _vecCells;
  std::vector<World25ObjectData*> _vecObjects;
  World25GlobData* _pGlobData;

  uint32_t _nCellGroups = 0;
  uint32_t _nCells = 0;
  uint32_t _nObjects = 0;

  //void writeData(std::vector<char>& buf, char* buf, int bufsize);
  //void readData(char* raw, uint32_t& __out_ iOff, float& __out_ ft);
  void testRW();

  void writeFloat(std::vector<char>& buf, float& f);
  void readFloat(std::vector<char>& buf, uint32_t& __out_ iOff, float& __out_ ft);

  void writeByte(std::vector<char>& buf, uint8_t& f);
  void readByte(std::vector<char>& buf, uint32_t& __out_ iOff, uint8_t& __out_ ft);

  void writeSInt32(std::vector<char>& buf, int32_t& f);
  void readSInt32(std::vector<char>& buf, uint32_t& __out_ iOff, int32_t& __out_ ft);

  void writeUInt32(std::vector<char>& buf, uint32_t& f);
  void readUInt32(std::vector<char>& buf, uint32_t& __out_ iOff, uint32_t& __out_ ft);

  void writeInt16(std::vector<char>& buf, int16_t& f);
  void readInt16(std::vector<char>& buf, uint32_t& __out_ iOff, int16_t& __out_ ft);

  void writeBytes(std::vector<char>& buf, char* dat, size_t datSize);
  void readBytes(std::vector<char>& buf, uint32_t& __out_ iOff, char* __out_ ft, size_t outSize);
  string_t getFriendlyVersion(uint32_t iVersion);

  void compress(std::vector<char>& buf);
  void decompress(std::vector<char>& buf);

  World25ObjectData* parseObject(std::vector<char>& buf, uint32_t iDataSizeBytes, uint32_t& iOff);
  void parseFile(std::vector<char>& buf, uint32_t iDataSizeBytes);

  void compressGroups(std::vector<WorldCellData*>& inCells, std::vector<W25CompressedCellGroup*>& outGroups);
  void decompressGroups(std::vector<W25CompressedCellGroup*>& inGroups, std::vector<WorldCellData*>& outCells);
  void parseGroups(std::vector<char>& buf, uint32_t& iOff);
public:
  void clearObjects();
  void clearCells();
  std::vector<WorldCellData*>& getCells() { return _vecCells; }
  std::vector<World25ObjectData*>& getObjects() { return _vecObjects; }

  World25GlobData* getGlobData() { return _pGlobData; }
  WorldCellFile(string_t loc);
  ~WorldCellFile();

  int loadAndParse();

  void save();
};
}//ns Game

#endif
