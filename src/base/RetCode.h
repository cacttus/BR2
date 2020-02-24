/**
*  @file SystemErrorDefs.h
*  @date September 21, 2011
*  @author MetalMario971
*/
#pragma once
#ifndef __SYSTEMERRORDEFS_27745226362187691023999_H__
#define __SYSTEMERRORDEFS_27745226362187691023999_H__

namespace BR2 {
/**
*  @typedef RetCode
*  @brief Generalized error codes (across entire application)
*/
typedef enum {
  //**** GENERAL ****
  GR_FAIL,        // - General Failure
  GR_OK,            // - Operation successful.

  GR_OUT_OF_MEMORY,    // - Out of Memory
  GR_FATAL,        // - Notifies the system that the engine must exit.

  GR_ERROR_FAILED_TO_FIND_ITEM_IN_LIST, // - Failed to find an item in a list.
  GR_INITIALIZE_FAIL, // - Failed to initialize the engine

  GR_NOT_IMPLEMENTED,    // The function or method wasn't implemented
  //**** SYSTEM WIDE ERRORS ****
  GR_LIBRARY_ERROR,    // - error within the library system.
  GR_FILE_ERROR,        // - Error within the file system.

  //**** STARTUP ERRORS ****
  GR_NO_GAMESCHEME_FOUND, // - No game scheme was found on startup
  GR_MULTIPLE_GAMESCHEMES_FOUND, // - There was more than one game scheme found

  //**** SCENEGRAPH **** 
  GR_SG_CONTINUE,    // - continue processing

  //*** ACTION LIST ****
  GR_ERROR_DUPLICATE_ACTION_NAME, // - There was a redundant name added to an action sequence.

  //*** IIC ****
  GR_ERROR_IIC_EDIT_PROHIBITED,    // - Someone tried to edit an immutable instanced class when it was locked.

  GR_IDLIST_DUPLICATE,    //duplicate id in idlist

  //*** FILE ERRORS ****
  GR_FILE_NOT_FOUND_ON_DISK,    // - A file was not found on the disk.
  GR_FILE_NOT_FOUND_IN_LIB,    // - A file was not found in thel ib

  GR_HEIGHTMAP_CREATION_FAIL,    // - Heightmap creation failed.

  FILE_CREATEFAILED,  // - The file could not be created.
  FILE_GENERAL,   //general error
  FILE_OPENFAILED, // - The file could not be opened.
  FILE_HANDLEISOPEN,  // - The file handle was already open before the previous operation.
  FILE_NOTFOUND,        // - THE FILE WAS NOT FOUND.    
  FILE_EOF,        // - THE FILE IS AT EOF .
  FILE_ABRUPTEOF, // The file encountered an abrupte End Of File while reading or writing.
  FILE_NULLWRITE,        // - THE FILE TRIED TO WRITE NULL.
  FILE_DOESNOTEXIST,        // - THE FILE DOES NOT EXIST.
  FILE_INVALID,        // - THE FILE IS INVALID
  FILE_NULLSIZE,        // - THE FILE HAS A NULL SIZE.
  FILE_INVALIDREADLOCATION,        // - THE FILE TRIED TO READ AT A LOCATION GREATER, OR LESS THAN THE SIZE OF THE FILE.
  FILE_INVALIDWRITELOCATION,        // - THE FILE TRIED TO READ AT A LOCATION GREATER, OR LESS THAN THE SIZE OF THE FILE.
  FILE_INVALIDPOINTER,        // - THE FILE LOCATION POINTER IS INVALID.
  FILE_INVALIDLOCATION,        // - THE FILE NAME IS INVALID
  FILE_CLOSED,        // - THE FILE IS CLOSED.
  FILE_INVALIDREADSIZE,        // - THE SIZE TO BE READ IS ZERO OR LESS THAN ZERO.
  FILE_INVALIDWRITESIZE,        // - THE SIZE TO WRITE IS ZERO OR LESS THAN ZERO.
  FILE_NODELIMITERS,        // - THERE WERE NO DELIMITERS SPECIFIED WHEN THE READTO() FUNCTION WAS CALLED.
  FILE_CREATEERROR,        // - THE FILE COULD NOT BE CREATED.
  FILE_INVALIDVERSION,        // - VERSION IS OUT OF DATE.
  FILE_PARSEERROR,        // - FAILED TO PARSE THE FILE.
  FILE_INVALIDTYPE,        // - THE TYPE OF FILE WAS INVALID
  FILE_ALREADYOPEN,        // - THE FILE IS ALREADY OPEN.
  FILE_ALREADYLOADED, // - The file has been previously loaded.
  FILE_BUFFEROVERFLOW,    // - The supplied buffer was too small for the function argument.
  FILE_BADSTATE,   // the file was in a bad state before the operation.
  FILE_WRITE_BUFFER_OVERRUN, // - There was an error writing data to the given buffer
  FILE_READ_BUFFER_OVERRUN, // - There was an error reading data to the given buffer
  FILE_CONVERSION_ERROR,    // - There was an error converting a file's data.
  //******** LIBRARY ERRORS *****

 // LIB_ERROR,  // general error..remove
 // LIB_FAIL,//general failure..remove
 // LIB_UNINITIALIZED,  // The library is uninitialized (create() was not called)
 // LIB_OUTOFMEMORY,        // - Memory ran out.. somewhere
 // LIB_FILENOTFOUND,       // - A file was not found
 // LIB_DIRNOTFOUND,       // - A directory was not found
 // LIB_FILEERROR,  //there was an error in the file system.
 // LIB_INVALIDVERSION,     // - Invalid LIbrary version
 // LIB_INVALIDHEADER,     // - The header was corrupt or wrong.
 // LIB_NOTFOUND,    // - The library was not found.
 // LIB_ALREADYREGISTERED,   // - The library was already registered with the system.
 // LIB_LIBRARYNOTFOUND, // - The specified library was not found
 // LIB_LOADFAILED, //the loading failed
 // LIB_NOTLOADED,   //the lib is not loaded
 // LIB_NOITEM,        //no item was found, or the item is missing
 // LIB_DUPLICATEFILE,   // tried to add a file, but it's name already exists in the library
 // LIB_DUPLICATENAMES, //there were duplicate item names in the library
 // LIB_METAALREADYLOADED, //the lib meta was already loaded
 // LIB_CREATIONFAILED, // Creation of the library failed.
 // LIB_META_NOT_FOUND,    // - Searching through the system could not find the given meta.
 // LIB_FILE_NOT_FOUND,    // - Searching the filesystem root couldnot locate the given file name.
 // LIB_COMPRESS_ERROR, // - A file compression error occurred while loading a file from the library.
 //
  //********** VARIOUS PARSE ERRORRS *********
  HIERARCHY_PARSEERROR,    // - There was an error parsing a hierarchy string.

  //********** INPUT ERRORS *********
  INPUT_FAILED_TO_ACQUIRE_MOUSE,            // - DirectX could not acquire the keyboard
  INPUT_FAILED_TO_ACQUIRE_KEYBOARD,        // - DirectX could not acquire the mouse
  INPUT_NOT_INITIALIZED,                    // - DirectX was not initialized
  INPUT_UNKNOWN_ERROR_CODE,                // - The error code is unknown
  INPUT_ERROR_DIRECTX,                    // - The error was in the directx api.
  INPUT_INVALID_BUTTON_STATE,                // - Invalid button state
  //********** EVENT ERRORS *********
  EVENT_MATCHED,        // - Not 100% on what htis is doing.   Basically the event handler tries to find a matching event handler for a given event
  EVENT_NOT_MATCHED, // Not of the above
  EVENT_HANDLED,        // - The event had a corresponding handler which was called.
  EVENT_NOT_HANDLED,    // - The event's handler could not be found.

  //********** REPOS ERRORS **********
  REPOS_FAILED_TO_DELETE_OBJECT,    // - The repository could not delete the object because it was not found in the system's hash map.
  REPOS_FILE_CONVERSION_FAILED, // - Could not convert a given file.

  //********** VIDEO ERRORS ***********
  FAILED_TO_CREATE_SDL_SURFACE,    // could not create an SDL Surface.
  VIDEO_FUNCTION_NOT_SUPPORTED,    // - A feature of the video card wasn't supported.
  OGL_SUBSYSTEM_ERROR,        // - Error in the OpenGL Subsystem
  DX_SUBSYSTEM_ERROR,            // - Error in the DirectX Subsystem
  //********** MESH ERRORS ***********
  MESH_COULD_NOT_FIND_MESH_IN_POOL,    // - There was an error freeing a mesh.  It could not be found in the given mesh pool.
  MESH_FAILED_TO_FIND_BUF_FOR_SPEC, // - Failed to find the mesh buffer corresponding to the given mesh spec.

  //********** WINDOWS / LINUX ERRORS ***********
  WIN_FAILED_TO_CREATE_WINDOW,    // - One of many various windows errors happened during window creation.
  WIN_WINDOW_MANAGER_WAS_NULL,    // - In the windows "callback" the window manager was null.

  //********** VBO ERRORS ******************
  VBO_OFFSET_NOT_FOUND, // the offset in the offset map was not found.

  //****Shaders
  SHADER_VARIABLE_BIND_ERROR,

  //***Buffer
  CIRCULAR_BUFFER_ERROR,
  BUFFER_READ_EMPTY,
  BUFFER_WRITE_FULL

  //**Net
  , SOCK_SUCCESS
  , SOCK_NOCLIENT             // - No client has connected
  , SOCK_BINDFAILED            // - Binding failed.
  , SOCK_ALREADYCLOSED        // - Tried to close the scket when the socket is already closed.
  , SOCK_INUSE                // - The socket is being used.
  , SOCK_COULDNOTSET            // - A parameter on the socket could not be set.
  , SOCK_LISTENERR            // - There was an error listening on this socket.
  , SOCK_ACCEPTERR            // - There was an error in accepting a connection.
  , SOCK_RECVERR                // - There was an error in receiving data.
  , SOCK_SENDERR                // - There was an error in sending data.
  , SOCK_POLLFAIL                // - There was an error polling a socket.
  , SOCK_STARTUPERR            // - Error in starting up winsock.
  , SOCK_NOCONNAVAIL            // - No connections are available when trying to accept connections, this is not necessarily an error, it just means there aren't any pending.
  , SOCK_CONNCLOSE            // - The connection was closed by the client.
  , SOCK_ARRAYOUTOFBOUNDS        // - An array was accessed out of bounds.
  , SOCK_NODATA                // - There was no data present at the socket ( to be read ).
  , SOCK_CANNOTBEREAD            // - The socket could not be read from, it is in a blocking state.
  , SOCK_SELERROR                // - An error in calling select.
  , SOCK_TRUE                    // - 
  , SOCK_FALSE                // - 
  , SOCK_DISCONNECT            // - The socket was disconnected during the call.
  , SOCK_CONNERR                // - There was an error during the connection process
  , SOCK_CONNPENDING            // - Returned when a client socket is currently connecting to another socket.
  , SOCK_TIMEOUT                // - The socket operation timed out.
  , SOCK_NO_DATA
  , SOCK_DATA
  , SOCK_ERR_ADDRINUSE
  , SOCK_READ_INVALID_PACK_SIZE
  , SOCK_READ_INVALID_CRC_VALUE
  , SOCK_READ_INVALID_SIZE_VALUE
  , SOCK_READ_INVALID_ORD_VALUE
  , SOCK_READ_INVALID_HEADER_VALUE
  , SOCK_READ_CRC_FAILED
  , SOCK_BUFFER_FULL

  ////////////////
  , GR_ERROR_COUNT//
}RetCode;


}//ns game



#endif
