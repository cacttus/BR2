/**
*  @file VulkanContext.h
*  @date August 10, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __VULKANCONTEXT_15654533273371902995_H__
#define __VULKANCONTEXT_15654533273371902995_H__

#include "../gfx/GraphicsContext.h"

namespace Game {
/**
*    @class VulkanContext
*    @brief
*
*/
class VulkanContext : public GraphicsContext {
public:
   // virtual bool load(std::shared_ptr<AppBase> br) override { return true; }
    //virtual void update(float delta)  override {}
    virtual bool chkErrRt(bool bDoNotBreak = false, bool doNotLog = false)  override {return true;}
    virtual bool chkErrDbg(bool bDoNotBreak = false, bool doNotLog = false) override {return true;}

    virtual void pushCullFace() {}
    virtual void popCullFace() {}
    virtual void pushBlend() {}
    virtual void popBlend() {}
    virtual void pushDepthTest() {}
    virtual void popDepthTest() {}

    VulkanContext();
    virtual ~VulkanContext() override;
};

}//ns Game



#endif
