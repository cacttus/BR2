/**
*  @file DisplayHeader.h
*  @date 7/18/2010
*  @author MetalMario971
*/
#pragma once
#ifndef __RENDERHEADER_H__
#define __RENDERHEADER_H__


#include "../base/BaseHeader.h"
#include "../math/Vec3x.h"
#include "../math/Vec4x.h"
#include "../math/Matrix4x4.h"

namespace BR2 {
// - Channel 1 is depth texture, 0 is base textures
#define GFX_TEXTURE_CHANNEL_BASE TEXTURE_CHANNEL_0
#define GFX_TEXTURE_CHANNEL_NORMAL TEXTURE_CHANNEL_1
#define GFX_TEXTURE_CHANNEL_DEPTH TEXTURE_CHANNEL_2

typedef enum {
    GFX_DISPLAY_MODE_FULLSCREEN,
    GFX_DISPLAY_WINDOW
}GfxDisplayMode;

namespace TextureChannel {
typedef enum {
    Channel0,// don't use anymore.  Use an integer instead
    Channel1,//
    Channel2,//
    Channel3,//
    Channel4,//
    Channel5,//
    Channel6,//
}e; }

typedef enum { 
    Deferred
    ,Shadow
    ,Pick
} RenderMode;


namespace ShaderClass { typedef enum { Shadow, Diffuse } e; }

namespace ProjectionMode { typedef enum {Perspective, Orthographic}e; }

#define GFX_OPENGL_DEFAULT_MIN_VERSION (4)
#define GFX_OPENGL_DEFAULT_MIN_SUB_VERSION (3)
#define GFX_FB_TARGET_SCREEN (NULL)

//These must match the FBO Master target and alos the individual map FBO targets
//      GL_R32F, GL_RED,GL_FLOAT
//      GL_RGBA16F, GL_RGBA, GL_FLOAT
#define SHADOW_CUBE_MAP_TEX_INTERNAL_FORMAT GL_RGBA16F
#define SHADOW_CUBE_MAP_TEX_FORMAT GL_RGBA
#define SHADOW_CUBE_MAP_TEX_TYPE GL_FLOAT

// - Maximum distnace the frustum can project to.
// The frustum must be guaranteed to never project beyond this number
// this is the "max" number used for culling objects out.
#define GFX_FRUSTUM_MAX_DISTANCE (1000000)

namespace TexWrap { typedef enum { Clamp, Repeat } e; }
namespace TexFilter { typedef enum { Linear, Nearest } e; }
namespace PipeBit { typedef enum { 
    Deferred, 
    Shadow, 
    Forward, 
    Debug,
    NonDepth,
    Transparent,
    BlitFinal,
    Full,
    MaxPipes } e; }

// - tells us how to orient the camera to destination positions based
// on the state of the  chraacter.
typedef enum {
    CCS_MOVING,
    CCS_IDLE
}CameraCharacterState;
typedef enum {
    VPA_LOOKZ_XLEFT,
    VPA_LOOKZ_XRIGHT
}ViewportAxis;    // - looking down positive z axis, with Y up,  x axis is to the left or right
//typedef enum {
//    VVW_CCW = GL_CCW,
//    VVW_CW = GL_CW
//}ViewportVertexWinding;

typedef enum {
    fpt_nbl, fpt_fbl, fpt_fbr, fpt_nbr, fpt_ntl, fpt_ftl, fpt_ftr, fpt_ntr
}FrustumPoint;

typedef enum {
    fp_near, fp_far, fp_left, fp_right, fp_top, fp_bottom
    /*fp_near, fp_far, fp_right, fp_left, fp_bottom, fp_top*/
}frustum_plane_index;


namespace ShaderStatus {
typedef enum {
    Uninitialized,
    Loaded,            // step1
    Compiled,        // step2
    Linked,
    CreateComplete,    //setp3

    CreateFailed,
    Unloaded,
    FileNotFound,
    CompileError,
    LoadError,
    Error,
} e;
}

namespace ShaderType {
typedef enum {
    st_use_extension,
    st_fragment,
    st_vertex,
    st_geometry,
    st_compute    //new 9/2014
} e; 
}

namespace OpenGLShaderVarType {
typedef enum {
      ut_notype 
    , GpuInt1  
    , GpuInt2
    , GpuInt3 
    , GpuInt4
    , GpuUint1 
    , GpuUint2 
    , GpuUint3 
    , GpuUint4
    , GpuFloat1 
    , GpuFloat2 
    , GpuFloat3
    , GpuFloat4 
    , GpuBool1 
    , GpuBool2 
    , GpuBool3 
    , GpuBool4 
    , GpuMat2 
    , GpuMat3 
    , GpuMat4 
    , GpuDouble1 
    , GpuDouble2 
    , GpuDouble3 
    , GpuDouble4
    , GpuUniformBuffer//This is sort of a 1 off, and doesn't really apply
    //////////////////
    , ut_var_count //
} e; }

//***Font stuff
//***Font stuff
//***Font stuff
//***Font stuff

 //32 - ASCII 0-31 are unprintable characters
 #define GFX_ASCII_FONT_RANGE_START (0)
 //0x20)
 //256
 //int32_t _charsetMax = 0x7E;    //126
 #define GFX_ASCII_FONT_RANGE_END (0xFF)

 typedef enum {
 GFX_ASCII_FONT_QUALITY_SMOOTH,
 GFX_ASCII_FONT_QUALITY_EXACT,
 }GfxAsciiFontQuality;

 typedef float FontSize; //in pixels

 typedef enum {
     ATLAS_BUILD_POS_TOP_LEFT// start at the top left
 }TextureAtlasBuildPosition;

 typedef enum {
     ATLAS_BUILD_DIR_RIGHT    //build to the right
 }TextureAtlasBuildDirection;

 namespace RenderTargetType { typedef enum { Color, Depth, Alpha, Bloom, Pick, Shadow } e; }
 namespace FramebufferState { typedef enum {Not_Initialized, Initialized } e; }
 //What channel teh texture effects
 namespace TextureType { typedef enum { Undefined, Color, Normal } e; }
 namespace UiImageSizeMode { typedef enum { Expand, Tile, Computed, Proportion } e; }
 namespace UiPositionMode { typedef enum { Relative, Static /*Fixed = relative to viewport.  We don't need this (yet)*/ } e; }//Static: auto adjust position, Relative - fixes position (set left, top)
 namespace UiDimUnit { typedef enum { Percent, Pixel, Auto/*, Initial*/ } e; }
 namespace UiOverflowMode { typedef enum { Show, Hide } e; }
 namespace UiDisplayMode { typedef enum { InlineWrap, Block, InlineNoWrap } e; }//Wrap: elements on same line and they wrap at the end of container, block - always wrap, inlinenowrap - inline and expand past container
 namespace UiAutoSizeMode { typedef enum { Fixed, Grow, Shrink, GrowAndShrink } e; }

 //FWDDCL
 class ShaderSubProgram;
 class CameraNode;
 class WindowViewport;
 class FrustumBase;
 class ShaderBase;
 class FboShader;
 class FramebufferForward;
 class ScreenQuadMesh;
 class Party;
 class Atlas;
 class AtlasSprite;
 class FrustumProjectionParameters;
 class PointLight;
 class Party;
 class Particle;
 class DeferredFramebuffer;
 class FontTextBuffer;
 class FontSpec;
 class IScreenText;
// class TextBoss;
 class GpuQuad3;
 class QuadBufferMesh;
 class FontSpec;
 class AtlasCellFrameSpec;
 class SpriteSpec;
 class SpriteInstance;
 class SpriteAnimationSpec;
 class SpriteAnimationInstance;
 class SpriteFile;
 class SpriteManager;
 class SpriteMap;
 class Texture2DSpec;
 class TextureAtlas2d;
 class GLContext;
 class HappySky;
 class UiElement;
 class Gui2d;
 class SpriteBucket;
 class SpriteSpec;
 class SpriteFrame;
 class Sprite;
 class ObjectButton;
 class VboData;
 class IboData;
 class ShaderMaker;
 class ShaderCompiler;
 class ShaderCache;
 class ShaderUniform;
 class ShaderAttribute;
 class RenderPipe;
 class LightNodePoint;
 class LightNodeBase;
 class LightManager;
 class FramebufferBase;
 class VaoShader;
 class ShaderUniformBlock;
 class GpuPointLight;
 class RenderParams;
 class ForwardFramebuffer;
 class FlyCam;
 class GpuComputeSync;
 class RenderTarget;
 class Picker;
 class ShadowFrustum;
 class ShadowBox;//1/22/18
 class ShadowBoxSide;
 class LightManager;
 class LightNodePoint;
 class LightNodeDir;
 class GraphicsApi;
 class OpenGLApi;
 class VulkanApi;
 class RenderSettings;
 class UiTex;
 class Ui3Tex;
 class Ui9Tex;
 class UiElement;
 class uDim;
 class UiSkin;
 class UiPixelUnit;
 class UiDesignSize;
 class UiDragInfo;
 class UiParser;
 class UiElement;
 class UiImage;
 class UiLabel;
 class UiButtonBase;
 class UiImageButton;
 class UiGridRow;
 class UiGrid;
 class Ui9Grid;
 class UiFlexButton;
 class UiVScrollbar;
 class UiVScrollbarThumb;
 class UiWindow;
 class UiCursor;
 class Gui2d;
 class MegaTex;
 class MtNode;
 class MtTex;
 class MtFont;
 class ShaderCache;
 class GraphicsWindow;
 class GraphicsContext;
 class VulkanContext;
 class GLContext;
 class GLProgramBinary;
 class VaoDataGeneric;

 //Classes to remove
 class MeshNode;

 namespace UiEventId {
 typedef enum {
     Mouse_Lmb_Up
     , Mouse_Lmb_Press
     , Mouse_Lmb_Down
     , Mouse_Lmb_Release
     , Mouse_Lmb_None
     , Mouse_Rmb_Up
     , Mouse_Rmb_Press
     , Mouse_Rmb_Down
     , Mouse_Rmb_Release
     , Mouse_Rmb_None
     , Mouse_Mmb_Up
     , Mouse_Mmb_Press
     , Mouse_Mmb_Down
     , Mouse_Mmb_Release
     , Mouse_Mmb_None
     , Scrollbar_Pos_Change
 } e;
 }



 class GpuPointLight : public GpuMemory {
 public:
// Packed into GPU FLOAT4
// - do not change order
     vec3 _vPos;  
     float _fRadius;    
 //    
     vec3 _vDiffuseColor;
     float _fPad;
   //  float _fSpecAmt;
//
   //  vec3 _vSpecColor;
   //  float _fSpecExp;
//
 };
 class GpuDirLight : public GpuMemory {
 public:
     // Packed into GPU FLOAT4
     // - do not change order
     vec3 _vPos;
     float _fMaxDistance;
//
     vec3 _vDir;
     float _fLinearAttenuation;
//    
     vec3 _vDiffuseColor;
     float _fPad;
   //  float _fSpecAmt;
//
  //   vec3 _vSpecColor;
  //   float _fSpecExp;
//
     mat4 _mView;
     mat4 _mProj;
     mat4 _mPVB;
 };
 class GpuDeferredParams : public GpuMemory {
 public:
     float _fFogDamp;
     float _fFogBlend;
     float _fFogDivisor;
     float _pad1;
     //
     vec4  _vFogColor;
     //
     int32_t _iPointLightCount;
     float _fHdrSampleExp; //Exponent to use converting input color textures to HDR
     float _fHdrToneMapExp; //Exponent to use when tone mapping back to LDR
     int32_t _iShadowBoxCount;
     //    
     vec3 _vViewPos;
     int32_t _iDirLightCount;
     //
     vec3 _vViewDir;
     float _fTimeSeconds;
     //
     vec3 _vAmbientColor;
     float _fAmbientIntensity;
     //
     float _fFocalDepth;
     float _fFocalRange;
     float _pad2;
     float _pad3;
 };

 //Goes here becauwse TreeNode inherits it.
 class Drawable : public VirtualMemoryShared<Drawable> {
 public:
   virtual void drawDeferred(RenderParams& rp) = 0;
   virtual void drawForward(RenderParams& rp) = 0;
   virtual void drawShadow(RenderParams& rp) = 0;
   virtual void drawForwardDebug(RenderParams& rp) = 0;
   virtual void drawNonDepth(RenderParams& rp) = 0; // draw the non-depth test items (last)
   virtual void drawTransparent(RenderParams& rp) = 0; //These come after the way after, the very end
   virtual void drawUI(RenderParams& rp) = 0;
   virtual void drawDebug(RenderParams& rp) = 0;
 };


}//ns game


#include "../gfx/RenderParams.h"

extern "C" {

//DereK: note if you are updating stb_truetype, make sure to comment these structs out 2/7/18
//STB namespace
typedef struct
{
    unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
    float xoff, yoff, xadvance;
} stbtt_bakedchar;

typedef struct
{
    float x0, y0, s0, t0; // top-left
    float x1, y1, s1, t1; // bottom-right
} stbtt_aligned_quad;
}



#endif
