/**
*
*    @file GameSaveFile.h
*    @date December 22, 2017
*    @author MetalMario971
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __GAMESAVEFILE_15139585672481540946_H__
#define __GAMESAVEFILE_15139585672481540946_H__

#include "../base/PoundFile.h"
#include "../bottle/BottleHeader.h"

namespace Game {
/**
*    @class GameSaveFile
*    @brief
*
*/
class GameFile : public PoundFile {
    //Inherited
    virtual void pkp(std::vector<t_string>& tokens);
    virtual void preLoad();
    virtual void postLoad();

    t_string _strWorldName;
public:
    GameFile();
    virtual ~GameFile() override;

    t_string getWorldName() { return _strWorldName;}
    OVERRIDES void save(t_string loc, std::shared_ptr<World25> pw); 

};

}//ns Game



#endif
