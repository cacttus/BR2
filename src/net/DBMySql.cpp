#include "DBMySql.h"
#include "DBAdmin.h"

namespace Game {;

/**
*
*/
db_code 
DBMySql::connect( DBAdmin* admin, char* dbName, char* hostIP )
{
    //char *name, *host;

    //disconnect();
    //
    //if(!dbName)
    //    name = DB_DEFAULT_NAME;
    //else
    //    name=dbName;
    //if(!hostIP)
    //    host = DB_DEFAULT_HOST;
    //else
    //    host=hostIP;
    //_pConnection = new mysqlpp::Connection( name, host, admin->getUser().c_str(), admin->getPassword().c_str() );
    //
    //setState(DBS_CONNECTED);
    return DB_OK;
}
/**
*
*/
db_code 
DBMySql::query( t_string& q )
{
    //if(getState()!=DBS_CONNECTED){
    //    return DB_NOT_CONNECTED;
    //}
    //mysqlpp::Query cquery = _pConnection->query(q.c_str());
    //try 
    //{
    //    
    //    _result = cquery.store();
    //}
    //catch(mysqlpp::Exception e ) 
    //{ 
    //    AssertOrThrow2(0);
    //}

    //
    return DB_OK;
}
/**
*
*/
db_code 
DBMySql::disconnect()
{
    //if(!_pConnection)
    //    return DB_OK;
    //delete _pConnection;
    //_pConnection=0;
    //setState(DBS_DISCONNECTED);
    return DB_OK;
}

DBMySql::DBMySql() {// : _pConnection(0) {
}

DBMySql::~DBMySql(){   
    disconnect();
}




}//ns game
