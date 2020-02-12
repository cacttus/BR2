/**
*  @file MobFile.h
*  @date December 6, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __MOBFILE_15125946621549004957_H__
#define __MOBFILE_15125946621549004957_H__

#include "../base/PoundFile.h"
#include "../model/ModelHeader.h"
#include "../world/WorldHeader.h"

namespace Game {

class MatData : public VirtualMemory {
public:
  string_t _strMatName = "";
  string_t _strDiffuseTex = "";
  float _fDiffuseTexInfluence = 0;
  float _fDiffuseTexAlpha = 1.0;
  string_t _strNormalTex = "";
  float _fNormalTexInfluence = 0;
  vec4 _v4DiffuseColor = vec4(1, 1, 1, 1); //Only if diffuse tex is not present.
  vec4 _v4SpecColor = vec4(1, 1, 1, 1);//color(3), intensity, hardness
  float _fSpecHardness = 1.0f;
  vec4 _v4Mirror = vec4(0, 0, 0, 0); //ambient + diffuse + spec **MUST DEFAULT TO ZERo** Mirror is disabeld by having a ZERO mirror
  bool _bTransparency = false;
  float _fTpIor = 1, _fTpFilter = 1, _fTpAlpha = 0;
  MatData() {

  }
};
//Data ofr obj file - meshes
class MeshSpecData : public VirtualMemory {
public:
  const int32_t c_iNoComponent = -999999999;
public:
  MeshSpecData();
  virtual ~MeshSpecData();

  void setParentType(ParentType::e pt) { _eParentType = pt; }

  bool tkObjFile(MobFile* pMobFile, std::vector<string_t>& tokens);
  std::shared_ptr<MeshData> makeSpec(MobFile* mb);

  void resetData();
  void setName(string_t name) { _strName = name; }
  void setParentName(string_t name) { _strParentName = name; }

private:
  string_t _strName;
  string_t _strParentName;
  mat4 _matBasis;
  mat4 _matParentInverse;
  PhysicsShapeType::e _ePhysicsShapeType = PhysicsShapeType::e::None;
  bool _bKinematicShape = false;//true if shape is attached.
  bool _bDynamicShape = false;//true if shape is attached.
  bool _bHideRender = false;
  ParentType::e _eParentType = ParentType::e::None;

  //*Don't clear these when you load the next mesh - indexes are defined by all the PREVIOUS vertexes. Stupid, but it's OBJ file.
  std::vector<vec3> _vecVerts;
  std::vector<vec3> _vecNormals;
  std::vector<vec2> _vecTCoords;
  std::vector<VertexWeightMob> _vecWeights;

  bool _bDebugDisableVertexCompression = false;
  std::map<ivec3*, int32_t, ivec3::Vec3xCompLess> _mapVertexCache;

  MatData* _pMatData = nullptr;

  //Mesh Specific data - when we've processed teh loaded data.
  //FragmentBufferData* _pFragmentBufferData = nullptr;
  //IndexBufferData* _pIndexBufferData = nullptr;

  bool _bHasV = false;
  bool _bHasX = false;
  bool _bHasN = false;
  std::vector<ModelVertexType> _vecMeshVerts;
  std::vector<v_index32> _vecMeshIndexes;
  std::vector<VertexWeightMob> _vecMeshVertexWeights;

  bool _bFlipTris = false;
  bool _bSwapUvs = false;
  bool _bCalcNormals = true;

  int32_t findCachedVertex(int32_t vi, int32_t xi, int32_t ni);
  void addCachedVertex(int32_t newIndex, int32_t vi, int32_t xi, int32_t ni);
  void clearVertexCache();
  //vec3 readVec3(t_string tok);
  //vec2 readVec2(t_string tok);
  //void loadObjFileContents(BinaryFile& pBufferedFile);
 // void parseGeom(t_string& tok);
  void parseFace(MobFile* pMobFile, string_t t0, string_t t1, string_t t2);
  int32_t parseFaceComponent(MobFile* pMobFile, string_t& tok, int32_t& strlind, int32_t iComponent);
  void addFaceVertex(MobFile* pMobFile, int32_t iVertex, int32_t iTCoord, int32_t iNormal);
  // void addCurrentSpec();
  void copySpecFragments(std::shared_ptr<MeshData> pSpec);
  int32_t addNewMeshVertex(int32_t vi, int32_t xi, int32_t ni);
  void parseWeights(MobFile* mb, VertexWeightMob& vw, int32_t iArmName, string_t strWeights);
  // mat4 parseMat4(t_string tok);
  void makeMaterialForSpec(MobFile*, std::shared_ptr<MeshData> ms);
  bool tkMaterial(MobFile* pMobFile, std::vector<string_t>& tokens);
  std::shared_ptr<PhysicsShape> makePhysicsShapeForSpec();
  std::shared_ptr<VertexFormat> getVertexFormatForSpec(MobFile* mb);
};

//Data for individual models - meshes + armatures
class ModDataLoad : public VirtualMemory {
public:
  string_t _strModName;
  int32_t _iFrameRate = 24;
  float _fVersion = 0.0f;
  std::set<std::shared_ptr<MeshData>> _setMeshSpecs;
  std::set<std::shared_ptr<Armature>> _setArmDatas;
  std::set<std::shared_ptr<ActionGroup>> _setActions;
  //std::set<std::shared_ptr<ActionGroup>> _setActions;

  bool tkAction(MobFile* mb, std::vector<string_t>& tokens);
  bool tkArms(MobFile* mb, std::vector<string_t>& tokens);
  bool tkMeshes(MobFile* mb, std::vector<string_t>& tokens);

  std::shared_ptr<MeshSpecData> _pCurMeshData = nullptr;
  std::shared_ptr<Armature> _pCurArmData = nullptr;
  std::shared_ptr<ActionKeys>  _pCurActionKeys = nullptr;

  ModDataLoad();
  virtual ~ModDataLoad();
};
/**
*  @class MobFile
*  @brief Model + Mesh + Animation file.  The result of loading a MOB file is you will have one or more meshes @c MeshCache, and one or more armatures @c BoneCache
*/
class MobFile : public PoundFile {
public:
  MobFile();
  virtual ~MobFile() override;
  ParentType::e parseParentType(string_t pt);
  string_t getMobDir();
  std::vector<std::shared_ptr<ModelSpec>>& getModelSpecs() { return _vecModelSpecs; }

private:
  float _fVersion = 0.02f;

  //*Technically we shouldn't have multiple MOD datas per file, but just adding
  //this as a swet lets us do that.  
  std::set<std::shared_ptr<ModDataLoad>> _setModData;
  std::vector<std::shared_ptr<ModelSpec>> _vecModelSpecs;

  std::shared_ptr<ModDataLoad> _pCurModDataLoad = nullptr;

  //Inherited
  virtual void pkp(std::vector<string_t>& tokens);
  virtual void preLoad();
  virtual void postLoad();
  void cacheObjectsAndComputeBoxes();
};

}//ns Game



#endif
