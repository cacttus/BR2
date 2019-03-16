/**
*
*    @file VertexAdjacencyMap.h
*    @date June 14, 2014
*    @author Derek Page
*
*    © 2014 
*
*
*/
#pragma once
#ifndef __VERTEXADJACENCYMAP_108092740165061970217353_H__
#define __VERTEXADJACENCYMAP_108092740165061970217353_H__

#include "../model/ModelHeader.h"
//#include "../base/ExLib.h"
//#include "../topo/IndexTypes.h"
//#include "../topo/VertexFormat.h"

namespace Game {;
/**
*    @class VertexAdjacencyMap
*    @brief Mapping of vertexes to adjacent faces.  Adjacent faces are offsets into the index buffer.
*
*/
class VertexAdjacencyMap : public VirtualMemory  {
public:
    typedef std::set<v_index32> FaceList;
    typedef std::map<v_index32, FaceList* > AdjacencyMap;
private:

    AdjacencyMap* _vmap = nullptr;    // Maps vertex indexes to face indexes in a face buffer.
public:

    //void addMapping( t_index index, t_uint32 face_index);
    void addMappingIgnoreDupes(v_index32 index, t_uint32 face_index);
    FaceList* getVertexIndexMapping(v_index32 index);

    //+-- CTOR/DTOR --+
    VertexAdjacencyMap();
    virtual ~VertexAdjacencyMap() override ;
};

}//ns game



#endif
