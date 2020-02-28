//*Material Params
uniform float _ufSpecIntensity;
uniform float _ufSpecHardness;
uniform float _ufMirrorPower;
uniform vec3 _ufSpecColor;
uniform vec4 _ufDiffuseColor;
uniform float _fTpAlpha ; // transparency only
uniform float _fTpIOR   ; // transparency only
uniform float _fTpFilter; // transparency only

//Alpha "discard" threshold
#define ALPHA_MASK 0.001f

//TODO:
//uniform sampler2D _ufTexColor0;//diffuse 
//uniform sampler2D _ufTexNormal0;//bump 

#define FLT_MAX 3.402823466e+38F
mat3 getLightMatrix(in vec3 planeNormal, in vec3 planeVertex){
	float d = - dot(planeVertex, planeNormal);
	vec3 pv = planeVertex+1.0;	//random neighbor vertex
	float dist = dot(planeNormal, pv) + d;	//distnace from p to plane
	vec3 pp = pv - (planeNormal * dist); // - project p onto plane
	vec3 tangent = normalize(pp - planeVertex); // normalize tangent arbitrary direction
	vec3 bitangent = cross(planeNormal, tangent);

	// vec3 tangent = normalize(cross(vec3(1,0,0), planeNormal));
	// vec3 binormal = normalize(cross(planeNormal, tangent));
		
	//note this should produce a matrix the same as the plaen matrix for normals at 0,1,0
	
	return mat3(tangent, planeNormal, bitangent);
}