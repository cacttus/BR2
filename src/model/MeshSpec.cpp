#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/BinaryFile.h"
#include "../gfx/Texture2DSpec.h"
#include "../model/ModelHeader.h"
#include "../model/Model.h"
#include "../model/Material.h"
#include "../model/ModelCache.h"
#include "../model/MeshSpec.h"
#include "../model/FragmentBufferData.h"
#include "../model/IndexBufferData.h"
#include "../model/VertexAdjacencyMap.h"
#include "../model/VertexFormat.h"
#include "../model/MeshNode.h"
#include "../model/VboData.h"
#include "../model/ShaderStorageBuffer.h"
#include "../model/VaoDataGeneric.h"
#include "../model/IboData.h"
#include "../model/VboData.h"
#include "../world/PhysicsShapes.h"
#include "../world/PhysicsShapes.h"
#include <iostream>

namespace BR2 {
MeshSpec::MeshSpec(string_t strName, std::shared_ptr<VertexFormat> vf, std::shared_ptr<ObjFile> objFile, std::shared_ptr<PhysicsShape> ps) : BaseSpec(strName) {
  _pVertexFormat = vf;
  _pObjectFile = objFile;
  _pPhysicsShape = ps;

  _matLocalMatrix = mat4::identity();
}
MeshSpec::MeshSpec(const void* pVerts, size_t vCount,
  const void* pIndexes, size_t iCount,
  std::shared_ptr<VertexFormat> fmt, std::shared_ptr<Material> pm) : MeshSpec("", fmt, nullptr, nullptr) {
  _pMaterial = pm;
  allocMesh(pVerts, vCount, pIndexes, iCount, nullptr);
}
MeshSpec::~MeshSpec() {
  // DEL_MEM(_pFrags);
  // DEL_MEM(_pIndexes);
  // DEL_MEM(_pFaceNormals);
 //  DEL_MEM(_pMaterial);
   //DEL_MEM(_pVertexAdjacencyMap);
   //DEL_MEM(_pVertsGpu);
  // DEL_MEM(_pVaoData);
}
//PROPS
void MeshSpec::setMaterial(std::shared_ptr<Material> m) {
  //DEL_MEM(_pMaterial);
  _pMaterial = m;
}

size_t MeshSpec::indexCount() { return _pVaoData->getIbo()->getNumElements(); }//_pIndexes ? _pIndexes->count() : 0; }
size_t MeshSpec::fragCount() { return _pVaoData->getVbo()->getNumElements(); }//_pFrags ? _pFrags->count() : 0; }
size_t MeshSpec::faceCount() { return _pVaoData->getIbo()->getNumElements() / 3; }//_pFaceNormals ? _pFaceNormals->count() : 0; }


//void MeshSpec::tryCreateConversionMapping()
//{
//    if(_pConversionMapping == NULL)
//        _pConversionMapping = new MeshConversionMapping();
//}


//////////////////////////////////////////////////////////////////////////
//void MeshSpec::deleteVertexAdjacencyMap() {
//    DEL_MEM(_pVertexAdjacencyMap);
//}
//void MeshSpec::deleteConversionMapping(){
//    AssertOrThrow2(_pConversionMapping != NULL);
//    DEL_MEM(_pConversionMapping);
//}
void MeshSpec::allocMesh(size_t nFrags, size_t nIndexes) {
  AssertOrThrow2((nIndexes % 3) == 0);
  AssertOrThrow2(_pVaoData != nullptr);
  _pVaoData->allocate(nFrags, nIndexes);

  // allocateFragments(nFrags);
  // allocateIndexes(nIndexes);
  //
  // if (use_face_normals) {
  //     allocateFaceNormals(nIndexes / 3);
  // }
  //

  //_face_normals.alloc(nIndexes/3);
}
//void MeshSpec::allocMesh(const void* pFrags, size_t nFrags, const v_index32* pIndexes, size_t nIndexes,
//    std::vector<GpuAnimatedMeshWeightData>& weightDatas, std::vector<GpuAnimatedMeshWeight>& weights) {
//    //New *quick* routine.
//
//}
void MeshSpec::allocMesh(const void* pFrags, size_t nFrags, const void* pIndexes, size_t nIndexes, std::vector<VertexWeightMob>* vecWeights) {
  t_timeval t0 = Gu::getMicroSeconds();

  if (_pVaoData == nullptr) {
    //Prevent multiple reallocations
    _pVaoData = std::make_shared<VaoDataGeneric>(Gu::getCoreContext(), _pVertexFormat);
  }

  _pVaoData->fillData(pFrags, nFrags, pIndexes, nIndexes);

  if (vecWeights != nullptr) {
    // This sloppy check is a consequence of even vw's matching v's
    // in the mob file. Even without skin we have vw's
    // We'll often get vecWeights in but there won't be any weight.
    bool bHasWeights = false;
    for (VertexWeightMob w : *vecWeights) {
      if (w._mapWeights.size() > 0) {
        for (std::pair<int32_t, std::map<int32_t, float>> p1 : w._mapWeights) {
          if (p1.second.size() > 0) {
            bHasWeights = true;
            break;
          }
        }
      }
    }
    if (bHasWeights) {
      _vecWeightsMob = *vecWeights;

      _eSkinStatus = MeshSkinStatus::e::Uninitialized; // Skin
    }
    else {
      _eSkinStatus = MeshSkinStatus::e::NoSkin;
    }
  }
  else {
    _eSkinStatus = MeshSkinStatus::e::NoSkin; // No Skin
  }



}
void MeshSpec::printWeightsToStdout() {
  int n = 0;
  for (VertexWeightMob w : _vecWeightsMob) {
    for (std::pair<int32_t, std::map<int32_t, float>> p1 : w._mapWeights) {
      // std::shared_ptr<Armature> pa = Gu::getModelCache()->getArmature(p1.first);
      // if (pa != nullptr) {
      std::cout << n << ": " << p1.first << std::endl;
      for (std::pair<int32_t, float> p2 : p1.second) {
        std::cout << "  (" << p2.first << "," << p2.second << ")";
      }
      std::cout << std::endl;
      // }
      // else {
      //     BroLogError("Armature not found - you probaby are calling this in the parse step before allocSkin()");
      // }
    }
    std::cout << std::endl;
    n++;
  }
}
void MeshSpec::printWeightBuffersToStdout() {
  //if (_pWeightOffsetsGpu != nullptr && _pWeightsGpu != nullptr) {
  //    if (_pWeightOffsetsGpu->getNumElements() == _pFrags->count()) {
  //        GpuAnimatedMeshWeightData* weightOffsetsGpu = new GpuAnimatedMeshWeightData[_pWeightOffsetsGpu->getNumElements()];
  //        GpuAnimatedMeshWeight* weightsGpu = new GpuAnimatedMeshWeight[_pWeightsGpu->getNumElements()];
  //        _pWeightOffsetsGpu->copyDataServerClient(_pWeightOffsetsGpu->getNumElements(), weightOffsetsGpu);
  //        _pWeightsGpu->copyDataServerClient(_pWeightsGpu->getNumElements(), weightsGpu);

  //        std::cout << "****************" << std::endl;
  //        std::cout << "weight offs  vert = (off into weights, count) <vert> => <model transform * vert>" << std::endl;
  //        for (size_t ele = 0; ele < _pWeightOffsetsGpu->getNumElements(); ++ele) {
  //            vec3 vBase = v3f(ele);
  //            vec3 vTransform = (getBind() * vec4(vBase, 1));

  //            std::cout << "v " << ele << " = (" << weightOffsetsGpu[ele]._offset << "," << weightOffsetsGpu[ele]._count << ") <" << vBase.toString(2) << "> => <" << vTransform.toString(2) << "> " << std::endl;
  //            for (GpuInt iw = 0; iw < weightOffsetsGpu[ele]._count; ++iw) {
  //                std::cout << "  " << weightsGpu[weightOffsetsGpu[ele]._offset + iw]._iArmJointOffset << "," << weightsGpu[weightOffsetsGpu[ele]._offset + iw]._weight << std::endl;
  //            }
  //        }
  //        std::cout << std::endl;


  //        std::cout << "****************" << std::endl;
  //        std::cout << "weights" << std::endl;
  //        for (size_t ele = 0; ele < _pWeightsGpu->getNumElements(); ++ele) {
  //            std::cout << "(" << weightsGpu[ele]._iArmJointOffset << "," << weightsGpu[ele]._weight << ")";
  //        }
  //        std::cout << std::endl;
  //        delete[] weightsGpu;
  //        delete[] weightOffsetsGpu;
  //    }
  //}

}
void MeshSpec::allocSkinMobFile(std::shared_ptr<ModelSpec> ms) {
  if (false) {
    printWeightsToStdout();
  }
  //Create skin on GPU
  t_timeval t0 = Gu::getMicroSeconds();
  BRLogInfo("Creating GPU skin '" + getName() + "'..");
  {
    if (_eSkinStatus == MeshSkinStatus::e::Uninitialized) {
      //2018/1/15 so we made all armatures be global in ModelSpec
      //sendVertsToGpu();
      //TODO - replace with new vao 1/26/18

      // Many cards have a max uniform buffer of 128 matrixes.  This should be enough for 1 model.
      //MAX_VERTEX_UNIFORM_COMPONENTS_ARB
      //[ARM0 mat1, mat2... ARM1 mat1 mat2...]
      fillWeightBuffersMob(ms);
      if (false) {
        printWeightBuffersToStdout();
      }
      _eSkinStatus = MeshSkinStatus::e::Allocated;
    }
  }
  BRLogInfo("..Done." + (uint32_t)((Gu::getMicroSeconds() - t0) / 1000) + "ms");
}
std::shared_ptr<MeshSpec> MeshSpec::createCopy() {
  std::shared_ptr<MeshSpec> ms = nullptr;

  ms = std::make_shared<MeshSpec>(getName(), ms->getVertexFormat(), nullptr);
  BRThrowNotImplementedException();

  //if (ms->_pFaceNormals != nullptr) {
  //    ms->_pFaceNormals->copyFrom(_pFaceNormals);
  //}
  //if (ms->_pIndexes != nullptr) {
  //    ms->_pIndexes->copyFrom(_pIndexes);
  //}
  //if (ms->_pTangents != nullptr) {
  //    ms->_pTangents->copyFrom(_pTangents);
  //}
  //if (ms->_pFrags != nullptr) {
  //    ms->_pFrags->copyFrom(_pFrags);
  //}
  //ms->_vecWeightsMob = _vecWeightsMob;

  return ms;
}
std::shared_ptr<MeshSpec> MeshSpec::mergeWith(std::shared_ptr<MeshSpec> other, bool automaticallyRecalculateIndexOffsets) {
  BRThrowNotImplementedException();
  //AssertOrThrow2(_pVaoData!=nullptr);
  //char* c = _pVaoData->getVbo()->copyDataServerClient()

  //if (_pFaceNormals != nullptr) {
  //    _pFaceNormals->appendToEnd(other->_pFaceNormals);
  //}
  //if (_pTangents != nullptr) {
  //    _pTangents->appendToEnd(other->_pTangents);
  //}
  //if (_pFrags != nullptr) {
  //    _pFrags->appendToEnd(other->_pFrags);
  //}

  // - Index manipulation
  //uint32_t otherMeshICount = (uint32_t)other->indexCount();
  //uint32_t oldICount = (uint32_t)indexCount();
  //if (_pIndexes != nullptr) {
  //    _pIndexes->appendToEnd(other->_pIndexes);
  //}

  //if (otherMeshICount && automaticallyRecalculateIndexOffsets) {
  //    addOffsetToIndexRange(otherMeshICount, oldICount, (uint32_t)indexCount());
  //}

  ////**You need to do the weights too.
  //BroThrowNotImplementedException();

  return std::dynamic_pointer_cast<MeshSpec>(shared_from_this());
}
/**
*    @fn
*    @brief Adds an offset to the given indexes for a specified range.
*/
//void MeshSpec::addOffsetToIndexRange(uint32_t offset, uint32_t startIndex, uint32_t endIndex)
//{
//    //Index integrity check
//    AssertOrThrow2(startIndex < 40000000);
//    AssertOrThrow2(endIndex < 40000000);
//
//    AssertOrThrow2(startIndex < endIndex);
//    AssertOrThrow2(endIndex <= indexCount());
//    for (size_t i = startIndex; i < endIndex; ++i) {
//        i32(i) += offset;
//    }
//}
//void MeshSpec::debugPrintDataToStdOut()
//{
//    t_string str;
//
//    StringUtil::appendLine(str, TStr(fragCount(), " Vertexes"));
//    for (size_t i = 0; i < fragCount(); ++i) {
//        StringUtil::appendLine(str, v3f(i).toString());
//    }
//
//    StringUtil::appendLine(str, TStr(indexCount(), " Indexes"));
//    for (size_t i = 0; i < indexCount(); ++i) {
//        str.append(TStr(",(", i32(i), ")"));
//    }
//
//    if (_pFrags->get_x2f1_component() != nullptr) {
//        StringUtil::appendLine(str, TStr(fragCount(), " TCoords"));
//        for (size_t i = 0; i < fragCount(); ++i) {
//            str.append(x2f(i).toString());
//        }
//    }
//    if (_pFrags->get_n3f1_component() != nullptr) {
//        StringUtil::appendLine(str, TStr(fragCount(), " Normals"));
//        for (size_t i = 0; i < fragCount(); ++i) {
//            str.append(n3f(i).toString());
//        }
//    }
//
//    BroLogDebug(str);
//}
/**
*    @fn computeMinimax();
*
*/
void MeshSpec::computeBox() {
  beginEdit();
  getBoundBoxObject()->_max = VEC3_MIN;
  getBoundBoxObject()->_min = VEC3_MAX;

  // - Lastly, minimax
  if (fragCount() > 0) {
    for (size_t i = 0; i < fragCount(); ++i) {
      getBoundBoxObject()->_min = vec3::minv(getBoundBoxObject()->_min, v3f(i));
      getBoundBoxObject()->_max = vec3::maxv(getBoundBoxObject()->_max, v3f(i));
    }
  }
  else {
    getBoundBoxObject()->_max = getBoundBoxObject()->_min = 0.0f;
  }
  //_bIsBoundBoxCalculated = true;

  endEdit();
}
//void MeshSpec::allocateFragments(size_t count)
//{
//    DEL_MEM(_pFrags);
//    _pFrags = new FragmentBufferData(_pVertexFormat);
//    _pFrags->allocate(count);
//}
//void MeshSpec::allocateIndexes(size_t count)
//{
//    DEL_MEM(_pIndexes);
//    _pIndexes = new IndexBufferData();
//    _pIndexes->allocate(count);
//}
//void MeshSpec::allocateFaceNormals(size_t count)
//{
//    DEL_MEM(_pFaceNormals);
//
//    _pFaceNormals = new Allocator<vec3>(count);
//    _pFaceNormals->alloca(count);
//
//}
//void MeshSpec::printFragmentsToStdout()
//{
//    //if(_frags->getFormat() == t_vertex_v3c4n3x2::getType())
//    for (size_t i = 0; i < _pFrags->count(); ++i)
//    {
//        //t_vertex_v3c4n3x2 vx = *(((t_vertex_v3c4n3x2*)getFragPtr())+i);
//        Vec3f v = v3f(i);
//        if (bListFormat) {
//            std::cout << "v " << i << " ";
//        }
//        std::cout << "(" << v.x << "," << v.y << "," << v.z << ") ";
//        if (bListFormat) {
//            std::cout << std::endl;
//        }
//    }
//
//}
//void MeshSpec::printDataToStdout(bool bListFormat) {
//    /// printWeightsToStdout();
//    /// printWeightBuffersToStdout();
//
//    size_t nElements = _pVaoData->getVbo()->getNumElements();
//    std::cout << nElements << " Frags:" << std::endl;
//    v_v3n3x2* test_read = new v_v3n3x2[nElements];
//    _pVaoData->getVbo()->copyDataServerClient(nElements, test_read);
//    for (size_t i = 0; i < nElements; i++) {
//        v_v3n3x2 v = test_read[i];
//        std::cout << "(" << v.v.x << "," << v.v.y << "," << v.v.z << ") ";
//    }
//
//    delete[] test_read;
//    std::cout << std::endl;
//    std::cout << std::endl;
//
//
//}
void MeshSpec::beginEdit() {
  //Copies the mesh data from the GPU to a temporary buffer for editing.
  if (_pFrags != nullptr) {
    BRLogError("Tried to begin a mesh edit when an edit was already in progress.");
  }
  else {
    _pFrags = copyFragsFromGpu();
  }
  if (_pIndexes != nullptr) {
    BRLogError("Tried to begin a mesh edit when an edit was already in progress.");
  }
  else {
    _pIndexes = copyIndexesFromGpu();
  }
}
void MeshSpec::endEdit() {
  //ends editing copying data back to the GPU
  if (_pFrags == nullptr) {
    BRLogError("Tried to end a mesh edit when no edit in progress.");
  }
  else {
    copyFragsToGpu(_pFrags);
    _pFrags = nullptr;
  }
  if (_pIndexes == nullptr) {
    BRLogError("Tried to end a mesh edit when no edit in progress");
  }
  else {
    copyIndexesToGpu(_pIndexes);
    _pIndexes = nullptr;
  }
}
v_index32& MeshSpec::i32(size_t index) {
  if (_pIndexes == nullptr) {
    BRThrowException("Tried to edit mesh without beginEdit()");
  }
  return _pIndexes->i32(index);
}
vec3& MeshSpec::v3f(size_t index) {
  if (_pFrags == nullptr) {
    BRThrowException("Tried to edit mesh without beginEdit()");
  }
  return _pFrags->v3f(index);
}
vec2& MeshSpec::x2f(size_t index) {
  if (_pFrags == nullptr) {
    BRThrowException("Tried to edit mesh without beginEdit()");
  }
  return _pFrags->x2f(index);
}
vec4& MeshSpec::c4f(size_t index) {
  if (_pFrags == nullptr) {
    BRThrowException("Tried to edit mesh without beginEdit()");
  }
  return _pFrags->c4f(index);
}
vec3& MeshSpec::n3f(size_t index) {
  if (_pFrags == nullptr) {
    BRThrowException("Tried to edit mesh without beginEdit()");
  }
  return _pFrags->n3f(index);
}
std::shared_ptr<FragmentBufferData> MeshSpec::getFrags() {
  if (_pFrags == nullptr) {
    BRLogError("Verts: Tried to end a mesh edit when no edit in progress.");
    Gu::debugBreak();
  }
  return _pFrags;
}
std::shared_ptr<IndexBufferData> MeshSpec::getIndexes() {
  if (_pIndexes == nullptr) {
    BRLogError("Indexes: Tried to end a mesh edit when no edit in progress.");
    Gu::debugBreak();
  }
  return _pIndexes;
}
std::shared_ptr<FragmentBufferData> MeshSpec::copyFragsFromGpu() {
  AssertOrThrow2(_pVaoData != nullptr);
  std::shared_ptr<FragmentBufferData> fb = std::make_shared<FragmentBufferData>(_pVertexFormat);
  fb->allocate(_pVaoData->getVbo()->getNumElements());
  _pVaoData->getVbo()->copyDataServerClient(_pVaoData->getVbo()->getNumElements(), fb->ptr());

  return fb;
}
std::shared_ptr<IndexBufferData> MeshSpec::copyIndexesFromGpu() {
  AssertOrThrow2(_pVaoData != nullptr);
  std::shared_ptr<IndexBufferData> ib = std::make_shared<IndexBufferData>();
  ib->allocate(_pVaoData->getIbo()->getNumElements());
  _pVaoData->getIbo()->copyDataServerClient(_pVaoData->getIbo()->getNumElements(), ib->ptr());

  return ib;
}
void MeshSpec::copyFragsToGpu(std::shared_ptr<FragmentBufferData> fb) {
  AssertOrThrow2(_pVaoData != nullptr);
  _pVaoData->getVbo()->copyDataClientServer(fb->count(), fb->ptr());
}
void MeshSpec::copyIndexesToGpu(std::shared_ptr<IndexBufferData> ib) {
  AssertOrThrow2(_pVaoData != nullptr);
  _pVaoData->getIbo()->copyDataClientServer(ib->count(), ib->ptr());
}
void MeshSpec::calculateVertexNormals() {
  std::vector<vec3> faces;
  std::vector<std::vector<int32_t>> adjacency;
  beginEdit();
  //Note: this doesn't copy anything to the GPU
  // If this calculation is done before we create mesh nodes
  // then this is OK as mesh nodes will make copies of this data.

  std::shared_ptr<FragmentBufferData> pFrags = getFrags();
  std::shared_ptr<IndexBufferData> pInds = getIndexes();

  for (size_t i = 0; i < fragCount(); i++) {
    adjacency.push_back(std::vector<int32_t>());
  }

  //Faces
  for (size_t ii = 0; ii < indexCount(); ii += 3) {
    int32_t iFace = (int32_t)ii / 3;

    int32_t i0 = pInds->i32(ii + 0);
    int32_t i1 = pInds->i32(ii + 1);
    int32_t i2 = pInds->i32(ii + 2);
    vec3 v0 = pFrags->v3f(i0);
    vec3 v1 = pFrags->v3f(i1);
    vec3 v2 = pFrags->v3f(i2);

    vec3 d0 = v2 - v0;
    vec3 d1 = v1 - v0;

    vec3 dNorm = d0.cross(d1).normalize();

    faces.push_back(dNorm);
    Gu::addIfDoesNotContain(adjacency[i0], iFace);
    Gu::addIfDoesNotContain(adjacency[i1], iFace);
    Gu::addIfDoesNotContain(adjacency[i2], iFace);
  }
  for (size_t vi = 0; vi < adjacency.size(); vi++) {
    vec3 n = vec3(0, 0, 0);
    for (size_t ai = 0; ai < adjacency[vi].size(); ai++) {
      int32_t faceIndex = adjacency[vi][ai];
      n += faces[faceIndex];
    }
    n /= (float)adjacency[vi].size();
    n.normalize();
    pFrags->n3f(vi) = n;
  }

  //Clear data
  for (size_t i = 0; i < fragCount(); i++) {
    adjacency[i].clear();
  }

  adjacency.clear();
  faces.clear();

  endEdit();
}
//void MeshSpec::createMaterial(t_string strName, std::shared_ptr<Texture2DSpec> tmd, std::shared_ptr<Texture2DSpec> tmn) {
//    DEL_MEM(_pMaterial);
//
//    _pMaterial = new Material(Gu::getGraphicsContext(), strName);
//    if (tmd != nullptr) {
//        _pMaterial->addTextureBinding(tmd, TextureChannel::e::Channel0);
//    }
//    if (tmn != nullptr) {
//        _pMaterial->addTextureBinding(tmn, TextureChannel::e::Channel1);
//    }
//}
void MeshSpec::fillWeightBuffersMob(std::shared_ptr<ModelSpec> ms) {

  // Debug values
  int32_t minOrd = 100000;
  int32_t maxOrd = 0;
  int32_t minCount = INT_MAX;
  int32_t maxCount = -INT_MAX;
  int32_t minOff = INT_MAX;
  int32_t maxOff = -INT_MAX;

  //Main weight values
  uint32_t ind = 0;
  uint32_t off = 0;
  uint32_t num_added_weights = 0;

  int64_t nFragInvalid = 0;
  int64_t nJointInvalid = 0;
  int64_t iValidJointId = -1;

  uint32_t nFragCount = (uint32_t)fragCount();
  size_t nTotalWeights = 0;
  for (VertexWeightMob w : _vecWeightsMob) {
    for (std::pair<int32_t, std::map<int32_t, float>> parm : w._mapWeights) {
      for (std::pair<int32_t, float> pweight : parm.second) {
        nTotalWeights++;
      }
    }
  }
  AssertOrThrow2(nFragCount > 0);
  AssertOrThrow2(nTotalWeights > 0);

  GpuAnimatedMeshWeightData* weightOffsetsGpu = new GpuAnimatedMeshWeightData[nFragCount];
  GpuAnimatedMeshWeight* weightsGpu = new GpuAnimatedMeshWeight[nTotalWeights];

  _pWeightOffsetsGpu = std::make_shared<ShaderStorageBuffer>(Gu::getCoreContext(), sizeof(GpuAnimatedMeshWeightData));
  _pWeightsGpu = std::make_shared<ShaderStorageBuffer>(Gu::getCoreContext(), sizeof(GpuAnimatedMeshWeight));

  std::set<int32_t> setUniqueJointOrdinals;

  //bool bAnus = false;
  //if(false){
  //    bAnus = true;
  //}

  for (uint32_t iFrag = 0; iFrag < nFragCount; ++iFrag) {
    VertexWeightMob& curWeight = _vecWeightsMob.at(iFrag);

    //Copy each weight.
    int32_t nLocalInvalid = 0;
    int32_t nTotalWeightsForVert = 0;
    //Loop <armature name hashed, <weights>>
    for (std::pair<int32_t, std::map<int32_t, float>> parms : curWeight._mapWeights) {
      //Loop <bone id for armature, weight>
      for (std::pair<int32_t, float> pweights : parms.second) {
        int32_t iJointOrdinal = getGpuJointOrdinal(ms, parms.first, pweights.first);
        float fWeight = pweights.second;

        setUniqueJointOrdinals.insert(iJointOrdinal);

        if (iJointOrdinal < 0) { //JOINT_ORDINAL_INVALID = -1
          nLocalInvalid++;
        }
        else if (iValidJointId == -1) {
          iValidJointId = iJointOrdinal;
        }
        //if(bAnus) { iJointOrdinal = 1-iJointOrdinal; }

        weightsGpu[num_added_weights]._iArmJointOffset = iJointOrdinal;
        weightsGpu[num_added_weights]._weight = fWeight;
        num_added_weights++;
        nTotalWeightsForVert++;

        //Testing / Debug
        if (iJointOrdinal >= 0 && iJointOrdinal > maxOrd) {
          maxOrd = iJointOrdinal;
        }
        if (iJointOrdinal >= 0 && iJointOrdinal < minOrd) {
          minOrd = iJointOrdinal;
        }

      }
    }
    if (nLocalInvalid > 0) {
      nFragInvalid++;
    }
    nJointInvalid += nLocalInvalid;

    //Calculate weight offsets last because we need to count the total number of weights in the buffer.
    weightOffsetsGpu[iFrag]._offset = off;
    weightOffsetsGpu[iFrag]._count = nTotalWeightsForVert;
    off += weightOffsetsGpu[iFrag]._count;

    ////Testing / Debug
    if (weightOffsetsGpu[iFrag]._count > maxCount) {
      maxCount = weightOffsetsGpu[iFrag]._count;
    }
    if (weightOffsetsGpu[iFrag]._count < minCount) {
      minCount = weightOffsetsGpu[iFrag]._count;
    }
    if (weightOffsetsGpu[iFrag]._offset < minOff) {
      minOff = weightOffsetsGpu[iFrag]._offset;
    }
    if (weightOffsetsGpu[iFrag]._offset > maxOff) {
      maxOff = weightOffsetsGpu[iFrag]._offset;
    }

  }

  //Fix invalid weights 
  if (nFragInvalid > 0) {
    for (uint32_t iW = 0; iW < num_added_weights; ++iW) {
      if (weightsGpu[iW]._iArmJointOffset < 0) {
        weightsGpu[iW]._iArmJointOffset = (GpuInt)iValidJointId;
        weightsGpu[iW]._weight = 0.0f; //disablinmg weght
      }
    }
  }

  ////copy data to gpu.
  _pWeightOffsetsGpu->allocate(nFragCount);
  _pWeightOffsetsGpu->copyDataClientServer(nFragCount, (void*)weightOffsetsGpu);
  _pWeightsGpu->allocate(nTotalWeights);
  _pWeightsGpu->copyDataClientServer(nTotalWeights, (void*)weightsGpu);

  testAccess(ms, weightOffsetsGpu, nFragCount, weightsGpu, nTotalWeights, &_vecWeightsMob);

  delete[] weightOffsetsGpu;
  delete[] weightsGpu;

  ////Skin Data Logging
  if (nFragInvalid > 0) {
    BRLogError(nJointInvalid + " invalid Joint IDs Encountered making "
      + nFragInvalid + " skin vertexes invalid, default Joint ID "
      + " - no joint. Check that SKN file has valid Joint IDs for vertexes.");
  }

  if (maxOrd > 200000) {
    BRLogWarn("Max joint index was HUGE, possibly invalid: " + maxOrd);
  }
  if (minOrd < 0) {
    BRLogWarn("Min joint index was invalid: " + minOrd);
  }

  BRLogDebug("Skin Stats:");
  BRLogDebug(">  min/max joint ord: " + minOrd + "/" + maxOrd);
  BRLogDebug(">  min/max weight off: " + minOff + "/" + maxOff);
  BRLogDebug(">  min/max weight cnt: " + minCount + "/" + maxCount);
  BRLogDebug(">  nTotalWeights: " + nTotalWeights);
  BRLogDebug(">  nFragCount: " + nFragCount);
  BRLogDebug(">  nJoints: " + setUniqueJointOrdinals.size());

  if (maxOrd >= (int32_t)setUniqueJointOrdinals.size()) {
    BRLogWarn(">  one or more ordinals exceeds joint matrix array size. Definite Gpu buffer overrun.");
  }
  if (maxOff >= (int32_t)nTotalWeights) {
    BRLogWarn(">  maxoff exceeds vertex array size. Definite Gpu buffer overrun.");
  }

  Gu::getCoreContext()->chkErrRt();
  setUniqueJointOrdinals.clear();

  //**We still need the vec weights to compute bone boxes later (optimally putting them here.
  //**We no longer need _vecWeights;
 // _vecWeightsMob.resize(0);
}
int32_t MeshSpec::getGpuJointOrdinal(std::shared_ptr<ModelSpec> ms, int32_t arm, int32_t joint) {
  //**This must corespond to ArmatureNode : _vecNodesOrdered
  //Because we allow multiple armatures now we have to sort our buffers by [Arm1, [bone,bone]... Arm2 [bone, bone]..]

  int32_t iJointOrd = 0;
  for (std::pair<int32_t, std::shared_ptr<Armature>> parm : ms->getArmatureMapOrdered()) {
    int32_t parm_joint = 0;
    for (std::pair<int32_t, std::shared_ptr<BoneSpec>> pbone : *parm.second->getBoneCacheOrdered()) {

      if (parm.first == arm && parm_joint == joint) {
        return iJointOrd;
      }

      parm_joint++;
      iJointOrd++;
    }
  }
  return -1;//Not Found
}

void MeshSpec::testAccess(std::shared_ptr<ModelSpec> ms, GpuAnimatedMeshWeightData* weightOffsetsGpu, size_t weightOffsetsGpuSize,
  GpuAnimatedMeshWeight* weightsGpu, size_t weightsGpuSize, std::vector<VertexWeightMob>* vecWeights) {
  if (_eSkinStatus != MeshSkinStatus::e::Uninitialized) {
    return;
  }

  BRLogDebug("Testing Skin Access..");

  //Armature Existence
  for (size_t iweight = 0; iweight < vecWeights->size(); ++iweight) {
    VertexWeightMob& vw = vecWeights->at(iweight);
    for (std::pair<Hash32, std::map<int32_t, float>> parms : vw._mapWeights) {
      if (ms->getArmatureById(parms.first) == nullptr) {
        BRLogError("Skin test failed.  Armature does not exist for one or more weights.");
        _eSkinStatus = MeshSkinStatus::e::Error;
        Gu::debugBreak();
        //break;
      }
    }
  }
  //Buffer Access
  for (size_t iWeightOff = 0; iWeightOff < weightOffsetsGpuSize; ++iWeightOff) {
    int32_t off = weightOffsetsGpu[iWeightOff]._offset;
    int32_t count = weightOffsetsGpu[iWeightOff]._count;
    if (off + count > (int32_t)weightsGpuSize) {
      BRLogError("Skin test failed.  Weight offset " + (off + count) + " was outside bounds of " + weightsGpuSize);
      _eSkinStatus = MeshSkinStatus::e::Error;
      Gu::debugBreak();
    }
    else {
      for (int iWeight = 0; iWeight < count; ++iWeight) {
        GpuAnimatedMeshWeight& gpuWeight = weightsGpu[off + iWeight];
        if (gpuWeight._iArmJointOffset < 0 || gpuWeight._iArmJointOffset > 10000) {
          BRLogError("Skin test failed.  Joint " + gpuWeight._iArmJointOffset + " invalid (or greater than 1000) ");
          _eSkinStatus = MeshSkinStatus::e::Error;
          Gu::debugBreak();
        }
        if (gpuWeight._weight < 0.0f) {
          BRLogError("Skin test failed.  Weight was invalid: " + gpuWeight._weight);
          _eSkinStatus = MeshSkinStatus::e::Error;
          Gu::debugBreak();
        }
      }


    }
  }
  //}

  BRLogDebug("..Skin test complete.");
}
void MeshSpec::deserialize(std::shared_ptr<BinaryFile> fb) {
  BaseSpec::deserialize(fb);

  fb->readBool(_bHideRender);
  bool bHasShape;
  fb->readBool(bHasShape);
  if (bHasShape) {
    int32_t shape;
    fb->readInt32(shape);
    PhysicsShapeType::e eShapeType = (PhysicsShapeType::e)shape;
    if (eShapeType == PhysicsShapeType::e::Hull) {
      _pPhysicsShape = std::make_shared<HullShape>();
    }
    if (eShapeType == PhysicsShapeType::e::Sphere) {
      _pPhysicsShape = std::make_shared<SphereShape>();
    }
    _pPhysicsShape->deserialize(fb);
  }
  int32_t iFormat;
  fb->readInt32(iFormat);
  int32ToVertexFormat(iFormat);

  int32_t nFrags;
  fb->readInt32(nFrags);
  std::shared_ptr<FragmentBufferData> pFrags = std::make_shared<FragmentBufferData>(_pVertexFormat);
  pFrags->allocate(nFrags);
  fb->read((const char*)pFrags->getBuf()->ptr(), pFrags->getBuf()->byteSize());

  int32_t nIndexes;
  fb->readInt32(nIndexes);
  std::shared_ptr<IndexBufferData> pIndexes = std::make_shared<IndexBufferData>();
  pIndexes->allocate(nIndexes);
  fb->read((const char*)pIndexes->getBuf()->ptr(), pIndexes->getBuf()->byteSize());

  allocMesh(pFrags->ptr(), nFrags, pIndexes->ptr(), nIndexes);

  //1/19 - verts are now always in GPU
  //sendVertsToGpu();

  bool bHasSkin;
  fb->readBool(bHasSkin);
  if (bHasSkin) {

    int32_t nWeightOffsets;
    fb->readInt32(nWeightOffsets);
    if (nWeightOffsets > 0) {
      GpuAnimatedMeshWeightData* weightOffsetsGpu = new GpuAnimatedMeshWeightData[nWeightOffsets];
      fb->read((const char*)weightOffsetsGpu, sizeof(GpuAnimatedMeshWeightData) * nWeightOffsets);
      _pWeightOffsetsGpu = std::make_shared<ShaderStorageBuffer>(Gu::getCoreContext(), sizeof(GpuAnimatedMeshWeightData));
      _pWeightOffsetsGpu->allocFill(nWeightOffsets, (void*)weightOffsetsGpu);
      delete[] weightOffsetsGpu;
    }

    int32_t nWeights;
    fb->readInt32(nWeights);
    if (nWeights > 0) {
      GpuAnimatedMeshWeight* weightsGpu = new GpuAnimatedMeshWeight[nWeights];
      fb->read((const char*)weightsGpu, sizeof(GpuAnimatedMeshWeight) * nWeights);
      _pWeightsGpu = std::make_shared<ShaderStorageBuffer>(Gu::getCoreContext(), sizeof(GpuAnimatedMeshWeight));
      _pWeightsGpu->allocFill(nWeights, (void*)weightsGpu);
      delete[] weightsGpu;
    }

    if (nWeights > 0 && nWeightOffsets > 0) {
      _eSkinStatus = MeshSkinStatus::e::Allocated;
    }
    else if ((nWeights > 0 && nWeightOffsets == 0) || (nWeights == 0 && nWeightOffsets > 0)) {
      BRLogError(" Weights were invalid for mesh.");
      Gu::debugBreak();
    }
  }

  //if (false) {
  //    this->debugPrintDataToStdOut();
  //}

  bool bHasMaterial;
  fb->readBool(bHasMaterial);
  if (bHasMaterial) {
    std::shared_ptr<Material> mat = std::make_shared<Material>();
    mat->deserialize(fb);
    setMaterial(mat);
  }
  else {
    BRLogWarn("Mesh didn't have a material..");
  }
}

int32_t MeshSpec::vertexFormatToInt32() {
  if (_pVertexFormat == v_v3n3x2::getVertexFormat()) { return 1; }
  else if (_pVertexFormat == v_v3x2::getVertexFormat()) { return 2; }
  else if (_pVertexFormat == v_v3n3::getVertexFormat()) { return 3; }
  else if (_pVertexFormat == v_v3::getVertexFormat()) { return 4; }
  else BRThrowException("Unsupported vertex format while serializing");
}
void MeshSpec::int32ToVertexFormat(int32_t i) {
  if (i == 1) { _pVertexFormat = v_v3n3x2::getVertexFormat(); }
  else if (i == 2) { _pVertexFormat = v_v3x2::getVertexFormat(); }
  else if (i == 3) { _pVertexFormat = v_v3n3::getVertexFormat(); }
  else if (i == 4) { _pVertexFormat = v_v3::getVertexFormat(); }
  else BRThrowException("Unsupported vertex format while deserializing");
}
void MeshSpec::serialize(std::shared_ptr<BinaryFile> fb) {
  BaseSpec::serialize(fb);

  fb->writeBool(std::move(_bHideRender));
  fb->writeBool(_pPhysicsShape != nullptr);
  if (_pPhysicsShape) {
    //int32_t shape;
    if (std::dynamic_pointer_cast<HullShape>(_pPhysicsShape) != nullptr) {
      fb->writeInt32(PhysicsShapeType::e::Hull);
    }
    else if (std::dynamic_pointer_cast<SphereShape>(_pPhysicsShape) != nullptr) {
      fb->writeInt32(PhysicsShapeType::e::Sphere);
    }
    else {
      BRThrowException("failed to write physcis shape, invalid shape type");
    }
    _pPhysicsShape->serialize(fb);
  }
  fb->writeInt32(vertexFormatToInt32());

  beginEdit();
  {
    std::shared_ptr<FragmentBufferData> pFrags = getFrags();
    std::shared_ptr<IndexBufferData> pIndexes = getIndexes();

    fb->writeInt32((int32_t)pFrags->count());
    fb->write((const char*)pFrags->getBuf()->ptr(), pFrags->getBuf()->byteSize());
    fb->writeInt32((int32_t)pIndexes->count());
    fb->write((const char*)pIndexes->getBuf()->ptr(), pIndexes->getBuf()->byteSize());
  }
  endEdit();

  if (_eSkinStatus == MeshSkinStatus::e::Allocated) {
    fb->writeBool(true);

    //write weights
    if (_pWeightOffsetsGpu != nullptr) {
      GpuAnimatedMeshWeightData* buf = new GpuAnimatedMeshWeightData[_pWeightOffsetsGpu->getNumElements()];
      size_t bufSizeBytes = sizeof(GpuAnimatedMeshWeightData) * _pWeightOffsetsGpu->getNumElements();
      _pWeightOffsetsGpu->syncRead((void*)buf, bufSizeBytes);
      fb->writeInt32((int32_t)_pWeightOffsetsGpu->getNumElements());
      fb->write((const char*)buf, bufSizeBytes);
      delete[] buf;
    }
    else {
      fb->writeInt32(0);
    }
    if (_pWeightsGpu != nullptr) {
      GpuAnimatedMeshWeight* buf = new GpuAnimatedMeshWeight[_pWeightsGpu->getNumElements()];
      size_t bufSizeBytes = sizeof(GpuAnimatedMeshWeight) * _pWeightsGpu->getNumElements();
      _pWeightsGpu->syncRead((void*)buf, bufSizeBytes);
      fb->writeInt32((int32_t)_pWeightsGpu->getNumElements());
      fb->write((const char*)buf, bufSizeBytes);
      delete[] buf;
    }
    else {
      fb->writeInt32(0);
    }
  }
  else {
    fb->writeBool(false);

  }

  fb->writeBool(_pMaterial != nullptr);
  if (_pMaterial) {
    _pMaterial->serialize(fb);
  }

}
//void MeshSpec::sendVertsToGpu(){
//    _pVertsGpu = std::make_sVboData(Gu::getGraphicsContext(), _pVertexFormat->getSizeBytes());
//    _pVertsGpu->allocate(_pFrags->count());
//    _pVertsGpu->copyDataClientServer(_pFrags->count(), _pFrags->ptr());
//}



}//ns game
