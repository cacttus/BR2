/**
*  @file GameSaveFile.h
*  @date December 22, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __GAMESAVEFILE_15139585672481540946_H__
#define __GAMESAVEFILE_15139585672481540946_H__

#include "../base/PoundFile.h"
#include "../bottle/BottleHeader.h"

namespace BR2 {
/**
*  @class GameFile
*  @brief 
*/
class GameFile : public PoundFile {
public:
  GameFile();
  virtual ~GameFile() override;

  string_t getWorldName() { return _strWorldName; }
  virtual void save(string_t loc, std::shared_ptr<World25> pw);
private:
  string_t _strWorldName;

  //Inherited
  virtual void pkp(std::vector<string_t>& tokens);
  virtual void preLoad();
  virtual void postLoad();
};

}//ns Game



#endif
