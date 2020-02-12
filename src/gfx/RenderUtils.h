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
*    @class RenderUtils
*    @brief    This class supplies some basic utility functions to render default
*    stuff to the screen.
*
*/
class RenderUtils : public VirtualMemory {
    STATIC void debugGetVertexArrayState(string_t& strOut);//breakpoint it
    STATIC void debugGetFramebufferAttachmentState(string_t& strOut);
    STATIC void debugGetTextureState(string_t& strOut);
    STATIC void debugGetAttribState(string_t& strOut);
    STATIC void debugGetBufferState(string_t& strOut);
    STATIC void debugGetLegacyViewAndMatrixStack(string_t& strOut);
    STATIC void debugPrintActiveUniforms(int iGlProgramId, string_t& strOut);
    STATIC void debugPrintFBOAttachment(string_t& strState, GLenum );
public:
    STATIC void setLineWidth(float width);

    //Conversion
    STATIC string_t openGlTypeToString(GLenum type);//TODO: move this to the GL hardware or Gd::..


    /// Rendering State
    STATIC void resetRenderState();
    STATIC string_t debugGetRenderState(bool bForceRun = false, bool bPrintToStdout = true, bool bSaveFramebufferTexture = true);//breakpoint it


    // - Utility
    STATIC void renderTexturedQuadAttrib(float size);//UNSAFE
    //STATIC void drawWireSphereShader(float fRadius, vec3& vOffset, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5);
    STATIC void drawWireSphereShader(float fRadius, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5, mat4* pMatrix = nullptr);
    STATIC void drawWireBoxShader(Box3f* box, vec3& vOffset, vec4& vColor);
    STATIC void drawSolidBoxShaded(Box3f* box, vec3& vOffset, vec4& vColor);
    //STATIC void drawPickBox(Box3f* box, uint32_t uiColorId);
    STATIC void drawAxisShader(float scale = 10.0f, float lineWidth = 2.0f, mat4& transform= mat4::identity()); // Renders an Axis at the origin.
    STATIC void drawFrustumShader(std::shared_ptr<FrustumBase> pf, vec4& avColor);//idk why there are 2
    STATIC void drawGridShader(float r = 1.0f, float g = 1.0f, float b = 1.0f, int32_t nSlices = 60,
                                float fSliceWidth = 20.0f,  vec3& center = vec3(0, 0, 0),std::shared_ptr<ShaderBase> pShader = NULL ); // Renders a grid.
    


    STATIC void saveTexture(string_t&& strLoc, GLuint iGLTexId, GLenum eTexTarget, int iCubeMapSide = -1); // Saves a GL texture by ID to the file path.
    STATIC void saveFramebufferAsPng(string_t&& strLoc, GLuint iFBOId = 0); // Saves a GL texture by ID to the file path.
    STATIC void createDepthTexture(GLuint* __out_ texId, int32_t w, int32_t h, bool bMsaaEnabled, int32_t nMsaaSamples, GLenum eRequestedDepth);
    //STATIC void createDepthTexture(GLuint& __out_ texId, int w, int h, GLenum depthSize);
    STATIC void getCompatibleDepthComponent(GLenum eRequestedDepth, std::function<void (GLenum)> func);

    static GLenum getSupportedDepthSize();
    STATIC bool getTextureBindingForTextureId(GLint testTex, GLenum& __out_ target, GLenum& __out_ binding);
    STATIC bool getTextureDataFromGpu(std::shared_ptr<Img32> __out_ image, GLuint iGLTexId, GLenum eTarget, int iCubeMapSide = -1);
    STATIC GLenum getTexBindingForTexTarget(GLenum eTarget);

};

}//ns game



#endif
