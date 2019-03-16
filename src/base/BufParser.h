/**
*
*    @file BufParser.h
*    @date July 20, 2011
*    @author Derek Page
*
*    © 2011 
*
*/
#pragma once
#ifndef __BUFPARSER_235220395253811918026664_H__
#define __BUFPARSER_235220395253811918026664_H__

#include "../base/BaseHeader.h"
#include "../base/BufParserHeader.h"
namespace Game {;

class CodeWriter;

class BufParser : public VirtualMemory { 
public:
    bool b_eof;
    t_ptr_state ptr_state;
    int line_num;

    std::vector<char>* _data;

    t_parse_state pstate;    
    t_func_parse_state func_ps;
    
    //TypeTable type_table;
    
    //t_string _filename;

    std::vector<proc_def> proc_defs;

    int template_level; // - the nested <<>> level while parsing a templated parameter or return type.

    bool _bSelfAllocated;
public:

    Token tok_buf;    // The current processed token
    func_def_in cur_func;

    //std::vector<Token> tokens;
    //
    std::vector<func_def_in> in_funcs;
    std::vector<func_def_out> out_funcs;

    char* ptr;

    t_file_state file_state;

    // - Ptr / Character
    bool charIsValidSentinelForName_C();
    bool charIsValidSentinelForName_C( char c );
    bool charIsValidForName_C();
    bool charIsValidForName_C(char c);
    bool charIsWs();
    bool charIsWs(char c);
    bool charIsEscape();
    bool charIsEscape(char c);

    // - PTR OPERATOR
    char charAt();
    char nextChar();
    char prevChar();
    //seeks one past the character
    bool seekPastChar( char c );
    void seekPastChars(std::vector<char> cv );
    void seekToChars( std::vector<char> cv );
    // - Increment / decrement pointer
    bool inc();
    void dec();
    bool eof(); // - Return if we are at end of file.
    void eatws();// - EAT
    void eatBlockComment();
    void eatLine();
    void eatBody();// - Eat the body of a statement/function from { to }
    void clearCCTokens();// - TOKENS
    bool is_cpp_keyword(Token t);// - Determines if a token is a keyword.
    void endCCFunc();// - Adds function to function list.
    void endCCToken();// - Ends a token
    void getCCToken();// - Retrieves an identifier, or an operator.
    
    // ----------MATCH
    //bool isIdentifierCC( Token t );
    //bool processCC();    // - Processes the file.
    //OutFile* parseOutCC(CodeWriter*); // - Creates the file
    //void write_codeCC ( OutFile* of, CodeWriter* pcw );    // - OUTPUT

    
    void getToken(char* cptr, std::vector<char> delims, bool ignoreWs=true );

    // --- VALIDATION
    ///bool check_array( t_string, int&, t_string&, bool& ); // - Return true if the input is an array.
    //bool validate_return_typeCC(func_type );
    bool is_ptr(const t_string s);

    // --- OPERATIONS
    //t_string strip_pointers( const t_string type );
    //int count_pointers( const t_string type );
    void tabAll( t_string& s, bool expandToSpaces=false, int nSpaces=4 );

    BufParser(t_string& data); // Locally allocates data.
    BufParser( const BufParser& rhs );
    BufParser( char* filePtr_in, t_string filename );
    BufParser();
    virtual ~BufParser() override ;

    FORCE_INLINE void BufParser::eatLine(int32_t& lineCountToAddTo);

};


FORCE_INLINE void BufParser::eatLine(int32_t& lineCountToAddTo)
{
    while(1)
    {
        if(charAt()=='\n')
        {
            if(inc()) 
            {
                //EOF
                return;
            }
            lineCountToAddTo++;
            break;
        }
        else if(inc())
        {
            //EOF
            return;
        }
    }
}


}

#endif
