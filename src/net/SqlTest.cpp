//#include "../net/SqlTest.h"
//
//
//namespace Game {;
//
//void SqlTest::sqlInit()
//{
//    try
//    {
//        conn = new mysqlpp::Connection( "TestDatabase", "10.0.1.38", "root", "*D09GamedbEng*32^" );
//        lastMySqlInteraction = GetTickCount();
//    }
//    catch(mysqlpp::Exception e)
//    {
//        std::cout<<__LINE__<<" EXCEPTION! : \n"<<e.what()<<std::endl;
//    }
//}
//bool SqlTest::userExists( t_string username )
//{
//    try {
//        t_string str=t_string("select username from TestTable where username=\"") + username + t_string("\"");
//        mysqlpp::Query query = conn->query(str.c_str());
//        mysqlpp::StoreQueryResult res = query.store();
//        lastMySqlInteraction = GetTickCount();
//        if( res.size() )
//            return true;
//        return false;
//    }
//    catch(mysqlpp::Exception e) {
//        std::cout<<__LINE__<<" EXCEPTION! : \n"<<e.what()<<std::endl;
//    }    
//}
//void SqlTest::sqlUpdateUser( t_string username, t_string text) 
//{
//    try
//    {
//        t_string str;
//        if( !userExists( username ) )
//        {
//            str = t_string("INSERT INTO TestTable (username, userdata) VALUES (\"") + username + t_string("\",\"") + text+ t_string("\")");                
//        }
//        else
//        {
//            str = t_string("UPDATE TestTable SET userdata=\"") + text + t_string("\" WHERE username=\"") + username + t_string("\"");                
//        }
//        mysqlpp::Query query = conn->query(str.c_str());
//        mysqlpp::StoreQueryResult res = query.store();
//        std::cout<<res.num_rows()<<" rows affected."<<std::endl;
//        lastMySqlInteraction = GetTickCount();
//    }
//    catch(mysqlpp::Exception e)
//    {
//        std::cout<<__LINE__<<" EXCEPTION! : \n"<<e.what()<<std::endl;
//    }
//}
//bool SqlTest::sqlRead( t_string username, __out_ t_string& text ) { 
//    try
//    {    
//        t_string str=t_string("select userdata from TestTable where username=\"") + username + t_string("\"");
//        mysqlpp::Query query = conn->query(str.c_str());
//        mysqlpp::StoreQueryResult res = query.store();
//        lastMySqlInteraction = GetTickCount();
//        if( !res.size() )
//            return false;    // - User not found.
//
//        if( res.num_rows() > 1 )
//            return false;    // - Redundancy, ambiguous so, we can't return correctly.
//
//        text = res[0][0];
//        return true;
//    }
//    catch(mysqlpp::Exception e)
//    {
//        std::cout<<__LINE__<<" EXCEPTION! : \n"<<e.what()<<std::endl;
//    }
//}
//SqlTest::SQLTest() : conn(NULL) {
//    
//}
//SqlTest::~SQLTest()
//{
//    DEL_MEM(conn);
//}
//
//
//}//ns game
