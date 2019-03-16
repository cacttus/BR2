#ifndef __NOISE_SH__
#define __NOISE_SH__
int static_mersenne(in int seed)
{
	 int i=5,y;
     
	 y = (0x6c078965 * (seed ^ (seed>>30))+i) & 0xffffffff;
	 y = y ^ (y>>11);
	 y = y ^ ((y<<7) & 0x9d2c5680);
	 y = y ^ ((y<<15) & 0xefc60000);
	 y = y ^ (y>>18);

	return y;
}
float static_3f(in vec3 vPos) 
{
    //returns [-1,1]
	int x1 = static_mersenne(floatBitsToInt(vPos.x));
	int y1 = static_mersenne(floatBitsToInt(vPos.y));
	int z1 = static_mersenne(floatBitsToInt(vPos.z));

	int aa = (x1^y1^z1);

	// - Returns the floating point part of the sum of 3 RNGs
	int a = (aa & 0x007fffff) | 0x40000000;

	return intBitsToFloat(a)-3.0f;
}
float static_3f_01(in vec3 vPos) 
{
    //returns [0,1]
    return (static_3f(vPos)+1.0f)*0.5f;
}
vec3 static_3fv(in vec3 vPos) 
{
    //Returns vector with components from [-1,1]
    vec3 vRet;
    int iRes_x = (static_mersenne(floatBitsToInt(vPos.x)) & 0x007fffff) | 0x40000000;
    int iRes_y = (static_mersenne(floatBitsToInt(vPos.y)) & 0x007fffff) | 0x40000000;
    int iRes_z = (static_mersenne(floatBitsToInt(vPos.z)) & 0x007fffff) | 0x40000000;
	vRet.x = intBitsToFloat(iRes_x) - 3.0f;
	vRet.y = intBitsToFloat(iRes_y) - 3.0f;
	vRet.z = intBitsToFloat(iRes_z) - 3.0f;

    return vRet;
}
vec3 static_3fv_01(in vec3 vPos) 
{
// returns vecotr in the range [0,1]
    vec3 ret = static_3fv(vPos);
    ret = (ret+1.0f) * 0.5f;
    return ret;
}
vec3 static_3fv_01_grad(in vec3 vPos, in float fScale) 
{
    // returns vecotr in the range [0,1]
    vec3 viPos = vec3(ivec3(vPos));
    vec3 v_itp[6];
    vec3 v_res[6];
    
    v_itp[0] = vec3(viPos.x  ,viPos.y  ,viPos.z  );
    v_itp[1] = vec3(viPos.x+1,viPos.y  ,viPos.z  );
    //v_itp[2] = vec3(viPos.x  ,viPos.y  ,viPos.z  );
    v_itp[3] = vec3(viPos.x  ,viPos.y+1,viPos.z  );
    //v_itp[4] = vec3(viPos.x  ,viPos.y  ,viPos.z  );
    v_itp[5] = vec3(viPos.x  ,viPos.y  ,viPos.z+1);
    
    v_res[0] = static_3fv_01(v_itp[0]);
    v_res[1] = static_3fv_01(v_itp[1]);
    v_res[2] = v_res[0] ;//static_3fv_01(v_itp[2]);
    v_res[3] = static_3fv_01(v_itp[3]);
    v_res[4] = v_res[0] ;//static_3fv_01(v_itp[4]);
    v_res[5] = static_3fv_01(v_itp[5]);
    
    vec3 vFrac = fract(vPos);
    
    vec3 lerp_x = mix(v_res[0], v_res[1], vFrac.x);
    vec3 lerp_y = mix(v_res[2], v_res[3], vFrac.y);
    vec3 lerp_z = mix(v_res[4], v_res[5], vFrac.z);
    
    
    return (lerp_x+lerp_y+lerp_z)/3.0f;
    
    //return mix(lerp_z, mix(lerp_x, lerp_y, vFrac.z), vFrac.y);
}












#endif