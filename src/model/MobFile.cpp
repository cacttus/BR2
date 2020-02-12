#include "../base/Base.h"
#include "../gfx/TexCache.h"
#include "../gfx/Texture2DSpec.h"
#include "../model/Material.h"
#include "../model/MobFile.h"
#include "../model/MeshData.h"
#include "../model/Model.h"
#include "../model/MeshCache.h"
#include "../model/ModelCache.h"
#include "../model/ModelThumb.h"

namespace BR2 {
MobFile::MobFile() {

}
MobFile::~MobFile() {
  _setModData.clear();
  //for (std::set<std::shared_ptr<ModDataLoad>>::iterator ite = _setModData.begin(); ite != _setModData.end(); ite++) {
  //    ModDataLoad* ml = *ite;
  //    DEL_MEM(ml);
  //}
}
void MobFile::pkp(std::vector<string_t>& tokens) {
  int iind = 1;
  if (lcmp(tokens[0], "mod_beg", 3)) {
    if (_pCurModDataLoad == nullptr) {
      _pCurModDataLoad = std::make_shared<ModDataLoad>();
      _pCurModDataLoad->_strModName = getCleanToken(tokens, iind);
      _pCurModDataLoad->_fVersion = TypeConv::strToFloat(getCleanToken(tokens, iind));
      if (_fVersion != _pCurModDataLoad->_fVersion) {
        BroLogError("Mob file verion mismatch got " + _pCurModDataLoad->_fVersion + ", but wanted " + _fVersion + ".");
        Gu::debugBreak();
      }
    }
    else {
      parseErr("Cur Mod Data was not null.");
    }
  }
  if (lcmp(tokens[0], "frr", 2)) {
    if (_pCurModDataLoad == nullptr) {
      parseErr("Cur Mod Data was null.");
    }
    else {
      int32_t fr = TypeConv::strToInt(getCleanToken(tokens, iind));
      _pCurModDataLoad->_iFrameRate = fr;
    }
  }

  else if (lcmp(tokens[0], "mod_end", 2)) {
    if (_pCurModDataLoad == nullptr) {
      parseErr("Cur Mod Data was null.");
    }
    else {
      _setModData.insert(_pCurModDataLoad);
      _pCurModDataLoad = nullptr;
    }
  }
  else if (_pCurModDataLoad != nullptr) {
    if (_pCurModDataLoad->tkAction(this, tokens) == false) {
      if (_pCurModDataLoad->tkArms(this, tokens) == false) {
        _pCurModDataLoad->tkMeshes(this, tokens);
      }
    }
  }


}
void  MobFile::preLoad() {
}
void  MobFile::postLoad() {

  cacheObjectsAndComputeBoxes();
}
string_t MobFile::getMobDir() {
  string_t path = FileSystem::getPathFromPath(this->_fileLoc);
  return path;
}
void MobFile::cacheObjectsAndComputeBoxes() {
  _vecModelSpecs.clear();
  for (std::shared_ptr<ModDataLoad> mdd : _setModData) {
    //Create Model Spec
    std::shared_ptr<ModelSpec> ms = std::make_shared<ModelSpec>(mdd->_strModName, mdd->_iFrameRate);
    _vecModelSpecs.push_back(ms);
    Gu::getModelCache()->addSpec(ms);

    BroLogInfo("  Caching data..");
    //Add specs to the Caches
    for (std::shared_ptr<Armature> pms : mdd->_setArmDatas) {
      ms->getArmatures().push_back(pms);
      ms->getArmatureMapOrdered().insert(std::make_pair(pms->getArmatureId(), pms));
    }
    for (std::shared_ptr<MeshData> pms : mdd->_setMeshSpecs) {
      pms->allocSkinMobFile(ms);
      ms->getMeshes().push_back(pms);
    }
    for (std::shared_ptr<ActionGroup> pa : mdd->_setActions) {
      pa->scaleKeys();
      ms->getActionGroups().push_back(pa);
    }

    BroLogInfo("  Making Thumb ..");
    //Gen THumb
    std::shared_ptr<Img32> thumb = ModelThumb::genThumb(ms, Gu::getEngineConfig()->getModelThumbSize());
    ms->setThumb(thumb);

  }
}


//////////////////////////////////////////////////////////////////////////
ModDataLoad::ModDataLoad() {

}
ModDataLoad::~ModDataLoad() {
  _setMeshSpecs.clear();
  _setArmDatas.clear();
  _pCurMeshData = nullptr;
  //  DEL_MEM(_pCurMeshData);//Lame but this is do because objf ile uses all verts in file.
}
bool ModDataLoad::tkAction(MobFile* mb, std::vector<string_t>& tokens) {
  int iind = 1;

  if (mb->lcmp(tokens[0], "act_beg", 3)) { //I.E. the motion
    if (_pCurActionKeys != nullptr) {
      mb->parseErr("Cur action keys data was not null", true, false);
    }
    else {
      string_t actName = mb->getCleanToken(tokens, iind);
      string_t objName = mb->getCleanToken(tokens, iind);

      //Try to create the gruop
      Hash32 ah = STRHASH(actName);
      std::shared_ptr<ActionGroup> pGroup = nullptr;
      for (std::shared_ptr<ActionGroup> ag : _setActions) {
        if (ag->getNameHash() == ah) {
          pGroup = ag;
          break;
        }
      }
      if (pGroup == nullptr) {
        pGroup = std::make_shared<ActionGroup>(actName, _iFrameRate);
        _setActions.insert(pGroup);
      }

      //Add this object action to the group.
      _pCurActionKeys = std::make_shared<ActionKeys>(objName);
      pGroup->addActionKeys(_pCurActionKeys);

    }
  }
  else if (mb->lcmp(tokens[0], "act_end", 3)) { //I.E. the motion
    if (_pCurActionKeys == nullptr) {
      mb->parseErr("Action was null.");
    }
    else {
      _pCurActionKeys = nullptr;
    }
  }
  //else if (lcmp(tokens[0], "kf_beg", 3)) {
  //    t_string bonename = getCleanToken(tokens, iind);
  //    _pCurKFBone = getBoneSpec(bonename);
  //    if (_pCurKFBone == nullptr) {
  //        parseErr(TStr("Failed to find KF bone ", bonename));
  //    }
  //    //curKFBone = loadGetOrAdd(path, bonename, bonez);
  //   // curKFCount = TypeConv::strToInt(tokens[2]);
  //}
  else if (mb->lcmp(tokens[0], "kf", 4)) {
    if (_pCurActionKeys == nullptr) {
      mb->parseErr(("Action was null."));
    }
    else {
      //Parse crap
      string_t strType = mb->getCleanToken(tokens, iind);
      int iTime = TypeConv::strToInt(mb->getCleanToken(tokens, iind));
      string_t strData = mb->getCleanToken(tokens, iind);

      KeyframeInterpolation::e eInterp;
      eInterp = KeyframeInterpolation::e::Linear;

      if (StringUtil::equalsi(strType, "m44")) {

        mat4 mat = mb->parseMat4(strData); //= bufferedFile.getTok();
        _pCurActionKeys->addKeyFrame(iTime, mat);
      }
      else if (StringUtil::equalsi(strType, "prs")) {
        vec3 p;
        vec4 r;
        vec3 s;
        if (mb->parsePrs(strData, p, r, s)) {
          quat rq = quat(r.x, r.y, r.z, r.w);
          _pCurActionKeys->addKeyFrame(iTime, p, rq, s);
        }
        else {
          mb->parseErr("Failed to parse keyframe PRS");
          Gu::debugBreak();
        }

      }
      else {
        mb->parseErr(Stz"Invalid keyframe type '" + strType + "'");
        Gu::debugBreak();
      }

    }
  }
  //else if (lcmp(tokens[0], "kf_end", 2)) {
  //    //t_string jt = bufferedFile.getTok();
  //    if (_pCurKFBone != nullptr) {
  //        //*Since we're moving to timeline animation this is no longer required.
  //        //processKeyframes(_pCurKFBone, _strCurAct);
  //        _pCurKFBone = nullptr;
  //    }
  //    else {
  //        BroLogError("Animation file - current bone not defined for end keyframe");
  //    }
  //}
  else {
    return false;
  }
  return true;
}
bool ModDataLoad::tkArms(MobFile* mb, std::vector<string_t>& tokens) {
  int iind = 1;

  if (mb->lcmp(tokens[0], "arm_beg", 5)) {
    if (_pCurArmData != nullptr) {
      mb->parseErr("Cur ARM data was not null", true, false);
    }
    else {
      string_t strName = mb->getCleanToken(tokens, iind);
      string_t strParentName = mb->getCleanToken(tokens, iind);
      string_t strParentType = mb->getCleanToken(tokens, iind);
      int32_t iId = TypeConv::strToInt(mb->getCleanToken(tokens, iind));
      //t_string strParent = mb->getCleanToken(tokens, iind);
      _pCurArmData = std::make_shared<Armature>(strName, iId);
      _pCurArmData->setParentName(strParentName, mb->parseParentType(strParentType));
    }
  }
  else if (mb->lcmp(tokens[0], "arm_end", 2)) {
    if (_pCurArmData == nullptr) {
      mb->parseErr("Cur ARM data was null", true, false);
    }
    else {
      _setArmDatas.insert(_pCurArmData);
      _pCurArmData = nullptr;
    }
  }
  else if (mb->lcmp(tokens[0], "arm_world", 2)) {
    if (_pCurArmData == nullptr) {
      mb->parseErr("Cur ARM data was null", true, false);
    }
    else {
      _pCurArmData->setBind(mb->parseMat4(mb->getCleanToken(tokens, iind)));
    }
  }
  else if (mb->lcmp(tokens[0], "arm_parent_inverse", 2)) {
    if (_pCurArmData == nullptr) {
      mb->parseErr("Cur ARM data was null", true, false);
    }
    else {
      _pCurArmData->setParentInverse(mb->parseMat4(mb->getCleanToken(tokens, iind)));
    }
  }
  else if (_pCurArmData != nullptr) {
    _pCurArmData->tkArmFile(mb, tokens);
  }
  else {
    return false;
  }
  return true;
}
ParentType::e MobFile::parseParentType(string_t strParentType) {
  if (StringUtil::equalsi(strParentType, "BONE")) {
    return ParentType::e::Bone;
  }
  else if (StringUtil::equalsi(strParentType, "ARMATURE")) {
    return ParentType::e::Armature;
  }
  else if (StringUtil::equalsi(strParentType, "OBJECT")) {
    return ParentType::e::Object;
  }
  else if (StringUtil::equalsi(strParentType, "NONE")) {
    return ParentType::e::None;
  }
  else {
    parseErr(Stz "Invalid parent type '" + strParentType);
    return ParentType::e::None;
  }

}
bool ModDataLoad::tkMeshes(MobFile* mb, std::vector<string_t>& tokens) {
  int iind = 1;
  if (mb->lcmp(tokens[0], "mpt_beg", 4)) {
    string_t strName = mb->getCleanToken(tokens, iind);
    string_t strParent = mb->getCleanToken(tokens, iind);
    string_t strParentType = mb->getCleanToken(tokens, iind);

    if (StringUtil::equalsi(strName, "mt_test.Cube.006")) {
      int ii = 0;
      ii++;
    }
    if (StringUtil::equalsi(strName, "mt_test.Sphere_Child")) {
      //92k verts..
      int ii = 0;
      ii++;
    }


    if (_pCurMeshData != nullptr) {
      //Do not reset hte obj data - obj file uses all verts above..
    }
    else {
      _pCurMeshData = std::make_shared<MeshSpecData>();

    }
    _pCurMeshData->setName(strName);
    if (StringUtil::equals(strName, strParent)) {
      mb->parseErr(Stz "Mesh node '" + strName + "' parent of itself");
    }
    else {
      _pCurMeshData->setParentName(strParent);
      _pCurMeshData->setParentType(mb->parseParentType(strParentType));
    }

  }
  else if (mb->lcmp(tokens[0], "mpt_end", 2)) {
    if (_pCurMeshData == nullptr) {
      mb->parseErr("Cur OBJ data was null", true, false);
    }
    else {
      std::shared_ptr<MeshData> ms = _pCurMeshData->makeSpec(mb);
      _setMeshSpecs.insert(ms);
      _pCurMeshData->resetData();
      //   DEL_MEM(_pCurObjData);
          //  _pCurObjData = nullptr;
    }
  }
  else if (_pCurMeshData != nullptr) {
    _pCurMeshData->tkObjFile(mb, tokens);
  }
  else {
    return false;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
MeshSpecData::MeshSpecData() {

}
MeshSpecData::~MeshSpecData() {
  _vecVerts.resize(0);
  _vecNormals.resize(0);
  _vecTCoords.resize(0);
  _vecWeights.resize(0);
  _vecMeshVertexWeights.resize(0);

  _vecMeshIndexes.resize(0);
  _vecMeshVerts.resize(0);

  clearVertexCache();
}
void MeshSpecData::resetData() {
  //Obj uses the verts above - so we reset the MESH verts here.

  _vecMeshVerts.resize(0);
  _vecMeshIndexes.resize(0);
  _vecMeshVertexWeights.resize(0);
  _bHasN = _bHasX = _bHasV = false;
  _bHideRender = false;
  _bKinematicShape = false;
  _eParentType = ParentType::e::None;

  clearVertexCache();
  _matBasis = mat4::identity();
  _matParentInverse = mat4::identity();
}
bool MeshSpecData::tkObjFile(MobFile* pMobFile, std::vector<string_t>& tokens) {
  int iind = 1;
  if (tkMaterial(pMobFile, tokens))
    ;
  else if (pMobFile->lcmp(tokens[0], "mpt_flip_tris", 2)) {
    _bFlipTris = TypeConv::strToBool(pMobFile->getCleanToken(tokens, iind));
  }
  else if (pMobFile->lcmp(tokens[0], "mpt_calc_normals", 2)) {
    _bCalcNormals = TypeConv::strToBool(pMobFile->getCleanToken(tokens, iind));
  }
  else if (pMobFile->lcmp(tokens[0], "mpt_swap_uvs", 2)) {
    _bSwapUvs = TypeConv::strToBool(pMobFile->getCleanToken(tokens, iind));
  }
  else if (pMobFile->lcmp(tokens[0], "v", 4)) {
    //C++ does not guarantee parameter order, thus iind would increment randomly WTF
    vec3 v(
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 1)),
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 2)),
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 3)));
    _vecVerts.push_back(v);
  }
  else if (pMobFile->lcmp(tokens[0], "vn", 4)) {
    vec3 v(
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 1)),
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 2)),
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 3)));
    _vecNormals.push_back(v);
  }
  else if (pMobFile->lcmp(tokens[0], "vt", 3)) {
    vec2 v(
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 1)),
      TypeConv::strToFloat(pMobFile->getCleanTokenInd(tokens, 2)));
    _vecTCoords.push_back(v);
  }
  else if (pMobFile->lcmp(tokens[0], "vw")) {

    int32_t iArmCount = TypeConv::strToInt(pMobFile->getCleanToken(tokens, iind));
    VertexWeightMob vw;
    for (int iarm = 0; iarm < iArmCount; ++iarm) {

      int32_t iArmId = TypeConv::strToInt(pMobFile->getCleanToken(tokens, iind));
      int32_t iCount = TypeConv::strToInt(pMobFile->getCleanToken(tokens, iind));
      string_t strWeights = pMobFile->getCleanToken(tokens, iind);

      if (iCount > 0) {
        parseWeights(pMobFile, vw, iArmId, strWeights);
      }
    }
    //Sloppy if no skin here, but this is how we defined it.
    _vecWeights.push_back(vw);
  }
  else if (pMobFile->lcmp(tokens[0], "g", 2)) {
    //parseGeom(tokens[1]); //this is how an OBJ file works, but we'e not doing this - useing MPT_END to delimit meshes
  }
  else if (pMobFile->lcmp(tokens[0], "f_shade", 2)) {
    //Obj file faces can be more than 4 indexes but we have to triangulate the model
    if (StringUtil::equalsi(tokens[1], "flat")) {

    }
    else if (StringUtil::equalsi(tokens[1], "flat")) {

    }
  }
  else if (pMobFile->lcmp(tokens[0], "f", 4)) {
    //Obj file faces can be more than 4 indexes but we have to triangulate the model
    parseFace(pMobFile, tokens[1], tokens[2], tokens[3]);
  }
  else if (pMobFile->lcmp(tokens[0], "mpt_basis", 2)) {
    _matBasis = pMobFile->parseMat4(pMobFile->getCleanToken(tokens, iind));
  }
  else if (pMobFile->lcmp(tokens[0], "mpt_parent_inverse", 2)) {
    _matParentInverse = pMobFile->parseMat4(pMobFile->getCleanToken(tokens, iind));
  }
  else if (pMobFile->lcmp(tokens[0], "physics_shape", 4)) {
    string_t strBody = pMobFile->getCleanToken(tokens, iind);
    _bKinematicShape = TypeConv::strToBool(pMobFile->getCleanToken(tokens, iind));
    _bDynamicShape = TypeConv::strToBool(pMobFile->getCleanToken(tokens, iind));
    if (StringUtil::equalsi(strBody, "sphere")) {
      _ePhysicsShapeType = PhysicsShapeType::e::Sphere;
    }
    else if (StringUtil::equalsi(strBody, "box")) {
      _ePhysicsShapeType = PhysicsShapeType::e::AABox;
    }
    else if (StringUtil::equalsi(strBody, "convex_hull")) {
      _ePhysicsShapeType = PhysicsShapeType::e::Hull;
    }
    else {
      pMobFile->parseErr(Stz "Physics shape '" + strBody + "' not supported.");
    }
  }
  else if (pMobFile->lcmp(tokens[0], "mpt_hide_render", 2)) {
    _bHideRender = TypeConv::strToBool(pMobFile->getCleanToken(tokens, iind));
  }
  else {
    return false;
  }

  return true;
}
bool MeshSpecData::tkMaterial(MobFile* pMobFile, std::vector<string_t>& tokens) {
  int iind = 1;

  if (pMobFile->lcmp(tokens[0], "mtllib", 2)) {
    //20160509 Not Supported
  }
  else if (pMobFile->lcmp(tokens[0], "usemtl", 2)) {
    //20160509 Not Supported
  }
  else if (pMobFile->lcmp(tokens[0], "mat_beg", 2)) {
    if (_pMatData != nullptr) {
      pMobFile->parseErr("Material was already defined.");
    }
    else {
      _pMatData = new MatData();

    }
  }
  else if (pMobFile->lcmp(tokens[0], "mat_end", 2)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      //   _pMatData = new MatData();
    }
  }
  else if (pMobFile->lcmp(tokens[0], "mat_tex_diffuse", 4)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      _pMatData->_strDiffuseTex = pMobFile->getCleanToken(tokens, iind);
      _pMatData->_fDiffuseTexInfluence = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_fDiffuseTexAlpha = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
    }
  }
  else if (pMobFile->lcmp(tokens[0], "mat_tex_normal", 3)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      _pMatData->_strNormalTex = pMobFile->getCleanToken(tokens, iind);
      _pMatData->_fNormalTexInfluence = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
    }
  }
  //else if (pMobFile->lcmp(tokens[0], "mat_ambient", 5)) {
  //    if (_pMatData == nullptr) {
  //        pMobFile->parseErr("Material was not defined.");
  //    }
  //    else {
  //        _pMatData->_v4AmbientColor.r() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
  //        _pMatData->_v4AmbientColor.g() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
  //        _pMatData->_v4AmbientColor.b() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
  //        _pMatData->_v4AmbientColor.a() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
  //    }
  //}
  else if (pMobFile->lcmp(tokens[0], "mat_diffuse", 5)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      _pMatData->_v4DiffuseColor.r() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4DiffuseColor.g() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4DiffuseColor.b() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4DiffuseColor.a() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
    }
  }
  else if (pMobFile->lcmp(tokens[0], "mat_spec", 6)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      _pMatData->_v4SpecColor.r() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4SpecColor.g() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4SpecColor.b() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4SpecColor.a() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_fSpecHardness = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
    }
  }
  else if (pMobFile->lcmp(tokens[0], "mat_mirror", 5)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      _pMatData->_v4Mirror.r() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4Mirror.g() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4Mirror.b() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_v4Mirror.a() = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
    }
  }
  else if (pMobFile->lcmp(tokens[0], "mat_transparency", 4)) {
    if (_pMatData == nullptr) {
      pMobFile->parseErr("Material was not defined.");
    }
    else {
      _pMatData->_bTransparency = true;
      _pMatData->_fTpAlpha = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_fTpIor = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
      _pMatData->_fTpFilter = TypeConv::strToFloat(pMobFile->getCleanToken(tokens, iind));
    }
  }
  else {
    return false;
  }
  return true;
}
void MeshSpecData::parseFace(MobFile* pMobFile, string_t t0, string_t t1, string_t t2) {
  std::vector <string_t> strVec;
  int32_t iComp;
  int32_t indices[3];
  string_t tok;

  // - Parse face groups x,y,z:  x/x/x  y/y/y  z/z/z
  for (int igroup = 0; igroup < 3; ++igroup) {
    //tok = pBufferedFile.getTok();
    if (igroup == 0) tok = t0;
    if (igroup == 1) tok = t1;
    if (igroup == 2) tok = t2;

    int32_t strlind = 0; // last index of '/' + 1 (one char after it)
    int32_t curComponent = 0; // the current component ex "v" inside of the v/t/n etc

    tok = StringUtil::trim(tok);
    StringUtil::split(tok, '/', strVec);

    //Parse face indexes x1/x2/x3
    for (iComp = 0; iComp < (int32_t)strVec.size(); ++iComp) {
      indices[iComp] = parseFaceComponent(pMobFile, tok, strlind, iComp);
    }

    addFaceVertex(pMobFile, indices[0], indices[1], indices[2]);//v, x, n

    strVec.clear();
  }
}
int32_t MeshSpecData::parseFaceComponent(MobFile* pMobFile, string_t& tok, int32_t& strlind, int32_t iComponent) {
  size_t strind = 0;// current index of '/'
  int32_t idx; // parsed vertex face index
  string_t rt;

  strind = tok.find_first_of('/', strlind);
  if (strind == string_t::npos) {
    //Last item in the 3 element list.
    strind = tok.length();
  }

  // Get the value of the face index
  if ((strind - strlind) == 0) {
    // - we have a xx//yy, If we have a missing index component then we just set the index to missing.
    //idx = MissingIndexValue;

    //20160509 we don't allow varied formats.
    //You're missing Tex Coords, Or Normals
 //   pMobFile->parseErr(TStr("Invalid object file format.  Missing face index '", iComponent, "' (it's a // or ///) -  faces need 3 components, vertex, texture, normal"), true, true);
    //20180112 we will allow this in a restricted sense - we allow UP TO v,x,n but also v,x, v,n, and v
    idx = c_iNoComponent;
  }
  else {
    rt = tok.substr(strlind, strind - strlind);
    idx = TypeConv::strToInt(rt);
  }

  strlind = (int32_t)strind + 1; // cut out the '//'

  return idx;
}
void MeshSpecData::addFaceVertex(MobFile* pMobFile, int32_t iVertex, int32_t iTCoord, int32_t iNormal) {
  int32_t vi, ni, xi;
  if (iVertex != c_iNoComponent) {
    _bHasV = true;
    vi = iVertex - 1;
    if (vi < 0) {
      pMobFile->parseErr("Vertex buffer underflow ", true, true);
    }
    if (vi >= (int32_t)_vecVerts.size()) {
      pMobFile->parseErr("Vertex buffer overflow at line ", true, true);
    }
  }
  else {
    pMobFile->parseErr("Mesh had no vertexes present(ERROR!)", true, true);
  }
  if (iNormal != c_iNoComponent) {
    _bHasN = true;
    ni = iNormal - 1;
    if (ni < 0) {
      pMobFile->parseErr("Normal buffer underflow at line ", true, true);
    }
    if (ni >= (int32_t)_vecNormals.size()) {
      pMobFile->parseErr("Normal buffer overflow at line ", true, true);
    }
  }
  else {
    ni = c_iNoComponent;
  }
  if (iTCoord != c_iNoComponent) {
    _bHasX = true;
    xi = iTCoord - 1;
    if (xi < 0) {
      pMobFile->parseErr("TCoord buffer underflow at line ", true, true);
    }
    if (xi >= (int32_t)_vecTCoords.size()) {
      pMobFile->parseErr("TCoord buffer overflow at line ", true, true);
    }
  }
  else {
    xi = c_iNoComponent;
  }

  int32_t newIndex = 0;

  //We're using this "vertex cache" to reduce vertex size by referenced face components in the OBJ
  newIndex = findCachedVertex(vi, xi, ni);
  if (newIndex < 0 || _bDebugDisableVertexCompression) {
    newIndex = addNewMeshVertex(vi, xi, ni);
  }

  _vecMeshIndexes.push_back(newIndex);
}
int32_t MeshSpecData::addNewMeshVertex(int32_t vi, int32_t xi, int32_t ni) {
  int32_t newIndex;
  ModelVertexType vx;

  //If the index is an invalid component
  vx.v = _vecVerts[vi];
  if (ni != c_iNoComponent) {
    vx.n = _vecNormals[ni];
  }
  if (xi != c_iNoComponent) {
    vx.x = _vecTCoords[xi];
  }

  newIndex = (int32_t)_vecMeshVerts.size();

  _vecMeshVerts.push_back(vx);
  _vecMeshVertexWeights.push_back(_vecWeights[vi]); // We have to duplicate the weight data.

  if (!_bDebugDisableVertexCompression) {
    addCachedVertex(newIndex, vi, xi, ni);
  }

  return newIndex;
}
std::shared_ptr<MeshData> MeshSpecData::makeSpec(MobFile* mb) {
  BroLogInfo("Adding mesh part '" + _strName + "'");

  std::shared_ptr<PhysicsShape> pShape = makePhysicsShapeForSpec();
  std::shared_ptr<VertexFormat> fmt = getVertexFormatForSpec(mb);
#ifdef _DEBUG
  if (_strName.find("cuddles_01b.cb_RArm") != std::string::npos) {
    int n = 0;
    n++;
  }
#endif
  std::shared_ptr<MeshData> pSpec = std::make_shared<MeshData>(_strName, fmt, nullptr, pShape);
  pSpec->setBind(_matBasis);
  pSpec->setParentName(_strParentName, _eParentType);
  pSpec->setParentInverse(_matParentInverse);
  pSpec->setHideRender(_bHideRender);
  makeMaterialForSpec(mb, pSpec);

  // Copy  spec vertexes.
  copySpecFragments(pSpec);

  //  _vecMeshes.push_back(pSpec);

  _vecMeshVerts.resize(0);
  _vecMeshIndexes.resize(0);
  _vecMeshVertexWeights.resize(0);
  clearVertexCache();

  _matBasis = mat4::identity();
  _matParentInverse = mat4::identity();
  return pSpec;
}
std::shared_ptr<VertexFormat> MeshSpecData::getVertexFormatForSpec(MobFile* mb) {
  if (_bHasV && _bHasX && _bHasN) {
    return v_v3n3x2::getVertexFormat();
  }
  else if (_bHasV && _bHasX) {
    return v_v3x2::getVertexFormat();
  }
  else if (_bHasV && _bHasN) {
    return v_v3n3::getVertexFormat();
  }
  else if (_bHasV) {
    return v_v3::getVertexFormat();
  }
  else {
    mb->parseErr("Invalid vertex format for mesh.", true, false);
    return v_v3::getVertexFormat();
  }
}
void MeshSpecData::makeMaterialForSpec(MobFile* mb, std::shared_ptr<MeshData> pSpec) {
  if (_pMatData != nullptr) {
    std::shared_ptr<AppBase> pRoom = Gu::getApp();
    AssertOrThrow2(pRoom != nullptr);

    std::shared_ptr<Texture2DSpec> diffuse = nullptr;
    std::shared_ptr<Texture2DSpec> normal = nullptr;
    //create material

    std::shared_ptr<Material> mat = std::make_shared<Material>(_pMatData->_strMatName);
    string_t path;

    if (StringUtil::isNotEmpty(_pMatData->_strDiffuseTex)) {

      //Texture should be placed in the same directory as the mob.
      path = FileSystem::combinePath(mb->getMobDir(), _pMatData->_strDiffuseTex);
      if (FileSystem::fileExists(path)) {
        std::shared_ptr<Texture2DSpec> pTex = Gu::getTexCache()->getOrLoad(path);
        mat->addTextureBinding(pTex, TextureChannel::e::Channel0, TextureType::e::Color, _pMatData->_fDiffuseTexInfluence);
      }
      else {
        BroLogError("Texture image file " + _pMatData->_strDiffuseTex + " not found!");
        Gu::debugBreak();
      }
    }
    if (StringUtil::isNotEmpty(_pMatData->_strNormalTex)) {
      path = FileSystem::combinePath(mb->getMobDir(), _pMatData->_strNormalTex);
      if (FileSystem::fileExists(path)) {
        std::shared_ptr<Texture2DSpec> pTex = Gu::getTexCache()->getOrLoad(path);
        mat->addTextureBinding(pTex, TextureChannel::e::Channel1, TextureType::e::Normal, _pMatData->_fNormalTexInfluence);
      }
      else {
        BroLogError("Texture image file " + _pMatData->_strNormalTex + " not found!");
        Gu::debugBreak();
      }
    }
    // mat->setAmbient(std::move(_pMatData->_v4AmbientColor));
    mat->setDiffuse(std::move(_pMatData->_v4DiffuseColor));
    mat->setSpec(std::move(_pMatData->_v4SpecColor));
    mat->setSpecHardness(std::move(_pMatData->_fSpecHardness));
    mat->setMirror(std::move(_pMatData->_v4Mirror));

    mat->setEnableTransparency(_pMatData->_bTransparency);
    mat->setTpIOR(_pMatData->_fTpIor);
    mat->setTpAlpha(_pMatData->_fTpAlpha);
    mat->setTpFilter(_pMatData->_fTpFilter);

    pSpec->setMaterial(mat);


    _pMatData = nullptr;
  }
}
std::shared_ptr<PhysicsShape> MeshSpecData::makePhysicsShapeForSpec() {
  std::shared_ptr<PhysicsShape> pRet = nullptr;
  //vec3* MeshUtils::getVertexElementOffset(vec3* verts, 
  //    size_t iElementIndex, size_t vOffBytes, size_t vStrideBytes, size_t vCount) {

  //_bKinematicShape 
  //_bDynamciShape
  //if (_ePhysicsShapeType == PhysicsShapeType::e::Hull) {
  //    HullShape* hs = new HullShape(_vecMeshVerts, _vecMeshIndexes);

  //    pShape = hs;
  //}
  //else if (_ePhysicsShapeType == PhysicsShapeType::e::Hull) {
  //    HullShape* hs = new HullShape();
  //    pShape = hs;
  //}

  return pRet;
}
void MeshSpecData::copySpecFragments(std::shared_ptr<MeshData> pSpec) {
  t_timeval t0;

  if (_bFlipTris) {
    t0 = Gu::getMicroSeconds();
    BroLogInfo("..Flipping mesh triangles..");
    for (size_t iInd = 0; iInd < _vecMeshIndexes.size(); iInd += 3) {
      v_index32 t = _vecMeshIndexes[iInd + 1];
      _vecMeshIndexes[iInd + 1] = _vecMeshIndexes[iInd + 2];
      _vecMeshIndexes[iInd + 2] = t;
    }
    BroLogInfo("..Done. " + (uint32_t)(Gu::getMicroSeconds() - t0) / 1000 + "ms");

  }
  if (_bCalcNormals) {
    t0 = Gu::getMicroSeconds();
    BroLogInfo("..Calc mesh Normals..");
    std::vector<float> fCountPerVert;
    for (size_t iVert = 0; iVert < _vecMeshVerts.size(); ++iVert) {
      _vecMeshVerts[iVert].n = 0;
      fCountPerVert.push_back(0);
    }
    for (size_t iInd = 0; iInd < _vecMeshIndexes.size(); iInd += 3) {
      int32_t i0 = _vecMeshIndexes[iInd + 0];
      int32_t i1 = _vecMeshIndexes[iInd + 1];
      int32_t i2 = _vecMeshIndexes[iInd + 2];
      vec3 v0 = _vecMeshVerts[i0].v;
      vec3 v1 = _vecMeshVerts[i1].v;
      vec3 v2 = _vecMeshVerts[i2].v;
      vec3 d0 = v1 - v0;
      vec3 d1 = v2 - v0;
      vec3 n = d1.cross(d0);
      _vecMeshVerts[i0].n += n;
      _vecMeshVerts[i1].n += n;
      _vecMeshVerts[i2].n += n;

      fCountPerVert[i0] += 1.0f;
      fCountPerVert[i1] += 1.0f;
      fCountPerVert[i2] += 1.0f;
    }
    for (size_t iVert = 0; iVert < _vecMeshVerts.size(); ++iVert) {
      if (fCountPerVert[iVert] <= 0) {
        //error - just ignore
        int nnn = 0;
        nnn++;
      }
      else {
        _vecMeshVerts[iVert].n /= fCountPerVert[iVert];
      }
    }


    BroLogInfo("..Done. " + ((uint32_t)(Gu::getMicroSeconds() - t0) / 1000) + "ms");
  }
  if (_bSwapUvs) {
    t0 = Gu::getMicroSeconds();
    BroLogInfo("..Swapping mesh UVs.");
    for (size_t iInd = 0; iInd < _vecMeshVerts.size(); iInd++) {
      float t = _vecMeshVerts[iInd].x.u();
      _vecMeshVerts[iInd].x.u() = _vecMeshVerts[iInd].x.v();
      _vecMeshVerts[iInd].x.v() = t;
    }
    BroLogInfo("..Done. " + ((uint32_t)(Gu::getMicroSeconds() - t0) / 1000) + "ms");
  }

  BroLogInfo("Copying Vertex Format...");
  std::vector<v_v3n3> verts_v3n3;
  std::vector<v_v3x2> verts_v3x2;
  std::vector<v_v3> verts_v3;
  void* vData = nullptr;
  size_t vDataSize = 0;
  if (pSpec->getVertexFormat() == v_v3n3x2::getVertexFormat()) {
    vData = (void*)_vecMeshVerts.data();
    vDataSize = _vecMeshVerts.size();
  }
  else if (pSpec->getVertexFormat() == v_v3n3::getVertexFormat()) {
    for (ModelVertexType v : _vecMeshVerts) {
      v_v3n3 c; c.v = v.v; c.n = v.n;
      verts_v3n3.push_back(c);
    }
    vData = (void*)verts_v3n3.data();
    vDataSize = verts_v3n3.size();
  }
  else if (pSpec->getVertexFormat() == v_v3x2::getVertexFormat()) {
    for (ModelVertexType v : _vecMeshVerts) {
      v_v3x2 c; c.v = v.v; c.x = v.x;
      verts_v3x2.push_back(c);
    }
    vData = (void*)verts_v3x2.data();
    vDataSize = verts_v3x2.size();
  }
  else if (pSpec->getVertexFormat() == v_v3::getVertexFormat()) {
    for (ModelVertexType v : _vecMeshVerts) {
      v_v3 c; c.v = v.v;
      verts_v3.push_back(c);
    }
    vData = (void*)verts_v3.data();
    vDataSize = verts_v3.size();
  }
  else {
    //Eventually we should make loading this generic and just use the FragmentBufferData or sometjhing
    BroThrowNotImplementedException();
  }
  BroLogInfo("Allocating...");
  //Allocate
  pSpec->allocMesh(
    vData, vDataSize,
    _vecMeshIndexes.data(), _vecMeshIndexes.size(),
    &_vecMeshVertexWeights);

  verts_v3n3.resize(0);
  verts_v3x2.resize(0);
  verts_v3.resize(0);

  BroLogInfo("Computing Bound Box...");
  pSpec->computeBox();

  BroLogInfo("..done");
}
int32_t MeshSpecData::findCachedVertex(int32_t vi, int32_t xi, int32_t ni) {
  ivec3 v;
  v.x = vi;
  v.y = xi;
  v.z = ni;

  std::map<ivec3*, int32_t, ivec3::Vec3xCompLess>::iterator ite = _mapVertexCache.find(&v);
  if (ite != _mapVertexCache.end()) {
    return ite->second;
  }
  return -1;
}
void MeshSpecData::addCachedVertex(int32_t newIndex, int32_t vi, int32_t xi, int32_t ni) {
  ivec3* v = new ivec3();
  v->x = vi;
  v->y = xi;
  v->z = ni;

  AssertOrThrow2(findCachedVertex(vi, xi, ni) == -1);

  _mapVertexCache.insert(std::make_pair(v, newIndex));
}
void MeshSpecData::clearVertexCache() {
  std::map<ivec3*, int32_t, ivec3::Vec3xCompLess>::iterator ite = _mapVertexCache.begin();
  for (; ite != _mapVertexCache.end(); ite++) {
    ivec3* ii = ite->first;
    DEL_MEM(ii);
  }
  _mapVertexCache.clear();
}
void MeshSpecData::parseWeights(MobFile* mb, VertexWeightMob& vw, int32_t iArmId, string_t strWeights) {
  size_t n = 0;
  char c;
  string_t val = "";
  bool joint = true;    // - True if we are parsing joint ID, false if weight.
  float fWeight;
  int32_t iJointId;

  // - Store weights mapped to the armature hash.
  while (n < strWeights.length()) {
    c = strWeights[n++];

    if (isdigit(c) || c == '-' || c == '.' || c == '+' || c == 'e') {
      val += c;
    }

    if (c == ',' || n == strWeights.length()) {
      if (joint) {
        iJointId = TypeConv::strToInt(val);
      }
      else {
        fWeight = TypeConv::strToFloat(val);
        std::map<int32_t, std::map<int32_t, float>>::iterator it;
        it = vw._mapWeights.find(iArmId);
        if (it == vw._mapWeights.end()) {
          vw._mapWeights.insert(std::make_pair(iArmId, std::map<int32_t, float>()));
          it = vw._mapWeights.find(iArmId);
        }
        if (iJointId == -1 || fWeight < 0) {
          int i = 0;
          i++;
          mb->parseErr(Stz "While Parsing: " + mb->getFileLoc() + ": Input joint ID was negative at line " + mb->getCurrentParseLine());
        }
        it->second.insert(std::make_pair(iJointId, fWeight));
      }
      joint = !joint;
      val = "";
    }

  }



}

}//ns Game
