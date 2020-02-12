/**
*
*    @file VertexAdjacencyMap.h
*    @date June 14, 2014
*    @author MetalMario971
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

namespace BR2 {
/**
*    @class VertexAdjacencyMap
*    @brief Mapping of vertexes to adjacent faces.  Adjacent faces are offsets into the index buffer.
*
*/
class VertexAdjacencyMap : public VirtualMemory {
public:
  typedef std::set<v_index32> FaceList;
  typedef std::map<v_index32, FaceList* > AdjacencyMap;
public:
  VertexAdjacencyMap();
  virtual ~VertexAdjacencyMap() override;
  //void addMapping( t_index index, uint32_t face_index);
  void addMappingIgnoreDupes(v_index32 index, uint32_t face_index);
  FaceList* getVertexIndexMapping(v_index32 index);
private:

  AdjacencyMap* _vmap = nullptr;    // Maps vertex indexes to face indexes in a face buffer.
};

}//ns game



#endif
