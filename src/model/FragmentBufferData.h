/**
*
*    @file FragmentBufferData.h
*    @date September 26, 2014
*    @author MetalMario971
*
*    © 2014
*
*
*/
#pragma once
#ifndef __FRAGMENTBUFFERDATA_3255418442216532730025046_H__
#define __FRAGMENTBUFFERDATA_3255418442216532730025046_H__

#include "../model/ModelHeader.h"
#include "../base/Allocator.h"

namespace BR2 {
/**
*  @class FragmentBufferData
*  @brief Allows access to interleaved fragment data. Stores vertexes (fragments) in a flexible buffer allowing multiple vertex formats.
*/
class FragmentBufferData : public VirtualMemory {
public:
  FragmentBufferData(std::shared_ptr<VertexFormat> vft);
  virtual ~FragmentBufferData() override;

  void allocate(size_t count);
  void deallocate();

  std::shared_ptr<VertexFormat> getFormat() { return _pVertexFormat; }
  size_t count();
  Allocator<char>* getBuf() { return _pFrags; }

  void copyFrom(std::shared_ptr<FragmentBufferData> fb);
  void copyFrom(const void* fb, int byteSize);
  void appendToEnd(std::shared_ptr<FragmentBufferData> fb);

  vec2& v2f(size_t index);//vertex
  vec3& v3f(size_t index);//vertex
  vec4& c4f(size_t index);//color
  vec3& n3f(size_t index);//normal
  vec2& x2f(size_t index);//tcoord
  const std::shared_ptr<VertexComponent> get_v3f1_component() { return _v3f1; }
  const std::shared_ptr<VertexComponent> get_n3f1_component() { return _n3f1; }
  const std::shared_ptr<VertexComponent> get_c4f1_component() { return _c4f1; }
  const std::shared_ptr<VertexComponent> get_x2f1_component() { return _x2f1; }

  void* ptr();

  // void* getVoidPtr(){ return (void*)mm_allocator<char>::ptr(); }
  //size_t getByteSize(){ return mm_allocator<char>::byteSize(); }
private:
  Allocator<char>* _pFrags = nullptr;    // - Can be any vertex type. Usually t_vertex_v3. We don't need colors in a mesh class when all meshes are going to be copied to other buffers anyways.  So we can apply colors when we need to. Of course there should be a place to store colors so if you need to go ahead and use t_vertex_x2c4n3t3v3
  std::shared_ptr<VertexComponent> _v2f1 = nullptr;
  std::shared_ptr<VertexComponent> _v3f1 = nullptr;
  std::shared_ptr<VertexComponent> _n3f1 = nullptr;
  std::shared_ptr<VertexComponent> _c4f1 = nullptr;
  std::shared_ptr<VertexComponent> _x2f1 = nullptr;
  std::shared_ptr<VertexFormat> _pVertexFormat = nullptr;
  size_t _iLastFragCount = 0;
  // - Direct memory access to based on a component index by which we can access the size of the elemement, and 
  // a type to cast the element to, and an index offset into the buffer.
  template < class Tx >
  FORCE_INLINE Tx& accessComponent(size_t index, std::shared_ptr<VertexComponent> pc) {
    return *((Tx*)getPointerToComponentAtIndex(index, pc));
  }
  void* getPointerToComponentAtIndex(size_t index, std::shared_ptr<VertexComponent> pc);

};

}//ns game



#endif
