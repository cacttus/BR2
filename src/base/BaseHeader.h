/**
*
*    @file BaseHeader.h
*    @date November 12, 2016
*    @author Derek Page
*
*    � 2016 
*
*
*/
#pragma once
#ifndef __BASEHEADER_1478973884162863251_H__
#define __BASEHEADER_1478973884162863251_H__

#include "../base/BuildConfig.h"
#include "../base/GlobalIncludes.h"
#include "../base/MachineTypes.h"
#include "../base/OperatingSystemHeader.h"
#include "../base/ConsoleColors.h"
namespace Game {
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
#define BRO_OS_IPHONE 1
#endif
#elif __WINDOWS__
#define BRO_OS_WINDOWS 1
#endif

#define OVERRIDES virtual 
#define MUST_OVERRIDE virtual
#define VIRTUAL virtual
#define SHADOWS

#define BRO_MAX_PATH 512

#define DEL_MEM(x) do{ if(x){delete x; x=nullptr;} } while(0);

#define FILESYSTEM_ROOT_DIRECTORY "./data"
#define CACHE_ROOT_DIRECTORY "./data/cache"

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

#define DOES_NOT_OVERRIDE 
#define DOES_NOT_INHERIT _NoInheritClassBase
#define OVERRIDE override
#define NOT_VIRTUAL
#define FORCE_INLINE inline
#define STATIC static
#ifndef __inout_ 
#define __inout_
#endif

#ifndef __out_ 
#define __out_
#endif

#ifndef __in_ 
#define __in_ 
#endif

#ifndef __ignored
#define __ignored
#endif

//optinoal parameter
#ifndef __opt_ 
#define __opt_
#endif

//Reference parameter (class does not own data)
#ifndef __ref_ 
#define __ref_
#endif

//By Value param
#ifndef __byval_
#define __byval_
#endif

// - The application runtime environment.
#define NotImplementedException() Exception("The method is not implemented.",__LINE__,__FILE__)
#define DeprecatedException() Exception("The method has been deprecated and removed.  Please see the method notes to use the correct replacement.",__LINE__,__FILE__)

#define MaxStaticStringBufferSize 16384

void staticDebugBreak(t_string str);
#define TIME_T_MIN (0)
#ifdef _DEBUG
#define AssertOrThrow2(x) do{ if(!(x)) { staticDebugBreak(Stz "Runtime Assertion: '"+ #x + "'. "); } }while(0)
#else
#define AssertOrThrow2(x) do{ if(!(x)) { BroThrowException("Runtime Assertion: '", #x, "'. "); } }while(0)
#endif


//GCC NOTE: GCC says allowing the use of an undeclared name is deprecated which would make us have to move TStr up 
//above BaseHeader.
//if you use '-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated
#define BroThrowException(x) throw new Exception(Stz x,__LINE__,__FILE__)
#define BroThrowNotImplementedException() throw new NotImplementedException()
#define BroThrowDeprecatedException() throw new DeprecatedException()
#define VerifyOrThrow(expr,x) do { if(!(expr)) BroThrowException(x); } while(0)
#define CheckGpuErrorsDbg() Gu::checkErrors()

//Note: this must remain a uint32_t, we serialize it as a uint32_t
typedef uint32_t Hash32;
typedef uint64_t NodeId;
#define NO_NODE_ID (0)

#define KX_(x) (GetAsyncKeyState(x) & 0x8000)


#define PPERF(x) x; Gu::pulsePerf(#x);

namespace EngineLoopState { typedef enum { SyncBegin, Update, Render, SyncEnd } e; }
namespace ColorSpace { typedef enum { Linear, SRGB } e; }
namespace ButtonState { typedef enum { Up, Press, Down, Release, None } e; }//2/11/18 added "None" for no mouse interation
namespace MouseButton { typedef enum { Left, Right, Middle } e; }
namespace PlayState { typedef enum { Playing, Paused, Stopped, Ended }e; }
namespace MemSize { typedef enum { MEMSZ_GIG2 = 536870912 } e; }
namespace FileWriteMode { typedef enum { Truncate, Append }e; }
namespace LoadState {typedef enum { NotLoaded, Loaded, LoadFailed } e;}
namespace KeyMod {typedef enum {
    None = 0x00,
    Shift = 0x01, Alt = 0x02, Ctrl = 0x04,
    ShiftDontCare = 0x08, AltDontCare = 0x10, CtrlDontCare = 0x20} e; }
namespace RenderSystem { typedef enum { OpenGL, Vulkan } e; }
namespace LineBreak { typedef enum { Unix, DOS }e; }

class SoundSpec;
class SoundInst;
class Sequence;
class Sequencer;
class AppBase;
class _NoInheritClassBase { };
class Logger;
class Texture2DSpec;
class GLContext;
class TexCache;
class Img32;
class BinaryFile;
class EasyNoise;
class FileSystem;
class Exception;
class SoundSpec;
class SoundCache;
class TouchInfo;
class Fingers;
class GameMemory;
class AppRunner;
class Engine;
class EngineConfig;
class EngineConfigFile;
class TreeNode;
class Package;
class Crc32;


template < class Tx >
class DynamicBuffer;
template < class Tx >
class GrowBuffer;
template < class Tx >
class Allocator;

class StreamBuffer;

class Net;

typedef std::string t_string;
typedef uint64_t t_timeval;

typedef t_string DiskLoc;
typedef int32_t t_date;            // - Date information [m][d][y1][y2] = [8][27][198][7]
typedef int64_t t_datetime;        // - A Date + the time.[m][d][y1][y2][h][m][s][ms]
typedef uint32_t t_time;
typedef std::vector<char> ByteBuffer;
typedef GameMemory PureMemory;//Pure memory without a VPtr
typedef PureMemory GpuMemory; //Pure memory without a VPtr

/**
*    @class GameMemory
*    @brief An object which inherits this class will be allocated in the memory system.
*/
class GameMemory : DOES_NOT_INHERIT {
public:
    NOT_VIRTUAL ~GameMemory() DOES_NOT_OVERRIDE {} // Upon tests - this also will get called when the class is deleted.
    GameMemory() {}

};

/**
*    @brief the difference between this and GameMemory is that we can't use
*    a virtual destructor for GameMemory in classes that can't have a VTable (such as a vec4)
*    but we need this virtual constraint on all classes that inherit GameMemory to make sure
*    they're calling their destructors.
*/
class VirtualMemory : public GameMemory {
public:
    VirtualMemory() {
    }
    virtual ~VirtualMemory() DOES_NOT_OVERRIDE {
    }
};
template < class Tx >
class VirtualMemoryShared : public std::enable_shared_from_this<Tx> {
public:
    VirtualMemoryShared() {
    }
    virtual ~VirtualMemoryShared() DOES_NOT_OVERRIDE {

    }
    template<class Tx> std::shared_ptr<Tx> getThis() { return std::dynamic_pointer_cast<Tx>(shared_from_this()); }

};
class OperatingSystemMemory : DOES_NOT_INHERIT {
public:
    OperatingSystemMemory() {
    }
    VIRTUAL ~OperatingSystemMemory() DOES_NOT_OVERRIDE {
    }
};
class GameMemoryManager : DOES_NOT_INHERIT {
public:
    static void* allocBlock(size_t newSize) {
        char* block = new char[newSize];
        return (void*)block;
    }
    static void freeBlock(void* m) {
        delete[] m;
    }
};

class RenderParams;
//Goes here becauwse TreeNode inherits it.
class Drawable : public VirtualMemoryShared<Drawable> {
public:
    virtual void drawDeferred(RenderParams& rp) = 0;
    virtual void drawForward(RenderParams& rp) = 0;
    virtual void drawShadow(RenderParams& rp) = 0;
    virtual void drawDebug(RenderParams& rp) = 0;
    virtual void drawNonDepth(RenderParams& rp) = 0; // draw the non-depth test items (last)
    virtual void drawTransparent(RenderParams& rp) = 0; //These come after the way after, the very end
};

//TODO: REMOVE
class Packet {};

//Helper string since we can't cast char* to std::string automatically.
#define Stz std::string("")+
//String Extensions  See MathHeader for math string extensions.
std::string operator+(const std::string& str, const char& rhs);
std::string operator+(const std::string& str, const int8_t& rhs);
std::string operator+(const std::string& str, const int16_t& rhs);
std::string operator+(const std::string& str, const int32_t& rhs);
std::string operator+(const std::string& str, const int64_t& rhs);
std::string operator+(const std::string& str, const uint8_t& rhs);
std::string operator+(const std::string& str, const uint16_t& rhs);
std::string operator+(const std::string& str, const uint32_t& rhs);
std::string operator+(const std::string& str, const uint64_t& rhs);
std::string operator+(const std::string& str, const double& rhs);
std::string operator+(const std::string& str, const float& rhs);


}//ns Game



#endif






