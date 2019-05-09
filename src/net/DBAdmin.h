/**
*
*    @file DBAdmin.h
*    @date December 14, 2010
*    @author Derek Page
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __DBADMIN_129951640311080106828200_H__
#define __DBADMIN_129951640311080106828200_H__

#include "../net/DBHeader.h"

namespace Game {;

/**
*
*
*/
class DBAdmin : public VirtualMemory { 
    friend class DBManager;
    
    t_string _username;    // U/n
    t_string _password;    // P
    
    DBAdmin( t_string& user, t_string& pass );// created by DBManager
    virtual ~DBAdmin() override ;//destroyed by the DBManager
public:

    t_string getUser(){ return _username; }
    t_string getPassword(){ return _password; }
    
};




}//ns game



#endif
