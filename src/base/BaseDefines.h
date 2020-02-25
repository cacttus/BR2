/**
*  @file BaseDefines.h
*  @date February 12, 2020
*  @author MetalMario971
*/
#pragma once
#ifndef __BASEDEFINES_15815732801329887558_H__
#define __BASEDEFINES_15815732801329887558_H__

#include "../base/MachineTypes.h"

namespace BR2 {
//Operating System global defines
//We can use either SDL's defines, or use the defines in the expected OS headers
// BR2_OS_WINDOWS, BR2_OS_ANDROID, BRO_OS_LINUX, BR2_OS_IOS, BRO_OS_OSX
#define MUST_OVERRIDE virtual
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

//Note: this must remain a uint32_t, we serialize it as a uint32_t
typedef uint32_t Hash32;
typedef uint64_t NodeId;
#define NO_NODE_ID (0)

////String
//#ifdef USE_STD_STRING
//typedef std::string string_t;
//#else
//#include "../base/StringWrapper.h"
//#endif
typedef std::wstring wstring_t;
typedef std::string string_t;

/************************************************************************/
/* Enums                                                                */
/************************************************************************/
namespace EngineLoopState { typedef enum { SyncBegin, Update, Render, SyncEnd } e; }
namespace ColorSpace { typedef enum { Linear, SRGB } e; }
namespace ButtonState { typedef enum { Up, Press, Down, Release, None } e; }//2/11/18 added "None" for no mouse interation
namespace MouseButton { typedef enum { Left, Right, Middle } e; }
namespace PlayState { typedef enum { Playing, Paused, Stopped, Ended }e; }
namespace MemSize { typedef enum { MEMSZ_GIG2 = 536870912 } e; }
namespace FileWriteMode { typedef enum { Truncate, Append }e; }
namespace LoadState { typedef enum { NotLoaded, Loaded, LoadFailed } e; }
namespace KeyMod {
typedef enum {
  None = 0x00,
  Shift = 0x01, Alt = 0x02, Ctrl = 0x04,
  ShiftDontCare = 0x08, AltDontCare = 0x10, CtrlDontCare = 0x20
} e;
}
namespace RenderSystem { typedef enum { OpenGL, Vulkan } e; }
enum class LineBreak { Unix, DOS };
//namespace LineBreak { typedef enum { Unix, DOS }e; }

/************************************************************************/
/* Forward Decl                                                         */
/************************************************************************/
class SoundSpec;
class SoundInst;
class Sequence;
class Sequencer;
class _NoInheritClassBase {};
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
class InputManager; // Renamed InputManager;
class GameMemory;
class AppRunner;
class Delta;
class EngineConfig;
class EngineConfigFile;
class TreeNode;
class Package; // ApplicationPackage;
class Crc32;
class StreamBuffer;
class Net;
class SyncTimer;
class FpsMeter;
class FrameSync;
//class Component;
class ProjectFile;
class CSharpScript;
class GraphicsWindow;
class OpenGLWindow;
class VulkanWindow;
class WindowManager;
class CSharpScript;
class CSharpCompiler;
class ScriptManager;
class Packet;

//Classes that will be removed
class AppBase;
class AppMain;

template < class Tx >
class DynamicBuffer;
template < class Tx >
class GrowBuffer;
template < class Tx >
class Allocator;

template <class TItem>
class HashMapItem;
template < class Tx >
class HashMap;


typedef uint64_t t_timeval;
typedef string_t DiskLoc;
typedef int32_t t_date;            // - Date information [m][d][y1][y2] = [8][27][198][7]
typedef int64_t t_datetime;        // - A Date + the time.[m][d][y1][y2][h][m][s][ms]
typedef uint32_t t_time;
typedef std::vector<char> ByteBuffer;
typedef GameMemory PureMemory;//Pure memory without a VPtr
typedef PureMemory GpuMemory; //Pure memory without a VPtr

/************************************************************************/
/* Base Classes                                                         */
/************************************************************************/
class GameMemory : DOES_NOT_INHERIT {
public:
  NOT_VIRTUAL ~GameMemory() DOES_NOT_OVERRIDE {} // Upon tests - this also will get called when the class is deleted.
  GameMemory() {}
};
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
  template<class Tx> std::shared_ptr<Tx> getThis() {
    return std::dynamic_pointer_cast<Tx>(this->shared_from_this());
  }
};
class OperatingSystemMemory : DOES_NOT_INHERIT {
public:
  OperatingSystemMemory() {
  }
  virtual ~OperatingSystemMemory() DOES_NOT_OVERRIDE {
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



}//ns BR2



#endif
