/**
*  @file VertexTypes.h
*  @date May 9, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __VERTEXTYPES_14943771063900412386_H__
#define __VERTEXTYPES_14943771063900412386_H__

#include "../model/ModelHeader.h"
#include "../math/MathAll.h"

namespace BR2 {
namespace VertexUserType { typedef enum { v2_01, v3_01, v4_01, v4_02, v4_03, c4_01, c3_01, n3_01, x2_01, i2_01, u2_01, None } e; }
typedef uint32_t v_index32;

class v_v2c4x2 {
public:
  vec2 v;
  vec4 c;
  vec2 x;
  float _xpad0;
  float _xpad1;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }

private:
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
};


//*Vertex formats are all initialized in the GLContex.h class.
class v_v3c4x2 {
public:
  vec3 v;
  float v2;
  vec4 c;
  vec2 x;
  float _xpad0;
  float _xpad1;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }

private:
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
};

class v_v3n3x2 {
public:
  vec3 v;
  float v_pad;
  //  float f0;
  vec3 n;
  float n_pad;
  //  float f1;
  vec2 x;
  float _xpad0;
  float _xpad1;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }

private:
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
};
class v_v3x2 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec3 v;
  float v_pad;
  vec2 x;
  float _xpad0;
  float _xpad1;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v3n3 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec3 v;
  float v_pad;
  vec3 n;
  float _xpad0;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v3 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec3 v;
  float v_pad;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v2x2 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec2 v;
  vec2 x;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v2c4 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec2 v;
  //float _pad0;//std430 type.
  //float _pad1;
  vec4 c;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
//class v_v3c3i4i4 {
//    friend class GLContext;
//    static std::shared_ptr<VertexFormat> _pVertexFormat;
//public:
//    vec3 v;
//    vec3 c;
//    vec2 uv;
//    ivec4 viTile;
//    static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
//};
class v_v3c3x2n3 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec3 v;
  float v_pad;
  vec3 c;
  float c_pad;
  vec2 x;
  float _xpad0;
  float _xpad1;
  vec3 n;
  float n_pad;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v3i2n3 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec3 v;
  float v_pad;
  ivec2 ix;
  vec3 n;
  float n_pad;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v3c4 {
public:
  v_v3c4() {}
  v_v3c4(const vec3& v3, const vec4& c4) { v = v3; c = c4; }

  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;

  vec3 v;
  float v_pad;
  vec4 c;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};
class v_v3c4x2n3 {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec3 v;
  float v_pad;
  vec4 c;
  vec2 x;
  float _xpad0;
  float _xpad1;
  vec3 n;
  float n_pad;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }

};

class v_GuiVert {
  friend class RenderUtils;
  static std::shared_ptr<VertexFormat> _pVertexFormat;
public:
  vec4 _rect;
  vec4 _clip;
  vec4 _tex;
  vec2 _texsiz;
  uvec2 _pick_color;
  static std::shared_ptr<VertexFormat> getVertexFormat() { return _pVertexFormat; }
};



}//ns Game



#endif
