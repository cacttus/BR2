#include "../base/BPUtil.h"
#include "../base/TypeConv.h"

namespace Game {

// +----------------------------- EXTENSION
// +----------------------------- EXTENSION
// +----------------------------- EXTENSION
// +----------------------------- EXTENSION
// +----------------------------- EXTENSION

t_ext getCCExt( t_string filename )
{
    size_t off = filename.rfind(".") + 1;

    if( off>=filename.size() || off==t_string::npos )
        throw new std::exception("Incorrect file name, no extension given or incorrect extension.");
    //t_string test_wtf = filename.substr(off,filename.size()-off);

    if(! filename.substr(off,filename.size()-off).compare("h") )
        return ext_h;
    else if(! filename.substr(off,filename.size()-off).compare("hpp") )
        return ext_hpp;
    else if(! filename.substr(off,filename.size()-off).compare("cc") )
        return ext_cc;
    else if(! filename.substr(off,filename.size()-off).compare("cpp") )
        return ext_cpp;
    else if(! filename.substr(off,filename.size()-off).compare("x3") )
        return ext_x3;


    else if(! filename.substr(off,filename.size()-off).compare("H") )
        return ext_h;
    else if(! filename.substr(off,filename.size()-off).compare("HPP") )
        return ext_hpp;
    else if(! filename.substr(off,filename.size()-off).compare("CC") )
        return ext_cc;
    else if(! filename.substr(off,filename.size()-off).compare("CPP") )
        return ext_cpp;
    else if(! filename.substr(off,filename.size()-off).compare("X3") )
        return ext_x3;


    throw new std::exception("incorrect extension.");
    return ext_cpp;
}
t_string uppercase( t_string& in ) { 
    for( size_t n=0; n<in.size(); ++n )
        in[n] = toupper(in[n]);
    return in;
}
t_string lowercase( t_string& in ) { 
    for( size_t n=0; n<in.size(); ++n )
        in[n] = tolower(in[n]);
    return in;
}
t_string getFilePart( t_string filename ) { 
    size_t off = filename.rfind(".");

    if( off>=filename.size() || off==t_string::npos )
        throw new std::exception("Incorrect file name, no extension given or incorrect extension.");

    return filename.substr(0,off);
}    

// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
// +-------------------------------- STRING
//
//int
//strToInt( t_string& s )
//{
//    return atoi( s.c_str() );
//}
//t_date            
//strToDate( const t_string& szDate )
//{
//    if(szDate.length()<10)
//        return 0;
//    t_string copy(szDate);
//    int m,d,y1,y2;
//    char buf[3];
//    buf[2]='\0';
//    buf[0] = szDate[0];
//    buf[1] = szDate[1];
//    m=strToInt( t_string((char*)buf) );
//    buf[0] = szDate[3];
//    buf[1] = szDate[4];
//    d=strToInt( t_string((char*)buf) );
//    buf[0] = szDate[6];
//    buf[1] = szDate[7];
//    y1=strToInt( t_string((char*)buf) );
//    buf[0] = szDate[8];
//    buf[1] = szDate[9];
//    y2=strToInt( t_string((char*)buf) );
//    t_date dt = (t_date)((m<<(24))|(d<<(16))|(y1<<(8))|(y2));
//
//    return dt;
//
//}
//t_date getDate()
//{
//    char buf[12];
//    _strdate_s((char*)buf,12);
//    t_string s = t_string((char*)buf);
//    s.insert(6,t_string((char*)"20"));
//    return strToDate(s);
//}
//t_string intToStr( const int i ) 
//{
//    //TEST____________________!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    //return t_string("");
//    int cpy(i);
//    char buf[16];
//    _itoa_s( cpy, (char*)buf, 16, 10 );
//
//    return t_string((char*)buf);
//}
//t_string    dateToStr( const t_date date )
//{
//    t_date d = date;
//    t_string ret;
//
//    ret+= intToStr((d>>24)&0xFF);
//    ret+= "/";
//    ret+= intToStr((d>>16)&0xFF);
//    ret+= "/";
//    ret+= intToStr((d>>8)&0xFF);
//    ret+= intToStr(d&0xFF);
//    return ret;
//}

}
