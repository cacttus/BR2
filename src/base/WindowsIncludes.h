/**
*  @file WindowsInclujdes.h
*  @date 20200220
*  @author MetalMario971
*/
#pragma once
#ifndef __WINDOWSOS_15822390262132272482_H__
#define __WINDOWSOS_15822390262132272482_H__

#include "../base/BuildConfig.h"
//Do not include this file in GlobalIncludes or any include headers.  Put in CPP file directly.

#ifdef BR2_OS_WINDOWS

//For opening folder

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <commdlg.h>

#ifdef _WIN32
#ifdef _WIN64
#include <Windows.h>
#include <TlHelp32.h>
#else
#include <TlHelp32.h>
#include <DbgHelp.h>
#endif
#endif



#endif

//**NOTE** This drags in WINNT.h  It's required to come after the Windows include.  As such, put it down into the CPP file.
#include "../ext/dirent.h"



#endif
