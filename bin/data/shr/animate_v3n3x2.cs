#version 430
// animation compute shader
//derek page 9/10/2014
// 10/7/2014
//objective:
//bind the vertex index to a list of joint matrixes and weight indexes
//#extension GL_ARB_enhanced_layouts : enable


struct t_vertex_v3n3x2 {
	vec3 v;
		float  _pad1;
	vec3 n;
		float  _pad2;
	vec2 x;
    float _pad3;
    float _pad4;
};

// - stores the initial offset of weight/bone influences for each vertex
// do not modify
struct wd_in_st {
	int wo;	//offset in the jw_in_st struct as in [v = [j,w][j,w]](off=0,count=2), [v=[j,w][j,w][j,w]](off=2,count=3), ....
	int wc;	//count in the jw_in_st stuct
	float pad[2];
};
// lists all joint weights.
// do not modify
struct jw_in_st {
	int joff; //Joint offset in j_in_man_st
	float wt;
	float pad[2];
};

// stores a list of computed joint matrixes.
// do not modify
struct j_in_man_st {
	mat4 jmat;
};


// - std430 is required in order to pack the data together.  the std140 qualitifer really fucks with your data as it
// puts everything on a vec4 boundary, for example if you just have a float in your buffer the std140 would align to 4 floats with 3 extra bytes of information.
layout(std430,binding = 0) buffer ssInWeightOffsets { wd_in_st wd_in[]; };
layout(std430,binding = 1) buffer ssInJointWeights { jw_in_st jw_in[]; };
//layout(std430,binding = 2) buffer ssInBindMatrices { j_in_bind_st j_in_bind[]; };
layout(std430,binding = 2) buffer ssInSkinMatrices { j_in_man_st j_in_skin[]; };

//do verts separate, one buffer for each vertex type.
layout(std430,binding = 3) buffer v_v3n3x2_buf_in { t_vertex_v3n3x2 vp_v3n3x2_in[]; };
layout(std430,binding = 4) buffer v_v3n3x2_buf_out { t_vertex_v3n3x2 vp_v3n3x2_out[]; };

//layout(std430,binding = 8) buffer ssOutBoundBox{ v_aabb vp_aabb_out; };

// these are stored in gl_WorkGroupSize.xyz
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

//FWDDCL
vec3 getVertex(uint vpos)
{
	return vp_v3n3x2_in[vpos].v;
}
vec3 getNormal(uint vpos)
{
	return vp_v3n3x2_in[vpos].n;
}
void setVertex(uint vpos, inout vec3 vt)
{
	vp_v3n3x2_out[vpos].v  = vt;
}
void setNormal(uint vpos, inout vec3 vn)
{
	vp_v3n3x2_out[vpos].n  = vn;
}
wd_in_st getWeight(uint vpos)
{
	return wd_in[vpos];
}
// float dbgClamp()
// {
	// return ( float(_ufVertexType) + float(wd_in[0].wo) + jw_in[0].wt + j_in_skin[0].jmat[0] + j_in_bind[0].jmat[0])*0.0001;
// }
void doSkin(wd_in_st weight, inout vec3 in_v, inout vec3 in_n)
{

	vec4 vtmp = vec4(in_v,1);
	vec4 ntmp = vec4(in_v + in_n,1);//Turn normal into a point
	vec4 vt = vec4(0,0,0,0);
	vec4 vn = vec4(0,0,0,0);

	float FLT_MAX= 3.402823466e+38F;

	int wo;
	float jw;
	int jwoff;
	mat4 jskin;
	mat4 jmul;
	
	for(int iinf=0 ;iinf< weight.wc; ++iinf)
	{
		wo = weight.wo;
		jw = jw_in[wo+iinf].wt;
		jwoff = jw_in[wo+iinf].joff;
		jskin = j_in_skin[jwoff].jmat;

		vt += (jskin*vtmp)*jw;
		vn += (jskin*ntmp)*jw;
	}
	
	in_v  = vt.xyz;
	in_n = normalize(vn.xyz - in_v);

}

void main()
{
	// get the position of the vertex

	uint vpos = 
	gl_GlobalInvocationID.z*gl_NumWorkGroups.z*gl_NumWorkGroups.z + 
	gl_GlobalInvocationID.y*gl_NumWorkGroups.y + 
	gl_GlobalInvocationID.x;
	
	// - LOAD VERT
	vec3 in_v = getVertex(vpos);
	vec3 in_n = getNormal(vpos);
	wd_in_st wt = getWeight(vpos);
	
	// - Do some skin
	doSkin(wt,in_v,in_n);

	// - STORE VERT
	setVertex(vpos,in_v);
	setNormal(vpos,in_n);

}

