//Shared lighting funcs
//2017 06 02

//***LIGHT
vec4 light(
  in vec3 vFragPos
, in vec4 vFragColor
, in vec3 vLightPos
, in vec4 vLightColor
, in float fLightRadius
, in float fLinearCutoff  // The distance from the point light radius. [0,1]
, in float fLightDotFrag
, in float fFragToLightDistance
, in vec3 vFragNormal
, in vec3 vFragToViewDir // Normalized fragment direction
, in vec3 vVertexLightDir // Normalized vertex - light
, in vec3 vSpecColor
, in float fSpecExp
)
{
    vec4 fragCol;

    #ifdef _DEBUG_HARD_LIGHTS
        fLinearCutoff = 1.0f;
    #endif

    // - Apply Alpha Blended RGB to color - Do not multiply vFragAlpha by a - we already do that in the ps
    vec3 vFragBlended = vFragColor.rgb;// * (1.0f-vFragAlpha.a) + vFragAlpha.rgb*vFragAlpha.a;

    // - Diffuse
    float fDiffuse = max(fLightDotFrag, 0);

    // - Spec 
    //float fShineExp = fSpecExp;//5.7093f;    //Note something high like 40.0 works for very shiny surfaces.
 // //  vec3 vSpecColor = vLightColor.rgb;//vec3(.99140f,.99144f,.98154f);
    //vec3 vReflect = reflect(vVertexLightDir, vFragNormal);
    //float eDotR = dot(vReflect, vFragToViewDir);
    //float fSpec = clamp(pow(eDotR, fShineExp), 0,1);        
    //
    float fSpec = 0.0f;
    // - Apply material parametrs
    vFragBlended += (vLightColor.rgb*fDiffuse + vSpecColor*fSpec) * fLinearCutoff;
    //TODO: ambient,diffuse,spec color should actually be light colors.  Remove vLightColor.

    // - Saturate alhpa value
    return vec4(vFragBlended, 1);
}

//**FOG**
vec4 fog(
  in vec3 vFragPos
, in vec4 vFragColor
, in float fFragCamDistance
) 
{
    float fAmount;
    vec4 vRetColor;
    float density = 0.001f;

    //OpenGL deafult fog
    //http://what-when-how.com/opengl-programming-guide/fog-blending-antialiasing-fog-and-polygon-offset-opengl-programming/
    //fAmount = exp(  -(fFragCamDistance * fFragCamDistance) );//new 
    
    //Heck with all this.  This makes a OK fog.  No BS
    fAmount = log(fFragCamDistance / g_deferredParams._fFogDivisor * g_deferredParams._fFogDamp); // 1- (1/x^2)
    fAmount = clamp(fAmount, 0,1);
    fAmount *= g_deferredParams._fFogBlend; //"global" blend factor, to change how much fog shows.
        
    vec3 vFogColor = g_deferredParams._vFogColor.rgb ;

    vRetColor.rgb = mix(vFragColor.rgb, vFogColor, fAmount);
    vRetColor.a = vFragColor.a;
    
    return vRetColor;
}
float pointOnRay_t( vec3 a, vec3 p ) {
    vec3 AP = p - a;    
    vec3 AB = a*-1.0f;    
    float ab2 = AB.x*AB.x + AB.y*AB.y + AB.z*AB.z;   
    float ap_ab = AP.x*AB.x + AP.y*AB.y + AP.z*AB.z;   
    float t = ap_ab / ab2;   
    return t;
}
//DIFFUSE
vec3 diffuse_and_spec(in vec3 vVertexLightDir, in vec3 fragNormal, in vec3 vFragToViewDir,
 in float specExp, in float specAmt, in float fLightDotFrag, in vec3 lightColor, in vec3 specColor, in float fLinearAttenuation){
    //Diffuse
    float fDiffuse = max(fLightDotFrag, 0);
    return (lightColor*fDiffuse) * fLinearAttenuation;
}
//diffuse color is 1 - x, spec is x, also light color comes into play into spec. not diffuse (afaik)


//**Mirror**
vec4 mirror(in vec4 vColor, in float fMirrorAmount, in vec3 vFragToViewDir, in vec3 fragNormal, in vec3 fragPos) {
    if(fMirrorAmount <= 0.0001) { 
        return vColor;
    }
    
    vec3 vReflect = reflect(vFragToViewDir, fragNormal);
    vec2 tex;
    tex.t = dot(normalize(vReflect), vec3(0.0, -1.0, 0.0));//This produces upside down ENV maps. swapped vec3.. with -1.0
    vReflect.y = 0.0;
    tex.s = dot(normalize(vReflect), vec3(1.0, 0.0, 0.0)) * 0.5;
    
    //float r = 10.0f;
    
    if (vReflect.z >= 0.0) {
        tex = (tex + 1.0) * 0.5;
    }
    else {
        tex.t = (tex.t + 1.0) * 0.5;
        tex.s = (-tex.s) * 0.5 + 1.0;
    }
    
    vec3 vOut = mix( vec3(vColor), vec3(texture(_ufTexEnv0, tex)), fMirrorAmount);
    return vec4(vOut, vColor.a);
}
void applyPointLights(inout vec3 sumDiffuse, inout vec3 sumSpec, in vec3 fragPos, in vec3 fragNormal, in vec3 vFragToViewDir, in vec3 vSpecColor, in float fSpecHardness, in float fSpecIntensity) {
    
    for(int iLight=0; iLight < g_deferredParams._iPointLightCount; ++iLight) {
		vec3 lightPos      = g_pointLights[iLight]._vPos;
		float fLightRadius = g_pointLights[iLight]._fRadius;
		vec3 lightColor    = g_pointLights[iLight]._vDiffuseColor;

        vec3 vVertexLightDir = normalize(lightPos - fragPos) ; //Light direction is on the plane.
        float fLightDotFrag = dot(fragNormal, vVertexLightDir); 
        if(fLightDotFrag <= 0.0f) {
            //This is not just for lights but prevents "shadow pass-through" where shadows show up on back faces.
            continue;
        }
        
        // - Attenuate light
        // Removed negative radius in favor of FLT_MAX (ant to reduced branchign)
        float fFragToLightDistance = distance(lightPos, fragPos);
        float fLinearAttenuation = 1.0 - clamp(fFragToLightDistance/fLightRadius, 0, 1);
        
        // In order to process the entire scene with ambient light we need to allow the light to cascade through the whole scene
        //continue if the light has no effect
        //Generally, it's going to output zero anyway - we can just remove this.
        if(fLinearAttenuation <= 0.0f) {
            continue;           
        }

        //Shadow
        float shadowMul = 1;
        if(textureSize(_ufShadowBoxSamples[iLight], 0).x > 1) {
            vec3 vVertexLightSpace = fragPos - lightPos;
            float bias = 0.9f;
            vec2 v = texture(_ufShadowBoxSamples[iLight], vVertexLightSpace, bias).rg;
            float shadowLen = v.r;
            
            if(abs(shadowLen - fFragToLightDistance) > 0.2f) {  //Prevent Self Shadow errors
                if(shadowLen < fFragToLightDistance){
                    shadowMul = 0.0f;//1.0f-v.g; // allows for transparent textures to have soft shadows
                 
                    //https://gamedev.stackexchange.com/questions/130888/what-are-screen-space-derivatives-and-when-would-i-use-them
                     //float luminance = dot(vec3(shadowMul,shadowMul,shadowMul),vec3(0.2126, 0.7152, 0.0722));
                     // float gradient = fwidth(0 );
                     // shadowMul *=  gradient;
                      
                 //continue;
                }
            }
        }

        //Diffuse
        float fDiffuse = max(fLightDotFrag, 0);
        sumDiffuse += (lightColor*fDiffuse) * fLinearAttenuation * shadowMul;
                
        //Spec
        vec3 vReflect = reflect(-vVertexLightDir, fragNormal);//note the reflet angle is not in vertex space, it's from vertex TO light
        float eDotR = clamp( pow(clamp(dot(vReflect, vFragToViewDir), 0,1), fSpecHardness), 0,1 );
        sumSpec += vSpecColor * fSpecIntensity * fLinearAttenuation * eDotR  * shadowMul;     
	}

}    

void applyDirLights(inout vec3 sumDiffuse, inout vec3 sumSpec, in vec3 fragPos, in vec3 fragNormal, in vec3 vFragToViewDir, in vec3 vSpecColor, in float fSpecHardness, in float fSpecIntensity){
    //!!
    //!!!SHADOW MUL NOT WORKING
    //**Dir lights - the shadows here don't work yet.
    for(int iLight=0; iLight < g_deferredParams._iDirLightCount; ++iLight) {
		vec3 lightPos      = g_dirLights[iLight]._vPos;
		vec3 lightDir      = g_dirLights[iLight]._vDir;
		vec3 lightColor    = g_dirLights[iLight]._vDiffuseColor;
        float maxDist      = g_dirLights[iLight]._fMaxDistance;
        mat4 mProj         = g_dirLights[iLight]._mProj;
        mat4 mView         = g_dirLights[iLight]._mView;
        mat4 pvb           = g_dirLights[iLight]._mPVB;
        vec3 lightDirFrag  = -lightDir; // light dir is supposed to be from the base of the vertex (here it is from the light position)
         
        //vec3 vVertexLightDir = lightDir;//normalize(lightPos - fragPos) ; //Light direction is on the plane.
        float fLightDotFrag = dot(fragNormal, lightDirFrag); 
        if(fLightDotFrag <= 0.0f) {
            //This is not just for lights but prevents "shadow pass-through" where shadows show up on back faces.
            continue;
        }
        
        // - Attenuate light
        // Removed negative radius in favor of FLT_MAX (ant to reduced branchign)
        float fFragToLightDistance = distance(lightPos, fragPos);
       // if(fFragToLightDistance > maxDist){
       //     continue;
       // }
       mat4 mBias = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0);
        //Shadow
        float fSelfShadowError = 0.2f;
        if(textureSize(_ufShadowFrustumSamples[iLight], 0).x > 1) {
        
           // float fNear = 1.0f;
           // 
           // vec3 planePt = lightPos + lightDir * fNear;
           // vec3 n = lightDir;
           // float d = -dot(n, planePt);
           //
           // vec3 p1 = lightPos;
           // vec3 p2 = fragPos;
           // 
           // float out_t = -(dot(n,p1) + d) / (dot((p2 - p1), n));//line intersect plane
           // 
           // vec3 projPt = (p2 - p1) * out_t;//world space pont projected on plane
           // 
           // vec3 right = normalize(cross(vec3(0,1,0), lightDir));
           // vec3 up = normalize(cross(lightDir, right));
           // 
           // 
           // projPt = projPt - planePt;//relative to plane center.
           // 
           // float dx = ((pointOnRay_t(right, projPt)+1.0f)*0.5) ;
           // float dy = ((pointOnRay_t(up, projPt)+1.0f)*0.5) ;
           //
           // 
            //vec2 tc = vec2(dx,dy);
            
            vec2 tc;
            vec4 fragLightTex = (mProj * mView * mBias * vec4(fragPos,1));
            float shadowLen = texture(_ufShadowFrustumSamples[iLight], tc).r;
            //  float shadowLen = texture(_ufShadowFrustumSamples[iLight], _tcoordOut).r;
            tc = fragLightTex.xy;
         //   if(abs(shadowLen - fFragToLightDistance) > fSelfShadowError) {  //Prevent Self Shadow errors
                if(shadowLen < fFragToLightDistance) {
                    continue;
                }
        //    }
        }
        float fLinearAttenuation = 1.0f;
        //Diffuse
        float fDiffuse = max(fLightDotFrag, 0);
        sumDiffuse += (lightColor*fDiffuse) * fLinearAttenuation;
                
        //Spec
        vec3 vReflect = reflect(-lightDirFrag, fragNormal);//note the reflet angle is not in vertex space, it's from vertex TO light
        float eDotR = clamp( pow(clamp(dot(vReflect, vFragToViewDir), 0,1), fSpecHardness), 0,1 );
        sumSpec += vSpecColor * fSpecIntensity * fLinearAttenuation * eDotR;    
       // fLightCountDiv += 1.0f;
       // sumSpecAngle += eDotR;
	}

}

vec4 computeFinalColor(in vec4 fragColor,in vec3 sumDiffuse,in vec3 sumSpec,in vec3 fragPos){
    //Apply light to texel
    vec4 vFinalColor;
    vFinalColor.rgb = fragColor.rgb * sumDiffuse + sumSpec;

    //Fog
    float fFragToViewDistance = distance(g_deferredParams._vViewPos, fragPos);
    vFinalColor = fog(fragPos, vFinalColor, fFragToViewDistance);

    // - HDR to LDR
    vFinalColor = toneMapLightFinal(vFinalColor, g_deferredParams._fHdrToneMapExp);
    vFinalColor.a = fragColor.a;
    
    return vFinalColor;
}

vec4 lightFragment(vec3 fragPos, vec4 fragColor,vec3 fragNormal,vec3 vSpecColor, float fSpecHardness, float fMirrorPower, float fSpecIntensity) {
    
	if(fragNormal.x==0.0 
	&& fragNormal.y==0.0 
	&& fragNormal.z==0.0)
	{
        //Texture access in a conditional this might fail..
        vec4 ret = toneMapLightFinal(fragColor, g_deferredParams._fHdrToneMapExp);
		ret.a = 1.0f;// Force A to 1.0
		return ret;
	}

    // Frag Temps
    vec3 vFragToViewDir = normalize(g_deferredParams._vViewPos - fragPos);

    //Mirror, Probably wrong.  We're just testing for now.
    fragColor = mirror(fragColor, fMirrorPower, vFragToViewDir, fragNormal, fragPos);
    
    vec3 sumSpec = vec3(0,0,0);
    float sumSpecAngle = 0;
    float fLightCountDiv =0;
    
	// Lighting Loop - We can optimize this by removing the for, and the continue and rendering light subset patches instead.
	vec3 sumDiffuse = vec3(
    g_deferredParams._vAmbientColor.r * g_deferredParams._fAmbientIntensity
    , g_deferredParams._vAmbientColor.g * g_deferredParams._fAmbientIntensity
    , g_deferredParams._vAmbientColor.b * g_deferredParams._fAmbientIntensity);
    
    //Lights, Shadows
    applyPointLights(sumDiffuse, sumSpec, fragPos, fragNormal, vFragToViewDir, vSpecColor, fSpecHardness, fSpecIntensity);
    applyDirLights(sumDiffuse, sumSpec, fragPos, fragNormal, vFragToViewDir, vSpecColor, fSpecHardness, fSpecIntensity);
    
    //Fog, Spec, and HDR
    return computeFinalColor(fragColor, sumDiffuse, sumSpec, fragPos);
}
