/**
*
*    @file DBManager.h
*    @date December 14, 2010
*    @author Derek Page
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __DBMANAGER_1545416257279213929058_H__
#define __DBMANAGER_1545416257279213929058_H__


#include "../net/DBHeader.h"
#include "../net/Database.h"
namespace Game {;

/**
*   @class DBManager
*   @brief Manager for the Database
*
*   DBManager* dbm = DBManager::getSingleton();
*
*   dbm->setAdmin( dbm->createAdmin( un,pass ) )
*
*   dbm->getDatabase()->connect(..)
*   dbm->getDatabase()->query(..)
*/
class DBManager : public VirtualMemory { 
   VectorEx<DBAdmin*> vecAdmins;
    
    static DBManager* pSingleton;

    Database* _pDB;//Virtual for other types of DB.
    
    DBAdmin*  _pAdmin;// - The Administrator
    
    t_string strHost;   // host name (IP address)
    t_string strDBName; // Database name

public:

    db_code     initialize( Database_Type db_type );
    
    DBAdmin*    createAdmin( t_string& username, t_string& pass );
    db_code     setAdmin( DBAdmin* admin );
    DBAdmin*    getAdmin();
    
    db_code setHost( t_string& host);
    db_code setDBName( t_string& name );
    
    Database* getDatabase();
    
    static DBManager* getSingleton()
    {
        if(!pSingleton)pSingleton = new DBManager();
        return pSingleton;
    }
    
    DBManager();
    virtual ~DBManager() override ;
    
};



}//ns game



#endif
