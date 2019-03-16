/**
*
*    @file MbiFile.h
*    @date January 13, 2018
*    @author Derek Page
*
*    © 2018 
*
*
*/
#pragma once
#ifndef __MBIFILE_15158731212931798061_H__
#define __MBIFILE_15158731212931798061_H__

#include "../base/IOBase.h"
#include "../model/ModelHeader.h"

namespace Game {
/**
*    @class MbiFile
*    @brief Mob Binary file.  The main file for all models.
*
*/
class MbiFile : public VirtualMemory {
public:
    const float c_fVersion = 0.3f;


private:
    std::vector<std::shared_ptr<ModelSpec>> _vecModels;
    void parseErr(t_string str, bool bDebugBreak, bool bFatal);
     std::shared_ptr<BufferedFile> _pFile = nullptr;
    t_string _fileLoc;
    void postLoad();
public:
    std::vector<std::shared_ptr<ModelSpec>>& getModelSpecs() { return _vecModels;}

    bool loadAndParse(t_string file);
    void save( t_string file);
    MbiFile();
    virtual ~MbiFile() override;
};

}//ns Game



#endif
