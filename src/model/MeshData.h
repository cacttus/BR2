/**
*  @file MeshSpec.h
*  @date October 16, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __MESHSPEC_157287882194863255526453_H__
#define __MESHSPEC_157287882194863255526453_H__

#include "../model/ModelHeader.h"
#include "../math/MathAll.h"
#include "../world/WorldHeader.h"
#include "../world/NodeData.h"

namespace BR2 {
/**
*  @class MeshSpec 
*  @brief Mesh geometry storage and operations.
*  TODO: inherit from base data, move complex methods to MeshNode
*/
class MeshData : public NodeData {
public:
  MeshData(std::shared_ptr<GLContext> ct); //Serialize Version
  MeshData(std::shared_ptr<GLContext> ct, const void* cVerts, size_t vCount, const void* cIndexes, size_t iCount, std::shared_ptr<VertexFormat> fmt, std::shared_ptr<Material> pm);
  MeshData(std::shared_ptr<GLContext> ct, string_t strName, std::shared_ptr<VertexFormat> vf, std::shared_ptr<ObjFile> objFile = nullptr, std::shared_ptr<PhysicsShape> ps = nullptr);
  virtual ~MeshData() override;

  //Edit funcs.
  void beginEdit();
  v_index32& i32(size_t index);
  vec3& v3f(size_t index);
  vec2& x2f(size_t index);
  vec4& c4f(size_t index);
  vec3& n3f(size_t index);
  void endEdit();

  virtual std::shared_ptr<NodeData> clone() override;
  virtual void copy(std::shared_ptr<NodeData> other) override;

  std::vector<VertexWeightMob>& getWeightsMob() { return _vecWeightsMob; }
  std::set<std::shared_ptr<BoneNode>>& getBoneCache() { return _boneCache; }


  void allocSkinMobFile(std::shared_ptr<ModelNode> ms); //This is called separately in the MOB loader.

  bool getHideRender() { return _bHideRender; }
  void setHideRender(bool b) { _bHideRender = b; }

  bool hasSkin() { return _eSkinStatus == MeshSkinStatus::e::Allocated; }
  void allocMesh(size_t nFrags, size_t nIndexes);
  void allocMesh(const void* pFrags, size_t nFrags, const void* pIndexes, size_t nIndexes, std::vector<VertexWeightMob>* weights = nullptr);

  std::shared_ptr<PhysicsShape> getPhysicsShape() { return _pPhysicsShape; }

  size_t indexCount();
  size_t fragCount();
  size_t faceCount();

  void setMaterial(std::shared_ptr<Material> m);
  std::shared_ptr<Material> getMaterial() { return _pMaterial; }
  mat4* getLocalMatrix() { return &_matLocalMatrix; }
  std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
  void calculateVertexNormals(); //Slow - Method recalculates normals and saves to GPU buffer.

  std::shared_ptr<VaoDataGeneric> getVaoData() { return _pVaoData; }
  std::shared_ptr<FragmentBufferData> getFrags();
  std::shared_ptr<IndexBufferData> getIndexes();

  std::shared_ptr<MeshData> mergeWith(std::shared_ptr<MeshData> other, bool automaticallyRecalculateIndexOffsets = true);
  void computeBox(Box3f* __inout box);

  std::shared_ptr<ShaderStorageBuffer> getWeightOffsetsGpu() { return _pWeightOffsetsGpu; }
  std::shared_ptr<ShaderStorageBuffer> getWeightsGpu() { return _pWeightsGpu; }

  virtual void deserialize(std::shared_ptr<BinaryFile> fb) override;
  virtual void serialize(std::shared_ptr<BinaryFile> fb) override;

protected:
  std::shared_ptr<VertexFormat> _pVertexFormat = nullptr;
  std::shared_ptr<VaoDataGeneric> _pVaoData = nullptr;
  std::shared_ptr<FragmentBufferData> _pFrags = nullptr;    //vertexes
  std::shared_ptr<IndexBufferData> _pIndexes = nullptr;    //indexes
  std::shared_ptr<Material> _pMaterial = nullptr;
  std::shared_ptr<ObjFile> _pObjectFile = nullptr;
  mat4 _matLocalMatrix;   //The world matrix loaded from the mesh file. **UNUSED 2016 - freeze all meshes before export. Oterwise we'd have problems with this.
  std::vector<VertexWeightMob> _vecWeightsMob;//MOB file weights - invalid if not imported
  std::shared_ptr<ShaderStorageBuffer> _pWeightOffsetsGpu = nullptr; //Skin 12/9/2017
  std::shared_ptr<ShaderStorageBuffer> _pWeightsGpu = nullptr;//Skin 12/9/2017
  std::set<std::shared_ptr<BoneNode>> _boneCache;
  MeshSkinStatus::e _eSkinStatus = MeshSkinStatus::e::Uninitialized;
  bool _bHideRender = false;
  std::shared_ptr<PhysicsShape> _pPhysicsShape = nullptr;

  std::shared_ptr<GLContext> _pContext = nullptr;
  std::shared_ptr<GLContext> getContext() { return _pContext; }

  void fillWeightBuffersMob(std::shared_ptr<ModelNode> ms);
  int32_t getGpuJointOrdinal(std::shared_ptr<ModelNode> ms, int32_t arm, int32_t joint);
  void testAccess(std::shared_ptr<ModelNode> ms, GpuAnimatedMeshWeightData* weightOffsetsGpu, size_t weightOffsetsGpuSize,
    GpuAnimatedMeshWeight* weightsGpu, size_t weightsGpuSize, std::vector<VertexWeightMob>* vecWeights);

  int32_t vertexFormatToInt32();
  void int32ToVertexFormat(int32_t i);
  void printWeightsToStdout();
  void printWeightBuffersToStdout();
  std::shared_ptr<FragmentBufferData> copyFragsFromGpu();
  std::shared_ptr<IndexBufferData> copyIndexesFromGpu();
  void copyFragsToGpu(std::shared_ptr<FragmentBufferData>);
  void copyIndexesToGpu(std::shared_ptr<IndexBufferData>);
};



}//ns BR2



#endif
