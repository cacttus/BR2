#include "../base/BaseHeader.h"
#include "../base/CSharpCompiler.h"
#include "../base/TextParser.h"
#include "../base/StringUtil.h"


namespace BR2 {
CSharpCompiler::CSharpCompiler() {
}
CSharpCompiler::~CSharpCompiler() {
}
std::shared_ptr<CSharpCompileResult> CSharpCompiler::compile(char* code) {
  std::shared_ptr<CSharpCompileResult> res = nullptr;
  return res;
/*
general - parse

#ifndef FORCE_INLINE
#ifdef BRO_OS_WINDOWS
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline
#endif
#endif

//May not be necessary
enum class CSharpScope {
  Class,
  Block,
}
enum class CSharpAccessModifier {
  Public,
  Private,
  Protected,
  None, //None is not applicable to variables within method scope.
};
enum class CSharpStorageClass {
  Static,
  Dynamic, //..not static
};
enum class CSharpOperator {
  Op_Add,
  Op_Sub,
  Op_Mul,
  Op_Div,
  Op_Assign, // x = 
  Op_Call,  //x()
  Op_PreInc, //++x
  Op_PreDec, //--x
  Op_PostInc, //x++ //This will be hard
  Op_PostDec, //x--
  Op_None
};
enum class CSharpControlWord {
  If_Then,
  Do_While,
  While_Loop,
  For_Loop,
  None
};
enum class CSharpParameterClass {
  None,
  ByValue,
  ByRef,
  Out,
  In
};
//[int|float|double|vec2|vec3|vec4|mat4|*Node|Gu]
enum class CSharpDataType {
  Int,
  Float,
  //..
  Vec2,
  //.
  Class, //Action<>, Function<>
};

ScriptManager::runScript(std::shared_ptr<CSharpScript> script){
  script->start();
  _runningScripts.push_back(script);
}
ScriptManager::update(float dt){
  for(auto script : _runningScripts){
    script->update(dt):
  }
}

class CSharpScript : public VirtualMemoryShared<CSharpContext>  {
private:
  void parse(context) {
    MUST
    Find class that inherits CSharpScript
    then find Start() and Update()
  }
public:
  std::shared_ptr<CSharpMethod> _start = nullptr;
  std::shared_ptr<CSharpMethod> _update = nullptr;
}

//This is the start method that's in C++
CSharpScript::start() {
  if (_start == nullptr) {
    BroLogErrorCycle("Script could not execute, no Start method was found");
  }
  else {
    std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
    context->init();
    _start.execute(context);
  }
}
CSharpScript::update(float dt) {
  if (_update == nullptr) {
    BroLogErrorCycle("Script could not execute, no Start method was found");
  }
  else {
    std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
    context->init();
    std::shared_ptr<CSharpFloat> delta = std::make_shared<CSharpFloat>();
    delta.assign(dt);
    context->getCallstack().push_back(delta);//*Push this parameter on the stack so Update() will receive it.
    _update.execute(context);
  }
}

//Callstack
class CSharpCallstack : public VirtualMemoryShared<CSharpContext> {
public:
 CSharpCallstack(std::shared_ptr<CSharpContext> context){
 _pContext = context;
 }
 std::vector<std::shared_ptr<CSharpVariable>> getItems() { return _callstack; }
 void push(std::shared_ptr<CSharpVariable> item) { 
  _callstack.push_back(item);
 }
 void push(std::vector<std::shared_ptr<CSharpVariable>> items) {
   for(auto item : items){
     this->push(item);
   }
 }
 std::shared_ptr<CSharpVariable> top(){
   if(_callstack.size()==0){
      _pContext->runtimeError("Stack Underflow.");
   }
 }
 std::vector<std::shared_ptr<CSharpVariable>> pop(size_t count = 1) { 
  std::vector<std::shared_ptr<CSharpVariable>> ret;
  for(size_t x=0; x<count; ++x) {
    std::shared_ptr<CSharpVariable> item = _callstack[_callstack.size()-1 - x];
    ret.push_back(item);
    _callstack.erase(_callstack.begin()+(_callstack.size()-1-x));
  }
  return ret;
 }
private:
  std::vector<std::shared_ptr<CSharpVariable>> _callstack; //Callstack.
  std::shared_ptr<CSharpContext> _pContext = nullptr;
};
class CSharpContext : public VirtualMemoryShared<CSharpContext> {
public:
  CSharpContext() {
  } 
  void init(){
    _callstack = std::make_shared<CSharpCallstack>(getThis<CSharpContext>());
  }
  std::shared_ptr<CSharpCallstack> getCallstack() { return _callstack; }
  void runtimeError(string_t err){
    BroLogError("Runtime Error: " err);
  }
  void compileError(string_t err){
    BroLogError("" + _strFile + "(" + _iLine + "," + _iChar +"): error: " err);
  }
private:
  string_t _strFile = "<invalid>";
  int32_t _line = -1; // the compile, or, runtime line.
  int32_t _char = -1; // the compile line char.
  int32_t _instructionPointer; // the current instruction point.er

  std::shared_ptr<CSharpCallstack> _callstack = nullptr;//Call init()
};

//Generic language item.
class CSharpLanguageItem : public VirtualMemoryShared<CSharpContext> {
public:
  virtual void parse(context) = 0 // Parse the code.
  virtual void execute(context) = 0;  //Execute the code.
protected:
  int32_t _iLine = -1;  //The line this element was found on.  If the expression spans multiple lines, this is the first line.
  string_t _code; //The code that was parsed to create this language item.
};

//Namespace
class CSharpNamespace : public CSharpLanguageItem {
public:
  string_t getIdentifier() { return _identifier; }
  std::vector<CSharpClass> getClasses() { return _classes; }
private:
  string_t _identifier = "<invalid>";
  std::vector<CSharpClass> _classes;
};

//Class
class CSharpClass : public CSharpLanguageItem {
public:
  virtual void execute(context) override {
    std::shared_ptr<CSharpMethod> main = findMain();
    if(main==nullptr){
      BroThrowException("Attempted to execute a class that has no static main() method");
    }
    else{
      main->execute(context);
    }
    //context->getMethod{
    //}
  }
private:
  //[public|private|sealed|protected] class MyClass [: MySuperClass] {
  string_t _identifier = "<invalid>";
  //These are in order of the class definition.
  std::vector<CSharpLanguageItem> _members_and_methods;
    //These are shortcuts, not in order of definition.
  std::vector<CSharpVariable> _members;
  std::vector<CSharpMethod> _methods;
};

//Variable
//[int|float|double|vec2|vec3|vec4|mat4|*Node] _myVar = [expr];
class CSharpVariable : public CSharpLanguageItem  {
public:
  string_t getIdentifier() { return _identifier; }
  CSharpDataType getType() { return _dataType; }

  virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual std::shared_ptr<CSharpVariable> sub(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual std::shared_ptr<CSharpVariable> mul(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual std::shared_ptr<CSharpVariable> div(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual std::shared_ptr<CSharpVariable> increment(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual std::shared_ptr<CSharpVariable> decrement(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual std::shared_ptr<CSharpVariable> call(std::shared_ptr<CSharpVariable> v, context) = 0;
  virtual int32_t toInt32(context) = 0; //Converts to int32
  virtual bool isInt32(context) = 0; //Returns false if this doesn't CONVERT to an int32
private:
  string_t _identifier = "<invalid>"; // The language identifier
  CSharpStorageClass _storageClass = CSharpStorageClass::Not_Static;
  CSharpAccessModifier _modifier = CSharpAccessModifier::None;
  std::shared_ptr<CSharpClass> _myClass = nullptr;  //nullptr, if this is a local variable.
  CSharpDataType _dataType = CSharpDataType::None; //This can basically be gotten from the subclass.
};

//Int
class CSharpInt : public CSharpVariable {
public:
  int32_t value(){ return _value; }
  virtual bool canConvertInt32() { return true; }
  virtual FORCE_INLINE toInt32() override {
    return _value;
  }
  virtual FORCE_INLINE std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> rhs, std::shared_ptr<CSharpContext> ctx) override;
  {
    if(v.dataType() == CS_DATATYPE_INT) {
      _value += std::dynamic_cast<CSharpInt>(rhs)->value();
    }
    else if(v.dataType() == CS_DATATYPE_DOUBLE) {
      _value += std::dynamic_cast<CSharpDouble>(rhs)->value();
    }
    else if(v.dataType() == CS_DATATYPE_FLOAT) {
      _value += std::dynamic_cast<CSharpFloat>(rhs)->value();
    }
    else {
      context->runtimeError("Could not add datatypes of '" + getType() + "' and '" + rhs->getType() "'");
    }
  }
private:
  int32_t _value = 0;
};

//Vec2
class CSharpVec2 : public CSharpVariable {
public:
  virtual FORCE_INLINE bool canConvertInt32() override { return false; }
  virtual FORCE_INLINE toInt32() override {
    BroThrowException("Int Typecast failed for variable '" + getIdentifier() + "'");
  }
private:
  vec2 _value;
};

//Property
// [int|..] MyProp { get[;|{..}] set[;|{...}] } = [..];
class CSharpClassProperty : public CSharpLanguageItem   {
public:
  std::shared_ptr<CSharpVariable> _value = nullptr;
  std::shared_ptr<CSharpMethod> _getter = nullptr;
  std::shared_ptr<CSharpMethod> _setter = nullptr;
};

//Method
//[Access_modifier] [static] [return_type] method(parms,..) {control block}
// Arguments are passed onto the stack using the stdcall convention.
class CSharpMethod : public CSharpLanguageItem {
public:
  CSharpStorageClass getStorageClass() { return _storageClass; }
  CSharpAccessModifier getAccessModifier() { return _modifier; }
  CSharpDataType getReturnType() { return _returnType; }

  virtual void execute(std::shared_ptr<CSharpContext> context) override {
    //Pop the stcack
    //std::vector<std::shared_ptr<CSharpLanguageItem>> params = context->getCallstack()->pop(_params.size());
    //So, somehow, here, we need the _codeBlock to be able to access these params.

    //Code block should put the return value on the stack
    _codeBlock->execute(context);
  }
private:
  CSharpStorageClass _storageClass;
  CSharpAccessModifier _modifier;
  CSharpDataType _returnType;

  //We should also provide the capability to call BR2 classes using the CSharpMethod syntax.
  std::vector<std::shared_ptr<CSharpParameter>> _params;
  std::shared_ptr<CSharpCodeBlock> _codeBlock = nullptr;
};

//Method Parameter
class CSharpParameter : public CSharpLanguageItem {
public:
  CSharpParameterClass getParameterClass() { return _parameterClass; }

private:
  std::shared_ptr<CSharpVariable> _variable = nullptr;
  CSharpParameterClass _parameterClass = CSharpParameterClass::ByValue;
};

//Code block
// { .. }
class CSharpCodeBlock : public CSharpLanguageItem {
public:
  virtual void execute(context) override {
    for(auto expr : _expressions) {
      expr->execute(context);
    }
  }
private:
  std::vector<std::shared_ptr<CSharpStatement>> _expressions;
};

//Statement
// delimited by a semicolon.
// myvar = (0 + variable *2 +(1/5 *myclass->method()));
//1/5 above is a "sub" expression
// An expression, may also be a statement, Equals is allowed here.
//  Note: Only assignment, call, increment, decrement, await, and new object expressions can be used as a statement
class CSharpStatement : public CSharpLanguageItem {
public:
  std::shared_ptr<CSharpVariable> evaluate(context) {
    std::shared_ptr<CSharpVariable> evaluated_value;
    context->getCallstack()->push(evaluated_value);
    for(auto term : _terms) {
      term->execute(context);
    }
    return evaluated_value;
  }
  virtual void execute(context) override {
    evaluate(context);
  }
private:
  std::vector<std::shared_ptr<CSharpExpressionTerm>> _terms; //These come In order
};

//Method Invocation
// MyClass.Method(par1,par2,..);
//or 
// Method(par1,par2..);
class CSharpMethodInvocation : CSharpLanguageItem {
public:
  virtual void execute(context) override {
    //Evaluate parameters
    std::vector<std::shared_ptr<CSharpVariable>> evaluated;
    for(auto param : _params) {
      evaluated.push_back( param->evaluate(context) );
    }
    //Push evaluated parameters onto stack.
    context->getCallstack()->push(evaluated);

    //*****This is incorrect.

    //Push Number of parameters onto stack.
    context->getCallstack()->push(std::make_shared<CSharpInt>(_params.size()));
    _method.execute(context);
  }
private:
  std::shared_ptr<CSharpMethod> _method = nullptr;
  std::vector<std::shared_ptr<CSharpStatement>> _params = nullptr;
};

//Reference to a variable, in a method.
class CSharpVariableReference : public CSharpLanguageItem {
public:
  //TODO:
private:
  int32_t _iVarId = -1;  //The index of the variable, in the method's compiled variable table.  This includes the method parameters.

};

//Expression Term
// constant, variable, or sub-expression
class CSharpExpressionTerm : public CSharpLanguageItem {
public:
  virtual void execute(context) override {
    //Our curently operating variable is on the top of the stack.
    std::shared_ptr<CSharpVariable> value = context->getCallstack()->top();

    if(_variable != nullptr) {
      if(_operator == CSharpOperator::Op_None) {
        //This is the left hand side, return it.
        value.assign(_variable);
        retur value;
      }
      else if(_operator == CSharpOperator::Op_Add) {
        value.add(_variable);
      }
      //..sub,mul,div
      else if(_operator == CSharpOperator::Op_Increment) {
        value.increment();
      }
      //...
      else {
        context->runtimeError("No condition for oprator '" + _operator + "'");
      }
    }
    else if(_methodInvocation != nullptr) {
      _methodInvocation->
      if(_operator == CSharpOperator::Op_None){
        //This is the left hand side, return it.
        value.assign(_variable);
        retur value;
      }
    }
  }
private:
  CSharpOperator _operator = CSharpOperator::Op_None; // =, +, -, *, /, call, or None for first term (first term has no operator, and is the storage)
  
  //Only one of these will be set.
  std::shared_ptr<CSharpVariableReference> _variable = nullptr; // this may be a constant.
  std::shared_ptr<CSharpStatement> _statement = nullptr;
  std::shared_ptr<CSharpMethodInvocation> _methodInvocation = nullptr;
};

//Control Statement
// if then, while, do, for
class CSharpControlStatement : public CSharpLanguageItem {
public:
  virtual void execute(context) override {
    if(_controlword == CSharpControlWord::If_Then) {
      std::shared_ptr<CSharpVariable> value = _condition.evaluate();
      if(!value.canConvertInt32()) {
        //This should have failed in compilation though.
        context->runtimeError("Failed to convert '"+ value->getIdentifier() +"' to an int32.");
      }
      else if(value.toInt32() > 0) {
        _codeBlock->execute(context);
      }
    }
  }
private:
  CSharpControlWord _controlword = CSharpControlWord::None; //ifthen, while, for, do
  std::shared_ptr<CSharpStatement> _condition = nullptr; // (var > 2)
  std::shared_ptr<CSharpCodeBlock> _codeBlock = nullptr; // { do this .. }
};


*/

  /*
  Note; if you add more delimiters make sure to update isEscapeCharacter()
  */
/**
char c;
  //char* ptr = buf;
  string_t token;
  int32_t _nStringProfile = 0;
  int32_t _nParenProfile = 0;
  int32_t _nBracketProfile = 0;

  _iLine = 1;

  bool _bEscape = false;

  TextParser tp(code);

  while (tp.eof() == false) {
    c = tp.charAt();

    if (tp.eof()) {

      break;
    }
    else if (c == '"') {
      _nStringProfile = ~_nStringProfile;

      token += c;
      tp.inc();
    }
    else if (_nStringProfile) {
      //Inside string - do not parse other delimiters
      token += c;
      tp.inc();
    }
    else if (c == '(') {
      _nParenProfile++;

      parseExpression(')');

      token += c;
      tp.inc();
    }
    else if (c == ')') {
      _nParenProfile--;

      if (_nParenProfile < 0) {
        displayError("Parentheses profile is invalid. You're missing a parentheses somewhere.");
      }

      token += c;
      tp.inc();
    }
    else if (c == '[') {
      _nBracketProfile++;

      token += c;
      tp.inc();
    }
    else if (c == ']') {
      _nBracketProfile--;

      if (_nBracketProfile < 0) {
        displayError("Bracket profile is invalid. You're missing a Bracket somewhere.");
      }

      token += c;
      tp.inc();
    }
    else if (c == '\n' || c == '\r') {

      tp.eatLine(_iLine);
    }
    // - If we're whitespace but we also are within a string or bracket then do not skip it, add it to
    // the argument.
    else if (StringUtil::isWs(c) && !(_nStringProfile || _nParenProfile || _nBracketProfile)) {
      if (token.length()) {
        _vecTokens.push_back(string_t(token));
      }
      token = "";
      tp.inc();
    }
    else {
      token += c;
      tp.inc();
    }

  }
*/
}



}//ns Game
