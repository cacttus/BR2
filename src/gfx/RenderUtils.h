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
*  @brief This class provides helper graphics utility functions.
*/
class RenderUtils : public VirtualMemory {
public:
  static void setLineWidth(std::shared_ptr<GLContext> context, float width);

  //Conversion
  static string_t openGlTypeToString(GLenum type);//TODO: move this to the GL hardware or Gd::..

  /// Rendering State
  static void resetRenderState(std::shared_ptr<GLContext> context);
  static string_t debugGetRenderState(std::shared_ptr<GLContext> context, bool bForceRun = false, bool bPrintToStdout = true, bool bSaveFramebufferTexture = true);//breakpoint it

  // - Utility
  static void renderTexturedQuadAttrib(std::shared_ptr<GLContext> context, float size);//UNSAFE
  //static void drawWireSphereShader(float fRadius, vec3& vOffset, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5);
  static void drawWireSphereShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> context, float fRadius, vec4& vColor, int32_t nSlices = 5, int32_t nStacks = 5, mat4* pMatrix = nullptr);
  static void drawWireBoxShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> context, Box3f* box, vec3& vOffset, vec4& vColor);
  static void drawSolidBoxShaded(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> context, Box3f* box, vec3& vOffset, vec4& vColor);
  //static void drawPickBox(Box3f* box, uint32_t uiColorId);
  static void drawAxisShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> context, float scale = 10.0f, float lineWidth = 2.0f, mat4& transform = mat4::identity()); // Renders an Axis at the origin.
  static void drawFrustumShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> context, std::shared_ptr<FrustumBase> pf, vec4& avColor);//idk why there are 2
  static void drawGridShader(std::shared_ptr<CameraNode> cam, std::shared_ptr<GLContext> context, float r = 1.0f, float g = 1.0f, float b = 1.0f, int32_t nSlices = 60,
    float fSliceWidth = 20.0f, vec3& center = vec3(0, 0, 0), std::shared_ptr<ShaderBase> pShader = NULL); // Renders a grid.

  static void saveTexture(std::shared_ptr<GLContext> context, string_t&& strLoc, GLuint iGLTexId, GLenum eTexTarget, int iCubeMapSide = -1); // Saves a GL texture by ID to the file path.
  static void saveFramebufferAsPng(std::shared_ptr<GLContext> context, string_t&& strLoc, GLuint iFBOId = 0); // Saves a GL texture by ID to the file path.
  static void createDepthTexture(std::shared_ptr<GLContext> context, GLuint* __out_ texId, int32_t w, int32_t h, bool bMsaaEnabled, int32_t nMsaaSamples, GLenum eRequestedDepth);
  //static void createDepthTexture(GLuint& __out_ texId, int w, int h, GLenum depthSize);
  static void getCompatibleDepthComponent(std::shared_ptr<GLContext> context, GLenum eRequestedDepth, std::function<void(GLenum)> func);

  static GLenum getSupportedDepthSize(std::shared_ptr<GLContext> context);
  static bool getTextureBindingForTextureId(GLint testTex, GLenum& __out_ target, GLenum& __out_ binding);
  static bool getTextureDataFromGpu(std::shared_ptr<GLContext> context, std::shared_ptr<Img32> __out_ image, GLuint iGLTexId, GLenum eTarget, int iCubeMapSide = -1);
  static GLenum getTexBindingForTexTarget(GLenum eTarget);

private:
  static void debugGetVertexArrayState(std::shared_ptr<GLContext> context, string_t& strOut);//breakpoint it
  static void debugGetFramebufferAttachmentState(std::shared_ptr<GLContext> context, string_t& strOut);
  static void debugGetTextureState(std::shared_ptr<GLContext> context, string_t& strOut);
  static void debugGetAttribState(std::shared_ptr<GLContext> context, string_t& strOut);
  static void debugGetBufferState(std::shared_ptr<GLContext> context, string_t& strOut);
  static void debugGetLegacyViewAndMatrixStack(std::shared_ptr<GLContext> context, string_t& strOut);
  static void debugPrintActiveUniforms(std::shared_ptr<GLContext> context, int iGlProgramId, string_t& strOut);
  static void debugPrintFBOAttachment(std::shared_ptr<GLContext> context, string_t& strState, GLenum);


};

}//ns BR2



#endif
