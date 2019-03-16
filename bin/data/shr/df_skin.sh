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
uniform int _ufWeightCount;
layout(std430,binding = 0) buffer ssInWeightOffsets { wd_in_st wd_in[]; };
layout(std430,binding = 1) buffer ssInJointWeights { jw_in_st jw_in[]; };
layout(std430,binding = 2) buffer ssInSkinMatrices { j_in_man_st j_in_skin[]; };

void doSkin(inout vec3 in_v, inout vec3 in_n) { 
    wd_in_st weight = wd_in[gl_VertexID];
	vec4 vtmp = vec4(in_v,1);
	vec4 ntmp = vec4(in_v + in_n,1);//Turn normal into a point
	vec4 vt = vec4(0,0,0,0);
	vec4 vn = vec4(0,0,0,0);

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
    //Same for all diffuse shaders.  
	in_n = normalize(vn.xyz - in_v);

}
