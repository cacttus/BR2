/**
*  @file TypeCast.h
*  @date April 8, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __TYPECAST_14601720251282440992_H__
#define __TYPECAST_14601720251282440992_H__

//#include "../base/BaseHeader.h"

namespace BR2 {


template < typename Tx, typename Ty >
inline Tx bro_safe_cast(Ty pb)
{
    //Note:
    // If this is giving compile errors make sure you
    // included both the .h definition files of the casted
    // object types.
    Tx ret = dynamic_cast<Tx>(pb);
    if (pb != nullptr && ret == nullptr) {
        throw 0;
    }
    //    AssertOrThrow2(ret!=NULL);
    return ret;
}
#define BroSafeCast bro_safe_cast
#define DirectCast(pObj, pType) dynamic_cast<pType>(pObj)
#define StaticCast(pObj, pType) static_cast<pType>(pObj)
#define SafeCast(pObj, pType) BroSafeCast<pType>(pObj)
#define UnsafeCast(pObj, pType) reinterpret_cast<pType>(pObj)


}//ns BR2



#endif
