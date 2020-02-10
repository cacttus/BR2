/**
*
*    @file MeshSpec.h
*    @date October 16, 2011
*    @author MetalMario971
*
*    � 2011
*
*
*/
#pragma once
#ifndef __MESHSPEC_157287882194863255526453_H__
#define __MESHSPEC_157287882194863255526453_H__


#include "../model/ModelHeader.h"
#include "../math/MathAll.h"
#include "../model/BaseNode.h"
#include "../world/WorldHeader.h"

namespace Game {
/**
*    @class MeshSpec
*    @brief Describes a geometry mesh.
*    @details 1/26/18 all data is now on GPU
*/
class MeshData : public BaseSpec {
public:
  MeshData() {}//Serialize Version
  MeshData(const void* cVerts, size_t vCount, const void* cIndexes, size_t iCount, std::shared_ptr<VertexFormat> fmt, std::shared_ptr<Material> pm);
  MeshData(t_string strName, std::shared_ptr<VertexFormat> vf, std::shared_ptr<ObjectFile> objFile = nullptr, std::shared_ptr<PhysicsShape> ps = nullptr);
  virtual ~MeshData() override;

  //Edit funcs.
  void beginEdit();
  v_index32& i32(size_t index);
  vec3& v3f(size_t index);
  vec2& x2f(size_t index);
  vec4& c4f(size_t index);
  vec3& n3f(size_t index);
  void endEdit();

  std::vector<VertexWeightMob>& getWeightsMob() { return _vecWeightsMob; }
  std::set<std::shared_ptr<BoneSpec>>& getBoneCache() { return _boneCache; }

  void allocSkinMobFile(std::shared_ptr<ModelSpec> ms); //This is called separately in the MOB loader.

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

  std::shared_ptr<MeshData> createCopy();// Duplicates this spec data
  std::shared_ptr<MeshData> mergeWith(std::shared_ptr<MeshData> other, bool automaticallyRecalculateIndexOffsets = true);
  void computeBox();

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

  //OBJ file stuff
  std::shared_ptr<ObjectFile> _pObjectFile = nullptr;
  mat4 _matLocalMatrix;   //The world matrix loaded from the mesh file. **UNUSED 2016 - freeze all meshes before export. Oterwise we'd have problems with this.
  //bool _bIsBoundBoxCalculated;

  std::vector<VertexWeightMob> _vecWeightsMob;//MOB file weights - invalid if not imported
  std::shared_ptr<ShaderStorageBuffer> _pWeightOffsetsGpu = nullptr; //Skin 12/9/2017
  std::shared_ptr<ShaderStorageBuffer> _pWeightsGpu = nullptr;//Skin 12/9/2017
  //std::shared_ptr<VboData> _pVertsGpu = nullptr;//Skin 12/9/2017
  std::set<std::shared_ptr<BoneSpec>> _boneCache;
  MeshSkinStatus::e _eSkinStatus = MeshSkinStatus::e::Uninitialized;
  bool _bHideRender = false;
  std::shared_ptr<PhysicsShape> _pPhysicsShape = nullptr;

  void fillWeightBuffersMob(std::shared_ptr<ModelSpec> ms);
  //void collectArmatures(std::shared_ptr<ModelSpec> ms);
  int32_t getGpuJointOrdinal(std::shared_ptr<ModelSpec> ms, int32_t arm, int32_t joint);
  void testAccess(std::shared_ptr<ModelSpec> ms, GpuAnimatedMeshWeightData* weightOffsetsGpu, size_t weightOffsetsGpuSize,
    GpuAnimatedMeshWeight* weightsGpu, size_t weightsGpuSize, std::vector<VertexWeightMob>* vecWeights);
  int32_t vertexFormatToInt32();
  void int32ToVertexFormat(int32_t i);
  //void sendVertsToGpu() ;
  void printWeightsToStdout();
  void printWeightBuffersToStdout();
  std::shared_ptr<FragmentBufferData> copyFragsFromGpu();
  std::shared_ptr<IndexBufferData> copyIndexesFromGpu();
  void copyFragsToGpu(std::shared_ptr<FragmentBufferData>);
  void copyIndexesToGpu(std::shared_ptr<IndexBufferData>);
};



}//ns game



#endif