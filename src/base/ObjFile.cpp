#include "../base/ObjFile.h"
#include "../base/Logger.h"
#include "../base/Gu.h"
#include "../base/StringUtil.h"
#include "../base/BinaryFile.h"
#include "../model/MeshNode.h"
#include "../model/VertexFormat.h"
#include "../model/MeshSpec.h"

namespace BR2 {
ObjFile::ObjFile(std::shared_ptr<GLContext> c) :
  _pContext(c)
  , _bDebugDisableVertexCompression(false) //**Set this to false to disable compressed vertexes (optimized meshes)
{
  _iCurrentLine = 0;
}
ObjFile::~ObjFile() {
  _vecVerts.resize(0);
  _vecNormals.resize(0);
  _vecTCoords.resize(0);
  _vecMeshVerts.resize(0);

  clearVertexCache();
}
void ObjFile::load(string_t& strFilePath, bool flipWinding) {
  BinaryFile bufferedFile;
  uint32_t iLastGroupOffset;    // - For NON relative vertexes (ones with no '-' in front of their indexes we have

  _sFileName = strFilePath;
  _bFlipWinding = flipWinding;

  iLastGroupOffset = 0;  // to keep track of this in order to not offset them in the buffer.

  // - Read the file into memory (that is the point of a buffered file).
  bool ret = bufferedFile.loadFromDisk(strFilePath);

  if (ret != true)
    BRThrowException("Failed to open Object File.");

  loadObjFileContents(bufferedFile);

  //Load the last spec
  addCurrentSpec();
}
vec3 ObjFile::readVec3(BinaryFile& pBufferedFile) {
  vec3 ret;
  ret.x = TypeConv::strToFloat(pBufferedFile.getTok());
  ret.y = TypeConv::strToFloat(pBufferedFile.getTok());
  ret.z = TypeConv::strToFloat(pBufferedFile.getTok());
  return ret;
}
vec2 ObjFile::readVec2(BinaryFile& pBufferedFile) {
  vec2 ret;
  ret.x = TypeConv::strToFloat(pBufferedFile.getTok());
  ret.y = TypeConv::strToFloat(pBufferedFile.getTok());
  return ret;
}
void ObjFile::loadObjFileContents(BinaryFile& pBufferedFile) {
  string_t    tok;
  vec3 temp_v3;
  vec2 temp_v2;

  while (!pBufferedFile.eof()) {
    tok = pBufferedFile.getTok(); // plows through whitespace.

    if (!tok.compare("mtllib")) {
      //20160509 Not Supported
    }
    else if (!tok.compare("usemtl")) {
      //20160509 Not Supported
    }
    else if (!tok.compare("v")) {
      _vecVerts.push_back(readVec3(pBufferedFile));
    }
    else if (!tok.compare("vn")) {
      _vecNormals.push_back(readVec3(pBufferedFile));
    }
    else if (!tok.compare("vt")) {
      _vecTCoords.push_back(readVec2(pBufferedFile));
    }
    else if (!tok.compare("g")) {
      parseGeom(pBufferedFile, tok);
    }
    else if (!tok.compare("f")) {
      parseFace(pBufferedFile, tok);
    }
    else if (!tok.compare("mat")) {
      BRLogWarn("Mesh Spec Local Matrix is deprectaed. Freeze all matrices before exporting meshes.");
      _matLocalMatrix = parseMat4(pBufferedFile);
    }
    _iCurrentLine++;

    // - Eat to the next line, that is eat \r\n
    pBufferedFile.eatLine();
  }

}
void ObjFile::parseGeom(BinaryFile& pBufferedFile, string_t& tok) {
  tok = pBufferedFile.getTokSameLineOrReturnEmpty();

  if (tok.length() > 0) {
    addCurrentSpec();
    _pCurrentSpec = std::make_shared<MeshSpec>(tok, ModelVertexType::getVertexFormat(), shared_from_this());
    //        _pCurrentSpec->setFileName(_sFileName);
  }
}
void ObjFile::parseFace(BinaryFile& pBufferedFile, string_t& tok) {
  std::vector <string_t> strVec;
  int32_t iComp;
  int32_t indices[3];

  // - Parse face groups x,y,z:  x/x/x  y/y/y  z/z/z
  for (int igroup = 0; igroup < 3; ++igroup) {
    tok = pBufferedFile.getTok();
    int32_t strlind = 0; // last index of '/' + 1 (one char after it)
    int32_t curComponent = 0; // the current component ex "v" inside of the v/t/n etc

    tok = StringUtil::trim(tok);
    StringUtil::split(tok, '/', strVec);

    //Parse face indexes x1/x2/x3
    for (iComp = 0; iComp < (int32_t)strVec.size(); ++iComp) {
      indices[iComp] = parseFaceComponent(tok, strlind, iComp);
    }

    addFaceVertex(indices[0], indices[1], indices[2]);//v, x, n

    strVec.clear();
  }
}
int32_t ObjFile::parseFaceComponent(string_t& tok, int32_t& strlind, int32_t iComponent) {
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
    //idx = ObjectFile::MissingIndexValue;

    //20160509 we don't allow varied formats.
    //You're missing Tex Coords, Or Normals
    BRThrowException(Stz "Invalid object file format.  Missing face index." + iComponent + " at line " + _iCurrentLine);
  }
  else {
    rt = tok.substr(strlind, strind - strlind);
    idx = TypeConv::strToInt(rt);
  }

  strlind = (int32_t)strind + 1; // cut out the '//'

  return idx;
}
void ObjFile::addFaceVertex(int32_t iVertex, int32_t iTCoord, int32_t iNormal) {
  int32_t vi = iVertex - 1;
  int32_t ni = iNormal - 1;
  int32_t xi = iTCoord - 1;

  if (vi < 0)
    BRThrowException("Vertex buffer underflow at line " + _iCurrentLine);
  if (ni < 0)
    BRThrowException("Normal buffer underflow at line " + _iCurrentLine);
  if (xi < 0)
    BRThrowException("TCoord buffer underflow at line " + _iCurrentLine);
  if (vi >= (int32_t)_vecVerts.size())
    BRThrowException("Vertex buffer overflow at line " + _iCurrentLine);
  if (ni >= (int32_t)_vecNormals.size())
    BRThrowException("Normal buffer overflow at line " + _iCurrentLine);
  if (xi >= (int32_t)_vecTCoords.size())
    BRThrowException("TCoord buffer overflow at line " + _iCurrentLine);

  int32_t newIndex = 0;

  //We're using this "vertex cache" to reduce vertex size by referenced face components in the OBJ
  newIndex = findCachedVertex(vi, xi, ni);
  if (newIndex < 0 || _bDebugDisableVertexCompression)
    newIndex = addNewMeshVertex(vi, xi, ni);

  _vecMeshIndexes.push_back(newIndex);
}
int32_t ObjFile::addNewMeshVertex(int32_t vi, int32_t xi, int32_t ni) {
  int32_t newIndex;
  ModelVertexType vx;

  vx.v = _vecVerts[vi];
  vx.n = _vecNormals[ni];
  vx.x = _vecTCoords[xi];
  // vx.c = vec4(1, 1, 1, 1);

  newIndex = (int32_t)_vecMeshVerts.size();

  _vecMeshVerts.push_back(vx);

  if (!_bDebugDisableVertexCompression) {
    addCachedVertex(newIndex, vi, xi, ni);
  }

  //add to conversion mapping.
  //_pCurrentSpec->tryCreateConversionMapping();
  //_pCurrentSpec->getConversionMapping()->addIfNew(vi, newIndex);

  return newIndex;
}
void ObjFile::addCurrentSpec() {
  if (_pCurrentSpec != NULL) {
    // Copy  spec vertexes.
    copySpecFragments(_pCurrentSpec);

    _vecMeshes.push_back(_pCurrentSpec);

    _vecMeshVerts.resize(0);
    _vecMeshIndexes.resize(0);
    clearVertexCache();
  }
  _matLocalMatrix = mat4::identity();
  _pCurrentSpec = NULL;

}

void ObjFile::copySpecFragments(std::shared_ptr<MeshSpec> pSpec) {
  if (_bFlipWinding) {
    for (size_t iInd = 0; iInd < _vecMeshIndexes.size(); iInd += 3) {
      v_index32 t = _vecMeshIndexes[iInd + 1];
      _vecMeshIndexes[iInd + 1] = _vecMeshIndexes[iInd + 2];
      _vecMeshIndexes[iInd + 2] = t;
    }
  }
  pSpec->allocMesh((const void*)_vecMeshVerts.data(), _vecMeshVerts.size(), _vecMeshIndexes.data(), _vecMeshIndexes.size());
  pSpec->computeBox();
}

int32_t ObjFile::findCachedVertex(int32_t vi, int32_t xi, int32_t ni) {
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
void ObjFile::addCachedVertex(int32_t newIndex, int32_t vi, int32_t xi, int32_t ni) {
  ivec3* v = new ivec3();
  v->x = vi;
  v->y = xi;
  v->z = ni;

  AssertOrThrow2(findCachedVertex(vi, xi, ni) == -1);

  _mapVertexCache.insert(std::make_pair(v, newIndex));
}
void ObjFile::clearVertexCache() {
  std::map<ivec3*, int32_t, ivec3::Vec3xCompLess>::iterator ite = _mapVertexCache.begin();
  for (; ite != _mapVertexCache.end(); ite++) {
    ivec3* ii = ite->first;
    DEL_MEM(ii);
  }
  _mapVertexCache.clear();
}
mat4 ObjFile::parseMat4(BinaryFile& bf) {
  // - Parse csv matrix string.
  string_t mat_str = bf.getTok();
  mat4 mOut;
  mat4::parse(mat_str, mOut);
  return mOut;

}

}//ns Game
