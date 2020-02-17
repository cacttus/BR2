///**
//*
//*  @file Articulation.h
//*  @date October 20, 2011
//*  @author MetalMario971
//*
//*    � 2011 
//*
//*
//*/
//#pragma once
//#pragma once
//#ifndef __ARTICULATION_86244648272512878120003_H__
//#define __ARTICULATION_86244648272512878120003_H__
//
//#include "../base/BaseHeader.h"
//#include "../model/ModelHeader.h"
//
//namespace Game {
//
///**
//*  @class Articulation
//*  @brief The articulation for a model.  A list of Joint trees and that's all
//*        the meat of animation comes from manipulating the joint trees themselves.  
//*  @details The joint trees for a model.  This is a bind-shape tree for the model.
//*
//*   For animations see:
//*        @c JointMotionData @c MotionSpec
//*
//*    Contained in:
//*  @c ModelAnimationSpec, @c ModelSpec
//*
//*    This is a spec class, it is instanced into:
//*        @c TimeNode
//*/
//class ArmatureSpec : public VirtualMemory {
//public:
//    friend class AnimationManager;
//private:
//    std::vector<JointSpecHierarchy*>    _vecJointTrees;
//    
//    JointOrdinalMap _mapJointOrdinals;    // - **This is a contiguous mapping of all joints in the model from a top level.
//    JointOrdinal _nextJointOrdinal;
//
//    uint32_t _nTotalJoints;    //sum total of all joints
//    mat4 _matObjectMatrix;  //Matrix of the armature in local object space.
//public:
//    ArmatureSpec();
//    OVERRIDES ~ArmatureSpec() OVERRIDE;
//
//    FORCE_INLINE uint32_t getNumJoints(){ return _nTotalJoints; }
//    FORCE_INLINE size_t numJointTrees(){ return _vecJointTrees.size(); }
//
//    mat4& getObjectMatrix() { return _matObjectMatrix; }
//
//    //later
//    //void makeJointIdsContiguousAndZeroBased();
//
//    // Ordered joint access
//    JointOrdinal getJointOrdinal(t_jointId j);
//    JointSpec* getJointByOrdinal(JointOrdinal o);
//    //t_jointId getJointIdByOrdinal(uint32_t o);
//
//    JointSpec* getJointById(t_jointId jointId);
//    void addJointTree(JointSpecHierarchy* pt);// add new joint tree root
//    JointSpecHierarchy* getJointTree( size_t i );// return joint tree at index i
//
//};
//
//}//ns BR2
//
//
//
//#endif
