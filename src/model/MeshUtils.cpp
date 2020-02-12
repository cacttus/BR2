#include "../base/GLContext.h"
#include "../base/Logger.h"
#include "../base/Exception.h"

#include "../base/TypeConv.h"
#include "../base/StringUtil.h"
#include "../model/MeshUtils.h"
#include "../model/MeshData.h"
#include "../model/VertexFormat.h"
#include "../model/FragmentBufferData.h"
#include "../model/IndexBufferData.h"
#include "../model/MeshNode.h"

namespace BR2 {

/**
*    @fn parseGenParams
*    @brief Parse a string defining generation params for a mesh into a class structure.

    for now class is just "gen"
    then specify algirhtm
    then specify parameters like calling a class method.

    gen.cone(1,1,1)

*/
bool MeshGenParams::logAndExit(string_t str) {
  BroLogError(str);
  return false;
}
bool MeshGenParams::parse(string_t genString) {
  // - 
  char* cx = new char[genString.length() + 1];
  int bufsiz = 128;
  char buf[128];
  char bufind = 0;
  int parseMode = 0;
  memset(cx, 0, genString.length() + 1);
  memset(buf, 0, bufsiz);
  strcpy_s(cx, genString.length() + 1, genString.c_str());
  int iparameter = 0;

  // - 
  char* c = cx;
  char ptc;
  while (*c) {
    ptc = *c;
    if (isalnum((int)ptc)) {
      buf[bufind++] = tolower(ptc);
    }
    else if (ptc == '.') {
      //if(parseMode==0);
      if (!(strlen((char*)buf))) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");
      _strClass = string_t(buf);
      memset(buf, 0, bufsiz);
      bufind = 0;
      parseMode = 1;
    }
    else if (ptc == '(') {
      if (!(parseMode == 1)) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");
      if (!(strlen((char*)buf))) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");
      iparameter = 0;
      string_t algorithm = string_t(buf);

      _eGenType = getGenerationAlgorithmFromString(algorithm);

      memset(buf, 0, bufsiz);
      bufind = 0;
      parseMode = 2;
    }
    else if (ptc == ',') {
      if (!(parseMode == 2)) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");;
      if (!(strlen((char*)buf))) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");;
      if (!(iparameter < MeshGenParams::MaxParams)) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");;
      _strParams[iparameter++] = string_t(buf);
      memset(buf, 0, bufsiz);
      bufind = 0;
      parseMode = 2;
    }
    else if (ptc == ')') {
      if (!(parseMode == 2)) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");;
      if (!(strlen((char*)buf))) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");;
      if (!(iparameter < MeshGenParams::MaxParams)) return logAndExit(" [MESH GEN] Sequence error parsing mesh gen string.");;
      _strParams[iparameter++] = string_t(buf);
      memset(buf, 0, bufsiz);
      bufind = 0;
      parseMode = 3;
    }

    c++;
  }
  delete[] cx;

  _uiParamCount = iparameter;

  if (parseMode != 3) {
    return logAndExit(" [MESH GEN] Format error in mesh generation parse string.");
  }

  return true;
}
MeshGenType::e MeshGenParams::getGenerationAlgorithmFromString(string_t str) {
  if (str == "cone")
    return MeshGenType::e::MGA_CONE;
  else if (str == "sphere")
    return MeshGenType::e::MGA_SPHERE;
  else if (str == "circle")
    return MeshGenType::e::MGA_CIRCLE;
  else if (str == "box")
    return MeshGenType::e::MGA_BOX;
  else if (str == "plane2")
    return MeshGenType::e::MGA_PLANE_2_SIDE;
  else if (str == "plane2x")
    return MeshGenType::e::MGA_CROSS_2_SIDE;

  BroThrowException(" [MESH GEN] Failed to get generation algorithm from string ");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool parseGenParams(string_t genString, MeshGenParams& __out_ params) {
  return params.parse(genString);
}


/**
*    @fn generateFromParameters
*    @brief
*/
std::shared_ptr<MeshData> MeshUtils::generateFromParameters(MeshGenParams* pgp) {
  std::shared_ptr<MeshData> ret_spec = NULL;
  MeshGenType::e mga = pgp->getGenType();

  int px = 0;

  if (mga == MeshGenType::e::MGA_CONE) {
    float a = TypeConv::strToFloat(pgp->getParam(px++));
    float b = TypeConv::strToFloat(pgp->getParam(px++));
    int32_t c = TypeConv::strToInt(pgp->getParam(px++));

    ret_spec = makeCone(a, b, c);
  }
  else if (mga == MeshGenType::e::MGA_SPHERE) {
    float a = TypeConv::strToFloat(pgp->getParam(px++));
    int32_t b = TypeConv::strToInt(pgp->getParam(px++));
    int32_t c = TypeConv::strToInt(pgp->getParam(px++));

    ret_spec = makeSphere(a, b, c);
  }
  else if (mga == MeshGenType::e::MGA_C2BB2S) {
    float a = TypeConv::strToFloat(pgp->getParam(px++));
    int32_t b = TypeConv::strToInt(pgp->getParam(px++));
    int32_t c = TypeConv::strToInt(pgp->getParam(px++));

    ret_spec = makeSphere(a, b, c);
  }
  else if (mga == MeshGenType::e::MGA_PLANE_2_SIDE) {
    float a = TypeConv::strToFloat(pgp->getParam(px++));
    float b = TypeConv::strToFloat(pgp->getParam(px++));
    bool c = TypeConv::strToBool(pgp->getParam(px++));

    ret_spec = makeBillboardXY(a, b, c);
  }
  else if (mga == MeshGenType::e::MGA_CROSS_2_SIDE) {
    float a = TypeConv::strToFloat(pgp->getParam(px++));
    float b = TypeConv::strToFloat(pgp->getParam(px++));
    bool c = TypeConv::strToBool(pgp->getParam(px++));

    ret_spec = makeCrossboardXY(a, b, c);
  }
  return ret_spec;
}
/**
*    @fn generate_arc_segment
*    Generate an arc segment -  a square mapped onto a paraboloid
*/
std::shared_ptr<MeshData> MeshUtils::makeArcSegment(float radius, float radius2, float refHeight, vec3 refPos, int32_t slices) {
  std::shared_ptr<MeshData> ma = std::make_shared<MeshData>(StringUtil::generate(), MeshMakerVert::getVertexFormat());//VertexFormatType::V_V3C4N3X2,IndexFormatType::INDEX_32_BIT_TYPE);
  int32_t nVerts = (slices + 1) * (slices + 1);
  int32_t nIndexes = (slices) * (slices) * 2 * 3;
  int32_t nFaces = nIndexes / 3;
  ma->allocMesh(nVerts, nIndexes);

  float aRadius = radius;
  float pRadius = radius2;

  // - Texture wrapping
  float texWrapX = 1.0f;
  float texWrapY = 1.0f;

  // - Clockwise vertex winding?
  bool cw_winding = false;

  float res = (aRadius) * (aRadius)-(pRadius) * (pRadius);
  float sqrtres = MathUtils::broSqrt(res);
  float PlaneSize = 2.0f * sqrtres;

  float PlaneDelta = PlaneSize / (float)slices;
  float TextureDelta = 2.0f / (float)slices;

  // variables used during the dome's generation
  float x_dist = 0.0f;
  float z_dist = 0.0f;
  float x_height = 0.0f;
  float z_height = 0.0f;
  float height = 0.0f;

  vec3 SV; // temporary vertex
  int count = 0;
  size_t offset;

  for (int i = 0; i <= slices; i++) {
    for (int j = 0; j <= slices; j++) {
      offset = i * (slices + 1) + j;
      AssertOrThrow2(offset < (size_t)nVerts);

      x_dist = (-0.5f * PlaneSize) + ((float)j * PlaneDelta);
      z_dist = (-0.5f * PlaneSize) + ((float)i * PlaneDelta);

      x_height = (x_dist) * (x_dist) / aRadius;
      z_height = (z_dist) * (z_dist) / aRadius;

      height = x_height + z_height;

      SV.x = x_dist;
      SV.y = 0.0f - height;
      SV.z = z_dist;

      ma->v3f(offset).x = SV.x;
      ma->v3f(offset).y = SV.y;
      ma->v3f(offset).z = SV.z;
      ma->x2f(offset).x = texWrapX * ((float)j * TextureDelta * 0.5f);
      ma->x2f(offset).y = texWrapY * (1.0f - (float)i * TextureDelta * 0.5f);

      ma->c4f(offset) = Color4f(1, 1, 1, 1);
    }
  }
  size_t fn_index; //face normal index
  // Done calculating vertices, calculate the indices
  int index = 0;
  Vec3f va, vb, vc, d1, d2;
  //Indices = new unsigned short[NumIndices];
  for (int i = 0; i < slices; i++) {
    for (int j = 0; j < slices; j++) {
      int startvert = (i * (slices)+j);

      if (cw_winding == false) {
        // Triangle 1
        ma->i32(index++) = startvert + slices + 1;
        ma->i32(index++) = startvert;
        ma->i32(index++) = startvert + 1;

        fn_index = (index - 3) / 3;

        // - Face Normal 1
        va = ma->v3f(ma->i32(index - 3));
        vb = ma->v3f(ma->i32(index - 2));
        vc = ma->v3f(ma->i32(index - 1));
        d1 = va - vc;
        d2 = vb - vc;
        // AssertOrThrow2( ((index-3)/3+0) < (int)ma->getFaceNormals()->count() );
        //    ma->getFaceNormals()->at(fn_index) = d1.cross(d2).normalize();

        //**Test: flat normals
        ma->n3f(ma->i32(index - 3)) =
          ma->n3f(ma->i32(index - 2)) =
          ma->n3f(ma->i32(index - 1)) = d1.cross(d2).normalize();

        // Triangle 2
        ma->i32(index++) = startvert + slices + 1;
        ma->i32(index++) = startvert + 1;
        ma->i32(index++) = startvert + slices + 2;

        fn_index = (index - 3) / 3;

        // - Face Normal 2
        va = ma->v3f(ma->i32(index - 3));
        vb = ma->v3f(ma->i32(index - 2));
        vc = ma->v3f(ma->i32(index - 1));
        d1 = va - vc;
        d2 = vb - vc;

        // AssertOrThrow2( ((index-3)/3) < (int)ma->getFaceNormals()->count() );
        //    
        //    ma->getFaceNormals()->at(fn_index) = d1.cross(d2).normalize();

            //**Test: flat normals
        ma->n3f(ma->i32(index - 3)) =
          ma->n3f(ma->i32(index - 2)) =
          ma->n3f(ma->i32(index - 1)) = d1.cross(d2).normalize();


        AssertOrThrow2(index - 1 < nIndexes);
        //assert( index-1 < nVerts );
      }
      else {
      }
    }//for
  }//for

  finalizeSpec(ma);

  return ma;
}
std::shared_ptr<MeshData> MeshUtils::makeBox(Box3f* pCube, vec4* color, Matrix4x4* applyMat, vec3* offset) {
  vec3 vi = pCube->_min;
  vec3 va = pCube->_max;

  vec3 f[8];
  f[BoxPoint::NBL] = vec3(vi.x, vi.y, vi.z);
  f[BoxPoint::NBR] = vec3(va.x, vi.y, vi.z);
  f[BoxPoint::NTL] = vec3(vi.x, va.y, vi.z);
  f[BoxPoint::NTR] = vec3(va.x, va.y, vi.z);
  f[BoxPoint::FBL] = vec3(vi.x, vi.y, va.z);
  f[BoxPoint::FBR] = vec3(va.x, vi.y, va.z);
  f[BoxPoint::FTL] = vec3(vi.x, va.y, va.z);
  f[BoxPoint::FTR] = vec3(va.x, va.y, va.z);

  return makeBox(f, color, applyMat, offset);
}
std::shared_ptr<MeshData> MeshUtils::makeBox(float length, vec4* color, mat4* applyMat, vec3* offset) {
  vec3 f[8];
  f[BoxPoint::FBL] = vec3(-1.0f, -1.0f, -1.0f);
  f[BoxPoint::FBR] = vec3(1.0f, -1.0f, -1.0f);
  f[BoxPoint::FTL] = vec3(-1.0f, 1.0f, -1.0f);
  f[BoxPoint::FTR] = vec3(1.0f, 1.0f, -1.0f);
  f[BoxPoint::NBL] = vec3(-1.0f, -1.0f, 1.0f);
  f[BoxPoint::NBR] = vec3(1.0f, -1.0f, 1.0f);
  f[BoxPoint::NTL] = vec3(-1.0f, 1.0f, 1.0f);
  f[BoxPoint::NTR] = vec3(1.0f, 1.0f, 1.0f);

  for (int i = 0; i < 8; ++i) {
    f[i] *= length;
  }

  return makeBox(f, color, applyMat, offset);
}
std::shared_ptr<MeshData> MeshUtils::makeBox(vec3(&extents)[8], Color4f* color, Matrix4x4* applyMat, vec3* offset) {
  std::shared_ptr<MeshData> ms = std::make_shared<MeshData>(StringUtil::generate(), MeshMakerVert::getVertexFormat());
  ms->allocMesh(6 * 4, 2 * 6 * 3);
  // - Instantiate everything
 //ms->allocateFaceNormals(6 * 2);
 //ms->allocateFragments();
 //ms->allocateIndexes();
  ms->beginEdit();
  {
    int frag = 0;
    int norm = 0;
    int ind = 0;

    //Mat
    if (applyMat) {
      Matrix4x4 m = *applyMat;
      Vector4 v4;
      for (int i = 0; i < 8; ++i) {
        v4 = Vector4(extents[i], 1);
        extents[i] = (m * v4);
      }
    }


    vec3 vn;

    //RIGHT
    ms->v3f(frag++) = extents[BoxPoint::FBR];
    ms->v3f(frag++) = extents[BoxPoint::NBR];
    ms->v3f(frag++) = extents[BoxPoint::FTR];
    ms->v3f(frag++) = extents[BoxPoint::NTR];
    vn = vec3(-1, 0, 0);
    for (size_t i = 1; i < 5; ++i) {
      ms->n3f(frag - i) = vn;
    }
    //ms->getFaceNormals()->at(norm++) = vn;
    //ms->getFaceNormals()->at(norm++) = vn;

    //LEFT
    ms->v3f(frag++) = extents[BoxPoint::NBL];
    ms->v3f(frag++) = extents[BoxPoint::FBL];
    ms->v3f(frag++) = extents[BoxPoint::NTL];
    ms->v3f(frag++) = extents[BoxPoint::FTL];
    vn = vec3(1, 0, 0);
    for (size_t i = 1; i < 5; ++i) {
      ms->n3f(frag - i) = vn;
    }
    //ms->getFaceNormals()->at(norm++) = vn;
    //ms->getFaceNormals()->at(norm++) = vn;

    //TOP
    ms->v3f(frag++) = extents[BoxPoint::NTR];
    ms->v3f(frag++) = extents[BoxPoint::NTL];
    ms->v3f(frag++) = extents[BoxPoint::FTR];
    ms->v3f(frag++) = extents[BoxPoint::FTL];
    vn = vec3(0, 1, 0);
    for (size_t i = 1; i < 5; ++i) {
      ms->n3f(frag - i) = vn;
    }
    //ms->getFaceNormals()->at(norm++) = vn;
    //ms->getFaceNormals()->at(norm++) = vn;
    //
    //BOTTOM
    ms->v3f(frag++) = extents[BoxPoint::FBR];
    ms->v3f(frag++) = extents[BoxPoint::FBL];
    ms->v3f(frag++) = extents[BoxPoint::NBR];
    ms->v3f(frag++) = extents[BoxPoint::NBL];
    vn = vec3(0, -1, 0);
    for (size_t i = 1; i < 5; ++i) {
      ms->n3f(frag - i) = vn;
    }
    //ms->getFaceNormals()->at(norm++) = vn;
    //ms->getFaceNormals()->at(norm++) = vn;

     //BACK
    ms->v3f(frag++) = extents[BoxPoint::NBR];
    ms->v3f(frag++) = extents[BoxPoint::NBL];
    ms->v3f(frag++) = extents[BoxPoint::NTR];
    ms->v3f(frag++) = extents[BoxPoint::NTL];
    vn = vec3(0, 0, -1);
    for (size_t i = 1; i < 5; ++i) {
      ms->n3f(frag - i) = vn;
    }
    //ms->getFaceNormals()->at(norm++) = vn;
    //ms->getFaceNormals()->at(norm++) = vn;

    //Front
    ms->v3f(frag++) = extents[BoxPoint::FBL];
    ms->v3f(frag++) = extents[BoxPoint::FBR];
    ms->v3f(frag++) = extents[BoxPoint::FTL];
    ms->v3f(frag++) = extents[BoxPoint::FTR];
    vn = vec3(0, 0, 1);
    for (size_t i = 1; i < 5; ++i) {
      ms->n3f(frag - i) = vn;
    }
    // ms->getFaceNormals()->at(norm++) = vn;
    // ms->getFaceNormals()->at(norm++) = vn;

    //INDEXES
    AssertOrThrow2(ms->getIndexes()->count() % 6 == 0);
    for (int fi = 0, vi = 0; fi < (int)ms->getIndexes()->count(); fi += 6, vi += 4) {
      ms->i32(fi + 0) = (v_index32)vi + 1;
      ms->i32(fi + 1) = (v_index32)vi + 0;
      ms->i32(fi + 2) = (v_index32)vi + 2;
      ms->i32(fi + 3) = (v_index32)vi + 1;
      ms->i32(fi + 4) = (v_index32)vi + 2;
      ms->i32(fi + 5) = (v_index32)vi + 3;
    }

    //VERTEX NORMALS AND COLORS
    int nnn = 0;
    for (unsigned int i = 0; i < ms->getFrags()->count(); i += 4) {
      for (int j = 0; j < 4; ++j) {
        //ms->n3f(i + j) = ms->getFaceNormals()->at(nnn);
        ms->c4f(i + j) = *color;
      }

      ms->x2f(i + 0).construct(0, 1);
      ms->x2f(i + 1).construct(1, 1);
      ms->x2f(i + 2).construct(1, 0);
      ms->x2f(i + 3).construct(0, 0);

      if (offset) {
        ms->v3f(i + 0) += *offset;
        ms->v3f(i + 1) += *offset;
        ms->v3f(i + 2) += *offset;
        ms->v3f(i + 3) += *offset;
      }

      nnn += 2;
    }
  }

  ms->endEdit();
  return ms;
}
/**
*    @fn generate_sphere
*    @brief Generate a sphere
*    VERTEX type is V_V3C4N3X2
*    CCW Winding
    vOffset = translates all points to the given offset
*    blnDoNormals = optimization routine - set to false to avoid normal square roots
*/
std::shared_ptr<MeshData> MeshUtils::makeSphere(float radius, int32_t slices, int32_t stacks, Color4f* color, vec3* vOffset, bool blnDoNormals) {
  AssertOrThrow2(slices >= 3 && stacks >= 3);
  AssertOrThrow2(radius > 0.0);
  std::shared_ptr<MeshData> ma = std::make_shared<MeshData>(StringUtil::generate(), MeshMakerVert::getVertexFormat());

  //Store Minimax for contact box
  vec3 vmin = Vec3f::VEC3X_MAX();
  vec3 vmax = Vec3f::VEC3X_MIN();
  int frag = 0;
  int cur_ind = 0;
  float theta;
  float phi;
  int topVert = 0;    // - first vertex in the pie
  int bottomVert;
  Color4f myc;
  size_t nFrags = (slices) * (stacks - 1) + 2;

  //Default color
  if (color == NULL)
    myc = Color4f(1, 1, 1, 1);
  else
    myc = *color;

  //Allocate
  // - Exactly the same as generating two circles, one with height.
  ma->allocMesh(nFrags, (((slices) * (stacks - 1)) * 3) * 2);

  bottomVert = (int)ma->fragCount() - 1;
  theta = 0;
  phi = 0;

  // - Generate top cap frag
  ma->v3f(frag).x = radius * sinf(phi) * cosf(theta);
  ma->v3f(frag).z = -radius * sinf(phi) * sinf(theta);//OpenGL...
  ma->v3f(frag).y = radius * cosf(phi);

  vmin = Vec3f::minv(ma->v3f(frag), vmin);
  vmax = Vec3f::maxv(ma->v3f(frag), vmax);

  ma->c4f(frag) = myc;
  if (blnDoNormals == true) {
    ma->n3f(frag) = ma->v3f(topVert);
    ma->n3f(frag).normalize();
  }

  //OpenGL TCOORDS
  ma->x2f(frag).x = 1.0f;
  ma->x2f(frag).y = 1.0f;

  frag++;

  for (int j = 0; j < stacks; ++j) {
    for (int i = 0; i < slices; ++i) {

      if (j == 0) {
        //top cap
        if (i < slices - 1) {
          ma->i32(cur_ind++) = topVert;
          ma->i32(cur_ind++) = frag;
          ma->i32(cur_ind++) = frag + 1;
        }
        else//seam vert
        {
          ma->i32(cur_ind++) = topVert;
          ma->i32(cur_ind++) = frag;
          ma->i32(cur_ind++) = topVert + 1;//the first vert of the stack ring
        }
      }
      else if (j == stacks - 1) {
        //bottom cap
        if (i < slices - 1) {
          ma->i32(cur_ind++) = bottomVert;    //Maintain CCW Winding
          ma->i32(cur_ind++) = bottomVert - slices + i + 1;
          ma->i32(cur_ind++) = bottomVert - slices + i;
        }
        else//seam vert
        {
          ma->i32(cur_ind++) = bottomVert;
          ma->i32(cur_ind++) = bottomVert - slices;
          ma->i32(cur_ind++) = bottomVert - slices + i;
        }
      }
      else {
        if (i < slices - 1) {
          //tri1
          ma->i32(cur_ind++) = frag - slices;
          ma->i32(cur_ind++) = frag;
          ma->i32(cur_ind++) = frag + 1;
          //tri2
          ma->i32(cur_ind++) = frag - slices;
          ma->i32(cur_ind++) = frag + 1;
          ma->i32(cur_ind++) = frag - slices + 1;
        }
        else//seam vert
        {
          //tri1
          ma->i32(cur_ind++) = frag - slices;
          ma->i32(cur_ind++) = frag;
          ma->i32(cur_ind++) = frag - slices + 1;
          //tri2
          ma->i32(cur_ind++) = frag - slices;
          ma->i32(cur_ind++) = frag - slices + 1;
          ma->i32(cur_ind++) = frag - slices - slices + 1;

        }
      }

      if (j < stacks - 1)//Dont make verts for the bottom stack (it is the bottom cap point we made before)
      {
        theta = (float)(((2.0f * M_PI) / (float)slices) * (float)i);
        phi = (float)(((M_PI) / (float)stacks) * (float)(j + 1));
        ma->v3f(frag).x = radius * sinf(phi) * cosf(theta);
        ma->v3f(frag).z = -radius * sinf(phi) * sinf(theta);//OpenGL...
        ma->v3f(frag).y = radius * cosf(phi);

        vmin = Vec3f::minv(ma->v3f(frag), vmin);
        vmax = Vec3f::maxv(ma->v3f(frag), vmax);

        if (blnDoNormals == true) {
          ma->n3f(frag) = ma->v3f(frag);
          ma->n3f(frag).normalize();
        }
        //OpenGL TCOORDS
        ma->x2f(frag).x = (float)i / ((float)slices - 1.0f);
        ma->x2f(frag).y = ((float)(stacks - j) / (float)stacks);//OpenGL (coord is bottom left of map)(
        ma->c4f(frag) = myc;

        frag++;
      }

    }
  }

  // Bottom cap frag
  ma->v3f(bottomVert).x = 0.0;
  ma->v3f(bottomVert).y = -radius;
  ma->v3f(bottomVert).z = 0.0;

  vmin = Vec3f::minv(ma->v3f(frag), vmin);
  vmax = Vec3f::maxv(ma->v3f(frag), vmax);

  if (blnDoNormals == true) {
    ma->n3f(bottomVert) = ma->v3f(bottomVert);
    ma->n3f(bottomVert).normalize();
  }
  ma->c4f(bottomVert) = myc;

  //OpenGL TCOORDS
  ma->x2f(frag).x = 0.0f;
  ma->x2f(frag).y = 0.0f;
  ma->c4f(frag) = myc;

  frag++;    // so the check works.

  AssertOrThrow2(cur_ind == ma->indexCount());
  AssertOrThrow2(frag == ma->fragCount());

  //If offset is set, set to frags
  if (vOffset != NULL) {
    for (int i = 0; i < (int)ma->fragCount(); ++i) {
      ma->v3f(i) += *vOffset;
    }
  }

  //20170516 - We construct the above in RHS coordinates, here we flip to LHS
  for (size_t iInd = 0; iInd < ma->indexCount(); iInd += 3) {
    v_index32 vi = ma->i32(iInd + 1);
    ma->i32(iInd + 1) = ma->i32(iInd + 2);
    ma->i32(iInd + 2) = vi;
  }


  finalizeSpec(ma, &vmin, &vmax);
  return ma;
}
/**
*    Generate a cone
*
*    CCW Winding
*/
std::shared_ptr<MeshData> MeshUtils::makeCone(float radius, float h, int32_t slices) {
  AssertOrThrow2(slices >= 3);
  AssertOrThrow2(radius > 0.0);
  std::shared_ptr<MeshData> ma = std::make_shared<MeshData>(StringUtil::generate(), MeshMakerVert::getVertexFormat());

  // - Exactly the same as generating two circles, one with height.


  ma->allocMesh((slices + 1) + 1, ((slices) * 3) * 2);

  int frag = 0, cur_ind = 0;

  // - Generate the CIRCLE bottom
  // - center
  ma->v3f(frag).x = 0.0;
  ma->v3f(frag).y = 0.0;
  ma->v3f(frag).z = 0.0;
  ma->x2f(frag).x = 0.5f;
  ma->x2f(frag).y = 0.5f;
  frag++;

  ma->v3f(frag).x = 0.0;
  ma->v3f(frag).y = h;
  ma->v3f(frag).z = 0.0;
  ma->x2f(frag).x = 0.5f;
  ma->x2f(frag).y = 0.5f;

  frag++;

  int center = 0;
  int top = 1;
  int first = 2;    // - first vertex in the pie

  for (int i = 0; i < slices; ++i) {
    ma->v3f(frag).x = radius * cosf((((2.0f * (float)M_PI) / (float)slices) * (float)i));
    ma->v3f(frag).y = 0.0;
    ma->v3f(frag).z = -radius * sinf((((2.0f * (float)M_PI) / (float)slices) * (float)i));//(negative)- for OpenGL

    ma->x2f(frag).x = 1.0f * ((float)i / (float)slices);
    ma->x2f(frag).y = 0.0f;

    //frags.at(index).c.r = (float)pMap->toGray(pix)/(float)255.0;
    //frags.at(index).c.g = (float)pMap->toGray(pix)/(float)255.0;
    //frags.at(index).c.b = (float)pMap->toGray(pix)/(float)255.0;

    if (i == slices - 1) {
      // - Close the circle
      // - Pie slice
      ma->i32(cur_ind++) = center;
      ma->i32(cur_ind++) = first;
      ma->i32(cur_ind++) = frag;

      ma->i32(cur_ind++) = top;
      ma->i32(cur_ind++) = frag;
      ma->i32(cur_ind++) = first;
    }
    else {
      // - Pie slice
      ma->i32(cur_ind++) = center;
      ma->i32(cur_ind++) = frag + 1;
      ma->i32(cur_ind++) = frag;
      // cone slice
      ma->i32(cur_ind++) = top;
      ma->i32(cur_ind++) = frag;
      ma->i32(cur_ind++) = frag + 1;
    }
    frag++;
  }

  finalizeSpec(ma);

  return ma;
}
/**
*    Generate a circle
*
*    CCW Winding
*/
std::shared_ptr<MeshData> MeshUtils::makeCircle(float radius, int32_t slices) {
  AssertOrThrow2(slices >= 3);
  AssertOrThrow2(radius > 0.0);

  std::shared_ptr<MeshData> ma = std::make_shared<MeshData>(StringUtil::generate(), MeshMakerVert::getVertexFormat());

  ma->allocMesh(slices + 1, (slices) * 3);

  int frag = 0, cur_ind = 0;
  // - center
  ma->v3f(frag).x = 0.0;
  ma->v3f(frag).y = 0.0;
  ma->v3f(frag).z = 0.0;
  frag++;

  for (int i = 0; i < slices; ++i) {
    ma->v3f(frag).x = radius * cosf((((2.0f * (float)M_PI) / slices) * i));
    ma->v3f(frag).y = 0.0;
    ma->v3f(frag).z = radius * sinf((((2.0f * (float)M_PI) / slices) * i));

    //frags.at(index).c.r = (float)pMap->toGray(pix)/(float)255.0;
    //frags.at(index).c.g = (float)pMap->toGray(pix)/(float)255.0;
    //frags.at(index).c.b = (float)pMap->toGray(pix)/(float)255.0;

    if (i == slices - 1) {
      // - Close the circle
      // - Pie slice
      ma->i32(cur_ind++) = 0;
      ma->i32(cur_ind++) = frag;
      ma->i32(cur_ind++) = 1;
    }
    else {
      // - Pie slice
      ma->i32(cur_ind++) = 0;
      ma->i32(cur_ind++) = frag;
      ma->i32(cur_ind++) = frag + 1;
    }
    frag++;
  }

  finalizeSpec(ma);
  return ma;
}
/**
*    @fn generate_billboard
*    @brief Creates 2 sided billboard along the XY axis in 3D without a z component
*        4 vertexes 12 indexes.
*
*    @param translateYToBottomOfBillboard) - translates the Y value to the bottom of the billboard for grass objects.
*/
std::shared_ptr<MeshData> MeshUtils::makeBillboardXY(float xscale, float yscale, bool translateYToBottomOfBillboard) {
  MeshMakerVert p[4];
  std::shared_ptr<MeshData> ma = std::make_shared<MeshData>(StringUtil::generate(), MeshMakerVert::getVertexFormat());
  ma->allocMesh(8, 12);

  int ip = 0;
  ma->v3f(0) = Vector3(+xscale, -yscale, 0);
  ma->v3f(1) = Vector3(-xscale, -yscale, 0);
  ma->v3f(2) = Vector3(-xscale, +yscale, 0);
  ma->v3f(3) = Vector3(+xscale, +yscale, 0);

  //Reverse Side
  ma->v3f(4) = Vector3(-xscale, -yscale, 0);
  ma->v3f(5) = Vector3(+xscale, -yscale, 0);
  ma->v3f(6) = Vector3(+xscale, +yscale, 0);
  ma->v3f(7) = Vector3(-xscale, +yscale, 0);

  ma->x2f(0) = Vec2f(0, 0);
  ma->x2f(1) = Vec2f(1, 0);
  ma->x2f(2) = Vec2f(1, 1);
  ma->x2f(3) = Vec2f(0, 1);

  ma->x2f(4) = Vec2f(1, 0);
  ma->x2f(5) = Vec2f(0, 0);
  ma->x2f(6) = Vec2f(0, 1);
  ma->x2f(7) = Vec2f(1, 1);

  ma->c4f(0) = Color4f(1, 1, 1, 1);
  ma->c4f(1) = Color4f(1, 1, 1, 1);
  ma->c4f(2) = Color4f(1, 1, 1, 1);
  ma->c4f(3) = Color4f(1, 1, 1, 1);

  ma->c4f(4) = Color4f(1, 1, 1, 1);
  ma->c4f(5) = Color4f(1, 1, 1, 1);
  ma->c4f(6) = Color4f(1, 1, 1, 1);
  ma->c4f(7) = Color4f(1, 1, 1, 1);

  ma->n3f(0) = Vec3f(0, 0, -1);
  ma->n3f(1) = Vec3f(0, 0, -1);
  ma->n3f(2) = Vec3f(0, 0, -1);
  ma->n3f(3) = Vec3f(0, 0, -1);

  ma->n3f(4) = Vec3f(0, 0, 1);
  ma->n3f(5) = Vec3f(0, 0, 1);
  ma->n3f(6) = Vec3f(0, 0, 1);
  ma->n3f(7) = Vec3f(0, 0, 1);

  // Translate if necessary
  if (translateYToBottomOfBillboard) {
    for (int i = 0; i < 8; ++i) {
      ma->v3f(i).y += yscale;
    }
  }

  /*
      0,1,2, 0,2,3
      0,2,1, 0,3,2
  */
  int32_t fi = 0;
  ma->i32(fi++) = 0;
  ma->i32(fi++) = 1;
  ma->i32(fi++) = 2;
  ma->i32(fi++) = 0;
  ma->i32(fi++) = 2;
  ma->i32(fi++) = 3;

  ma->i32(fi++) = 4;
  ma->i32(fi++) = 5;
  ma->i32(fi++) = 6;
  ma->i32(fi++) = 4;
  ma->i32(fi++) = 6;
  ma->i32(fi++) = 7;

  finalizeSpec(ma);

  return ma;
}
/**
*    @fn
*    @brief Generates a X cross billboard.
*/
std::shared_ptr<MeshData> MeshUtils::makeCrossboardXY(float xscale, float yscale, bool translateYToBottomOfBillboard) {
  std::shared_ptr<MeshData> a = makeBillboardXY(xscale, yscale, translateYToBottomOfBillboard);
  std::shared_ptr<MeshData> b = makeBillboardXY(xscale, yscale, translateYToBottomOfBillboard);

  mat4 m = mat4::getRotationDeg(90, 0, 1, 0);

  for (int i = 0; i < 8; ++i) {
    b->v3f(i) = m * vec4(b->v3f(i), 1);
    b->n3f(i) = m * vec4(b->n3f(i), 1);
    b->n3f(i).normalize();
  }

  a->mergeWith(b);

  //delete b; //HEAP:
  b = nullptr;

  finalizeSpec(a);
  return a;
}

void MeshUtils::finalizeSpec(std::shared_ptr<MeshData> ms, vec3* minV, vec3* maxV) {
  //TODO: remove the contact box crap (opr is  it needed for OBBs? )
  //_pContactManager->generateDefaultBoundaryContactBoxForMesh(ms, minV, maxV);
  //ms->computeMinimax();
}
std::shared_ptr<MeshNode> MeshUtils::createScreenQuadMesh(int w, int h) {
  std::shared_ptr<MeshNode> quadMesh = nullptr;

  std::vector<v_v3x2> verts;
  std::vector<v_index32> inds;

  float fw = (float)w;
  float fh = (float)h;

  verts.push_back(v_v3x2());
  verts.push_back(v_v3x2());
  verts.push_back(v_v3x2());
  verts.push_back(v_v3x2());

  verts[0].v = vec3(0, 0, 0);
  verts[1].v = vec3(fw, 0, 0);
  verts[2].v = vec3(0, fh, 0);
  verts[3].v = vec3(fw, fh, 0);

  verts[0].x = vec2(0, 1);
  verts[1].x = vec2(1, 1);
  verts[2].x = vec2(0, 0);
  verts[3].x = vec2(1, 0); //**Notice: here opengl texture untis don't match the vertex units becuase textures are upside down.

  inds.push_back(0);
  inds.push_back(1);
  inds.push_back(3);

  inds.push_back(0);
  inds.push_back(3);
  inds.push_back(2);

  quadMesh = MeshNode::create(std::make_shared<MeshData>(verts.data(),
    verts.size(), inds.data(), inds.size(), v_v3x2::getVertexFormat(), nullptr
    )
  );


  return quadMesh;
}
vec3* MeshUtils::getVertexElementOffset(vec3* verts, size_t iElementIndex, size_t vOffBytes, size_t vStrideBytes, size_t vCount) {
  //Similar to opengl's way of claculating components this gets you a vertex at a element index
  AssertOrThrow2(iElementIndex < vCount);

  vec3* ret = (vec3*)((char*)verts + vStrideBytes * iElementIndex + vOffBytes);

  return ret;
}







}//ns game
