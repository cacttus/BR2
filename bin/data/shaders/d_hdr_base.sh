#include "d_base_fn.sh"

//20160527
// HDR rendering 
vec4 sample2d(in sampler2D smp, in vec2 pos)
{
    return texture(smp, vec2(pos));
}
//Convert a COLOR sample to a higher dynamic range (HDR)
vec4 sample2d_hdr(in sampler2D smp, in vec2 pos, in float exp)
{
    vec4 tx = texture(smp, vec2(pos));
    
    tx.r = pow(tx.r, exp);
    tx.g = pow(tx.g, exp);
    tx.b = pow(tx.b, exp);
    //tx.a = pow(tx.r,2.2f);
    
    return tx;
}
//Convert back from HDR to LDR
vec4 toneMapLightFinal(in vec4 vFinalColor, in float toneExp)
{
    vec3 final;
    
    final = vec3(1.0) - exp(-vFinalColor.rgb * toneExp);
    final = pow(final, vec3(1.0f/toneExp));

    return vec4(final, 1.0f);
}