//#include "../model/ArmatureSpec.h"
//#include "../model/JointSpecHierarchy.h"
//#include "../model/JointSpec.h"
//#include "../model/JointList.h"
//
//
//namespace Game {
////void Articulation::makeJointIdsContiguousAndZeroBased()
////{
////
////}
///**
//*  @fn getJointOrdinal
//*  @brief returns an invalid joint ordinal if it wasn't found.
//*    otherwise returns the ordinal the zero-based contiguous offset of a joint, which is not the joint ID)
//*/
//JointOrdinal ArmatureSpec::getJointOrdinal(t_jointId jId)
//{
//    JointOrdinalMap::iterator ite = _mapJointOrdinals.begin();
//    for(;ite!=_mapJointOrdinals.end();ite++)
//        if(ite->second->getID().id==jId)
//            return ite->first;
//    return JOINT_ORDINAL_INVALID;
//}
///**
//*  @fn getJointByOrdinal
//*  @brief Given an ordinal (the zero-based contiguous offset of a joint) return that joint
//*/
//JointSpec* ArmatureSpec::getJointByOrdinal(JointOrdinal jOrd)
//{
//    JointOrdinalMap::iterator ite = _mapJointOrdinals.find(jOrd);
//    if(ite!=_mapJointOrdinals.end())
//        return ite->second;
//    return NULL;
//}
///**
//*  @fn 
//*  @brief Find and do
//*/
//JointSpec* ArmatureSpec::getJointById(t_jointId jointId)
//{
//    JointSpec* j=NULL;
//    for(size_t i=0; i<_vecJointTrees.size(); ++i)
//    {
//        if( (j=_vecJointTrees[i]->findJointByID( jointId )) != NULL )
//            return j;
//    }
//    return j;
//}
///**
//*  @fn 
//*  @brief This not only adds trees but also indexes joint ordinals.
//*/
//void ArmatureSpec::addJointTree(JointSpecHierarchy* pt)
//{
//    _nTotalJoints += pt->getNumJoints();
//    _vecJointTrees.push_back(pt);
//
//    //I assume the reason for this is that articulations can have multiple joint trees
//    // with multiple trees we need a way to keep track of different joint ordinals so we make the 
//    // joint IDs unique... yet this screws up the system becuase jont ordinals must be the same as
//    // we passed them in.
//    for(size_t i=0; i<pt->getJointList()->pJoints.size(); ++i)
//    {
//        pt->getJointList()->pJoints[i]->_myOrdinal = _nextJointOrdinal++;
//        pt->getJointList()->pJoints[i]->setArticulationSpec(this);
//
//        _mapJointOrdinals.insert( pt->getJointList()->pJoints[i]->_myOrdinal, pt->getJointList()->pJoints[i] );
//    }
//
//}
//// return joint tree at index i
//JointSpecHierarchy* ArmatureSpec::getJointTree( size_t i )
//{
//    return _vecJointTrees[i];
//}
//
//ArmatureSpec::ArmatureSpec() : 
//_nTotalJoints(0), 
//_nextJointOrdinal(0) 
//{
//
//}
//ArmatureSpec::~ArmatureSpec()
//{
//    for(size_t i=0; i<_vecJointTrees.size(); ++i)
//        delete _vecJointTrees[i];
//    _vecJointTrees.resize(0);
//    
//    _mapJointOrdinals.clear();
//}
//
//
//
//}//ns game
