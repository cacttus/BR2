/**
*  @file Date.h
*  @date August 23, 2010
*  @author MetalMario971
*/
#pragma once
#ifndef __DATE_2689426881206782656826687_H__
#define __DATE_2689426881206782656826687_H__

#include "../base/BaseHeader.h"
namespace Game {

#define TIME_HH_RSH (0)
#define TIME_MM_RSH (6)
#define TIME_SS_RSH (13)
#define TIME_MS_RSH (20)

#define TIME_HH_LSH (25)
#define TIME_MM_LSH (18)
#define TIME_SS_LSH (11)
#define TIME_MS_LSH (2)


//masks for the given time components if their least significant bit is at bit 0
#define TIME_HH_CH_MASK (64+32+16+8+4+2+1)
#define TIME_MM_CH_MASK (64+32+16+8+4+2+1)
#define TIME_SS_CH_MASK (64+32+16+8+4+2+1)
#define TIME_MS_CH_MASK (256+128+64+32+16+8+4+2+1)
#define TIME_UNUSED_CH_MASK (2+1)

// - counts of time bits in the integer
#define TIME_HH_BIT_CT (7)
#define TIME_MM_BIT_CT (7)
#define TIME_SS_BIT_CT (7)
#define TIME_MS_BIT_CT (9)
#define TIME_UNUSED_BIT_CT (2)

class DateTime : public VirtualMemory {
public:
    STATIC t_date getDate();
    STATIC t_date getDatePart(t_datetime);
    STATIC t_time getTimePart(t_datetime);
    STATIC t_datetime getDateTime();
    STATIC t_datetime getDateTime(t_date d,t_time t);
    STATIC t_datetime getDateTime(int32_t mm, int32_t dd, int32_t y1, int32_t y2, int32_t h, int32_t m, int32_t s, int32_t ms);
    STATIC t_date getDate(int32_t mm, int32_t dd, int32_t y1, int32_t y2);
    STATIC t_time getTime(int32_t hb,int32_t mm, int32_t ss, int32_t ms);

    STATIC t_datetime fromUTime(time_t t);

    STATIC t_date strToDate( const string_t& szDate, char separator = '/' );
    STATIC t_datetime strToDateTime( const string_t& szDateTime );
    STATIC t_time strToTime(string_t& time, char separator = ':');
    STATIC string_t dateTimeToStr(t_datetime d);    // - Convert DateTime to string.
    STATIC string_t timeToStr(
          t_time t
        , string_t delim = string_t(":")
        , bool hour=true
        , bool minute = true
        , bool second = true
        , bool millisecond=true
        );
    STATIC string_t dateTimeToStrNoDelim(t_datetime d);    // - Convert DateTime to string.
    STATIC string_t timeToStrNoDelim(t_time t);

    STATIC string_t dateGetYearStr( const t_date date ) ;// Get the year as a string
    STATIC string_t dateGetDayStr( const t_date date );// Get the day as a string
    STATIC string_t dateGetMonthStr( const t_date date );//Get the month as a string.
    STATIC string_t dateGetMonthName( t_date date );// Get the name of the month.
    STATIC string_t dateGetMonthNameAbbr( t_date date );    // Get the name of the month abbreviated.
    STATIC string_t dateToStrNoSlash( const t_date date );// Same as above but outputs the date as mmddyyyy

    STATIC bool dtLessThan(t_datetime d1,t_datetime d2);
    STATIC bool dateLessThan(t_date d1,t_date d2);
    STATIC bool timeLessThan(t_time t1,t_time t2);
    STATIC string_t dateToStr( const t_date date );// Get the date as a string,  as mm/dd/yyyy

    STATIC int32_t dateGetDay( t_date d );// Return the integer number of the day
    STATIC int32_t dateGetMonth( t_date d );// Return the integer number of the month
    STATIC int32_t dateGetYear( t_date d );// return the integer number of the year.
    
    STATIC uint32_t timeGetHour(t_time t);
    STATIC uint32_t timeGetMinute(t_time t);
    STATIC uint32_t timeGetSecond(t_time t);
    STATIC uint32_t timeGetMS(t_time t);

    STATIC t_time getTime();    // - Returns a system-dependant time with h,m,s,ms.  This can be converted to and from s,ms,m,h using system functions.
    STATIC t_time getTime(t_timeval milliseconds);    // - Milliseconds to time
    STATIC t_time getTime(uint32_t hh, uint32_t mm, uint32_t ss, uint32_t ms);    // - components to time
    STATIC int hour();                // - Get the current hour. 
    STATIC int minute();            // - Get the current minute.
    STATIC int second();            // - Get the current second.
    STATIC int ms();                // - Get the current millisecond. ( out of 1000 )
    STATIC string_t getTimeString();//time as string. hh:mm:ss:uuuu
};

}//ns game



#endif
