#include "../net/DBManager.h"

#include "../base/Exception.h"

#include "../net/DBMySql.h"
#include "../net/DBAdmin.h"

namespace Game {;
    DBManager* DBManager::pSingleton = 0;
/**
*
*
*/
db_code 
DBManager::initialize( Database_Type db_type )
{

    if( db_type==DB_MYSQL )
    {
        _pDB = new DBMySql();
    }
    else
    {
        throw Exception("Could not associate a database type when initializing DBManager", __LINE__, __FILE__);
    }

    return DB_OK;
}
/**
*
*/
DBAdmin*  
DBManager::getAdmin(){ 
    return _pAdmin; 
}
/**
*
*/
DBAdmin*  
DBManager::createAdmin( t_string& username, t_string& pass )
{
    
    DBAdmin* admin = new DBAdmin(username,pass);
    vecAdmins.push_back(admin);
    
    return admin;
}
/**
*
*/
db_code  
DBManager::setAdmin( DBAdmin* admin )
{
    AssertOrThrow2(admin!=NULL);
    _pAdmin = admin;
    
    return DB_OK;
}
/**
*
*/
db_code 
DBManager::setHost( t_string& host)
{
    strHost=host;
    return DB_OK;
}
/**
*
*/
db_code 
DBManager::setDBName( t_string& name )
{
    strDBName=name;
    return DB_OK;
}
/**
*
*/
Database* 
DBManager::getDatabase()
{
    return _pDB;
}


/********CTOR
***/
DBManager::DBManager() : _pDB(0), _pAdmin(0)
{}
DBManager::~DBManager()
{
    if(_pDB)
        delete _pDB;

    for( t_memsize i=0; i<vecAdmins.size(); ++i )
        delete vecAdmins[i];

}


}//ns game
