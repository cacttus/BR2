/**
*
*    @file Database.h
*    @date December 14, 2010
*    @author Derek Page
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __DATABASE_156182938936262355720499_H__
#define __DATABASE_156182938936262355720499_H__

#include "../net/DBHeader.h"

namespace Game {;

/**
*
*   @brief A Database base class.
*/
class Database : public VirtualMemory { 
    friend class DBManager;
    DBState _state;
protected:
    void setState(DBState s){ _state=s; }
public:
    DBState getState(){ return _state; }
    
    virtual db_code connect( DBAdmin* admin, char* dbName = 0, char* hostIP = 0 )=0;
    virtual db_code disconnect()=0;
    virtual db_code query( t_string& q )=0;

    Database() : _state(DBS_UNINITIALIZED) {}
};




}//ns game



#endif
