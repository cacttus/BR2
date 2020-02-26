/**
*  @file RenderUtils.h
*  @date May 16, 2012
*  @author MetalMario971
*/
#pragma once
#ifndef __RENDERUTILS_215792320710911295042735_H__
#define __RENDERUTILS_215792320710911295042735_H__

#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"

namespace BR2 {
/**
*  @class RenderUtils
*  @brief Provides utility functions for creating basic shapes, and rendering to the screen.
*/
class RenderUtils : public VirtualMemory {
public:
  static void setLineWidth(float width);
  //Conversion
  static string_t openGlTypeToString(GLenum type);//TODO: move this to the GL hardware or Gd::..

  /// Rendering State
  static void resetRenderState();
  static string_t debugGetRenderState(bool bForceRun = false, bool bPrintToStdout = true, bool bSaveFramebufferTexture = true);//breakpoint it

  // - Utility
  static void renderTexturedQuadAttrib(std::shared_ptr<CameraNode> cam, float size);//UNSAFE
  //static void drawWireSphereShader(float fRadius, vec3& vOffset, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5);
  static void drawWireSphereShader(std::shared_ptr<CameraNode> cam, float fRadius, vec4& vColor, int32_t nSlices = 5, int32_t nStacks = 5, mat4* pMatrix = nullptr);
  static void drawWireBoxShader(std::shared_ptr<CameraNode> cam, Box3f* box, vec3& vOffset, vec4& vColor);
  static void drawSolidBoxShaded(std::shared_ptr<CameraNode> cam, Box3f* box, vec3& vOffset, vec4& vColor);
  //static void drawPickBox(Box3f* box, uint32_t uiColorId);
  static void drawAxisShader(std::shared_ptr<CameraNode> cam, float scale = 10.0f, float lineWidth = 2.0f, mat4& transform = mat4::identity()); // Renders an Axis at the origin.
  static void drawFrustumShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<FrustumBase> pf, vec4& avColor);//idk why there are 2
  static void drawGridShader(std::shared_ptr<CameraNode> cam, float r = 1.0f, float g = 1.0f, float b = 1.0f, int32_t nSlices = 60,
    float fSliceWidth = 20.0f, vec3& center = vec3(0, 0, 0), std::shared_ptr<ShaderBase> pShader = NULL); // Renders a grid.

  static void saveTexture(string_t&& strLoc, GLuint iGLTexId, GLenum eTexTarget, int iCubeMapSide = -1); // Saves a GL texture by ID to the file path.
  static void saveFramebufferAsPng(string_t&& strLoc, GLuint iFBOId = 0); // Saves a GL texture by ID to the file path.
  static void createDepthTexture(GLuint* __out_ texId, int32_t w, int32_t h, bool bMsaaEnabled, int32_t nMsaaSamples, GLenum eRequestedDepth);
  //static void createDepthTexture(GLuint& __out_ texId, int w, int h, GLenum depthSize);
  static void getCompatibleDepthComponent(GLenum eRequestedDepth, std::function<void(GLenum)> func);

  static GLenum getSupportedDepthSize();
  static bool getTextureBindingForTextureId(GLint testTex, GLenum& __out_ target, GLenum& __out_ binding);
  static bool getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image, GLuint iGLTexId, GLenum eTarget, int iCubeMapSide = -1);
  static GLenum getTexBindingForTexTarget(GLenum eTarget);

  static void makeVertexFormats();

private:
  static void debugGetVertexArrayState(string_t& strOut);//breakpoint it
  static void debugGetFramebufferAttachmentState(string_t& strOut);
  static void debugGetTextureState(string_t& strOut);
  static void debugGetAttribState(string_t& strOut);
  static void debugGetBufferState(string_t& strOut);
  static void debugGetLegacyViewAndMatrixStack(string_t& strOut);
  static void debugPrintActiveUniforms(int iGlProgramId, string_t& strOut);
  static void debugPrintFBOAttachment(string_t& strState, GLenum);


};

}//ns game



#endif
