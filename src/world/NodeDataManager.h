/**
*  @file NodeDataManager.h
*  @date 20200219
*  @author MetalMario971
*/
#pragma once
#ifndef __NODEDATAMANAGER_15821500662524728098_H__
#define __NODEDATAMANAGER_15821500662524728098_H__


namespace BR2 {
/**
*  @class NodeDataManager
*  @brief
*/
class NodeDataManager : public VirtualMemoryShared<NodeDataManager> {
public:
    NodeDataManager();
    virtual ~NodeDataManager() override;
};

}//ns Game



#endif
