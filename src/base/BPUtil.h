/**
*
*    @file BPUtil.h
*    @date July 20, 2011
*    @author Derek Page
*
*    © 2011 
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __BPUTIL_9680225714549155925593_H__
#define __BPUTIL_9680225714549155925593_H__



#include "../base/BaseHeader.h"

namespace Game {;

typedef enum { 
    ext_h,
    ext_cc,
    ext_hpp,
    ext_cpp,
    ext_x3,
}t_ext;

//--------------------------------------------------------------
// - FUNC PROTOTYPES
//replaced by filesystem getExtension
t_ext            getCCExt( t_string filename ); //get c/c++ ext
//t_string        uppercase( t_string& in );
//t_string        lowercase( t_string& in );
t_string        getFilePart( t_string filename );
// +------------- DATE -------------+
/*t_string        strdate();
t_date            getDate();
t_string        dateToStr( const t_date date );
t_string intToStr( const int i );*/

// allows you to show line number when std::exception is thrown
//t_string throw_line( char* msg, int line );

}
#endif
