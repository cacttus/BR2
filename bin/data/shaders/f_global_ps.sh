#version 330 core
#include "df_ps_structs.sh"

//Lights
vec4 doLights(in vec3 v3Out, in vec3 n3Out) {
    
    vec4 sumLights = vec4(0,0,0,0);
    for(int iLight=0; iLight<g_deferredParams._iPointLightCount; iLight++) {
        vec3 light_d = g_pointLights[iLight]._pos - v3Out;
        float dist2 = dot(light_d, light_d); //length ^2
    
        vec3 ldp = normalize(light_d);
        float lamb = clamp(dot(n3Out, ldp), 0, 1);
    
        float falloff = 1.0f - clamp(dist2 / (g_pointLights[iLight]._r * g_pointLights[iLight]._r), 0.0f, 1.0f);
        
        sumLights += g_pointLights[iLight]._c * falloff * lamb;
    }
    sumLights = clamp(sumLights, 0, 1);
    
    return sumLights;
}
