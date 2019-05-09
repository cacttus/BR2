/**
*
*    @file DBHeader.h
*    @date December 14, 2010
*    @author Derek Page
*
*    © 2011 
*   @desc Database Header
*
*/
#pragma once
#ifndef __DBHEADER_126332978270051701630162_H__
#define __DBHEADER_126332978270051701630162_H__

#include "../base/OperatingSystemHeader.h"
//#include "../dev/mysqlpp/mysql++.h"
#include "../base/ExLib.h"
//#include "serverdll_header.h"
namespace Game {;

#define DB_DEFAULT_HOST "127.0.0.1"
#define DB_DEFAULT_NAME "database"

typedef enum {
    DB_FAIL,
    DB_OK,
    DB_NOT_CONNECTED, //the database must be connected before querying.
}db_code;

typedef enum { 
    DB_MYSQL,
    DB_ORACLE
}Database_Type;

typedef enum { 
    DBS_UNINITIALIZED,
    DBS_CONNECTED,
    DBS_DISCONNECTED
}DBState;

class Database;
class DBAdmin;
class DBMySql;
class DBManager;



}//ns game



#endif
