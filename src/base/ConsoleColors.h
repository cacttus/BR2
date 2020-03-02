#pragma once
#ifndef __CONSOLE_COLORS_H__
#define __CONSOLE_COLORS_H__

//#include "../base/OperatingSystemHeader.h"

#ifdef _WIN32
/*
FOREGROUND_BLUE    Text color contains blue.
FOREGROUND_GREEN    Text color contains green.
FOREGROUND_RED    Text color contains red.
FOREGROUND_INTENSITY    Text color is intensified.
BACKGROUND_BLUE    Background color contains blue.
BACKGROUND_GREEN    Background color contains green.
BACKGROUND_RED    Background color contains red.
BACKGROUND_INTENSITY    Background color is intensified.
COMMON_LVB_LEADING_BYTE    Leading byte.
COMMON_LVB_TRAILING_BYTE    Trailing byte.
COMMON_LVB_GRID_HORIZONTAL    Top horizontal.
COMMON_LVB_GRID_LVERTICAL    Left vertical.
COMMON_LVB_GRID_RVERTICAL    Right vertical.
COMMON_LVB_REVERSE_VIDEO    Reverse foreground and background attributes.
COMMON_LVB_UNDERSCORE    Underscore.
*/
#define ConsoleColorWhite()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY )
#define ConsoleColorGray()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED )
#define ConsoleColorRed()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED | FOREGROUND_INTENSITY )
#define ConsoleColorGreen()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN | FOREGROUND_INTENSITY 
#define ConsoleColorBlue()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_BLUE | FOREGROUND_INTENSITY 
#define ConsoleColorPink()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY 
#define ConsoleColorCyan()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY )
#define ConsoleColorYellow()    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY )
#define ConsoleColorBlack()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0 )
#define ConsoleBackWhite()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY )
#define ConsoleBackBlack()        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), BACKGROUND_INTENSITY )
#else
#define ConsoleColorWhite() 
#define ConsoleColorGray()  
#define ConsoleColorRed()   
#define ConsoleColorGreen() 
#define ConsoleColorBlue()  
#define ConsoleColorPink()  
#define ConsoleColorCyan()  
#define ConsoleColorYellow()
#define ConsoleColorBlack() 
#define ConsoleBackWhite()  
#define ConsoleBackBlack()  
#endif

#endif 