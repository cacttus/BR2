//#include "../math/MathAll.h"
//
//#include "../model/MeshSpecHierarchy.h"
//#include "../base/ObjectFile.h"
//
//
//namespace Game {
//;
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//MeshSpecHierarchy::MeshSpecHierarchy() :
//    MeshSpec(ModelVertexType::getVertexFormat())
//{
//    //_pMeshFactory->registerHierarchy(this);
//}
//MeshSpecHierarchy::~MeshSpecHierarchy()
//{//
//   // if(_pMeshFactory!=NULL)//Important! do not remove or else we recur forever when MeshFcotry deletes its superfluous data.
//   //     _pMeshFactory->notifyDestroy(MeshSpecHierarchy*(this));
//    //**NOTE! we do not actually delete referenced specs here becasue they
//    //are owned by MeshFactory!
//}
////////////////////////////////////////////////////////////////////////////
////Make all parts the same name.
////VertexFormatType MeshSpecHierarchy::getVertexFormatType() {
////    VertexFormatType vft;
////    
////    iterateBreadthFirst<MeshSpec>([&](MeshSpec* pt) {
////        vft = pt->getVertexFormatType();
////        return false; //easy out after first mesh.
////    });
////
////    return vft;
////}
////void MeshSpecHierarchy::setAllPartsModName(t_string& name, bool onlySetIfNull)
////{
////    iterateBreadthFirst<MeshSpec>([&](MeshSpec* pt) {
////        if(onlySetIfNull && pt->getPartName().length()==0)
////            pt->setPartName(name);
////        else
////            pt->setPartName(name);
////        return true;
////    });
////}
//void MeshSpecHierarchy::getAllMeshes(std::vector<MeshSpec*>& ret)
//{
//
//    iterateBreadthFirst<MeshSpec>([&](MeshSpec* pt) {
//        //20160516 - Skip over root node.
//        //if (pt->getIsHierarchyRoot() == false) { //**This was commented out
//            ret.push_back(pt);
//       // }
//        return true;
//    });
//
//}
//void MeshSpecHierarchy::setDefaultMaterialForAllSpecs(MaterialSpec* mt)
//{
//    iterateBreadthFirst<MeshSpec>([&](MeshSpec* pt) {
//        pt->setMaterial(mt);
//        return true;
//    });
//}
////void MeshSpecHierarchy::setDefaultShaderForAllSpecs(ShaderSpec* mt)
////{
////    iterateBreadthFirst<MeshSpec>([&](MeshSpec* pt) {
////        pt->setShader(mt);
////        return true;
////    });
////}
///**
//*  @fn mergeWith
//*  @brief Merges with the model on the right.  The merge is flat, so it is added to the root.
//*/
//void MeshSpecHierarchy::mergeWith(MeshSpecHierarchy* eat)
//{
//    BroThrowNotImplementedException();
//    //TODO: this is wrong currently because MeshSpecHierarchy's empty ROOT node
//    // it must be removed here if we hit this.
//    eat->detachFromParent();
//    this->attachChild((TreeNode*)eat);
//}
///**
//*    Find the first instance of a mesh with the given name
//*/
//MeshSpec* MeshSpecHierarchy::findFirstMeshByName(t_string& name, bool caseSensitive)
//{
//    MeshSpec* pRet = NULL;
//    t_string strFormattedName;
//
//    if (!caseSensitive) {
//        strFormattedName = StringUtil::lowercase(name);
//    }
//    else {
//        strFormattedName = name;
//    }
//
//    iterateBreadthFirst<MeshSpec>([&](MeshSpec* ms) {
//        t_string strMeshName, meshname;
//        if (!caseSensitive)
//            strMeshName = StringUtil::lowercase(ms->getName());
//        else
//            strMeshName = ms->getName();
//
//        if (strMeshName.compare(strFormattedName) == 0)
//        {
//            pRet = ms;
//            return false;
//        }
//        return true;
//    });
//
//    return pRet;
//}
///**
//*  @fn calculateMasterBoundBox
//*  @brief calculate the master bouning box
//*/
//void MeshSpecHierarchy::calculateMasterBoundBox()
//{
// // BroThrowNotImplementedException();
//  getBoundBox()->_min = VEC3_MAX;
//  getBoundBox()->_max = VEC3_MIN;
//
//  iterateBreadthFirst<MeshSpec>([&](MeshSpec* pt) {
//      if (pt->getIsBoundBoxCalculated() == false) {
//          pt->computeBox();
//      }
//
//      getBoundBox()->_min = Vec3f::minv(getBoundBox()->_min, pt->getMin());
//      getBoundBox()->_max = Vec3f::maxv(getBoundBox()->_max, pt->getMax());
//
//      return true;
//  });
//}
//
//
//}//ns BR2
