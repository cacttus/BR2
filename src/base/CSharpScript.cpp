#include "../base/CSharpScript.h"
#include "../base/CSharp.h"
#include "../base/Logger.h"
#include "../base/HashMap.h"

namespace BR2 {
//#ifndef FORCE_INLINE
//#ifdef BR2_OS_WINDOWS
//#define FORCE_INLINE __forceinline
//#else
//#define FORCE_INLINE inline
//#endif
//#endif
//
////May not be necessary
//enum class CSharpScope {
//  Class,
//  Block,
//};
//enum class CSharpAccessModifier {
//  Public,
//  Private,
//  Protected,
//  None, //None is not applicable to variables within method scope.
//};
//enum class CSharpStorageClass {
//  Static,
//  Dynamic, //..not static
//};
//enum class CSharpOperator {
//  Op_Add,
//  Op_Sub,
//  Op_Mul,
//  Op_Div,
//  Op_Assign, // x =
//  Op_Call,  //x()
//  Op_PreInc, //++x
//  Op_PreDec, //--x
//  Op_PostInc, //x++ //This will be hard
//  Op_PostDec, //x--
//  Op_None
//};
//enum class CSharpControlWord {
//  If_Then,
//  Do_While,
//  While_Loop,
//  For_Loop,
//  None
//};
//enum class CSharpParameterClass {
//  None,
//  ByValue,
//  ByRef,
//  Out,
//  In
//};
////[int|float|double|vec2|vec3|vec4|mat4|*Node|Gu]
//enum class CSharpDataType {
//  None,
//  Int16,
//  Int32,
//  Int64,
//  Float,
//  Double,
//  Decimal,
//  Vec2,
//  Vec3,
//  Vec4,
//  Class, //Action<>, Function<>
//};
//enum class CSharpCallingConvention {
//  None,
//  CDecl,
//  StdCall,
//};
//
////class CSharpObject {
////public:
////  
////  string_t ToString() {
////    return "";
////  }
////};
//
//
//class CSharpCallstack : public VirtualMemoryShared<CSharpContext> {
//public:
//  CSharpCallstack(std::shared_ptr<CSharpContext> context) {
//    _pContext = context;
//  }
//  virtual ~CSharpCallstack() override {
//  }
//  std::vector<std::shared_ptr<CSharpVariable>> getItems() { return _callstack; }
//  void push(std::shared_ptr<CSharpVariable> item) {
//    _callstack.push_back(item);
//  }
//  void push(std::vector<std::shared_ptr<CSharpVariable>> items) {
//    for (auto item : items) {
//      this->push(item);
//    }
//  }
//  std::shared_ptr<CSharpVariable> top() {
//    if (_callstack.size() == 0) {
//      _pContext->runtimeError("Stack Underflow.");
//    }
//  }
//  std::vector<std::shared_ptr<CSharpVariable>> pop(size_t count = 1) {
//    std::vector<std::shared_ptr<CSharpVariable>> ret;
//    for (size_t x = 0; x < count; ++x) {
//      std::shared_ptr<CSharpVariable> item = _callstack[_callstack.size() - 1 - x];
//      ret.push_back(item);
//      _callstack.erase(_callstack.begin() + (_callstack.size() - 1 - x));
//    }
//    return ret;
//  }
//private:
//  std::vector<std::shared_ptr<CSharpVariable>> _callstack; //Callstack.
//  std::shared_ptr<CSharpContext> _pContext = nullptr;
//};
//
//
//class CSharpContext : public VirtualMemoryShared<CSharpContext> {
//public:
//  CSharpContext() {}
//  virtual ~CSharpContext() override {}
//  void init() {
//    _callstack = std::make_shared<CSharpCallstack>(getThis<CSharpContext>());
//  }
//  std::shared_ptr<CSharpCallstack> getCallstack() { return _callstack; }
//  void runtimeError(string_t err) {
//    BRLogError("Runtime Error: " + err);
//  }
//  void compileError(string_t err) {
//    BRLogError("" + _strFile + "(" + _iLine + "," + _iChar + "): error: " + err);
//  }
//private:
//  string_t _strFile = "<invalid>";
//  int32_t _iLine = -1; // the compile, or, runtime line.
//  int32_t _iChar = -1; // the compile line char.
//  int32_t _instructionPointer; // the current instruction point.er
//
//  std::shared_ptr<CSharpCallstack> _callstack = nullptr;//Call init()
//};
//
////Language definition classes, class, method, property, code block, variables, statements.
//class CSharpLanguage : public VirtualMemory {
//public:
//  virtual void parse(std::shared_ptr<CSharpContext> context) = 0; // Parse the code.
//  virtual void execute(std::shared_ptr<CSharpContext> context) = 0;  //Execute the code.
//protected:
//  int32_t _iLine = -1;  //The line this element was found on.  If the expression spans multiple lines, this is the first line.
//  string_t _code; //The code that was parsed to create this language item.
//};
//
//
////Namespace
////Essentially this is also a CSharpScope.  however, that isn't necessary.
//class CSharpNamespace : public CSharpLanguage {
//public:
//  string_t getIdentifier() { return _identifier; }
//  std::vector<CSharpClass> getClasses() { return _classes; }
//  std::shared_ptr<CSharpNamespace> parent() {return _parent;}
//  std::vector<std::shared_ptr<CSharpNamespace>> children() { return _children; }
//private:
//  string_t _identifier = "<invalid>";
//  std::vector<CSharpClass> _classes;
//  std::shared_ptr<CSharpNamespace> _parent = nullptr;
//  std::vector<std::shared_ptr<CSharpNamespace>> _children;
//};
//
//typedef uint64_t CSharpTypeHash;
//
////Class Or struct base
//class CSharpStruct : public CSharpLanguage {
//public:
//  CSharpStruct(std::shared_ptr<CSharpNamespace> ns, string_t struct_name) {
//    _iTypeHash = Hash::computeStringHash64bit(struct_name);
//    _namespace = ns;
//  }
//
//  std::shared_ptr<CSharpNamespace> getNamespace() {return _namespace;}
//
//  HashMap<string_t> aliases() { return _aliases; }
//  Hash32 typeHash() { return _iTypeHash; }
//  string_t getIdentifier() { return _identifier; }
//  CSharpDataType getType() { return _eDataType; }
//
//
//  std::map<CSharpOperator, std::map<CSharpTypeHash, std::shared_ptr<CSharpMethod>>> operators() { return _operators; }
//  std::vector<std::function<void()>> cpp_lambdas() { return _cpp_lambdas; }
//
//  //So these should actually be methods.
//  //virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) ;
//  //virtual std::shared_ptr<CSharpVariable> sub(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//  //virtual std::shared_ptr<CSharpVariable> mul(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//  //virtual std::shared_ptr<CSharpVariable> div(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//  //virtual std::shared_ptr<CSharpVariable> increment(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//  //virtual std::shared_ptr<CSharpVariable> decrement(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//
//  //Call is really the () operator, but () can't be overloaded in c#
//  //virtual std::shared_ptr<CSharpVariable> call(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//
//  bool isClass() { return _baseClass != nullptr; }
//
//  HashMap<CSharpVariable> getMembers() { return _members; }
//  HashMap<CSharpMethod> getMethods() { return _methods; }
//
//  //virtual std::shared_ptr<CSharpType> getType(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
//  virtual string_t ToString(std::shared_ptr<CSharpVariable> v, std::shared_ptr<CSharpContext> context) = 0;
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
//  std::map<CSharpOperator, std::map< CSharpTypeHash, std::shared_ptr<CSharpMethod> > > _operators;
//  HashMap<string_t> _aliases;
//
//  //[public|private|sealed|protected] class MyClass [: MySuperClass] {
//  CSharpTypeHash _iTypeHash = 0; /// This is how we're going to locate types.
//  string_t _identifier = "<invalid>";
//  CSharpStorageClass _storageClass = CSharpStorageClass::Dynamic;
//  CSharpAccessModifier _modifier = CSharpAccessModifier::None;
//  CSharpDataType _eDataType = CSharpDataType::None;
//  std::shared_ptr<CSharpClass> _pClass = nullptr;//Not set if we're not a class.
//  HashMap<CSharpVariable> _members;
//  HashMap<CSharpMethod> _methods;
//  std::shared_ptr<CSharpNamespace> _namespace;
//
//  std::vector<std::function<void()>> _cpp_lambdas;
//
//  //If is a class then this WILL be defined.  Base class is object.
//  std::shared_ptr<CSharpClass> _baseClass = nullptr;
//};
////class CSharpType : public CSharpStruct {
////public:
////
////  virtual string_t toString() {
////    if (_eDataType == CSharpDataType::None) { return "None"; }
////    else if (_eDataType == CSharpDataType::Int16) { return "Int16"; }
////    else if (_eDataType == CSharpDataType::Int32) { return "Int32"; }
////    else if (_eDataType == CSharpDataType::Int64) { return "Int64"; }
////    else if (_eDataType == CSharpDataType::Float) { return "Float"; }
////    else if (_eDataType == CSharpDataType::Double) { return "Double"; }
////    else if (_eDataType == CSharpDataType::Class) {
////      return "Class";
////    }
////  }
////private:
////};
//
//
//
////Global Base class for all objects
//class CSharpClass : public CSharpStruct {
//public:
//  std::shared_ptr<CSharpClass> getBase() { return _baseClass; }
//private:
//};
//
////Int
////class CSharpInt : public CSharpStruct {
////public:
////  CSharpInt() {
////    //So we will add each operator method. - + - * () 
////    addMethod(std::make_shared<CSharpMethod>(std::shared_ptr<CSharpInt>, std::shared_ptr<CSharpInt>)
////
////
////  }
////
////  int32_t value() { return _value; }
////  virtual bool canConvertInt32() { return true; }
////  //virtual int32_t toInt32(std::shared_ptr<CSharpContext> context) override {
////  //  return _value;
////  //}
////  //virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> rhs, std::shared_ptr<CSharpContext> context) override {
////  //  if (rhs->getType() == CSharpDataType::Int32) {
////  //    _value += std::dynamic_pointer_cast<CSharpInt>(rhs)->value();
////  //  }
////  //  else if (rhs->getType() == CSharpDataType::Double) {
////  //    BRThrowNotImplementedException();
////  //    //_value += std::dynamic_pointer_cast<CSharpDouble>(rhs)->value();
////  //  }
////  //  else if (rhs->getType() == CSharpDataType::Float) {
////  //    _value += std::dynamic_pointer_cast<CSharpFloat>(rhs)->value();
////  //  }
////  //  else {
////  //    context->runtimeError("Could not add datatypes of '" + getType() + "' and '" + rhs->getType()->toString() + "'");
////  //  }
////  //}
////private:
////  int32_t _value = 0;
////};
//
//////Float
////class CSharpFloat : public CSharpStruct {
////public:
////  int32_t value() { return _value; }
////  virtual bool canConvertInt32() { return true; }
////  virtual toInt32(std::shared_ptr<CSharpContext> context) override {
////    return _value;
////  }
////  virtual std::shared_ptr<CSharpVariable> add(std::shared_ptr<CSharpVariable> rhs, std::shared_ptr<CSharpContext> ctx) override {
////    if (v->dataType() == CSharpDataType::Int32) {
////      _value += std::dynamic_pointer_cast<CSharpInt>(rhs)->value();
////    }
////    else if (v.dataType() == CSharpDataType::Double) {
////      _value += std::dynamic_pointer_cast<CSharpDouble>(rhs)->value();
////    }
////    else if (v.dataType() == CSharpDataType::Float) {
////      _value += std::dynamic_pointer_cast<CSharpFloat>(rhs)->value();
////    }
////    else {
////      context->runtimeError("Could not add datatypes of '" + getType() + "' and '" + rhs->getType() "'");
////    }
////  }
////private:
////  int32_t _value = 0;
////};
//
////
//////Vec2
////class CSharpVec2 : public CSharpVariable {
////public:
////  virtual bool canConvertInt32() override { return false; }
////  virtual toInt32(context) override {
////    BRThrowException("Int Typecast failed for variable '" + getIdentifier() + "'");
////  }
////private:
////  vec2 _value;
////};
//
////Property
////A class Property
//// [int|..] MyProp { get[;|{..}] set[;|{...}] } = [..];
//class CSharpProperty : public CSharpLanguage {
//public:
//  std::shared_ptr<CSharpVariable> _value = nullptr;
//  std::shared_ptr<CSharpMethod> _getter = nullptr;
//  std::shared_ptr<CSharpMethod> _setter = nullptr;
//};
//
////Method
////[Access_modifier] [static] [return_type] method(parms,..) {control block}
//// Arguments are passed onto the stack using the stdcall convention.
//// int MyMeth(float a, ref MyClass b)  < CSharpMethod { CSharpParameter, CSharpParameter }
//// {  < CSharpCodeBlock 
////    int x = 0;      < variable declaration CSharpVariable
////    int y = 5;      < CSharpVariableDeclaration
////    int z = (b.Prop + x);     < CSharpVariableDeclaration
////
//// }
//class CSharpMethod : public CSharpLanguage {
//public:
//  CSharpStorageClass getStorageClass() { return _storageClass; }
//  CSharpAccessModifier getAccessModifier() { return _modifier; }
//  CSharpDataType getReturnType() { return _returnType; }
//
//  virtual void execute(std::shared_ptr<CSharpContext> context) override {
//    //execute() here is likely invalid, because we're using CSharpMethodInvocation below.
//  }
//private:
//  CSharpStorageClass _storageClass;
//  CSharpAccessModifier _modifier;
//  CSharpDataType _returnType;
//  CSharpCallingConvention _callingConvention = CSharpCallingConvention::StdCall; //StdCall - callee cleans the stack.
//
//  //We should also provide the capability to call BR2 classes using the CSharpMethod syntax.
//  std::vector<std::shared_ptr<CSharpParameter>> _params;
//  std::shared_ptr<CSharpScope> _codeBlock = nullptr;
//};
//
////Method Parameter
////The only reason we need this class is to define the call reference, otherwise it's just a csharpvariable
//class CSharpParameter : public CSharpLanguage {
//public:
//  CSharpParameterClass getParameterClass() { return _parameterClass; }
//
//private:
//  std::shared_ptr<CSharpVariable> _variable = nullptr;
//  CSharpParameterClass _parameterClass = CSharpParameterClass::ByValue;
//};
//
////Variable Declaration (Storage class)
////[int|float|double|vec2|vec3|vec4|mat4|*Node] _myVar = [expr];
//class CSharpVariable : public CSharpLanguage {
//public:
//  string_t getIdentifier() { return _identifier; }
//
//  virtual int32_t toInt32(std::shared_ptr<CSharpContext> context) = 0; //Converts to int32
//  virtual bool isInt32(std::shared_ptr<CSharpContext> context) = 0; //Returns false if this doesn't CONVERT to an int32
//
//private:
//  string_t _identifier = "<invalid>"; // The language identifier
//  std::shared_ptr<CSharpClass> _myClass = nullptr;  //nullptr, if this is a local variable.
//  CSharpDataType _dataType = CSharpDataType::None; //This can basically be gotten from the subclass.
//};
//
//
////Scope -- Code block
//// { .. }
//class CSharpScope : public CSharpLanguage {
//public:
//  virtual void execute(std::shared_ptr<CSharpContext> context) override {
//    //Push all the locals on the callstack.
//    //Note: C# does not allow redefining enclosing scope parameters.  so you wouldn't have to do that.
//
//    for (auto expr : _expressions) {
//      expr->execute(context);
//    }
//  }
//private:
//  std::vector<std::shared_ptr<CSharpStatement>> _expressions;
//};
//
////Statement
//// delimited by a semicolon. Ex:
//// myvar = (0 + variable *2 + (1/5 * myclass->method() * -1));
////      0 term - term
////      + variable - term
////      * 2 - term
////      + (1/5 * myclass.Method()) - subexpression
////            1 - term
////            /5 - term
////            * myclass.Method() - term 
////                myclass - term
////                .Method() - term
////            * -1 - term 
////              -1 - term -  unary operator is its own term.
//// An expression, may also be a statement, Equals is allowed here.
////  Note: Only assignment, call, increment, decrement, await, and new object expressions can be used as a statement
//// ** Variable may also be declared here.  The variable must be placed on the stack, if so.
//class CSharpStatement : public CSharpLanguage {
//public:
//  std::shared_ptr<CSharpVariable> evaluate(std::shared_ptr<CSharpContext> context) {
//    std::shared_ptr<CSharpVariable> evaluated_value;
//    context->getCallstack()->push(evaluated_value);
//    for (auto term : _terms) {
//      term->execute(context);
//    }
//    return evaluated_value;
//  }
//  virtual void execute(std::shared_ptr<CSharpContext> context) override {
//    evaluate(context);
//  }
//private:
//  //Here a variable may also be declared.
//  std::vector<std::shared_ptr<CSharpTerm>> _terms; //These come In order
//};
//
////Method Invocation
//// my_class_instance.MyMethod(par1,par2,..);
////or
//// Method(par1,par2..);
//class CSharpMethodInvocation : CSharpLanguage {
//public:
//  virtual void execute(std::shared_ptr<CSharpContext> context) override {
//    //Evaluate parameters
//    std::vector<std::shared_ptr<CSharpVariable>> evaluated;
//    for (auto param : _params) {
//      evaluated.push_back(param->evaluate(context));
//    }
//    //Push evaluated parameters onto stack.
//    context->getCallstack()->push(evaluated);
//
//    //*****This is incorrect.
//
//    std::shared_ptr<CSharpVariable> var;
//
//    //Push Number of parameters onto stack.
//    context->getCallstack()->push(std::make_shared<CSharpInt>(_params.size()));
//    
//    _method->execute(context);
//  }
//private:
//  std::shared_ptr<CSharpMethod> _method = nullptr;
//  std::vector<std::shared_ptr<CSharpStatement>> _params = nullptr;
//};
//
////Reference to a variable, in a method.
//class CSharpVariableReference : public CSharpLanguage {
//public:
//  //TODO:
//private:
//  int32_t _iVarId = -1;  //The index of the variable, in the method's compiled variable table.  This includes the method parameters.
//
//};
//
////Expression Term
////  A term, an operator "+,-,*,/" and a variable, method, or constant
//// constant "2" , variable "2", or sub-expression ()
//class CSharpTerm : public CSharpLanguage {
//public:
//  virtual void execute(std::shared_ptr<CSharpContext> context) override {
//    //Our curently operating variable is on the top of the stack.
//    std::shared_ptr<CSharpVariable> value = context->getCallstack()->top();
//
//    if (_variable != nullptr) {
//      if (_operator == CSharpOperator::Op_None) {
//        //This is the left hand side, return it.
//        value.assign(_variable);
//        retur value;
//      }
//      else if (_operator == CSharpOperator::Op_Add) {
//        value.add(_variable);
//      }
//      //..sub,mul,div
//      else if (_operator == CSharpOperator::Op_Increment) {
//        value.increment();
//      }
//      //...
//      else {
//        context->runtimeError("No condition for oprator '" + _operator + "'");
//      }
//    }
//    else if (_methodInvocation != nullptr) {
//      _methodInvocation->
//        if (_operator == CSharpOperator::Op_None) {
//          //This is the left hand side, return it.
//          value.assign(_variable);
//          retur value;
//        }
//    }
//  }
//private:
//  CSharpOperator _operator = CSharpOperator::Op_None; // =, +, -, *, /, call, or None for first term (first term has no operator, and is the storage)
//
//  //Only one of these will be set.
//  std::shared_ptr<CSharpVariableReference> _variable = nullptr; // this may be a constant.
//  std::shared_ptr<CSharpStatement> _statement = nullptr;
//  std::shared_ptr<CSharpMethodInvocation> _methodInvocation = nullptr;
//};
//
////Control Statement
//// if then, while, do, for
//class CSharpControlStatement : public CSharpLanguage {
//public:
//  virtual void execute(std::shared_ptr<CSharpContext> context) override {
//    if (_controlword == CSharpControlWord::If_Then) {
//      std::shared_ptr<CSharpVariable> value = _condition.evaluate();
//      if (!value.canConvertInt32()) {
//        //This should have failed in compilation though.
//        context->runtimeError("Failed to convert '" + value->getIdentifier() + "' to an int32.");
//      }
//      else if (value.toInt32() > 0) {
//        _codeBlock->execute(context);
//      }
//    }
//  }
//private:
//  CSharpControlWord _controlword = CSharpControlWord::None; //ifthen, while, for, do
//  std::shared_ptr<CSharpStatement> _condition = nullptr; // (var > 2)
//  std::shared_ptr<CSharpScope> _codeBlock = nullptr; // { do this .. }
//};
//
//
//#pragma region CSharpScript_Internal
//class CSharpUsing {
//public:
//  string_t _string;
//};
//
//class CSharpScript_Internal {
//public:
//  //**CLASSES AS WELL.  Classes can exist without namespaces.  Thus, so can enums, etc ..
//  std::vector<std::shared_ptr<CSharpNamespace>> _namespaces;
//  void compile(std::shared_ptr<CSharpCompileContext> context);
//};
////Script level language context
//enum class C_Script {
//  None, 
//  NamespaceDecl, NamespaceId,
//  UsingDecl, UsingId
//};
//void CSharpScript_Internal::compile(std::shared_ptr<CSharpCompileContext> context) {
//  //*Classes can exist without namespaces too.
//
//  //getTok method should tokenize all delimiters, even such as: 'namespace x;'
//  // returning: namespace, x, and ;
//  // we have: delimiters we return, such as '.' and ';'
//  // whitespace delimiters are not returned
//  
//  //check for existence of the identifier.
//  // then check to see if its in the right place.
//
//  char* c;
//  string_t code = "using System;\nnamespace TestNS\n{\nclass TestClass\n{\nint x;\nint testMethod(int param)\n{\nthis.x+=param;//test comment\n}\n}\n}/*end namespace, also, test comment*/\n\n\n";
//  string_t tok;
//  string_t invalid_code = "using System;\nclass TestClass\n{\nint x;\nint testMethod(int param)\n{\nthis.x+=param;//test comment\n}\n}\nint invalid_field_outside_class;\n\n";
//
//  C_Script ctx = C_Script::None;
//  while (code.length()) {
//    tok = context->getTok(code, c); //return tok, also eat the string.
//
//    if (StringUtil::equals(tok, "namespace")) {
//      //it looks like after anything invalid, the parser just parses regardless.
//      ctx = C_Script::NamespaceDecl;
//      
//      context->compileError("The name '" + tok + "' does not exist in the current context.");
//      context->compileError("A namespace cannot directly contain members such as fields or methods.");
//    }
//    else if (StringUtil::equals(tok, "{")) {
//      if (ctx == C_Script::None) {
//        ctx = C_Script::NamespaceDecl;
//      }
//    }
//    else if (StringUtil::equals(tok, ";")) {
//      if (ctx == C_Script::None) {
//        ctx = C_Script::NamespaceDecl;
//      }
//    }
//    else /*identifier, or, error*/ {
//      if (ctx == C_Script::NamespaceDecl) {
//        //this is an identifier
//        //Invalid identifiers spit out 'Identifier Expected'
//        if (context->validateIdentifier(tok)) {
//          C_Script::NamespaceDecl
//        }
//        else {
//
//        }
//
//      }
//      else if (ctx == C_Script::NamespaceId) {
//      //Error
//        context->compileError("{ expected");
//      }
//    }
//  }
//  //gather all using's
//  // using->add(make_shared<CSharpUsing)
//  //* A using clause must precede all other elements
//  //gather all namespaces.
//  //  std::make_shared<CSharpNamespace>( string_t internal_code )
//  // foreach namespace
//  //   namespace->compile(context)
//
//
//
//}
//
//
//#pragma endregion
//
//#pragma region CSharpScript
//
//CSharpScript::CSharpScript() {
//  _pint = std::make_unique<CSharpScript_Internal>();
//}
//CSharpScript::~CSharpScript() {
//  _pint = nullptr;
//}
//
//////execute is only available for the root class.
////virtual void execute(std::shared_ptr<CSharpContext> context) override {
////  if (StringUtil::equals(_baseClass->getIdentifier(), "Component")) {
////    std::shared_ptr<CSharpMethod> m = GetType().GetMethods();
////
////    if (_start) {
////      _start->execute(context);
////    }
////  }
////  else {
////    BRThrowException("Attempted to execute a class that was not derived from a script component.");
////  }
////}
// 
//void CSharpScript::initScriptSystem() {
//  //Here define all the built-in classes like int, float, etc..
//
//  std::shared_ptr<CSharpNamespace> cs_System = std::make_shared<CSharpNamespace>("System");
//
//  //Struct Definitions
//  std::shared_ptr<CSharpStruct> cs_Int32 = std::make_shared<CSharpStruct>(cs_System, "Int32");
//  cs_Int32->aliases().add("int");
//
//  //So to execute an integral type operator method we're creating our own integral types, with methods.
//  // CSharpMethod can be either a set of CSharpStatement's or a (set of) C++ lambda(s).
//  // For integral type operator methods we use a c++ lambda.
//
//  //Operators
//
//  //Int32
//  std::map<CSharpTypeHash, std::shared_ptr<CSharpMethod> > add_methods;
//  //Int32 + Int32
//  std::shared_ptr<CSharpMethod> cs_Int32_plus_Int32= std::make_shared<CSharpMethod>();
//  cs_Int32_plus_Int32->cpp_lambdas().push_back([]() {
//
//    })
//
//  add_methods.insert(std::make_pair(cs_Int32->typeHash(), cs_Int32_plus_Int32));
//
//  cs_Int32->operators().insert(std::make_pair(CSharpOperator::Op_Add, add_methods));
//}
//std::shared_ptr<CSharpScript> CSharpScript::compile(std::shared_ptr<CSharpCompileContext> ct) {
//
//  //gather all class references first.
//  if (!ct->_bGatheredClasses) {
//    //gather
//  }
//  std::shared_ptr<CSharpScript> ret = std::make_shared<CSharpScript>();
//  ret->_pint->compile(ct);
//
//  std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
//
//
//
//}
////This is the start method that's in C++
//void CSharpScript::start() {
//  //**If the compiled script has a start method, run that, otherwise run the _start lambda on this (c++) script.
//
//  //if (_start == nullptr) {
//  //  BRLogErrorCycle("Script could not execute, no Start method was found");
//  //}
//  //else {
//  //  std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
//  //  context->init();
//  //  _start.execute(context);
//  //}
//}
//void CSharpScript::update(float dt) {
//  //if (_update == nullptr) {
//  //  BRLogErrorCycle("Script could not execute, no Start method was found");
//  //}
//  //else {
//  //  std::shared_ptr<CSharpContext> context = std::make_shared<CSharpContext>();
//  //  context->init();
//  //  std::shared_ptr<CSharpFloat> delta = std::make_shared<CSharpFloat>();
//  //  delta.assign(dt);
//  //  context->getCallstack().push_back(delta);//*Push this parameter on the stack so Update() will receive it.
//  //  _update.execute(context);
//  //}
//}
//#pragma endregion
//
#pragma region CSharpScript_Internal
class CSharpScript_Internal {
public:
  //std::shared_ptr<CSharpMethod> _start = nullptr;
  //std::shared_ptr<CSharpMethod> _update = nullptr;
};

#pragma endregion
//
//
//
///*
//Note; if you add more delimiters make sure to update isEscapeCharacter()
//*/
///**
//char c;
//  //char* ptr = buf;
//  string_t token;
//  int32_t _nStringProfile = 0;
//  int32_t _nParenProfile = 0;
//  int32_t _nBracketProfile = 0;
//
//  _iLine = 1;
//
//  bool _bEscape = false;
//
//  TextParser tp(code);
//
//  while (tp.eof() == false) {
//    c = tp.charAt();
//
//    if (tp.eof()) {
//
//      break;
//    }
//    else if (c == '"') {
//      _nStringProfile = ~_nStringProfile;
//
//      token += c;
//      tp.inc();
//    }
//    else if (_nStringProfile) {
//      //Inside string - do not parse other delimiters
//      token += c;
//      tp.inc();
//    }
//    else if (c == '(') {
//      _nParenProfile++;
//
//      parseExpression(')');
//
//      token += c;
//      tp.inc();
//    }
//    else if (c == ')') {
//      _nParenProfile--;
//
//      if (_nParenProfile < 0) {
//        displayError("Parentheses profile is invalid. You're missing a parentheses somewhere.");
//      }
//
//      token += c;
//      tp.inc();
//    }
//    else if (c == '[') {
//      _nBracketProfile++;
//
//      token += c;
//      tp.inc();
//    }
//    else if (c == ']') {
//      _nBracketProfile--;
//
//      if (_nBracketProfile < 0) {
//        displayError("Bracket profile is invalid. You're missing a Bracket somewhere.");
//      }
//
//      token += c;
//      tp.inc();
//    }
//    else if (c == '\n' || c == '\r') {
//
//      tp.eatLine(_iLine);
//    }
//    // - If we're whitespace but we also are within a string or bracket then do not skip it, add it to
//    // the argument.
//    else if (StringUtil::isWs(c) && !(_nStringProfile || _nParenProfile || _nBracketProfile)) {
//      if (token.length()) {
//        _vecTokens.push_back(string_t(token));
//      }
//      token = "";
//      tp.inc();
//    }
//    else {
//      token += c;
//      tp.inc();
//    }
//
//  }
//*/






}//ns BR2
