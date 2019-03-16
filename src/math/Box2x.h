/**
*    @file RectList.h
*    @author Derek Page
*
*    © 2011
*    @date 7 / 10 / 2008 - mgRect
*    1/23/2010 - changed name from mgRectMap to RectList
*    20150207 begin adding methods
*    2/5/18changed name from Quad2f to Box2x
*/
#pragma once
#ifndef __MG_RECTMAP_H__
#define __MG_RECTMAP_H__

#include "../math/Vec2x.h"

namespace Game {
;
/**
*    @class Box2x
*    @brief Basic rect class.
*/
template < class Tx >
class Box2x : public VirtualMemory {
public:
    typedef Vec2x<Tx> VecType;
    VecType _p0, _p1;

    FORCE_INLINE Box2x();
    FORCE_INLINE Box2x(VecType& min, VecType& max);
    FORCE_INLINE Box2x(const Tx& min_x, const Tx& min_y, const Tx& max_x, const Tx& max_y);

    FORCE_INLINE void construct(const Tx& min_x, const Tx& min_y, const Tx& max_x, const Tx& max_y);
    FORCE_INLINE void construct(const VecType& min, const VecType& max);

    FORCE_INLINE Tx width()  const { return _p1.x - _p0.x; }
    FORCE_INLINE Tx height() const { return _p1.y - _p0.y; }
    FORCE_INLINE Tx x() const { return _p0.x; }
    FORCE_INLINE Tx y() const { return _p0.y; }

    FORCE_INLINE VecType center() { return tl() + (br() - tl())*0.5f; }
    FORCE_INLINE Tx& top() { return _p0.y; }
    FORCE_INLINE Tx& left() { return _p0.x; }
    FORCE_INLINE Tx& bottom() { return _p1.y; }
    FORCE_INLINE Tx& right() { return _p1.x; }
    FORCE_INLINE const Tx& top() const { return _p0.y; }
    FORCE_INLINE const Tx& left() const { return _p0.x; }
    FORCE_INLINE const Tx& bottom() const { return _p1.y; }
    FORCE_INLINE const Tx& right() const { return _p1.x; }
    //these are needed for legacy
    FORCE_INLINE VecType& p0() { return _p0; }
    FORCE_INLINE VecType& p1() { return _p1; }
    FORCE_INLINE VecType& tl() { return _p0; }
    FORCE_INLINE VecType& br() { return _p1; }
    FORCE_INLINE VecType& _min() { return _p0; }//Underscore is because of the min() math define in windef.h
    FORCE_INLINE VecType& _max() { return _p1; }

    FORCE_INLINE const VecType& p0() const { return _p0; }
    FORCE_INLINE const VecType& p1() const { return _p1; }
    FORCE_INLINE const VecType& tl() const { return _p0; }
    FORCE_INLINE const VecType& br() const { return _p1; }
    FORCE_INLINE const VecType& _min() const { return _p0; }//Underscore is because of the min() math define in windef.h
    FORCE_INLINE const VecType& _max() const { return _p1; }

    FORCE_INLINE bool rectIntersectEasyOut(Box2x& cc) const;
    FORCE_INLINE bool containsPointInclusive(const VecType& point) const;
    FORCE_INLINE void scale(VecType* v1, VecType* pCenter = NULL);
    FORCE_INLINE void genResetLimits();
    FORCE_INLINE void genExpandByBox(Box2x<Tx>& pc);
    FORCE_INLINE void genExpandByPoint(const VecType& pt);
    FORCE_INLINE void expand(Box2x<Tx>& rhs);
    FORCE_INLINE void validate();
    FORCE_INLINE t_string toString();
    FORCE_INLINE void genReset();
};
template < class Tx > 
FORCE_INLINE void Box2x<Tx>::genResetLimits() {
    _p0 = VEC3_MAX;
    _p1 = VEC3_MIN;
}
template < class Tx > 
FORCE_INLINE void Box2x<Tx>::genExpandByBox(Box2x<Tx>& pc) {
    genExpandByPoint(pc._p0);
    genExpandByPoint(pc._p1);
}
template < class Tx > 
FORCE_INLINE void Box2x<Tx>::genExpandByPoint(const VecType& pt) {
    _p0 = VecType::minv(_p0, pt);
    _p1 = VecType::maxv(_p1, pt);
}
//////////////////////////////////////////////////////////////////////////
template < class Tx > 
FORCE_INLINE Box2x<Tx>::Box2x() {
}
template < class Tx > 
FORCE_INLINE Box2x<Tx>::Box2x(VecType& min, VecType& max) :_p0(min), _p1(max) {
}
template < class Tx > 
FORCE_INLINE Box2x<Tx>::Box2x(const Tx& min_x, const Tx& min_y, const Tx& max_x, const Tx& max_y) {
    construct(min_x, min_y, max_x, max_y);
}
template < class Tx > 
FORCE_INLINE void Box2x<Tx>::construct(const Tx& min_x, const Tx& min_y, const Tx& max_x, const Tx& max_y) {
    _p0.x = min_x;
    _p0.y = min_y;
    _p1.x = max_x;
    _p1.y = max_y;
}
template < class Tx >
FORCE_INLINE void Box2x<Tx>::construct(const VecType& min, const VecType& max) {
    _p0 = min;
    _p1 = max;
}
//////////////////////////////////////////////////////////////////////////
template < class Tx >
FORCE_INLINE bool Box2x<Tx>::rectIntersectEasyOut(Box2x& cc) const
{
    if (cc._min().x > _max().x)
        return false;
    if (cc._min().y > _max().y)
        return false;

    if (_min().x > cc._max().x)
        return false;
    if (_min().y > cc._max().y)
        return false;

    //  max < min
    if (cc._max().x < _min().x)
        return false;
    if (cc._max().y < _min().y)
        return false;

    if (_max().x < cc._min().x)
        return false;
    if (_max().y < cc._min().y)
        return false;

    return true;
}
template < class Tx >
FORCE_INLINE bool Box2x<Tx>::containsPointInclusive(const VecType& point) const
{
    if (point.x < _min().x)
        return false;
    if (point.y < _min().y)
        return false;
    if (point.x > _max().x)
        return false;
    if (point.y > _max().y)
        return false;
    return true;
}
template < class Tx >
FORCE_INLINE void Box2x<Tx>::scale(VecType* v1, VecType* pCenter)
{
    VecType vcenter;
    if (pCenter == NULL)
        vcenter = center();

    tl() = vcenter + (tl() - vcenter)*(*v1);
    br() = vcenter + (br() - vcenter)*(*v1);
}
template < class Tx >
FORCE_INLINE void Box2x<Tx>::genReset()
{
    //Reset genreation - set min=max to make invalid
    p0() = vec2::Vec2x_MAX();
    p1() = vec2::Vec2x_MIN();
}
template < class Tx >
FORCE_INLINE void Box2x<Tx>::expand(Box2x<Tx>& rhs)
{
    p0() = vec2::minv(p0(), rhs.p0());
    p1() = vec2::maxv(p1(), rhs.p1());
}
template < class Tx >
FORCE_INLINE void Box2x<Tx>::validate()
{

    if (p0().x > p1().x)
        Gu::debugBreak();
    if (p0().y > p1().y)
        Gu::debugBreak();
    if ((p0().x > p1().x) || (p0().y > p1().y))
    {
        // if(!Gu::getIsDevMode())
        //     BroLogError("Quad2 bound box is out of range ", toString());
    }
}
template < class Tx >
FORCE_INLINE t_string Box2x<Tx>::toString()
{
    return TStr("{", p0().toString(), "},{", p1().toString(), "}");
}


typedef Box2f MinRect;



}

#endif





















