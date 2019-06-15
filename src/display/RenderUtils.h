/**
*
*    @file RenderUtils.h
*    @date May 16, 2012
*    @author Derek Page
*
*    © 2012 
*
*
*/
#pragma once
#ifndef __RENDERUTILS_215792320710911295042735_H__
#define __RENDERUTILS_215792320710911295042735_H__


#include "../display/DisplayHeader.h"
#include "../math/MathAll.h"

namespace Game {
/**
*    @class RenderUtils
*    @brief    This class supplies some basic utility functions to render default
*    stuff to the screen.
*
*/
class RenderUtils : public VirtualMemory {
    STATIC void debugGetVertexArrayState(t_string& strOut);//breakpoint it
    STATIC void debugGetFramebufferAttachmentState(t_string& strOut);
    STATIC void debugGetTextureState(t_string& strOut);
    STATIC void debugGetAttribState(t_string& strOut);
    STATIC void debugGetBufferState(t_string& strOut);
    STATIC void debugGetLegacyViewAndMatrixStack(t_string& strOut);
    STATIC void debugPrintActiveUniforms(int iGlProgramId, t_string& strOut);
    STATIC void debugPrintFBOAttachment(t_string& strState, GLenum );
public:
    STATIC void setLineWidth(float width);

    //Conversion
    STATIC t_string openGlTypeToString(GLenum type);//TODO: move this to the GL hardware or Gd::..


    /// Rendering State
    STATIC void resetRenderState();
    STATIC t_string debugGetRenderState(bool bForceRun = false, bool bPrintToStdout = true, bool bSaveFramebufferTexture = true);//breakpoint it


    // - Utility
    STATIC void renderTexturedQuadAttrib(float size);//UNSAFE
    //STATIC void drawWireSphereShader(float fRadius, vec3& vOffset, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5);
    STATIC void drawWireSphereShader(float fRadius, vec4& vColor, int32_t nSlices=5, int32_t nStacks=5, mat4* pMatrix = nullptr);
    STATIC void drawWireBoxShader(Box3f* box, vec3& vOffset, vec4& vColor);
    STATIC void drawSolidBoxShaded(Box3f* box, vec3& vOffset, vec4& vColor);
    //STATIC void drawPickBox(Box3f* box, t_uint32 uiColorId);
    STATIC void drawAxisShader(float scale = 10.0f, float lineWidth = 2.0f, mat4& transform= mat4::identity()); // Renders an Axis at the origin.
    STATIC void drawFrustumShader(std::shared_ptr<FrustumBase> pf, vec4& avColor);//idk why there are 2
    STATIC void drawGridShader(float r = 1.0f, float g = 1.0f, float b = 1.0f, int32_t nSlices = 60,
                                float fSliceWidth = 20.0f,  vec3& center = vec3(0, 0, 0),std::shared_ptr<ShaderBase> pShader = NULL ); // Renders a grid.
    


    STATIC void saveTexture(t_string&& strLoc, GLuint iGLTexId, GLenum eTexTarget, int iCubeMapSide = -1); // Saves a GL texture by ID to the file path.
    STATIC void saveFramebufferAsPng(t_string&& strLoc, GLuint iFBOId = 0); // Saves a GL texture by ID to the file path.
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
