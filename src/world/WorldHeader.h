/**
*  @file WorldHeader.h
*  @date January 4, 2018
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLDHEADER_15150858034241992580_H__
#define __WORLDHEADER_15150858034241992580_H__

#include "../model/ModelHeader.h"

namespace BR2 {
#define PHY_MIN_VEL_LEN_2 (0.00003f)
#define PHY_DELTA_P_LEN_2 (0.000003f)
#define PHY_COLLIDE_PADDING_EPSILON (0.0003f)

//Do not change order
namespace PhysicsGridSide { typedef enum { gL, gR, gB, gT, gA, gF, gC } e; }
namespace PhysicsShapeType { typedef enum { None, Hull, Sphere, AABox } e; }

class BoxCollision;
class PhysicsNode;
class CheckedSet;
class StuckPair;
class GridManifold;
class NodeManifold;
class PhysicsGrid;
class PhysicsManager;
class PhysicsGridAwareness;
class PhysicsShape;
class SphereShape;
class HullShape;
class RenderBucket;
class BvhCollectionParams;
class RenderBucket;
class Scene;
class WorldObject;

}//ns BR2



#endif
