/**
*
*    @file NetworkUtils.h
*    @date June 20, 2015
*    @author Derek Page
*
*    © 2015 
*
*
*/
#pragma once
#ifndef __NETWORKUTILS_221962548231829252804361_H__
#define __NETWORKUTILS_221962548231829252804361_H__

#include "../net/NetHeader.h"

namespace Game {;
//class NetSerializer {
//
//};
/**
*    @class NetworkUtils
*    @brief
*
*/
class NetworkUtils : public VirtualMemory {
private:
    template < typename Tx >
    STATIC t_bool unpackTx(t_string& __inout_ str, Tx& __out_ ret)
    {
        t_uint64 tsize;
 
        tsize = sizeof(Tx);
    
        if(str.length()<tsize)
        {
#ifdef _DEBUG
            BroThrowException("String Was too small: ",str.length()," for parse value.",tsize);
#endif
            return false;
        }
    
        ret = *((Tx*)str.c_str());

        str = str.substr(tsize,str.length()-tsize);
        return true;
    }
    template < typename Tx >
    STATIC t_string packTx(const Tx& __in_ value)
    {
        t_string strRet;
        strRet = t_string((char*)&value, sizeof(Tx));
        return strRet;
    }
//    static void addPackTypeToMap(char* packStr, NetPackType packType);
public:
    //////////////////////////////////////////////////////////////////////////
    //Data conversion
    // ** the input packet string data is eaten as conversion takes place
    STATIC t_bool unpackUint32(t_string& __inout_ str, t_uint32& __out_ ret){ return unpackTx<t_uint32>(str, ret); }
    STATIC t_bool unpackInt32 (t_string& __inout_ str, t_int32&  __out_ ret){ return unpackTx<t_int32 >(str, ret); }
    STATIC t_bool unpackUint64(t_string& __inout_ str, t_uint64& __out_ ret){ return unpackTx<t_uint64>(str, ret); }
    STATIC t_bool unpackString(t_string& __inout_ str, t_string& __out_ ret){
        t_uint64 size;
        if(!unpackUint64(str, size))
            return false;

        ret = str.substr(0, size);
        str = str.substr(size, str.length()-size);

        return true;
    }
    STATIC t_string packUint64(const t_uint64& __in_ val){ return packTx<t_uint64>(val); }
    STATIC t_string packUint32(const t_uint32& __in_ val){ return packTx<t_uint32>(val); }
    STATIC t_string packString(const t_string& __in_ val){ 
        t_string strPacked;
        strPacked += packUint64(val.length());
        strPacked += val;
        return strPacked;
    }

    //////////////////////////////////////////////////////////////////////////
    //Old conversion methods
    STATIC t_string GetSockErrStr( RetCode ret );
    STATIC t_string GetSockErrName( RetCode ret );
    STATIC t_string GetSockStateName( SocketState s );
    STATIC t_string GetSockStateDesc( SocketState s );
    STATIC t_string getIPFromHost( t_string hostName );    // - Retrieves an IP address in string form from a host name in string form, similar to a DNS Lookup.
    FORCE_INLINE STATIC t_int32 stoms( int s ) {return s*1000;}
    FORCE_INLINE STATIC t_int32 mtos( int m ) {return m*60;}
    

    //static NetPackType getPackType(t_uint64 iPackId);
    //static t_uint64 getPackId(NetPackType ePackType);



    //static MapEx<NetPackType, t_uint64> _mapPacketType;
    //static MapEx<t_uint64, NetPackType> _mapPacketHeaderId;

    //static void initializeGlobals();





    NetworkUtils(){}
    virtual ~NetworkUtils() override{}
};

}//ns game



#endif
