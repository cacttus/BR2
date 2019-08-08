///**
//*
//*    @file OperatingSystemHeader.h
//*    @date October 9, 2014
//*    @author Derek Page
//*
//*    © 2014 
//*
//*
//*/
//#pragma once
//#ifndef __OPERATINGSYSTEMHEADER_1315321194287381982523307_H__
//#define __OPERATINGSYSTEMHEADER_1315321194287381982523307_H__
//
//#include "../base/BuildConfig.h"
//
//typedef enum {
//    RE_WIN32,
//    RE_WIN64,
//    RE_LINUX32,
//    RE_LINUX64,
//}RuntimeEnvironment;
//
////For operating system configuration see BuildCOnfig.h
//
//#ifdef BRO_OS_WINDOWS
//#define _CRT_SECURE_NO_DEPRECATE
////**These must come in order
//#ifdef _WIN32
//#undef BRO_X64
//#define BRO_X86
//#endif
//
//#ifdef _WIN64
//#define BRO_X64
//#undef BRO_X86
//#endif
//#else
//#if __GNUC__
//
//#if __x86_64__ || __ppc64__
//#define BRO_X64
//#else
//#define BRO_X86
//#endif
//
//#else
//    //OS bits not implemented
//    #error "Operating System Error"
//#endif
//
//#endif
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#ifdef BRO_OS_WINDOWS
//
////Note: this must be defined to add winsock before including windows.h
//#define _WINSOCKAPI_
////Lean windows library.
////#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
//
////#pragma warning( disable:4251 ) /* vc++ "needs to have dll-interface" warning */
//
//
//
//
//
//// - TODO: causes slow memory !
//#ifdef _DEBUG
////Force CRT to use the debug verions of new/malloc
////#define CRTDBG_MAP_ALLOC
//#endif
////
////#define WIN32_LEAN_AND_MEAN
////#define WIN32_EXTRA_LEAN
////#include <Windows.h>
////#include <Winbase.h>
//
//// For get process memory info
////#include <Psapi.h>
////#pragma comment(lib,"psapi.lib")
//
////Debugging information
//
//
////#ifdef BRO_X86
////Required for stack trace, &c
////#include <TlHelp32.h>
////#endif
////Needed for SHGetFolderPath
////#include <shlobj.h>
//
////#include <DbgHelp.h>
////#pragma comment(lib,"Dbghelp.lib")
//
////Directx
////#ifdef BRO_USE_DIRECTX
////
//////Dx Error
////#include <DxErr.h>
////
//////Direct Input
////#define DIRECTINPUT_VERSION 0x0800
////#include <dinput.h>
////
//////Direct Sound.  MMsystem required windows sound system
////#include <MMSystem.h>
////#include <dsound.h>
////#pragma comment(lib,"dsound.lib")
////#endif
////BRO_USE_DIRECTX
////
////#ifdef __APPLE__
////#ifdef TARGET_OS_IPHONE
////#define IS_IPHONE 1
////#endif
////#elif __WINDOWS__
////#define IS_DESKTOP 1
////#define BRO_OS_WINDOWS 1
////#endif
////
//////Winsock
////
////
////#endif
//////BRO_USE_WINSOCK
////
//////BRO_OS_WINDOWS
////#elif BRO_OS_ANDROID
//////#include <android/cpu-features.h>
////#endif
////typedef enum {
////    OS_MSG_QUIT,// the window received a quit message.
////    OS_MSG_NONE// no message specified, or invalid message
////}OsWindowMessage;
//
////BRO_OS_ANDROID
////BRO_OS_WINDOWS
//
////#ifndef BRO_OS_WINDOWS
//////We need to define stdcall
////#define __stdcall
////#endif
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////#include "../base/StlHeader.h"
//
////typedef int errno_t;
//
////Threading
////Threading
////Threading
////Threading
////Threading
////Threading
////Threading
////Threading
////#define MAIN_THREAD_ID (0)
////#define MLT_SYS_THREAD_ID (-1)
////
////#ifdef BRO_OS_WINDOWS
////typedef DWORD (WINAPI *ThreadCallbackMethod)(void*);
////#define ThreadHandle HANDLE
////#else
////typedef unsigned long (__stdcall *ThreadCallbackMethod)(void*);
////#define ThreadHandle uint32_t
////#endif
//
////This enum is the result of the windows thread message procedure.
//
////
////#define NOT_VIRTUAL
////#define STATIC static
////#define PRIVATE_METHOD
////#define PUBLIC_METHOD
////
////#ifdef BRO_OS_WINDOWS
////#define FORCE_INLINE __forceinline
////#else
////#define FORCE_INLINE inline
////#endif
//
////input / output / inout / ignored parameters
////THESE CONFLICT WITH ANDROID's C++ LIBRARY
// //#ifndef __inout_ 
// //#define __inout_
// //#endif
//
// //#ifndef __out_ 
// //#define __out_
// //#endif
//
// //#ifndef __in_ 
// //#define __in_ 
// //#endif
//
// //#ifndef __ignored
// //#define __ignored
// //#endif
//
// ////optinoal parameter
// //#ifndef __opt_ 
// //#define __opt_
// //#endif
//
// ////Reference parameter (class does not own data)
// //#ifndef __ref_ 
// //#define __ref_
// //#endif
//
// ////By Value param
// //#ifndef __byval_
// //#define __byval_
// //#endif
//
////#ifndef TRUE
////#define TRUE true
////#endif
////#ifndef FALSE
////#define FALSE false
////#endif
////
////#ifdef _MSC_VER
////#define BRO_DEBUG_BREAK __debugbreak()
////#else
//////**raise(SIGTRAP) doesjn't work on andreodid, neither does assert(0) wtf??
////#ifdef NDEBUG
//////**NDEBUG removes assert
////#define BRO_DEBUG_BREAK
////#else
////#define BRO_DEBUG_BREAK assert(0)
////#endif
////#endif
////
////#define RUNTIME_SOLID_INTEGER ((unsigned int)(sizeof(unsigned int)*0xFF))
////
//
////#ifdef MAX_PATH
////#define BRO_MAX_PATH MAX_PATH
////#else
////#define BRO_MAX_PATH PATH_MAX
////#endif
//
////////////////////////////////////////////////////////////////////////////
//
//// - Units for the system - 
//// this is not used globally but loosely so don't depend on it ehverywehrey
////    see: Metric.h
////#define UNITS_PER_METER (1.0f)
//
//// - The application runtime environment.
////#define NotImplementedException() Exception("The method is not implemented.",__LINE__,__FILE__)
////#define DeprecatedException() Exception("The method has been deprecated and removed.  Please see the method notes to use the correct replacement.",__LINE__,__FILE__)
////
//////GCC NOTE: GCC says allowing the use of an undeclared name is deprecated which would make us have to move TStr up 
//////above BaseHeader.
//////if you use '-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated
////#define BroThrowException(...) throw new Exception(TStr(__VA_ARGS__),__LINE__,__FILE__)
////#define BroThrowNotImplementedException() throw new NotImplementedException()
////#define BroThrowDeprecatedException() throw new DeprecatedException()
//
////#define AssertOrThrow(x) \
////    if(!(x)) throw("Runtime Assertion.",__LINE__,__FILE__) 
//
//
//#ifdef _DEBUG
////
////FORCE_INLINE void AsmCpuDebugBreak()
////{
////#ifdef BRO_X64
////    BRO_DEBUG_BREAK;
////#else
////    __asm int 3; x86 debug break interrupt
////#endif
////}
////
//////Debug validations.  indicates validations that could be necessary in 
////// prod. build but if they aren't breaking in debug we can turn them off.
////// this also enables us to turn them on in prod. builds if needed.
////#define _DEBUGVALIDATION
////
////FORCE_INLINE void DebugValidate(int xx)
////{
////    if(!xx)
////    {
////        AsmCpuDebugBreak();
////    }
////}
//
////Debug Code
////#define Debug(xx) xx
////
////#define  AssertOrThrow2(x) DebugValidate((x))
////#define VerifyOrThrow(expr,...) ((!(expr)) ? (DebugValidate(false)) : 0)
////
////#define CheckNull(x)  AssertOrThrow2((x)!=NULL)
////
////#else
//////_DEBUG
////
////#define DebugValidate(xx) 
////#define Debug(xx)
////
////#define  AssertOrThrow2(x) (((x)==NULL) ? (BroThrowException("Runtime Assertion.")) : (0))
////
////#define VerifyOrThrow(expr,...) ((!(expr)) ? (BroThrowException(__VA_ARGS__)) : 0)
////
////#define CheckNull(x) (((x)==NULL) ? (BroThrowException("Runtime Assertion: Null Exception. Object not set to instance of an object.")) : (0))
////
////
////#endif
//
//// Syntax helper.
////#define OVERRIDES virtual 
////#define MUST_OVERRIDE virtual
////#define VIRTUAL virtual
////#define SHADOWS
////
////class _NoInheritClassBase { };
////
////#define DOES_NOT_OVERRIDE 
////#define DOES_NOT_INHERIT _NoInheritClassBase
////#define OVERRIDE override
////
//////#define OVERRIDABLE(x) virtual x =NULL
////
////#define MarkStub() 
//
////Shorthand for testing true of bool to avoid compile warning
////#define IsTrue(x) ((x) > 0)
////#define IsFalse(x) ((x)== 0)
////
//////Slot map defines
////#define SLOT_EMPTY (NULL)
////#define INVALID_SLOT (-1)
////
////// - String Defines
////// - Result of a strcmp or std::string::compare
////#define STRING_CMP_MATCH (0)
////
////#define StrNotFound (std::string::npos)
////
////#define EnumToInt32(x) (*((int32_t*)(&x)))
////
////
////#ifdef _UNICODE
////#define Wstr(x) L#x
////#else
////#define Wstr(x) x
////#endif
////
////
////#ifndef BRO_OS_WINDOWS
//////This is for string wrapper.
////#ifndef __CLR_OR_THIS_CALL
////#define __CLR_OR_THIS_CALL
////#endif
//
//
//
//#endif
//
//
////////////////////////////////////////////////////////////////////////////
////#define PROPERTY(typeName,varName) protected: #typeName _##varName; public: FORCE_INLINE #typeName get##varName{ return _##varName; } FORCE_INLINE void set##varName(#typeName& x){ _##varName = x; } 
//
//
////include trap
//#endif
