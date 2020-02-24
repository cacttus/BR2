/**
*  @file MeshCreator.h
*  @date October 26, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __MESHCREATOR_81522344517012612529631_H__
#define __MESHCREATOR_81522344517012612529631_H__

#include "../base/BaseHeader.h"
#include "../model/ModelHeader.h"

namespace Game {
namespace MeshGenType { typedef enum { MGA_CONE, MGA_SPHERE, MGA_CIRCLE, MGA_BOX, MGA_C2BB2S, MGA_PLANE_2_SIDE, MGA_CROSS_2_SIDE }e; }

class MeshGenParams : public VirtualMemory {
public:
  static const int MaxParams = 8;
public:
  MeshGenType::e getGenType() { return _eGenType; }
  t_string getParam(size_t i) { return _strParams[i]; }
  bool parse(t_string str);
private:
  t_string _strClass; // generation class
  MeshGenType::e _eGenType; // algorithm for generation
  t_string _strParams[MaxParams]; // params to algorithm
  uint32_t _uiParamCount = 0;
  std::vector<MeshGenParams*> _vecSubMeshParams;
  MeshGenType::e getGenerationAlgorithmFromString(t_string str);
  bool logAndExit(t_string);
};

/**
*  @class MeshMaker
*  @brief Encapsulated methods to generate meshes.  Note that the MeshSpec is new'd and not freed!!
*/
class MeshUtils : public VirtualMemory {
public:
  typedef v_v3c4x2n3 MeshMakerVert;
public:
  MeshUtils() {}
  virtual ~MeshUtils() override {}
  static std::shared_ptr<MeshSpec> makeCone(float radius, float h, int32_t slices); //YOU MUST ADD THE SPEC TO THE MESH MANAGER!!!!! @cMeshFactory
  static std::shared_ptr<MeshSpec> makeSphere(float radius, int32_t slices, int32_t stacks, vec4* color = NULL, vec3* vOffset = NULL, bool blnDoNormals = true);//YOU MUST ADD THE SPEC TO THE MESH MANAGER!!!!! @cMeshFactory
  static std::shared_ptr<MeshSpec> makeArcSegment(float radius, float radius2, float refHeight, Vec3f refPos, int32_t slices);
  static std::shared_ptr<MeshSpec> makeBillboardXY(float xscale, float yscale, bool translateYToBottomOfBillboard = false);
  static std::shared_ptr<MeshSpec> makeCrossboardXY(float xscale, float yscale, bool translateYToBottomOfBillboard = false);
  static std::shared_ptr<MeshSpec> makeCircle(float radius, int32_t slices);
  static std::shared_ptr<MeshSpec> makeBox(Box3f* pCube, vec4* color, Matrix4x4* applyMat, vec3* offset);
  static std::shared_ptr<MeshSpec> makeBox(float length, vec4* color, mat4* applyMat, vec3* offset);
  static std::shared_ptr<MeshSpec> makeBox(vec3(&extents)[8], Color4f* color, Matrix4x4* applyMat, vec3* offset);
  static bool parseGenParams(t_string genString, MeshGenParams& __out_ params);
  static std::shared_ptr<MeshSpec> generateFromParameters(MeshGenParams* pgp);
  static std::shared_ptr<MeshNode> createScreenQuadMesh(int w, int h);
  static vec3* getVertexElementOffset(vec3* verts, size_t iElementIndex, size_t vOffBytes, size_t vStrideBytes, size_t vCount);
private:
  // ContactManager* _pContactManager;
  static void finalizeSpec(std::shared_ptr<MeshSpec> ms, vec3* minV = NULL, vec3* maxV = NULL);
};





}//ns game



#endif
