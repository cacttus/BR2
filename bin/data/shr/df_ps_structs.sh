
struct GpuDeferredParams {
    float _fFogDamp;
    float _fFogBlend;    
    float _fFogDivisor;
    float _pad1;
//
    vec4  _vFogColor;
//
    int _iPointLightCount;
    float _fHdrSampleExp; //Exponent to use converting input color textures to HDR
    float _fHdrToneMapExp; //Exponent to use when tone mapping back to LDR
    int _iShadowBoxCount;
//    
    vec3 _vViewPos;
    int _iDirLightCount;
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
struct GpuPointLight {
    vec3 _vPos;
    float _fRadius;
//    
    vec3 _vDiffuseColor;
    float _fPad;
//    
 //   vec3 _vSpecColor;
 //   float _fSpecExp;

};
struct GpuDirLight {
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
//
  //   vec3 _vSpecColor;
  //   float _fSpecExp;
//
    mat4 _mView;
    mat4 _mProj;
    mat4 _mPVB;
 };
 //Must match engine.config


layout(std140) uniform UfDeferredParams { GpuDeferredParams g_deferredParams; };
#define MAX_POINT_LIGHTS 32
#define MAX_DIR_LIGHTS 4
#define MAX_CUBE_SHADOW_SAMPLES 4
#define MAX_FRUS_SHADOW_SAMPLES 4
layout(std140) uniform UfPointLights { GpuPointLight g_pointLights[MAX_POINT_LIGHTS]; };
layout(std140) uniform UfDirLights { GpuDirLight g_dirLights[MAX_DIR_LIGHTS]; };
uniform samplerCube _ufShadowBoxSamples[MAX_CUBE_SHADOW_SAMPLES];	//this constant must match MaxShadowInfluences in Engine.dat
uniform sampler2D _ufShadowFrustumSamples[MAX_FRUS_SHADOW_SAMPLES];	//this constant must match MaxShadowInfluences in Engine.dat


uniform sampler2D _ufTexEnv0;//enviro map, for mirror


//OpenGL ES 3.1 ipmlmements SSBOs
//layout(std430, binding = 0) buffer UfPointLights { LightData g_pointLights[]; };
//layout(std430, binding = 1) buffer UfDeferredParams { DeferredParams g_deferredParams[]; };








