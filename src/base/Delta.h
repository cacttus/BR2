/**
*  @file Delta.h
*  @date November 5, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __DELTA_14783644163757421765_H__
#define __DELTA_14783644163757421765_H__

#include "../base/BaseHeader.h"


namespace Game {

/**
*    @class Delta
*    @brief Store and retrieve the frame delta per thread
*
*/
class Delta : public GameMemory {
private:
    Uint64 _iDeltaNow=0, _iDeltaLast=0;
    double _fDelta = 0;

public:
    Delta();
    virtual ~Delta();
   
//    std::shared_ptr<Viewport> getViewport() { return _pViewport; }

    //void init();
    void update();// updateDelta(); //**Call before step()
    float get() { return (float)_fDelta; }
    //GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path);
    //void makeTestVao();
    //void userZoom(int iAmount);
    //void updateTouch(std::shared_ptr<Fingers> pInput);
    
};

}//ns Game



#endif
