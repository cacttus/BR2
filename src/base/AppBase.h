///**
//*  @file AppBase.h
//*  @date January 9, 2017
//*  @author MetalMario971
//*/
//#pragma once
//#ifndef __ROOMBASE_1483942071882744649_H__
//#define __ROOMBASE_1483942071882744649_H__
//
//#include "../base/AppHeader.h"
//#include "../gfx/GfxHeader.h"
//#include "../math/MathAll.h"
//
//
//namespace Game {
///**
//*  @class AppBase
//*  @brief Base class for application.
//*
//*/
//class AppBase : public Drawable {
//public:
//    AppBase();
//    virtual ~AppBase() override;
//    virtual void init() = 0; //*Note this is not called right after inst
//
//    virtual void drawDebug();
//
//    //virtual void screenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) { };
//    virtual void idle(t_timeval us) { }
//
//    virtual void updateTouch(std::shared_ptr<InputManager>, float dt) = 0;
//    virtual void userZoom(int amount) = 0;
//
//    virtual void serverPacketReceived(std::shared_ptr<Packet> temp) = 0;
//};
//
//}//ns Game
//
//
//
//#endif
