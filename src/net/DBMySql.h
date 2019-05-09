/**
*
*    @file DBMySql.h
*    @date December 14, 2010
*    @author Derek Page
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __DBMYSQL_122412388913225130024829_H__
#define __DBMYSQL_122412388913225130024829_H__

#include "DBHeader.h"
#include "Database.h"

namespace Game {;

class DBMySql : public Database { 
public:

    db_code connect( DBAdmin* admin, char* dbName = 0, char* hostIP = 0 );
    db_code query( t_string& q );
    db_code disconnect();
    DBMySql();
    virtual ~DBMySql() override ;

};




}//ns game



#endif
