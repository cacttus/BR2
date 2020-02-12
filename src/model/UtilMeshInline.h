/**
*
*    @file UtilMeshInline.h
*    @date April 29, 2016
*    @author MetalMario971
*
*    © 2016
*
*
*/
#pragma once
#ifndef __UTILMESHINLINE_14619781622646582835_H__
#define __UTILMESHINLINE_14619781622646582835_H__

#include "../model/UtilMesh.h"

namespace BR2 {
/**
*    @class UtilMeshInline
*    @brief A substitute for inline OpenGL draw calls.
*/
class UtilMeshInline : public UtilMesh {
public:
  UtilMeshInline(std::shared_ptr<GLContext> ctx);
  virtual ~UtilMeshInline() override;

  void setDefaultColor(vec4& v) { _vDefaultColor = v; }

  OVERRIDES void generate() OVERRIDE;
  OVERRIDES void preDraw()  OVERRIDE {}
  OVERRIDES void postDraw() OVERRIDE {}

  void begin(GLenum type);
  void vt1(v_v3c4& v1);
  void vt2(v_v3c4& v1, v_v3c4& v2);
  void vt2(vec3& v1, vec3& v2, vec4* color = nullptr);
  void end();//End vertex processing and draw the primitives


  void addBox(Box3f* b, vec4* color = nullptr);
  void addBox(vec3* points, vec4* color = nullptr);
private:
  //GrowBuffer<v_v3c4>* _pGrowBuffer;
  std::vector<v_v3c4> _pBuf;
  v_v3c4 _curVert;
  vec4 _vDefaultColor;
  void pushCurVert();
};
class UtilMeshInline2d : public UtilMesh {
public:
  UtilMeshInline2d(std::shared_ptr<GLContext> ctx);
  virtual ~UtilMeshInline2d() override;

  void setDefaultColor(vec4& v) { _vDefaultColor = v; }

  OVERRIDES void generate() OVERRIDE;
  OVERRIDES void preDraw()  OVERRIDE {}
  OVERRIDES void postDraw() OVERRIDE {}

  void begin(GLenum type);
  void vt1(v_v2c4& v1);
  void vt2(vec2& v1, vec2& v2, vec4* color = nullptr);
  void end();//End vertex processing and draw the primitives
private:
  //GrowBuffer<v_v2c4>* _pGrowBuffer;
  std::vector<v_v2c4> _pBuf;
  v_v2c4 _curVert;
  vec4 _vDefaultColor;
  void pushCurVert();


};
}//ns Game



#endif
