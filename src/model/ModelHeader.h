/**
*  @file ModelHeader.h
*  @date November 25, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __MODELHEADER_1480137176306951373_H__
#define __MODELHEADER_1480137176306951373_H__

#include "../base/BaseHeader.h"
#include "../base/TreeNode.h"
#include "../math/MathAll.h"

namespace Game {
//GPU Types
typedef int32_t GpuInt;
typedef float GpuFloat;
typedef mat4 GpuMat4f;
typedef vec3 GpuVec3f;
typedef ivec3 GpuVec3i;    // X Y Z size of the block.  This actually is square
typedef uint32_t GpuUInt;
//typedef std::future<bool> GpuFuture;
typedef GLuint GpuTexId;
typedef GLenum GpuEnum;
typedef GLuint GpuBufferId;

//Make this global. stop the madness.
typedef v_v3n3x2 ModelVertexType;

//namespace KeyframeDataType { typedef enum { Undefined, Mat4, Prs } e; }
namespace KeyframeInterpolation { typedef enum { Undefined, Bezier, Linear } e; }
namespace BoxPoint { typedef enum { NBL, NBR, NTL, NTR, FBL, FBR, FTL, FTR } e; }
//namespace GpuAnimationMemoryModel { typedef enum { MODEL_CPU_ANIMATION_OR_STATIC_MODEL, MODEL_GPU_ANIMATION_WITH_SWAP_BUFFERS } e; }
namespace MeshSkinStatus { typedef enum { Uninitialized, NoSkin, Allocated, Error } e; }
//namespace ModelDrawMode { typedef enum { Color, Pick } e; }
namespace ParentType { typedef enum { None, Bone, Object, Armature } e; }

//////////////////////////////////////////////////////////////////////////
class UtilMeshInline2d;

//Class FWD
class ModFile;
class VboData;
class VaoDataGeneric;
class IboData;
class ObjFile;
class MeshCache;
class KeyFrame;
class BoneSpec;
class Mover;
class Animator;
class BoneNode;
class PRSAnimation;
class RotateTo;
class MeshNode;
class ShaderStorageBuffer;
class MeshSpec;
class UtilMesh;
class UtilMeshBox;
class UtilMeshAxis;
class UtilMeshGrid;
class UtilMeshInline;
class UtilMeshPickBox;
class UtilMeshSphere;
class RenderUtils;
class Material;
class VertexAdjacencyMap;
class FragmentBufferData;
class IndexBufferData;
class VertexComponent;
class VertexFormat;
class TextBufferMesh;
class TileMesh25;
class ModelSpec;
class ModelNode;
class GpuBufferData;
class MobFile;
class Armature;
class ArmatureNode;
class ActionKeys; //Action was ambiguous
class ActionGroup;
class BaseSpec;
//class BaseNode;
class BaseNode;
class PhysicsNode;
class MbiFile;
class OBB;
class ModelCache;

//////////////////////////////////////////////////////////////////////////

class MeshBufferData {
public:
  std::shared_ptr<FragmentBufferData> _frags;
  std::shared_ptr<IndexBufferData> _indexes;
};

class VertexBufferPointer {
public:
  void* _pBuf;
  size_t _iOffset;
  size_t _iStride;
};

class VertexWeightMob : public VirtualMemory {
public:

  //Hash32 _iArmNameHashed;//hashed name of the armature that owns the bones
  //we're now using armature ID instead of the hashed name.
  std::map<int32_t, std::map<int32_t, float>> _mapWeights;
  VertexWeightMob() {}
  virtual ~VertexWeightMob() { _mapWeights.clear(); }
};


class GpuAnimatedMeshWeightData : public GpuMemory {
public:
  GpuInt _offset;    // offset into weight buffer (GpuAnimatedMeshWeight)
  GpuInt _count;    // count of weights

  float pad[2];
};
//Stores the mesh weights.
class GpuAnimatedMeshWeight : public GpuMemory {
public:
  GpuInt _iArmJointOffset;    // - Offset into joint matrix buffer 12/2017 - sorted by armature! (GpuAnimatedMeshBindMatrix / GpuAnimatedMeshSkinMatrix)
  GpuFloat _weight;        // weight of joint
  float pad[2];
};
//class GpuAnimatedMeshBindMatrix : public GpuMemory {
//public:
//    GpuMat4f _matrix;
//};

// - Dynamic data.
class GpuAnimatedMeshSkinMatrix : public GpuMemory {
public:
  GpuMat4f _matrix;
};

class GpuAnimatedMeshBoundBox : public GpuMemory {
public:
  GpuVec3f _min;
  float f0;
  GpuVec3f _max;
  float f1;
};

}//ns Game


#include "../model/VertexTypes.h"

#endif
