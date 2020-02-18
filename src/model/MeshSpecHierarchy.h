///**
//*
//*  @file MeshSpecHierarchy.h
//*  @date October 19, 2011
//*  @author MetalMario971
//*/
//#pragma once
//#ifndef __MESHSPECHIERARCHY_10692127717953093025331_H__
//#define __MESHSPECHIERARCHY_10692127717953093025331_H__
//
//#include "../base/BaseHeader.h"
//#include "../model/ModelHeader.h"
//#include "../model/MeshSpec.h"
//namespace Game {
///**
//*  @class MeshSpecHierarchy
//*  @brief Contains a list of roots which point to hierarchies of meshes.
//*  @details: It would be intuitive and correct to get rid of MeshSpecHierarchy and make the
//*    first spec simply a MeshSpec.  However, because you can have a mesh tree with multiple roots
//*    we need something to contain all the roots together in order to cohesively iterate the tree.
//*    MeshSpec needs to be the base class in order for the TreeNode::iterateBreadthFirst to work.
//*    This class creates a "dummy" mesh spec without any data as the root node of the given mesh.  This effectively
//*    always creates a root mesh node.
//*    TODO: replciate this to the Skin/Joint Hierarchy
//*/
//class MeshSpecHierarchy : public MeshSpec {
//    friend class MeshManager;
//    //AxisAlignedBoundBox3f* _boundBoxMasterNotTransformed;    // - UNTRANSFORMED bounding box of all meshes in the bind pose. 
//    bool _autoGenerateContactBoxes;
//
//public:
//    MeshSpecHierarchy();
//    virtual ~MeshSpecHierarchy() override;
//
//    //Get/Set
//  //  VertexFormatType getVertexFormatType();
//    void getAllMeshes(std::vector<MeshSpec*>& ); // - Get a flat vector of all the meshes in this hierarhcy.
//    void setAutoGenerateContactBoxes(bool b){ _autoGenerateContactBoxes=b; }
//    bool getAutoGenerateContactBoxes(){ return _autoGenerateContactBoxes; }
//    virtual bool getIsHierarchyRoot() { return true; }
//    //Methods
//    void calculateMasterBoundBox();
//    void mergeWith(MeshSpecHierarchy* eatThisOne);    // - Merges with the given hierarchy deleting it.
//    //void setAllPartsModName(t_string& name, bool onlySetIfNull=false);    // sets name of all MOD names for parts.
//    void setDefaultMaterialForAllSpecs(MaterialSpec* mt);    // does what it says.  This is done only on load, and we really don't anticipate swapping mateirals in the simpel game engine.
//    //void setDefaultShaderForAllSpecs(TRef(ShaderSpec) mt);
//    MeshSpec* findFirstMeshByName(t_string& name, bool caseSensitive); // - Get the mesh with the given name
//};
//
//}//ns BR2
//
//
//
//#endif
