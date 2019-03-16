//20151126
#include "d_base_fn.sh"

//Shaadow shaders
#extension GL_EXT_gpu_shader4 : require

//Matrix uniforms.  If not used they will be ignored by the GL.
uniform mat4 _ufView;
uniform mat4 _ufProj;	//projection matrix
uniform mat4 _ufModel; // model space
uniform mat4 _ufNormal; // bump map / normal matrix
