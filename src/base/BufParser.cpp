#include "../base/BufParser.h"
#include "../base/Exception.h"

namespace Game {;


//// - Specify a codewriter class.
//void BufParser::write_codeCC ( OutFile* of, CodeWriter* pcw )
//{
//    pcw->write(of);
//}
//
// tab all lines of a string
//
void BufParser::tabAll( t_string& s, bool expandToSpaces, int nSpaces )
{
    t_string strtab;
    
    if(expandToSpaces){
        for( int i=0; i<nSpaces; ++i ){
            strtab+= ' ';
        }
    }
    else
        strtab="\t";

    s = strtab + s;

    for( size_t i=0; i<s.size(); ++i ){
        if(s[i]=='\n'){
                s.insert((int)i+1,strtab);
        }
    }
}
// - Ptr / Character
bool BufParser::charIsValidSentinelForName_C(){
    return ( (isalpha(charAt())) || (charAt()=='_') );
}
bool BufParser::charIsValidSentinelForName_C( char c ){
    return ( (isalpha(c)) || (c=='_') );
}
bool BufParser::charIsValidForName_C(){
    return ( (isalnum(charAt())) || (charAt()=='_') );
}
bool BufParser::charIsValidForName_C(char c){
    return ( (isalnum(c)) || (c=='_') );
}
bool BufParser::charIsWs(){ return ( (charAt()==' ') || (charAt()=='\t') || (charAt()=='\r') || (charAt()=='\n') ); }
bool BufParser::charIsWs(char c){ return ( (c==' ') || (c=='\t') || (c=='\r') || (c=='\n') ); }
bool BufParser::charIsEscape(){ return ( (charAt()=='\t') || (charAt()=='\r') || (charAt()=='\n') ); }
bool BufParser::charIsEscape(char c){ return ( (c=='\t') || (c=='\r') || (c=='\n') ); }

// - PTR OPERATOR
char BufParser::charAt(){ return *(ptr); }
char BufParser::nextChar(){ 
    if( ptr_state==ptr_eof)
        throw new std::exception("Tried to get next Character from end of file.(nextChar)");
    return *(ptr+1);
}
char BufParser::prevChar(){ 
    if( ptr_state==ptr_begin)
        throw new Exception("Tried to get Previous character from beginning of file.",__LINE__,__FILE__);
    return *(ptr-1) ;
}
//seeks one past the character
bool BufParser::seekPastChar( char c ){
    while(charAt()!=c){
        if( ptr_state==ptr_eof )
            throw new Exception("Tried to get next Character from end of file. (seekPastChar)",__LINE__,__FILE__);
        inc();
    }
    if( ptr_state==ptr_eof )
        return true;
    inc();
    return false;
}
/**
*    @fn seekToChars()
*    @brief Seeks to any of hte input chars and increments the pointer past it
*/
void BufParser::seekPastChars( std::vector<char> cv ){
    seekToChars(cv);
    inc();
}
/**
*    @fn seekToChars()
*    @brief Seek pointer until one of the specified characters is reached.
*/
void BufParser::seekToChars( std::vector<char> cv ){
    bool exit=false;
    while(1)
    {
        for( unsigned int i=0; i<cv.size(); ++i)
        {
            if(charAt()==cv[i])
            {
                exit=true;
                break;
            }
        }

        if(exit)break;

        if( ptr_state==ptr_eof )
            throw new Exception("Tried to get next Character from end of file. (seekToChar)",__LINE__,__FILE__);
        inc();
    }
}
/**
*    inc()
*    increment pointer
*/
bool BufParser::inc(){ 
    if( b_eof ) 
        return eof(); //throw new Exception("Tried to parse file past EOF.",__LINE__,__FILE__); 
    ptr++; 
    
    if(charAt()=='\r'&&nextChar()=='\n'){ 
        line_num++;
    }

    ptr_state = ptr_seek;
    return eof(); 
}
/**
*    dec()
*    decrement pointer
*/
void BufParser::dec(){ 
    if(ptr_state==ptr_begin)
        throw new Exception("Tried to decrement the pointer at the beginning of the file.",__LINE__,__FILE__); 
    
    if(charAt()=='\r'&&nextChar()=='\n') {
        line_num--;

    }
    if( b_eof ) 
    {
        b_eof=false;
    }
    ptr--; 


    ptr_state = ptr_seek;
}
/**
*    eof()
*    return true if the poitner is at eof
*/
bool BufParser::eof(){ 
    if(b_eof) 
        return true;
    if( b_eof=(*(ptr)==0) )
        ptr_state = ptr_eof;
    return b_eof;
}

// - EAT
// - EAT
// - EAT
// - EAT
// - EAT
// - EAT
// - EAT
/**
*    eatws()
*    eats white space
*/
void BufParser::eatws(){ while(charIsWs()) inc(); }
/**
*    eatBlockComment()
*    eats a /* comment.
*/
void BufParser::eatBlockComment(){
    if(charAt()=='/' && nextChar()=='*'){
        while(1){
            if((charAt()=='*')&&(nextChar()=='/')){
                if(inc()) return;
                if(inc()) return;
                break;
            }
            else if(inc()) return;
        }
    }
    else if( nextChar()==0 )
        throw new Exception(" [Parser] Unmatched Comment Encountered at line ",__LINE__,__FILE__);
        
}
/**
*    @fn eatLine()
*    @breif Eats a line until \n
*/
void BufParser::eatLine()
{
    while(1)
    {
        if(charAt()=='\n')
        {
            if(inc()) return;
            break;
        }
        else if(inc()) return;
    }
}

/**
*    @fn eatBody
*    @brief Eat the body of a statement/function
*/
void BufParser::eatBody()
{
    if(charAt()=='{')
    {
        while(1)
        {
            if(charAt()=='}')
            {
                inc();
                break;
            }
            else
                inc();
        }
    }
    else if( nextChar()==0 )
        throw new Exception( "Unmatched Comment Encountered at line ",__LINE__,__FILE__ );
}
// ----------TOKENS
// ----------TOKENS
// ----------TOKENS
// ----------TOKENS
// ----------TOKENS
// ----------TOKENS
// ----------TOKENS
//void BufParser::clearCCTokens() 
//{
//    tokens.clear();
//}
bool BufParser::is_cpp_keyword(Token t) 
{
    if(
        (!t.compare("inline"))||
        (!t.compare("if"))||
        (!t.compare("else"))||
        (!t.compare("virtual"))||
        (!t.compare("volatile"))||
        (!t.compare("typedef"))||
        (!t.compare("enum"))||
        (!t.compare("class"))||
        (!t.compare("struct"))||
        (!t.compare("union"))||
        (!t.compare("public"))||
        (!t.compare("private"))||
        (!t.compare("bool"))||
        (!t.compare("short"))||
        (!t.compare("int"))||
        (!t.compare("long"))||
        (!t.compare("float"))||
        (!t.compare("double"))||
        (!t.compare("register"))||
        (!t.compare("auto"))||
        (!t.compare("static"))||
        (!t.compare("register"))||
        (!t.compare("extern"))||
        (!t.compare("const"))||
        (!t.compare("namespace"))||
        (!t.compare("try"))||
        (!t.compare("catch"))||
        (!t.compare("throw"))||
        (!t.compare("finally"))||
        (!t.compare("#if"))||
        (!t.compare("#ifdef"))||
        (!t.compare("defined"))||
        (!t.compare("#pragma"))||
        (!t.compare("#else"))||
        (!t.compare("#endif"))||
        (!t.compare("finally"))||
        (!t.compare("finally"))||
        (!t.compare("finally"))||
        (!t.compare("finally"))||
        (!t.compare("finally"))||
        (!t.compare("finally"))
        )
        return true;
    return false;
}
/**
*    Only applicable to function parser
*/
void BufParser::endCCFunc()
{
    this->in_funcs.push_back(this->cur_func);
    this->cur_func.reset();
}
/**
*    Only applicable to function parser
*/
void BufParser::endCCToken(){
    // - Should check to see what the token is

    //for the token....
    Token t = tok_buf;

    if( t=="inline" ){
        if(func_ps==fp_none){
            cur_func.b_inline=true;
            func_ps=fp_inline;
        }
        else throw new Exception( "Unexpected inline in function", __LINE__,__FILE__
        );
    }

    else if( t=="const" ){
        if(func_ps==fp_inline){
            func_ps=fp_rt_const;
            cur_func.rtype.b_const=true;
        }
        else if(func_ps==fp_params){
            func_ps=fp_body_const;
            cur_func.b_body_const=true;
        }
        else throw new Exception("Unexpected const", __LINE__,__FILE__
            );
    }
    else if( t=="\"C\"" ){
        if(func_ps==fp_inline){
            eatLine();
            dec();
            func_ps=fp_none;
        }
        else throw new Exception( "Unexpected C declaration", __LINE__,__FILE__
            );
    }
    else if( t=="NOX_API" ){
        // do nothing
        //WARNING:TODO: hard coded the dll export symbol.
    }
    else if( func_ps==fp_none ){
        if(t=="using"){
            eatLine();
            dec();// after we see whitespace we end the token and automatically inc() so we need to dec() here.
        }
        else if(t=="namespace"){
            eatLine();
            dec();
        }
        else if(t=="extern"){
            func_ps=fp_inline;
            // - it is either exter "C" or an extern function
            //eatLine();
            //dec();
        }
        else {
            func_ps=fp_rtype;
            cur_func.rtype.str=t;
        }
    }
    else if( func_ps==fp_inline ){
        func_ps=fp_rtype;
        cur_func.rtype.str=t;
    }
    else if( func_ps==fp_rt_const ){
        func_ps=fp_rtype;
        cur_func.rtype.str=t;
    }
    else if(func_ps==fp_rtype){
        func_ps=fp_name;
        cur_func.name=t;
    }
    else if(func_ps==fp_name){
        func_ps=fp_params;
        func_type p;
        p.clear();
        p.str = t;
        cur_func.parms.push_back(p);
    }
    else if(func_ps==fp_params){
        //func_ps=fp_body;
        func_type p;
        p.clear();
        p.str = t;
        cur_func.parms.push_back(p);
    }
    else throw new Exception( "Unmatched function state", __LINE__,__FILE__ );

    pstate=ps_begin;
    tok_buf.clear();
}

/**
*    Only applicable to function parser
*/
void BufParser::getCCToken(){
    while(!eof()) {
        //for the current char...
        char c = charAt();

        if( isalpha(c) ) {
            if(pstate==ps_token)
                tok_buf+=c;
            else if(pstate==ps_begin){
                tok_buf+=c;
                pstate=ps_token;
            }
            inc();
        }
        else if(c=='_') {
            if(pstate==ps_token)
                tok_buf+=c;
            else if(pstate==ps_begin){
                tok_buf+=c;
                pstate=ps_token;
            }
            inc();
        }
        else if(c=='*') {
            if(pstate==ps_token)
            {
                tok_buf+=c;
                inc();
            }
            else if(pstate==ps_begin) {
                if(func_ps==fp_rtype)
                {
                    // - The function has white space between the return type
                    // and the pointer asterisk (which  is valid) .
                    //if(charIsWs(nextChar()))
                        cur_func.rtype.str+='*';
                        inc();
                        //else
                        //throw std::exception(
                        //throw_line( "illegal '*' in function name", __LINE__,__FILE__
                        //);
                }
                else if(func_ps==fp_params)
                {
                    // - The function has white space between the parameter name
                    // and the pointer asterisk (which  is valid).
                    //if(charIsWs(nextChar())||(nextChar()==',')||(nextChar()==')'))
                        cur_func.parms[cur_func.parms.size()-1].str+='*';
                        inc();

                        //seek to the next bit of info
                        std::vector<char> v;
                        v.push_back(',');
                        v.push_back('*');
                        v.push_back(')');
                        seekToChars(v);

                    //else
                    //    throw std::exception(
                    //    throw_line( "illegal '*' in parameter name", __LINE__,__FILE__
                    //    );
                }
                else{
                    tok_buf+=c;
                    pstate=ps_token;
                    inc();
                }
            }
        }
        else if(isdigit(c)) {
            if(pstate==ps_token)
                tok_buf+=c;
            else throw new Exception( "number encountered as sentinel for function definition", __LINE__,__FILE__
                );
            inc();
        }
        else if(charIsWs()) 
        {
            // - We cannot end a token inside of a template, instead we'll parse
            // the entire template as a type
            if(pstate==ps_token && template_level==0) 
            {
                //whitespace doesn't end params
                endCCToken();
                if(func_ps==fp_params) 
                {
                    std::vector<char> v;
                    v.push_back(',');
                    v.push_back('*');
                    v.push_back(')');
                    seekToChars(v);
                }
                else
                    inc();
            }
            else inc();

            //without token just eat the ws.
        }
        else if(c=='/') {
            if(nextChar()=='*')
                eatBlockComment();
            else if(nextChar()=='/')
                eatLine();
            else throw new Exception( "Unrecognized character '/' ", __LINE__,__FILE__
                );
        }
        else if(c==','){
            if(func_ps==fp_params){
                // we just parsed a param
                if(pstate==ps_token&&template_level==0){
                    endCCToken();
                }
                //else ignore
            }
            else if(func_ps==fp_name){
                // - we just parsed a param that had no identifier and ended with a ,
                if(pstate==ps_token&&template_level==0){
                    endCCToken();
                }
                //else ignore
            }
            else throw new Exception( "Unexpected comma. ", __LINE__,__FILE__
                );
            inc();
        }
        else if(c=='('){
            if(pstate==ps_token) 
                //**WARNING: might be a 
                //templated function (let's assume we don't use tmplated functions for C exports)
                endCCToken();
            inc();
        }
        else if(c==')'){
            if(pstate==ps_token&&template_level==0)
                endCCToken();

            // We have the function, and we need to eat the body, but
            // it could be a prototype.

            // seek to either a ; (for proto) or { (for function)
            std::vector<char> v;
            v.push_back(';');
            v.push_back('{');
            seekToChars( v );
            if(eof())
                throw new Exception( "Unexpected end of file.", __LINE__,__FILE__
                    );

            // - Eat everything until we are out of the function.
            if(charAt()==';')
                eatLine();
            else if(charAt()=='{')
                seekPastChar('}');
            else
            throw new Exception( "Invalid body for function. ", __LINE__,__FILE__
            );

            // - Add the function definition to the list.
            endCCFunc();
            if(eof())
                break;
            func_ps = fp_none;
            pstate = ps_begin;
        }
        else if(c=='}'){
            // clearing for now since we might be inside of an
            // 'extern "C" {' block.
            pstate=ps_begin;
            tok_buf.clear();
            inc();
        }
        else if(c=='#'){
            //# directives
            eatLine();
        }
        else if(c=='"'){
            tok_buf+=c;
            inc();
        }
        else if(c=='<'){
            if(pstate==ps_token)
            {
                template_level++;
                // Templated class
                tok_buf+=c;
                inc();
            }
        }
        else if(c=='>'){
            if(pstate==ps_token)
            {
                template_level--;
                // Templated class
                tok_buf+=c;
                inc();
            }
        }
        else{
            inc();
        }


        /*
        else if \
            throw exception "escape character in file"
        */
    }

}//endToken()



/**
*    Only applicable to function parser
*/
//bool BufParser::isIdentifierCC( Token t )
//{
//    bool ret;
//
//    if( !charIsValidSentinelForName_C() )
//        return false;
//
//    for( unsigned int i=0; i<t.length(); ++i )
//    {
//        if(!charIsValidForName_C())
//            return false;
//    }
//
//    return ret;
//}
///**
//*    @fn parseOutCC
//*    @brief Only applicable to function parser.
//*/
//OutFile* BufParser::parseOutCC(CodeWriter* pcw)
//{
//    OutFile* of = new OutFile();
//
//    pstate=ps_begin;
//
//    while( !eof() ){
//        getCCToken();
//    }
//
//    if(processCC())
//        write_codeCC(of,pcw);
//    else
//        std::cin.get();
//    
//
//    return of;
//}
//
////
////    pre: the set of functions read from the .h file
////    post:    a set of CLR compatible functions in the CLR file.
////            a set of function pointer typedefs for the functions.
////            a table of all of the types used in all of the functions.
//bool BufParser::processCC() 
//{
//    /*
//    func_def_out f;
//    t_string s;
//    t_string nparm, ppp;
//
//    
//    // - Cache all Return/Parameter types, also Cache the M structures.
//    for( size_t i=0; i<in_funcs.size(); ++i ){
//        // - Write the proc defs.
//        proc_def d;
//
//        // - Redundant and cyclical reference I know.
//        d.in_func = in_funcs[i];
//        d.proc_def_name = in_funcs[i].name + "_proc";
//        in_funcs[i]._proc_def = in_funcs[i].name + "_proc";
//
//        proc_defs.push_back(d);
//
//        // **** Construct the out function ****
//        f.reset();
//        f.name = in_funcs[i].name;
//        StringUtil::uppercase(f.name);
//
//        s="";
//
//        // input is scalar
//        if(in_funcs[i].rtype.str.compare("int*"))
//            s=type_table.find_out(
//                strip_pointers(in_funcs[i].rtype.str)
//                );
//        else
//            s="int*";
//
//
//        // - Return types cannot be arrays for various reasons.
//        if(!s.compare(""))
//        {
//            // - We could not find the class in the table, which means the return type
//            // is a class from the C++ engine, so we have to create a new class which will
//            // wrap it.  The new class is called the M class, and always prefixes the other
//            // class with an "M". such as MCamera MPlayer..
//            s=strip_pointers(in_funcs[i].rtype.str);
//            type_table.add_type( s, t_string("M")+s );
//            s = t_string("M")+s;
//        }
//
//        f.rtype = s;
//
//        t_string    parm;
//        func_type    out_param;
//
//        // - Add parameter types (M.. types) to the table.
//        for( int p=0; p<in_funcs[i].parms.size(); ++p )
//        {
//            out_param.clear();
//            parm = in_funcs[i].parms[p].str;
//
//            // - Check to see if this is an array.
//            // Note afterwords the param type will be the data type of the array.
//            // and is_array and is_array_ptr will determine if the type is an array.
//            if(check_array(
//                in_funcs[i].parms[p].str,
//                in_funcs[i].parms[p].array_dims,
//                parm,
//                in_funcs[i].parms[p].is_array_ptr))
//            {
//                // input is an array
//                in_funcs[i].parms[p].is_array=out_param.is_array=true;
//                in_funcs[i].parms[p].str = parm;
//            }
//            
//            out_param.is_array_ptr = in_funcs[i].parms[p].is_array_ptr;
//            out_param.b_const = in_funcs[i].parms[p].b_const;
//            out_param.dt = in_funcs[i].parms[p].dt;
//            out_param.array_dims = in_funcs[i].parms[p].array_dims;
//            
//            // - Here: we set the parm to the type.
//            // the parm might have pointers!
//            //in_funcs[i].parms[p].str = 
//            if( parm.compare("int*") )
//                out_param.str = type_table.find_out(
//                strip_pointers(parm)//cannot call for int*
//                );
//            else
//                out_param.str = "IntPtr";//special case, but we could do find_out it'd just be slower
//
//            //in_funcs[i].parms[p].nPointers = out_param.nPointers = count_pointers(parm);
//
//            if(!out_param.str.compare(""))
//            {
//                s=strip_pointers(parm);
//                out_param.str = t_string("M")+s;
//                type_table.add_type( s, out_param.str );
//                s = t_string("M")+s;
//            }
//            f.parms.push_back(out_param);
//        }
//        
//        f.infunc = in_funcs[i];
//        out_funcs.push_back(f);
//    }
//
//    //- Validate all return types
//    bool failed=false;
//    for( size_t i=0; i<in_funcs.size(); ++i ){
//        if(!validate_return_typeCC(in_funcs[i].rtype))
//            failed=true;
//    }
//
//    if(failed)
//    {
//        std::cout<<"One or more return types failed. Correct and try again."<<std::endl;
//        return false;
//    }
//
//    // - Validate the parameters
//    failed=false;
//    for( size_t i=0; i<in_funcs.size(); ++i ){
//        for( size_t j=0; j<in_funcs[i].parms.size(); ++j ){
//            //supposed to be "return type" diddnt do a parm type fn yet.
//            if(!validate_return_typeCC(in_funcs[i].parms[j]))
//                failed=true;
//        }
//    }
//
//    if(failed)
//    {
//        std::cout<<"One or more parameter types failed. Correct and try again."<<std::endl;
//        return false;
//    }
//
//*/
//    return true;
//}
//
//    Checks to see if the type is also an array.
//
//
//bool BufParser::check_array( t_string str, int& dimension, t_string& type, bool& is_ptr )
//{
//    int pos=0;
//
//    dimension=0;
//    while( (pos=str.find('<',pos))!=t_string::npos) {
//        dimension++;
//        pos++;
//    }
//
//    if(str[str.size()-1]=='*')
//        is_ptr=true;
//    else
//        is_ptr=false;
//
//    int p1 = str.rfind('<') +1;
//    int p2 = str.find('>',p1);
//    if(p1<p2){
//        type = str.substr(p1,p2-p1);
//        return true;
//    }
//    return false;
//}

// - Make sure all MStructs are pointers and all
// Structs/integrals are not.
// To return arrays of data in c++ use a std::vector
//bool BufParser::validate_return_typeCC(func_type param)
//{
//    /*
//    t_datatype t=dt_notype;
//
//    t_string stripped = strip_pointers(param.str);
//    t_string unstripped = param.str;
//
//    // - int* is the only integral pointer allowed.  All else are M structs.
//    if(!param.str.compare("int*"))
//        return true;
//
//    for( size_t i=0; i<type_table.vin.size(); ++i )
//    {
//        if(type_table.vin[i].name==stripped)
//        {
//            t=type_table.vin[i].t;
//            break;
//        }
//    }
//
//    if(t==dt_notype)
//        std::cout<<"Could not find type in type table '"<<param.str<<"'"<<std::endl;
//
//    if(is_ptr(param.str))
//    {
//        if(t==dt_struct||t==dt_integral)
//        {
//            std::cout<<"Bad type for '"<<param.str<<"'. Integral/Struct cannot be a pointer."<<std::endl;
//            //std::cout<<"'"<<s<<"'. Is pointer (WARNING)."<<std::endl;
//            return false;
//        }
//    }
//    else if(t==dt_m_struct)
//    {
//        std::cout<<"Bad type for '"<<param.str<<"'. Must be a pointer."<<std::endl;
//        return false;
//    }
//    */
//    return true;
//}

bool BufParser::is_ptr(const t_string s){
    return (s.rfind('*')!=t_string::npos);
}
//bool BufParser::isDelim( char c, std::vector<char> delims ) {
//    return (delims.find(c)!=std::vector<char>::end());
//}
/**
*    getToken()
*    Gets the standard token for any character /number etc.
*    The returned value is in the tok_buf.
*    ignoreWs is just an additional set of ws delimiters to 'delims'
*/
void BufParser::getToken(char* cptr, std::vector<char> delims, bool ignoreWs )
{
    //tok_buf=t_string("");
    ptr = cptr;

    while(!eof()) 
    {
        //for the current char...
        char c = charAt();
        
        if(ignoreWs)
        {
            //if(isWs(c))
                ;
        }
  //      else if( isDelim(c,delims) ) 
        //{
  //          return;
  //      }
        tok_buf+=c;
        inc();
    }

}//endToken()




// - Removes all '*'
//t_string BufParser::strip_pointers( const t_string type ){
//    t_string s = type;
//    if(is_ptr(s))
//        s=s.substr(0,s.length()-(s.length()-s.find('*')));
//    return s;
//}
//// - Counts all '*'
//int BufParser::count_pointers( const t_string type ){
//    t_string s = type;
//    size_t pos=0;
//    int n=0;
//    if(is_ptr(s))
//        while(pos=s.find('*')){n++,pos++;}
//
//    return n;
//}

BufParser::BufParser( const BufParser& rhs ) {
    b_eof = rhs.b_eof;

    ptr = rhs.ptr;
    file_state = rhs.file_state;
}
BufParser::BufParser() : 
_data(NULL),
line_num(1), 
b_eof(0), 
template_level(0), 
file_state(state_normal), 
pstate(ps_begin), 
func_ps(fp_none),
ptr_state(ptr_begin) ,
ptr(NULL),
//_filename(""),
_bSelfAllocated(false)
{ 
    cur_func.reset();
}
BufParser::BufParser(char* filePtr_in, t_string filename ) : 
_data(NULL),
line_num(1), 
b_eof(0), 
template_level(0), 
file_state(state_normal), 
pstate(ps_begin), 
func_ps(fp_none),
ptr_state(ptr_begin) ,
_bSelfAllocated(false)
{ 
    ptr = filePtr_in; 
    cur_func.reset();
    //_filename = filename; 
}

BufParser::BufParser(t_string& data) : 
_data(NULL),
line_num(1), 
b_eof(0), 
template_level(0), 
file_state(state_normal), 
pstate(ps_begin), 
func_ps(fp_none),
ptr_state(ptr_begin),
_bSelfAllocated(true)
{ 
    _data = new std::vector<char>();
    _data->reserve((data.length() + 1));

    // Allocate this data locally.
    //ptr = new char[data.length()+1];//HEAP:
    //_data->copyFrom(data.c_str(),data.length(),0,0);
    ///_data->at(data.length()) = NULL;//Null terminate file
    
    memcpy((void*)_data->data(), data.c_str(), data.length());
    *((char*)(_data->data()+data.length())) = 0;//Null terminate file
    ptr = _data->data();

    cur_func.reset();
    //_filename = ""; 
}

BufParser::~BufParser()
{
    DEL_MEM(_data);
}
}