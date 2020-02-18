/**
*  @file MeshUtils.h
*  @date October 26, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __MESHCREATOR_81522344517012612529631_H__
#define __MESHCREATOR_81522344517012612529631_H__

#include "../base/BaseHeader.h"
#include "../model/ModelHeader.h"

namespace BR2 {
// MESH TYPE IN @c MeshCreator
namespace MeshGenType { typedef enum { MGA_CONE, MGA_SPHERE, MGA_CIRCLE, MGA_BOX, MGA_C2BB2S, MGA_PLANE_2_SIDE, MGA_CROSS_2_SIDE }e; }

class MeshGenParams : public VirtualMemory {
public:
  static const int MaxParams = 8;
public:
  MeshGenType::e getGenType() { return _eGenType; }
  string_t getParam(size_t i) { return _strParams[i]; }
  bool parse(string_t str);

private:
  string_t _strClass; // generation class
  MeshGenType::e _eGenType; // algorithm for generation
  string_t _strParams[MaxParams]; // params to algorithm
  uint32_t _uiParamCount = 0;
  std::vector<MeshGenParams*> _vecSubMeshParams;
  MeshGenType::e getGenerationAlgorithmFromString(string_t str);
  bool logAndExit(string_t);
};

/**
*  @class MeshMaker
*  @brief Encapsulated methods to generate meshes.
*  
*  @note Note that the MeshSpec is new'd and not freed!!
*          YOU MUST ADD THE SPEC TO THE MESH MANAGER!!!!! @cMeshFactory
*/
class MeshUtils : public VirtualMemory {
public:
  typedef v_v3c4x2n3 MeshMakerVert;
public:
  MeshUtils() {}
  virtual ~MeshUtils() override {}
  static std::shared_ptr<MeshNode> createScreenQuadMesh(std::shared_ptr<GLContext> context, int w, int h);
  static std::shared_ptr<MeshData> makeCone(std::shared_ptr<GLContext> context, float radius, float h, int32_t slices); //YOU MUST ADD THE SPEC TO THE MESH MANAGER!!!!! @cMeshFactory
  static std::shared_ptr<MeshData> makeSphere(std::shared_ptr<GLContext> context, float radius, int32_t slices, int32_t stacks, vec4* color = NULL, vec3* vOffset = NULL, bool blnDoNormals = true);//YOU MUST ADD THE SPEC TO THE MESH MANAGER!!!!! @cMeshFactory
  static std::shared_ptr<MeshData> makeArcSegment(std::shared_ptr<GLContext> context, float radius, float radius2, float refHeight, Vec3f refPos, int32_t slices);
  static std::shared_ptr<MeshData> makeBillboardXY(std::shared_ptr<GLContext> context, float xscale, float yscale, bool translateYToBottomOfBillboard = false);
  static std::shared_ptr<MeshData> makeCrossboardXY(std::shared_ptr<GLContext> context, float xscale, float yscale, bool translateYToBottomOfBillboard = false);
  static std::shared_ptr<MeshData> makeCircle(std::shared_ptr<GLContext> context, float radius, int32_t slices);
  static std::shared_ptr<MeshData> makeBox(std::shared_ptr<GLContext> context, Box3f* pCube, vec4* color, Matrix4x4* applyMat, vec3* offset);
  static std::shared_ptr<MeshData> makeBox(std::shared_ptr<GLContext> context, float length, vec4* color, mat4* applyMat, vec3* offset);
  static std::shared_ptr<MeshData> makeBox(std::shared_ptr<GLContext> context, vec3(&extents)[8], Color4f* color, Matrix4x4* applyMat, vec3* offset);
  static bool parseGenParams(string_t genString, MeshGenParams& __out_ params);
  static std::shared_ptr<MeshData> generateFromParameters(std::shared_ptr<GLContext> cam, MeshGenParams* pgp);
  static vec3* getVertexElementOffset(vec3* verts, size_t iElementIndex, size_t vOffBytes, size_t vStrideBytes, size_t vCount);
private:
  // ContactManager* _pContactManager;
  STATIC void finalizeSpec(std::shared_ptr<MeshData> ms, vec3* minV = NULL, vec3* maxV = NULL);
};





}//ns BR2



#endif
