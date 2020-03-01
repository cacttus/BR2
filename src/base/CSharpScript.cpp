#include "../base/CSharpScript.h"
#include "../base/CSharp.h"
#include "../base/Logger.h"
#include "../base/HashMap.h"
#include "../base/TextParser.h"
#include <unordered_map>
#include <algorithm>
namespace BR2 {
#ifndef FORCE_INLINE
#ifdef BR2_OS_WINDOWS
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline
#endif
#endif

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
  None,
  Int16,
  Int32,
  Int64,
  Float,
  Double,
  Decimal,
  Vec2,
  Vec3,
  Vec4,
  Class, //Action<>, Function<>
};
enum class CSharpCallingConvention {
  None,
  CDecl,
  StdCall,
};

class CSRuntimeContext;
class CSharpVariable;
class CSharpStruct;
class CSharpMethod;
class CSharpParameter;
class CSharpScope;
class CSharpStatement;
class CSharpTerm;

class CSCallstack : public VirtualMemoryShared<CSRuntimeContext> {
public:
  CSCallstack(std::shared_ptr<CSRuntimeContext> context) {
    _pContext = context;
  }
  virtual ~CSCallstack() override {
  }
  std::vector<std::shared_ptr<CSharpVariable>> getItems() { return _callstack; }
  void push(std::shared_ptr<CSharpVariable> item) {
    _callstack.push_back(item);
  }
  void push(std::vector<std::shared_ptr<CSharpVariable>> items) {
    for (auto item : items) {
      this->push(item);
    }
  }
  std::shared_ptr<CSharpVariable> CSCallstack::top();
  std::vector<std::shared_ptr<CSharpVariable>> pop(size_t count = 1) {
    std::vector<std::shared_ptr<CSharpVariable>> ret;
    for (size_t x = 0; x < count; ++x) {
      std::shared_ptr<CSharpVariable> item = _callstack[_callstack.size() - 1 - x];
      ret.push_back(item);
      _callstack.erase(_callstack.begin() + (_callstack.size() - 1 - x));
    }
    return ret;
  }
private:
  std::vector<std::shared_ptr<CSharpVariable>> _callstack; //Callstack.
  std::shared_ptr<CSRuntimeContext> _pContext = nullptr;
};

//Base class for where we are in the file/line
//Likely, we'll store file/line inside the classes themselves, negating the need for a RUntimeContext and CompileCOntext to share base
class CSharpContext : public VirtualMemoryShared<CSharpContext> {
public:
  CSharpContext() {}
  virtual ~CSharpContext() override {}
  virtual void init() = 0;
  virtual void error(string_t err) = 0;
};
class CSRuntimeContext : public CSharpContext {
public:
  void init() {
    //_callstack = std::make_shared<CSCallstack>(getThis<CSRuntimeContext>());
  }
  virtual void error(string_t err) override {
    BRLogError("Runtime Error: " + err);
  }
  std::shared_ptr<CSCallstack> getCallstack() { return _callstack; }
private:
  std::shared_ptr<CSCallstack> _callstack = nullptr;//Call init()
  int32_t _instructionPointer; // the current instruction point.er
};
std::shared_ptr<CSharpVariable> CSCallstack::top() {
  if (_callstack.size() == 0) {
    _pContext->error("Stack Underflow.");
  }
}

//Language definition classes, class, method, property, code block, variables, statements.
class CSharpLanguage : public VirtualMemory {
public:
  virtual void execute(std::shared_ptr<CSRuntimeContext> context) {
    BRThrowNotImplementedException();
  } //Execute the code.
protected:
  int32_t _iLine = -1;  //The line this element was found on.  If the expression spans multiple lines, this is the first line.
  string_t _code; //The code that was parsed to create this language item.
};


//Namespace
//Essentially this is also a CSharpScope.  however, that isn't necessary.
class CSharpNamespace : public CSharpLanguage {
public:
  string_t getIdentifier() { return _identifier; }
  std::vector<CSharpStruct> getClasses() { return _classes; }
  std::shared_ptr<CSharpNamespace> parent() { return _parent; }
  std::vector<std::shared_ptr<CSharpNamespace>> children() { return _children; }
private:
  string_t _identifier = "<invalid>";
  std::vector<CSharpStruct> _classes;
  std::shared_ptr<CSharpNamespace> _parent = nullptr;
  std::vector<std::shared_ptr<CSharpNamespace>> _children;
};

typedef uint64_t CSharpTypeHash;

//Class Or struct base
class CSharpStruct : public CSharpLanguage {
public:
  CSharpStruct(std::shared_ptr<CSharpNamespace> ns, string_t struct_name) {
    _iTypeHash = Hash::computeStringHash64bit(struct_name);
    _namespace = ns;
  }

  std::shared_ptr<CSharpNamespace> getNamespace() { return _namespace; }

  HashMap<string_t> aliases() { return _aliases; }
  Hash32 typeHash() { return _iTypeHash; }
  string_t getIdentifier() { return _identifier; }
  CSharpDataType getType() { return _eDataType; }


  std::map<CSharpOperator, std::map<CSharpTypeHash, std::shared_ptr<CSharpMethod>>> operators() { return _operators; }
  std::vector<std::function<void()>> cpp_lambdas() { return _cpp_lambdas; }

  //So these should actually be methods.
  //virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) ;
  //virtual std::shared_ptr<CSharpVariable> sub(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
  //virtual std::shared_ptr<CSharpVariable> mul(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
  //virtual std::shared_ptr<CSharpVariable> div(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
  //virtual std::shared_ptr<CSharpVariable> increment(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
  //virtual std::shared_ptr<CSharpVariable> decrement(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;

  //Call is really the () operator, but () can't be overloaded in c#
  //virtual std::shared_ptr<CSharpVariable> call(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
  std::shared_ptr<CSharpStruct> getBase() { return _baseClass; }

  bool isClass() { return _baseClass != nullptr; }

  HashMap<std::shared_ptr<CSharpVariable>> getMembers() { return _members; }
  HashMap<std::shared_ptr<CSharpMethod>> getMethods() { return _methods; }

  //virtual std::shared_ptr<CSharpType> getType(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
  virtual string_t ToString(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) {
    BRThrowNotImplementedException();
  }

  virtual string_t toString() {
    if (_eDataType == CSharpDataType::None) { return "None"; }
    else if (_eDataType == CSharpDataType::Int16) { return "Int16"; }
    else if (_eDataType == CSharpDataType::Int32) { return "Int32"; }
    else if (_eDataType == CSharpDataType::Int64) { return "Int64"; }
    else if (_eDataType == CSharpDataType::Float) { return "Float"; }
    else if (_eDataType == CSharpDataType::Double) { return "Double"; }
    else if (_eDataType == CSharpDataType::Class) {
      return "Class";
    }
  }
private:
  std::map<CSharpOperator, std::map< CSharpTypeHash, std::shared_ptr<CSharpMethod> > > _operators;
  HashMap<string_t> _aliases;

  //[public|private|sealed|protected] class MyClass [: MySuperClass] {
  CSharpTypeHash _iTypeHash = 0; /// This is how we're going to locate types.
  string_t _identifier = "<invalid>";
  CSharpStorageClass _storageClass = CSharpStorageClass::Dynamic;
  CSharpAccessModifier _modifier = CSharpAccessModifier::None;
  CSharpDataType _eDataType = CSharpDataType::None;
  std::shared_ptr<CSharpStruct> _pClass = nullptr;//Not set if we're not a class.
  HashMap<std::shared_ptr<CSharpVariable>> _members;
  HashMap<std::shared_ptr<CSharpMethod>> _methods;
  std::shared_ptr<CSharpNamespace> _namespace;

  std::vector<std::function<void()>> _cpp_lambdas;

  //If is a class then this WILL be defined.  Base class is object.
  std::shared_ptr<CSharpStruct> _baseClass = nullptr;
};
//class CSharpType : public CSharpStruct {
//public:
//
//  virtual string_t toString() {
//    if (_eDataType == CSharpDataType::None) { return "None"; }
//    else if (_eDataType == CSharpDataType::Int16) { return "Int16"; }
//    else if (_eDataType == CSharpDataType::Int32) { return "Int32"; }
//    else if (_eDataType == CSharpDataType::Int64) { return "Int64"; }
//    else if (_eDataType == CSharpDataType::Float) { return "Float"; }
//    else if (_eDataType == CSharpDataType::Double) { return "Double"; }
//    else if (_eDataType == CSharpDataType::Class) {
//      return "Class";
//    }
//  }
//private:
//};



//Global Base class for all objects
class CSharpClass : public CSharpStruct {
public:
private:
};

//Int
//class CSharpInt : public CSharpStruct {
//public:
//  CSharpInt() {
//    //So we will add each operator method. - + - * () 
//    addMethod(std::make_shared<CSharpMethod>(std::shared_ptr<CSharpInt>, std::shared_ptr<CSharpInt>)
//
//
//  }
//
//  int32_t value() { return _value; }
//  virtual bool canConvertInt32() { return true; }
//  //virtual int32_t toInt32(std::shared_ptr<CSharpContext> context) override {
//  //  return _value;
//  //}
//  //virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> rhs, std::shared_ptr<CSharpContext> context) override {
//  //  if (rhs->getType() == CSharpDataType::Int32) {
//  //    _value += std::dynamic_pointer_cast<CSharpInt>(rhs)->value();
//  //  }
//  //  else if (rhs->getType() == CSharpDataType::Double) {
//  //    BRThrowNotImplementedException();
//  //    //_value += std::dynamic_pointer_cast<CSharpDouble>(rhs)->value();
//  //  }
//  //  else if (rhs->getType() == CSharpDataType::Float) {
//  //    _value += std::dynamic_pointer_cast<CSharpFloat>(rhs)->value();
//  //  }
//  //  else {
//  //    context->runtimeError("Could not add datatypes of '" + getType() + "' and '" + rhs->getType()->toString() + "'");
//  //  }
//  //}
//private:
//  int32_t _value = 0;
//};

////Float
//class CSharpFloat : public CSharpStruct {
//public:
//  int32_t value() { return _value; }
//  virtual bool canConvertInt32() { return true; }
//  virtual toInt32(std::shared_ptr<CSharpContext> context) override {
//    return _value;
//  }
//  virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> rhs, std::shared_ptr<CSharpContext> ctx) override {
//    if (v->dataType() == CSharpDataType::Int32) {
//      _value += std::dynamic_pointer_cast<CSharpInt>(rhs)->value();
//    }
//    else if (v.dataType() == CSharpDataType::Double) {
//      _value += std::dynamic_pointer_cast<CSharpDouble>(rhs)->value();
//    }
//    else if (v.dataType() == CSharpDataType::Float) {
//      _value += std::dynamic_pointer_cast<CSharpFloat>(rhs)->value();
//    }
//    else {
//      context->runtimeError("Could not add datatypes of '" + getType() + "' and '" + rhs->getType() "'");
//    }
//  }
//private:
//  int32_t _value = 0;
//};

//
////Vec2
//class CSharpVec2 : public CSharpVariable {
//public:
//  virtual bool canConvertInt32() override { return false; }
//  virtual toInt32(context) override {
//    BRThrowException("Int Typecast failed for variable '" + getIdentifier() + "'");
//  }
//private:
//  vec2 _value;
//};

//Property
//A class Property
// [int|..] MyProp { get[;|{..}] set[;|{...}] } = [..];
class CSharpProperty : public CSharpLanguage {
public:
  std::shared_ptr<CSharpVariable> _value = nullptr;
  std::shared_ptr<CSharpMethod> _getter = nullptr;
  std::shared_ptr<CSharpMethod> _setter = nullptr;
};

//Method
//[Access_modifier] [static] [return_type] method(parms,..) {control block}
// Arguments are passed onto the stack using the stdcall convention.
// int MyMeth(float a, ref MyClass b)  < CSharpMethod { CSharpParameter, CSharpParameter }
// {  < CSharpCodeBlock 
//    int x = 0;      < variable declaration CSharpVariable
//    int y = 5;      < CSharpVariableDeclaration
//    int z = (b.Prop + x);     < CSharpVariableDeclaration
//
// }
class CSharpMethod : public CSharpLanguage {
public:
  CSharpStorageClass getStorageClass() { return _storageClass; }
  CSharpAccessModifier getAccessModifier() { return _modifier; }
  CSharpDataType getReturnType() { return _returnType; }

  virtual void execute(std::shared_ptr<CSRuntimeContext> context) override {
    //execute() here is likely invalid, because we're using CSharpMethodInvocation below.
  }
private:
  CSharpStorageClass _storageClass;
  CSharpAccessModifier _modifier;
  CSharpDataType _returnType;
  CSharpCallingConvention _callingConvention = CSharpCallingConvention::StdCall; //StdCall - callee cleans the stack.

  //We should also provide the capability to call BR2 classes using the CSharpMethod syntax.
  std::vector<std::shared_ptr<CSharpParameter>> _params;
  std::shared_ptr<CSharpScope> _codeBlock = nullptr;
};

//Method Parameter
//The only reason we need this class is to define the call reference, otherwise it's just a csharpvariable
class CSharpParameter : public CSharpLanguage {
public:
  CSharpParameterClass getParameterClass() { return _parameterClass; }

private:
  std::shared_ptr<CSharpVariable> _variable = nullptr;
  CSharpParameterClass _parameterClass = CSharpParameterClass::ByValue;
};

//Variable Declaration (Storage class)
//[int|float|double|vec2|vec3|vec4|mat4|*Node] _myVar = [expr];
class CSharpVariable : public CSharpLanguage {
public:
  string_t getIdentifier() { return _identifier; }

  virtual int32_t toInt32(std::shared_ptr<CSRuntimeContext> context) = 0; //Converts to int32
  virtual bool isInt32(std::shared_ptr<CSRuntimeContext> context) = 0; //Returns false if this doesn't CONVERT to an int32

private:
  string_t _identifier = "<invalid>"; // The language identifier
  std::shared_ptr<CSharpStruct> _myClass = nullptr;  //nullptr, if this is a local variable.
  CSharpDataType _dataType = CSharpDataType::None; //This can basically be gotten from the subclass.
};


//Scope -- Code block
// { .. }
class CSharpScope : public CSharpLanguage {
public:
  virtual void execute(std::shared_ptr<CSRuntimeContext> context) override {
    //Push all the locals on the callstack.
    //Note: C# does not allow redefining enclosing scope parameters.  so you wouldn't have to do that.

    //for (auto expr : _expressions) {
    //  expr->execute(context);
    //}
  }
private:
  std::vector<std::shared_ptr<CSharpStatement>> _expressions;
};

//Statement
// delimited by a semicolon. Ex:
// myvar = (0 + variable *2 + (1/5 * myclass->method() * -1));
//      0 term - term
//      + variable - term
//      * 2 - term
//      + (1/5 * myclass.Method()) - subexpression
//            1 - term
//            /5 - term
//            * myclass.Method() - term 
//                myclass - term
//                .Method() - term
//            * -1 - term 
//              -1 - term -  unary operator is its own term.
// An expression, may also be a statement, Equals is allowed here.
//  Note: Only assignment, call, increment, decrement, await, and new object expressions can be used as a statement
// ** Variable may also be declared here.  The variable must be placed on the stack, if so.
class CSharpStatement : public CSharpLanguage {
public:
  std::shared_ptr<CSharpVariable> evaluate(std::shared_ptr<CSRuntimeContext> context) {
    //std::shared_ptr<CSharpVariable> evaluated_value;
    //context->getCallstack()->push(evaluated_value);
    //for (auto term : _terms) {
    //  term->execute(context);
    //}
    //return evaluated_value;
  }
  virtual void execute(std::shared_ptr<CSRuntimeContext> context) override {
    evaluate(context);
  }
private:
  //Here a variable may also be declared.
  std::vector<std::shared_ptr<CSharpTerm>> _terms; //These come In order
};

//Method Invocation
// my_class_instance.MyMethod(par1,par2,..);
//or
// Method(par1,par2..);
class CSharpMethodInvocation : CSharpLanguage {
public:
  virtual void execute(std::shared_ptr<CSRuntimeContext> context) override {
    ////Evaluate parameters
    //std::vector<std::shared_ptr<CSharpVariable>> evaluated;
    //for (auto param : _params) {
    //  evaluated.push_back(param->evaluate(context));
    //}
    ////Push evaluated parameters onto stack.
    //context->getCallstack()->push(evaluated);

    ////*****This is incorrect.

    //std::shared_ptr<CSharpVariable> var;

    ////Push Number of parameters onto stack.
    //context->getCallstack()->push(std::make_shared<CSharpInt>(_params.size()));

    //_method->execute(context);
  }
private:
  std::shared_ptr<CSharpMethod> _method = nullptr;
  std::vector<std::shared_ptr<CSharpStatement>> _params;
};

//Reference to a variable, in a method.
class CSharpVariableReference : public CSharpLanguage {
public:
  //TODO:
private:
  int32_t _iVarId = -1;  //The index of the variable, in the method's compiled variable table.  This includes the method parameters.

};

//Expression Term
//  A term, an operator "+,-,*,/" and a variable, method, or constant
// constant "2" , variable "2", or sub-expression ()
class CSharpTerm : public CSharpLanguage {
public:
  virtual void execute(std::shared_ptr<CSRuntimeContext> context) override {
    //Our curently operating variable is on the top of the stack.
    std::shared_ptr<CSharpVariable> value = context->getCallstack()->top();

    //if (_variable != nullptr) {
    //  if (_operator == CSharpOperator::Op_None) {
    //    //This is the left hand side, return it.
    //    value.assign(_variable);
    //    retur value;
    //  }
    //  else if (_operator == CSharpOperator::Op_Add) {
    //    value.add(_variable);
    //  }
    //  //..sub,mul,div
    //  else if (_operator == CSharpOperator::Op_Increment) {
    //    value.increment();
    //  }
    //  //...
    //  else {
    //    context->runtimeError("No condition for oprator '" + _operator + "'");
    //  }
    //}
    //else if (_methodInvocation != nullptr) {
    //  //_methodInvocation->
    //  if (_operator == CSharpOperator::Op_None) {
    //    //This is the left hand side, return it.
    //    value.assign(_variable);
    //    retur value;
    //  }
    //}

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
class CSharpControlStatement : public CSharpLanguage {
public:
  virtual void execute(std::shared_ptr<CSRuntimeContext> context) override {
    if (_controlword == CSharpControlWord::If_Then) {
      std::shared_ptr<CSharpVariable> value = _condition->evaluate(context);
      //if (!value.canConvertInt32()) {
      //  //This should have failed in compilation though.
      //  context->runtimeError("Failed to convert '" + value->getIdentifier() + "' to an int32.");
      //}
      //else if (value.toInt32() > 0) {
      //  _codeBlock->execute(context);
      //}
    }
  }
private:
  CSharpControlWord _controlword = CSharpControlWord::None; //ifthen, while, for, do
  std::shared_ptr<CSharpStatement> _condition = nullptr; // (var > 2)
  std::shared_ptr<CSharpScope> _codeBlock = nullptr; // { do this .. }
};


#pragma region CSharpScript_Internal
class CSharpScript_Internal {
public:
  //**CLASSES AS WELL.  Classes can exist without namespaces.  Thus, so can enums, etc ..
  std::vector<std::shared_ptr<CSharpNamespace>> _namespaces;
  void compile(std::shared_ptr<CSharpCompileContext> context);
};
//Script level language context
enum class C_Script {
  None,
  NamespaceDecl, NamespaceId,
  UsingDecl, UsingId
};
void CSharpScript_Internal::compile(std::shared_ptr<CSharpCompileContext> context) {
  //*Classes can exist without namespaces too.

  //getTok method should tokenize all delimiters, even such as: 'namespace x;'
  // returning: namespace, x, and ;
  // we have: delimiters we return, such as '.' and ';'
  // whitespace delimiters are not returned

  //check for existence of the identifier.
  // then check to see if its in the right place.

  //char* c;
  //string_t code = "using System;\nnamespace TestNS\n{\nclass TestClass\n{\nint x;\nint testMethod(int param)\n{\nthis.x+=param;//test comment\n}\n}\n}/*end namespace, also, test comment*/\n\n\n";
  //string_t invalid_code = "using System;\nclass TestClass\n{\nint x;\nint testMethod(int param)\n{\nthis.x+=param;//test comment\n}\n}\nint invalid_field_outside_class;\n\n";
  //string_t tok;

  //C_Script ctx = C_Script::None;
  //while (code.length()) {
  //  tok = context->getTok(code, c); //return tok, also eat the string.

  //  if (StringUtil::equals(tok, "namespace")) {
  //    //it looks like after anything invalid, the parser just parses regardless.
  //    ctx = C_Script::NamespaceDecl;

  //    context->compileError("The name '" + tok + "' does not exist in the current context.");
  //    context->compileError("A namespace cannot directly contain members such as fields or methods.");
  //  }
  //  else if (StringUtil::equals(tok, "{")) {
  //    if (ctx == C_Script::None) {
  //      ctx = C_Script::NamespaceDecl;
  //    }
  //  }
  //  else if (StringUtil::equals(tok, ";")) {
  //    if (ctx == C_Script::None) {
  //      ctx = C_Script::NamespaceDecl;
  //    }
  //  }
  //  else /*identifier, or, error*/ {
  //    if (ctx == C_Script::NamespaceDecl) {
  //      //this is an identifier
  //      //Invalid identifiers spit out 'Identifier Expected'
  //      if (context->validateIdentifier(tok)) {
  //        C_Script::NamespaceDecl
  //      }
  //      else {

  //      }

  //    }
  //    else if (ctx == C_Script::NamespaceId) {
  //      //Error
  //      context->compileError("{ expected");
  //    }
  //  }
  //}


  //gather all using's
  // using->add(make_shared<CSharpUsing)
  //* A using clause must precede all other elements
  //gather all namespaces.
  //  std::make_shared<CSharpNamespace>( string_t internal_code )
  // foreach namespace
  //   namespace->compile(context)



}


#pragma endregion

#pragma region CSharpScript

CSharpScript::CSharpScript() {
  _pint = std::make_unique<CSharpScript_Internal>();
}
CSharpScript::~CSharpScript() {
  _pint = nullptr;
}

////execute is only available for the root class.
//virtual void execute(std::shared_ptr<CSharpContext> context) override {
//  if (StringUtil::equals(_baseClass->getIdentifier(), "Component")) {
//    std::shared_ptr<CSharpMethod> m = GetType().GetMethods();
//
//    if (_start) {
//      _start->execute(context);
//    }
//  }
//  else {
//    BRThrowException("Attempted to execute a class that was not derived from a script component.");
//  }
//}

void CSharpScript::initScriptSystem() {
  //Here define all the built-in classes like int, float, etc..

  //std::shared_ptr<CSharpNamespace> cs_System = std::make_shared<CSharpNamespace>("System");

  ////Struct Definitions
  //std::shared_ptr<CSharpStruct> cs_Int32 = std::make_shared<CSharpStruct>(cs_System, "Int32");
  //cs_Int32->aliases().add("int");

  ////So to execute an integral type operator method we're creating our own integral types, with methods.
  //// CSharpMethod can be either a set of CSharpStatement's or a (set of) C++ lambda(s).
  //// For integral type operator methods we use a c++ lambda.

  ////Operators

  ////Int32
  //std::map<CSharpTypeHash, std::shared_ptr<CSharpMethod> > add_methods;
  ////Int32 + Int32
  //std::shared_ptr<CSharpMethod> cs_Int32_plus_Int32 = std::make_shared<CSharpMethod>();
  //cs_Int32_plus_Int32->cpp_lambdas().push_back([]() {

  //  })

  //  add_methods.insert(std::make_pair(cs_Int32->typeHash(), cs_Int32_plus_Int32));

  //  cs_Int32->operators().insert(std::make_pair(CSharpOperator::Op_Add, add_methods));
}
std::shared_ptr<CSharpScript> CSharpScript::compile(std::shared_ptr<CSharpCompileContext> ct) {

  //gather all class references first.
  if (!ct->_bGatheredClasses) {
    //gather
  }
//  std::shared_ptr<CSharpScript> ret = std::make_shared<CSharpScript>();
//  ret->_pint->compile(ct);
//
  //std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();



}
//This is the start method that's in C++
void CSharpScript::start() {
  //**If the compiled script has a start method, run that, otherwise run the _start lambda on this (c++) script.

  //if (_start == nullptr) {
  //  BRLogErrorCycle("Script could not execute, no Start method was found");
  //}
  //else {
  //  std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
  //  context->init();
  //  _start.execute(context);
  //}
}
void CSharpScript::update(float dt) {
  //if (_update == nullptr) {
  //  BRLogErrorCycle("Script could not execute, no Start method was found");
  //}
  //else {
  //  std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
  //  context->init();
  //  std::shared_ptr<CSharpFloat> delta = std::make_shared<CSharpFloat>();
  //  delta.assign(dt);
  //  context->getCallstack().push_back(delta);//*Push this parameter on the stack so Update() will receive it.
  //  _update.execute(context);
  //}
}
#pragma endregion

enum class CSIntegralType {
  Int, Float, Double, Decimal,
};
/*
      using := "using"
      namespace := "namespace"
      class := "class"
      public := "public"
      private := "private"
      protected := "protected"
      final := "final"
      sealed := "sealed"
      internal := "internal"
      lbrace := "{"
      rbrace := "}"
      lparen := "("
      rparen := ")"
      lbracket := "["
      rbracket := "]"
      inc := "++"
      dec := "--"
      semicolon := ';'
      equals := "="
      plusequals := "+="
      minusequals := "-="
      plus := "+"
      minus := "-"
      times := "*"
      divide := "/"
      dot := "."
      colon := ":"
      new := "new"
*/
//
enum class CSTokenType {
  None,

  //////////////////////////////////////////////////////////////////////////
  // Tokens
  TokensStart,
  //Tab, Space, //unnecessary
  Whitespace,
  Class, Struct, Enum,
  If, While, For, Do,
  Public, Private, Protected,
  Sealed, Final, Internal,
  Throw, Catch, Finally,
  Get, Set,
  Namespace, Using,
  Equals, Plus, Times, Divide, Minus, // = + * / -
  PlusEquals, MinusEquals, TimesEquals, DivideEquals,// += -= *= /=
  GreaterThan, LessThan,// > <
  LogicalOr, LogicalAnd, LogicalNot, // || && !
  BitwiseOr, BitwiseAnd, BitwiseNot, // | & ~
  RShift, LShift,// >> <<
  Dot, Colon,// . :
  Inc, Dec, //++ --
  New, //new 
  Semicolon,//;
  LBrace, RBrace, //{ }
  LParen, RParen, //( )
  LBracket, RBracket,//[ ]
  VerbatimLiteral, //@
  SQuote, DQuote,// ' "
  TokensEnd,
  
  //Variables
  Word, //Word is defined as a token, but it's also a grammatical construct.  It's a defualt item if not in token table.
  Number,

  //////////////////////////////////////////////////////////////////////////
  //Grammar 
  DataType, //int, float, class..
  String, //""
  StringLiteral, //''
  ZeroClosure,  //*
  PositiveClosure,  //+
  Block, // {..} OR Inline block (single line) ** 
  Expression, // any values id|(..)
};
typedef CSTokenType LT;

enum CSContext {
  Global, Namespace,
  ClassBody, MethodBody,
  Inherited //Inherited from the enclosing context
};
class CSGrammarExp {
public:
  CSGrammarExp() {}
  CSGrammarExp(std::vector<CSTokenType> toks, CSContext ctx) { _grammar = toks; _context = ctx; }

  std::vector<CSTokenType> grammar() { return _grammar; }
  std::vector<CSContext> contexts() { return _contexts; }
  CSContext context() { return _context; }
private:
  std::vector<CSTokenType> _grammar;
  std::vector<CSContext> _contexts;//Valid contexts
  CSContext _context;//The context of this expression
};
string_t code = "using System;\nnamespace TestNS\n{\nclass TestClass\n{\nint x;\nint testMethod(int param)\n{\nthis.x+=param;//test comment\n}\n}\n}/*end namespace, also, test comment*/\n\n\n";
string_t invalid_code = "using System;\nclass TestClass\n{\nint x;\nint testMethod(int param)\n{\nthis.x+=param;//test comment\n}\n}\nint invalid_field_outside_class;\n\n";

class GrammarTest {
public:
  void test() {

    //CSGrammarExp xp_namespace(std::vector<CSTokenType>({ LT::Namespace, LT::Identifier, LT::Block }), CSContext::Namespace);
    //xp_namespace.contexts().push_back(CSContext::Global);
    //xp_namespace.contexts().push_back(CSContext::Namespace);

    //CSGrammarExp xp_using_directive(std::vector<CSTokenType>({ LT::Using, LT::Identifier, LT::Semicolon }), CSContext::Global);
    //xp_using_directive.contexts().push_back(CSContext::Global);

    //CSGrammarExp xp_using_inline(std::vector<CSTokenType>({ LT::Using, LT::Identifier, LT::Block }), CSContext::MethodBody);
    //xp_using_inline.contexts().push_back(CSContext::MethodBody);

    //CSGrammarExp xp_class(std::vector<CSTokenType>({ LT::Class, LT::Identifier, LT::LeftBrace, LT::ZeroClosure, LT::RightBrace }), CSContext::ClassBody);
    //xp_class.contexts().push_back(CSContext::Global);
    //xp_class.contexts().push_back(CSContext::Namespace);

    //CSGrammarExp xp_declare(std::vector<CSTokenType>({ LT::DataType, LT::Identifier, LT::Semicolon }), CSContext::Inherited);
    //xp_declare.contexts().push_back(CSContext::MethodBody);
    //xp_declare.contexts().push_back(CSContext::ClassBody);

    //CSGrammarExp xp_assign(std::vector<CSTokenType>({ LT::DataType, LT::Identifier, LT::Equals, LT::Expression, LT::Semicolon }), CSContext::Inherited);
    //xp_assign.contexts().push_back(CSContext::MethodBody);
    //xp_assign.contexts().push_back(CSContext::ClassBody);

    //CSGrammarExp xp_property(std::vector<CSTokenType>({ LT::DataType, LT::Identifier, LT::LeftBrace, LT::Get, LT::Set, LT::RightBrace }), CSContext::Inherited);
    //xp_property.contexts().push_back(CSContext::ClassBody);
    /*
    3 Compile Phases && Phase 1 and 2 can be rolled together

      **Output data of this operation should be a CSharpStruct, with the internal CSMethod, variables, statements, and control blocks**


     Example:
       class B { int x = (5 + (myvar/2)); }
     Like: CSConstruct

        <CSToken> Class, Identifier, LBrace, DataType, Identifier, Equals, LParen, Number, Plus, LParen, Identifier, Divide, Number, RParen, RParen, Semicolon, RBrace

        <CSConstruct>  Class, Identifier, Block -> {  Datatype, Identifier, Equals, Statement -> {Constant, Plus, Statement -> {Identifier, Divide, Constant } }, Semicolon }
          CSConstruct has children if CSConstructType is a Block, Statement, or ArrayBounds

    Note:
        I imagine we'll need to differentiate Class_Begin and Class_End, instead of dividing into blocks, in order to parse internal contents.  As you can have a
            class MyClass { = 1 grammar
        without an end brace, but if you're parsing constructs first, then you won't parse the internal contents of the class.
        BClass, EClass, BMethod, EMethod, BStatement, EStatement, BArray, EArray


        //GrammarConstruct will contain the clean, and positive closures.
        class CSGrammarToken {
        public:
          const int KleeneClosure = -1
          const int PositiveClosure = -2;
          CSGrammarType _type;    //the lexical type.
          int _count = -1, -2, 0+;   -2 = 1 or more (Positive).  -1 = 0 or more (Kleene)

          CSGrammarToken(CSGrammarType type, int count)
        };


      [] 0 or 1
        <CSGrammar>
            <CSGrammarToken> =  [CSScopeModifier] [CSAccessModifier] CSStructure [Colon CSInheritedClass] Block

          Note SUB-GRAMMARS.  A statement grammar can't be in global scope.  Yet, this is NOT how C# is compiled.  They don't take context into consideration when matching patterns.


                ** Construct the grammar tree. **

                Const_Match_List[DataType] [ Int, Float, Class_Ident ]


                  //Create the code structures, class, methods, etc by matching tokens.
                  createCode()
                  {
                    int grammar_position = 0;
                    List<grammars> grams = AllGrammars.

                    for each CSGrammarToken cc in CodeFile  //.. class, identifier, block
                    {
                      checkGrammar(cc, grams)
                    }
                  }

                  checkGrammar(CSGrammarToken cc, &grams, &grammar_position)
                  {
                      prev_grams = grams.copy;
                      grams = filter(cc, &grams, &grammar_position)
                      grammar_position++

                      if prev_grams.length == 1
                      {
                        if(grams.length ==0)
                        {
                          //we were onto a grammar, but it got messed up
                          error "expected" + prev_grams[0][grammar_position-1]
                        }
                        else if( grams.length==1 && grammar_position == gram.size())
                        {
                          //reset grammar pos, for a new grammar.
                          grammar_position = 0;

                          //create the code structure from this grammar, and parse its contents
                          structure = gram.createLanguageStructure()    //such as CSharpStructure
                          structure.parseInternal(this (which is, the parser), grams
                        }
                      }
                      else
                      {
                        if grams.length == 0
                        {
                          //We had multiple possible grammars, but we don't know which one.
                          error "unexpected" +tok
                        }
                      }
                   }
                  filter(cc, &grams, &grammar_position)
                  {
                      list new_grams

                      foreach gram in grams
                      {
                        if grammar_position < grams.length
                        {
                          if matchType(gram[grammar_position], cc)
                          {
                            new_grams.add(gram)
                          }
                        }
                      }

                      return new_grams;
                  }
                  matchType(Lex gram, Lex tok)
                  {
                      //const match list is an array mapping one lexical type to another.
                      // For example:
                      //     ClassModifier = Public | Private | Protected
                      //A lexical type cannot be a token type.  Block != {
                      if( Const_Match_List[gram].contains(tok) ){
                        return true;
                      }
                      return false;
                  }



    //Here we define our own "regex" matching system.  We omit the character matching of regex's in place of using
    //  enum identifiers.  This will be a lot quicker.

    //we need a way to allow a G_G or CSToken in the constructor.

    class __G {
      bool match(tok){
        if(tok == _tok){
          return true;
        }
        return false;
      }
      CSToken _tok
    }
    enum G_Closure { One, Kleene, Positive }
    //Basically a statement ( ) in a regex * or +
    __G_G : __G {
      G_Closure _closure  = G_Closure::One; //= One,  Kleene(K), Positive(P)
      std::vector<G_G> _exprs;
      bool match(tok) = 0
      G_G(std::vector<G_G> exprs, closure = G_Closure::One){
        _exprs = exprs;
      }
    }
    //A statement separating with | (a|b|c..)
    __G_OR : __G_G
      bool match(tok) {
        foreach g in <G>{
          if tok == g{
            return true
           }
        }
        return false;
      }

      //we might be able to pass { } in instead of requiring defines
      #define G_OR(v, k) (new __G_OR(std::vector<__G_G>({v}), k))
      #define G_G(v, k) (new __G_G(std::vector<__G_G>({v}), k))
      #define CT_ CSToken::

      lexing
      structure validation << may not be possible
      grammar checking
      code generation (for us, meta-class creation)

      std::map<string_t, G_G> grammars;

      //Recursive Descent (LL) parsers are easy to understand and write
      //LR requires grammar and is apparently faster.  It has a single lookahead terminal to determine grammatical structure.
      //We're not following a formal Backus Naur system.  Just make it work ok.

      //Dynamic programming is eliminating recursion by storing recursion results in variables and using a loop instead.
      //LR avoids recursion by dynamic programming, thus eliminates time complexity of an LL parser.
      //shift-reduce - an efficient table-driven method of creating a "parse tree"
      //parse-tree
      // a = 3;
      // term assign 4 delim
      // reduce to
      // variable_assignment
      //https://web.archive.org/web/20160305041504/http://dragonbook.stanford.edu/lecture-notes/Stanford-CS143/08-Bottom-Up-Parsing.pdf
      "Shift-reduce parsing is the most commonly used and the most powerful of the bottom-up techniques"
      Shift = (push next token from input on stack, advance input)
      Reduce = (pop right-hand side of production off stack, push left-hand side, no change in input)   `
      Each grammar is a replace/rewrite system of some kind.
      This is actualy really easy to create either way, the grammars are just going to be hard.
      You must validate a reduction, because you can reduce to a dead end.
      An LR parser uses two tables:
      1.The action table Action[s,a] tells the parser what to do when the state on top of the stack is s and terminal a is the next input token.
      The possible actions are to shift a state onto the stack, to reduce the handle on top of the stack, to accept the input, or to report an error.
      2.The goto table Goto[s,X] indicates the new state to place on top of the stack after a reduction of the nonterminal X while state s is on top of the stack.

      LL is recursive descent and less powerful than LALR.  LR will allow us to show multiple errors and such.

      if there is an error, clean the  stack, then you can process stuff after an error

      CSRegEx

      //////////////////////////////////////////////////////////////////////////
      //Lexical definitions
      using := "using"
      namespace := "namespace"
      class := "class"
      public := "public"
      private := "private"
      protected := "protected"
      final := "final"
      sealed := "sealed"
      internal := "internal"
      lbrace := "{"
      rbrace := "}"
      lparen := "("
      rparen := ")"
      lbracket := "["
      rbracket := "]"
      inc := "++"
      dec := "--"
      semicolon := ';'
      equals := "="
      plusequals := "+="
      minusequals := "-="
      plus := "+"
      minus := "-"
      times := "*"
      divide := "/"
      dot := "."
      colon := ":"
      new := "new"

      //////////////////////////////////////////////////////////////////////////
      //Grammar Table... jeez this is crazy.
      //still needs: Arrays, and New classes
      storage_operator := (equals|plusequals|minusequals|timesequals|divideequals)
      binary_operator := (plus|minus|times|divide|dot)
      unary_operator := (inc|dec|minus)
      keyword := (class|public|private|protected|...)
      storage_class := (static|const)
      access_modifier := (public|private|protected)
      state_modifier := (final|sealed|internal);

      using_directive := using (scoped_access_operator)* (namespace_identifier|class_identifier) semicolon
      namespace_decl := namespace lbrace (class_decl|enum_decl)* rbrace

      enum_decl := (access_modifier)? enum lbrace enum_identifier_decl rbrace

      class_decl := (static)? (access_modifier)? (state_modifier)? class class_identifier_decl (inherited_class)? lbrace (method_decl|property_decl|class_variable_decl)+ rbrace
      struct_decl := (state_modifier)? struct identifier lbrace (method_decl|property_decl|class_variable_decl)+ rbrace

      variable_identifier_decl := create_identifier(class) // this function will create a new identifier CSharpVariable, CSharpStruct or CSharpMethod, if its already found in the current context, then emit error.
      variable_identifier := lookup_identifier(variable) // - this is a method that we use to find the declared identifer in the given scope
      class_identifier_decl := create_identifier(variable)
      enum_identifier_decl := create_identifier(enum)
      method_identifier_decl := create_identifier(method)
      property_identifier_decl := create_identifier(property)

      inherited_class := colon class_identifier

      property_decl := data_type property_identifier_decl ((lbrace ( (get (method_block)? semicolon)? (set (method_block)? semicolon)? )? rbrace)|semicolon)
      data_type := (integral_type|user_type)
      integral_type := (int|float|decimal|double|Int32)
      method_block := lbrace (statement)* rbrace

      class_variable_decl := (access_modifier)? (storage_class)? (variable_decl | variable_assign_decl)

      variable_inc_dec := (pre_incdec|post_incdec)
      variable_decl := data_type variable_identifier_decl semicolon
      variable_assign_decl := data_type identifier equals expression semicolon
      variable_assignment := variable storage_operator expression semicolon
      pre_incdec := (unary_operator variable_identifier)
      post_incdec := (variable_identifier unary_operator)

      statement := (variable_inc_dec | variable_assignment | variable_decl | variable_decl_assignment | method_call)  // a +=b  a = b a-- --a
      constant := number
      number := floating_point_number|int32_number|int64_number|decimal_number
      decimal_number := [0-9]*.[0-9]*d

      scoped_access_operator := (namespace_identifier|class_identifier) dot
      local_method_identifier := lookup_identifier(method) -- this must take into account the preceding class access operators. if not found, then method_identifier is looked up
      class_identifier := lookup_identifier(class)    //This is the identifier
      method_call := ((method_identifier|paren_expr) dot method_param)
      method_identifier := ((scoped_access_operator)* local_method_identifier)

      expression := (lvalue | ( (term)+ lvalue) )  //2+(1*5)
      paren_expr := lparen expression rparen
      term := binary_operator lvalue
      lvalue := (constant|variable_identifier|paren_expr|method_call)


      //////////////////////////////////////////////////////////////////////////
      //Root grammars
      variable_decl ==> CSharpVariable
      class_decl ==>  CSharpStruct
      statement ==> CSharpStatement

      what is the grammar for a data type?

      grammars.insert(std::make_pair("class_declaration_begin",
      {
        G_G(G_OR({CT_Public, CT_Protected, CT_Private}), G_Closure::Kleene),
        G_G(G_OR({CT_Final, CT_Sealed, CT_Internal}), G_Closure::Kleene),
        G_G(G_OR({CT_Class, CT_Struct})),
        G_G({CT_Unique_Identifier}),
        G_G(G_G({CT_Colon, CT_Unique_Identifier,1), G_Closure::Kleene),
        G_G({CT_LParen}, K)
      });


    //*Old ideas**
          Lex everything into CSToken, ignore contexts and constructs like {} ()
            class CSToken
            Keyword, Identifier, Number, LBrace, RBrace, LParen, RParen, SQuote, DQuote, Semicolon, Operator
            pump lexical errors
          Create contexts and constructs, moving tokens into blocks, like "" and {} and ()
            class CSConstruct
              <CSConstruct>
            Class, Struct .. (other keywords).. String, Identifier, Datatype (do not check datatype existence yet), Block, Statement
            pump construct errors
              We are not creating the Language classes yet (like CSharpStruct).  We're scoping all blocks, and making a 'tree' of datatypes.
              Note Code blocks can be either a block, or statement, due to the 'freebie'
          Match grammars
            class CSGrammar
            pump grammar errors
      */

      /*
      for all lexed tokens
        for all grammars, match a grammar.
          WHEN NO grammar matches.
            if PREVIOUSLY only ONE grammar matched
              try to say that you were matching a grammar, but you have an error
              expected '' (expected { } after class)
              error: unexpected '..'
      */

  }
};


class CSToken {
public:
  CSToken() {}
  CSToken(string_t tok, CSTokenType type) { _token = tok; _type = type; }
  string_t token() { return _token; }
  CSTokenType& type() { return _type; } //If CSTokenType is keyword, then use keyword() below.
private:
  string_t _token = "";
  CSTokenType _type = CSTokenType::None;
};
class TokenTableEntry {
public:
  TokenTableEntry(string_t s, CSTokenType t) {
    _str = s;
    _tok = t;
  }
  const string_t str() const { return _str; }
  const CSTokenType tok() const { return _tok; }
private:
  string_t _str;
  CSTokenType _tok;
};
class CSLex : public VirtualMemory {
public:
  enum class Ele { Word, Number, Ws, Sym, Undefined };// Element Class
  const string_t c_test = "using System; namespace MyNS { class MyCL{int[] x = new int[]; int b(float a ){ return b + a*(2*(3+.0006d); }} }";
  //We can use Dec or Int.Intf.. which since we need to parse 'f' out I think Int.Int is also relevant.
public:

  CSLex(string_t filename, string_t code) {
    _strFile = filename;
    _tp = std::make_shared<TextParser>(const_cast<char*>(code.c_str()));
    makeTable();
  }
  virtual ~CSLex() override {
    for (auto t : _tokenTable) {
      delete t;
    }
    _tokenHashTable.clear();
  }
  void lex_error(string_t err) {
    BRLogError("" + _strFile + " (" + _tp->linenum() + "," + _tp->charnum() + "): error: " + err);
  }
  void makeTable() {
#define MPA(str,sym) do{ _tokenTable.push_back(new TokenTableEntry(str, CSTokenType::sym)); _tokenHashTable.insert(std::make_pair(str,CSTokenType::sym)); } while(0);
  //  MPA("\t", Tab); MPA(" ", Space);
    MPA("class", Class); MPA("struct", Struct); MPA("enum", Enum);
    MPA("if", If); MPA("while", While); MPA("for", For); MPA("do", Do);
    MPA("public", Public); MPA("private", Private); MPA("protected", Protected);
    MPA("sealed", Sealed); MPA("final", Final); MPA("internal", Internal);
    MPA("throw", Throw); MPA("catch", Catch); MPA("finally", Finally);
    MPA("get", Get); MPA("set", Set);
    MPA("namespace", Namespace); MPA("using", Using);
    MPA("=", Equals); MPA("+", Plus); MPA("-", Minus); MPA("*", Times); MPA("/", Divide);
    MPA("+=", PlusEquals); MPA("-=", MinusEquals); MPA("*=", TimesEquals); MPA("/=", DivideEquals);
    MPA("||", LogicalOr); MPA("&&", LogicalAnd); MPA("!", LogicalNot);
    MPA(">", GreaterThan); MPA("<", LessThan);
    MPA("|", BitwiseOr); MPA("&", BitwiseAnd); MPA("~", BitwiseNot);
    MPA(">>", RShift); MPA("<<", LShift);
    MPA(".", Dot); MPA(":", Colon);  MPA(";", Semicolon);
    MPA("++", Inc); MPA("--", Dec);
    MPA("new", New);
    MPA("{", LBrace); MPA("}", RBrace);
    MPA("(", LParen); MPA(")", RParen);
    MPA("[", LBracket); MPA("]", RBracket);
    MPA("'", SQuote); MPA("\"", DQuote);
    MPA("@", VerbatimLiteral);

    //sort table by grammatical superset
    std::sort(_tokenTable.begin(), _tokenTable.end(), [](const TokenTableEntry* lhs, const TokenTableEntry* rhs) {
      return lhs->str().compare(rhs->str());
      });

    //TODO: check sort is descending
    Gu::debugBreak();

    //validate table
    int c = (int)CSTokenType::TokensEnd - (int)CSTokenType::TokensStart;
    string_t missing = "";
    for (int t = (int)CSTokenType::TokensStart + 1; t < c; ++t) {
      bool found = false;
      for (size_t ti = 0; ti < _tokenTable.size(); ti++) {
        if ((CSTokenType)t == _tokenTable[ti]->tok()) {
          found = true;
          break;
        }
      }
      if (!found) {
        missing += Stz TypeConv::intToStr(t);
      }
    }
    if (missing.length() != 0) {
      BRLogError("Missing Tokens from Lex table: " + missing);
      Gu::debugBreak();
    }
  }
  void lex() {
    char c = ' ';
    while (_tp->eof() == false) {
      c = _tp->charAt();

      if (_tp->eof()) {
        break;
      }
      else {
        matchToken(c);
        _token += c;
        _tp->inc();
      }
    }
  }
  void matchToken(int next) {
    Ele next_ele = getClass(next);

    //Initialize token with first char
    if (_token.length() == 0) {
      _ele = next_ele;
    }
    else {
      process(next, next_ele);
    }
  }
  void process(int next, Ele next_ele) {
    //Taken a set T, a set E+ and element X, !T(EX) -> T(E)
    if (_ele == Ele::Word) {
      if (next_ele == Ele::Ws || next_ele == Ele::Sym) {
        CSTokenType type = findExact(_token);
        if (type == CSTokenType::None) {
          type = CSTokenType::Word;
        }
        pushTok(type, next_ele);
      }
    }
    else if (_ele == Ele::Number) {
      if (next_ele != Ele::Number) {
        //Note we aren't checking for float symbols '.' '-' 'e' 'f' and 'd' here..  we may do that in the grammar step although it's probably not the most optimal
        pushTok(CSTokenType::Number, next_ele);
      }
    }
    else if (_ele == Ele::Sym) {
      if (next_ele == Ele::Sym) {
        CSTokenType type = findExact(_token);
        if (type != CSTokenType::None) {
          CSTokenType test = findExact(_token + next);
          if (test == CSTokenType::None) {
            //Split up, for example +=.5 we want to split += from .
            pushTok(type, next_ele);
          }
        }
      }
      else { //symbol, delimited by number, ws, and word
        CSTokenType type = findExact(_token);
        if (type != CSTokenType::None) {
          pushTok(type, next_ele);
        }
      }
    }
    else if (_ele == Ele::Ws) {
      if (next_ele != Ele::Ws) {
        pushTok(CSTokenType::Whitespace, next_ele);
      }
    }
  }
  CSTokenType findExact(string_t tok) {
    //returns None if not found.
    std::unordered_map<string_t, CSTokenType>::iterator it = _tokenHashTable.find(tok);
    if (it == _tokenHashTable.end()) {
      return CSTokenType::None;
    }
    return it->second;
  }
  bool isWs(int c) {
    return ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
  }
  bool isSym(int c) {
    if ((c == '=') || (c == '+') || (c == '-') || (c == '*') || (c == '/') ||
      (c == '\\') || (c == '~') || (c == '!') || (c == '&') || (c == '|') ||
      (c == ';') || (c == ':') || (c == '.') ||
      (c == '{') || (c == '}') || (c == '(') || (c == ')') || (c == '[') || (c == ']') ||
      (c == '"') || (c == '\'') || (c == '@')
      ) {
      return true;
    }
    return false;
  }
  bool isWord(int next) {
    bool w = isalpha(next) || next == '_';
    return w;
  }
  bool isInt(int next) {
    bool n = isdigit(next);
    return n;
  }
  Ele getClass(int next) {
    Ele ele = Ele::Undefined;
    if (isWs(next)) {
      ele = Ele::Ws;
    }
    else if (isWord(next)) {
      ele = Ele::Word;
    }
    else if (isInt(next)) {
      ele = Ele::Number; // We start as Int, but promote to Dec if we find a '.'
    }
    else if (isSym(next)) {
      ele = Ele::Sym;
    }
    return ele;
  }
  void pushTok(CSTokenType type, Ele next_ele, string_t error = "") {
    if (error.length()) {
      _errors.push_back("Error '" + _token + "' " + error);
    }
    else {
      _parsedTokens.push_back(new CSToken(_token, type));
    }
    resetTok(next_ele);
  }
  void resetTok(Ele next_ele) {
    //reset
    _token = "";
    _ele = next_ele;
  }

private:
  std::unordered_map<string_t, CSTokenType> _tokenHashTable;
  std::vector<TokenTableEntry*> _tokenTable;
  std::vector<CSToken*> _parsedTokens;
  std::vector<std::string> _errors;
  string_t _token;
  Ele _ele;

  //CSTokenType _tokenType;
  bool _bStringProfileDouble = false;
  bool _bStringProfileSingle = false;
  int32_t _nParenProfile = 0;
  int32_t _nBracketProfile = 0;
  bool _bVerbatimLiteral;
  string_t _strFile = "<invalid>";

  bool _bEscape = false;
  std::shared_ptr<TextParser> _tp = nullptr;
};


/*
Note; if you add more delimiters make sure to update isEscapeCharacter()
*/
/**

*/






}//ns BR2
