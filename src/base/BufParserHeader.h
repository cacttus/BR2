/**
*
*    @file BufParserHeader.h
*    @date July 20, 2011
(6/7/2011)
*    @author Derek Page
*
*    © 2011 
*
*    © 2011 
*
*
*/
#pragma once
#ifndef __BUFPARSERHEADER_5050184502818530126822_H__
#define __BUFPARSERHEADER_5050184502818530126822_H__


#include "BPUtil.h"

namespace Game {;

typedef enum {
    ptr_begin, // beginning of file
    ptr_seek, // ptr is in the file
    ptr_eof // ptr is end of file
}t_ptr_state;

typedef enum {
    state_normal,
    //state_beginend,        // - The tokenizer is parsing between <begin><end>
}t_file_state;
// - The state of the parser like inside function, etc,
typedef enum {
    ps_token,// - currently doing a token
    ps_begin // - i.e. not currently parsing a function/structure body.
}t_parse_state;

//This typedef will specify what state of the function we were JUST IN
// not currently parsing. i.e. we parse the params while in the state fp_name.
// once a param was parsed, the engine sets the state to fp_params
typedef enum {
    fp_none,
    fp_inline, // inline has been parsed
    fp_extern,
    fp_rt_const, // return type const has been parsed
    fp_body_const,
    fp_rtype,    //return tyep has been parsed
    fp_name, //function name
    fp_params, //params
    fp_body
}t_func_parse_state;

typedef enum {
    dt_notype,        // - no type defined
    dt_m_struct,    // - an M.. class
    dt_struct,        // - a static structure class defined in C# and C++ such as Vec3, Mat33.. 
    dt_integral,    // - integral, such as float, int..
    //dt_special,        // - Indicates to the outputter that a special conversion is to take place for this type.
    dt_string,        // a t_string
    //dt_array,        // array which is either VectorEx or ..
    //dt_charptr,    // a c++ char*
}t_datatype;

//t_string outp;

/**
*    @struct func_type
*    @brief A function type.
*    
*/
struct func_type {
    t_datatype dt;    // - type of data (see above)
    bool is_array;// - True if this type is a VectorEx (array)
    bool is_array_ptr;    // - if this is also a pointer to an array: vector<..>*
    int array_dims;    // - The dimension of the array, if this type is an array.
    t_string str;    // - The string rep for this type (the actual code value)
    bool b_const;        // - If this type is const
    void clear(){
        array_dims=0;
        b_const=false;
        str.clear();
        is_array=false;
        is_array_ptr=false;
        dt=dt_integral;
    }
};

struct Token : public t_string {};
struct proc_def;
// -------------------------------
struct func_def_in { 
    bool b_inline;    // if the func is inline (don't need)
    //bool b_ret_const;
    bool b_body_const;
    t_string    _proc_def;
    t_string name;    // name of the func
    std::vector<func_type> parms;    // parameters
    t_string body;    // body     (don't need)
    //t_string rtype;
    func_type rtype;

    void reset(){
        b_inline=b_body_const=false;
        name.clear();
        parms.clear();
        body.clear();
        rtype.clear();
    }
};
struct func_def_out {
    t_string name;
    t_string rtype;
    std::vector<func_type> parms;
    func_def_in infunc;
    void reset(){
        name.clear();
        parms.clear();
        rtype.clear();
    }
};

// --------------------------------------
// a datatype with a string rep.
struct CType {
    t_string name;
    t_datatype t;
    CType( t_string n, t_datatype ot){ name=n; t=ot; }
};


// --------------------------------------
// definition of a proc?
struct proc_def{
    t_string proc_def_name;
    func_def_in in_func;
};


}
#endif
