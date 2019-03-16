// //20151126
// #include "d_base_fn.sh"


// // - stores the initial offset of weight/bone influences for each vertex
// // do not modify
// struct wd_in_st {
	// int wo;	//offset in the jw_in_st struct as in [v = [j,w][j,w]](off=0,count=2), [v=[j,w][j,w][j,w]](off=2,count=3), ....
	// int wc;	//count in the jw_in_st stuct
	// float pad[2];
// };
// // lists all joint weights.
// // do not modify
// struct jw_in_st {
	// int joff; //Joint offset in j_in_man_st
	// float wt;
	// float pad[2];
// };

// // stores a list of computed joint matrixes.
// // do not modify
// struct j_in_man_st {
	// mat4 jmat;
// };
// // - stores a list of joint bind matrixes.
// //do nto modify
// //struct j_in_bind_st {
// //	mat4 jmat;
// //};

// //AABB struct
// // struct v_aabb {	
	// // vec3 min;
	// // vec3 max;
// // };


// // - std430 is required in order to pack the data together.  the std140 qualitifer really fucks with your data as it
// // puts everything on a vec4 boundary, for example if you just have a float in your buffer the std140 would align to 4 floats with 3 extra bytes of information.
// layout(std430,binding = 0) buffer ssInWeightOffsets { wd_in_st wd_in[]; };
// layout(std430,binding = 1) buffer ssInJointWeights { jw_in_st jw_in[]; };
// //layout(std430,binding = 2) buffer ssInBindMatrices { j_in_bind_st j_in_bind[]; };
// layout(std430,binding = 2) buffer ssInSkinMatrices { j_in_man_st j_in_skin[]; };

// //layout(std140) uniform UfWeightOffsets { LightData g_pointLights[64]; };
// //layout(std140) uniform UfJointWeights { DeferredParams g_deferredParams; };

// //do verts separate, one buffer for each vertex type.
// //layout(std430,binding = 3) buffer v_v3n3x2_buf_in { t_vertex_v3n3x2 vp_v3n3x2_in[]; };
// //layout(std430,binding = 4) buffer v_v3n3x2_buf_out { t_vertex_v3n3x2 vp_v3n3x2_out[]; };
