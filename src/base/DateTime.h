/**
*
*    @file Date.h
*    @date August 23, 2010
*    @author MetalMario971
*
*    © 2011
*
*
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
  static t_date getDate();
  static t_date getDatePart(t_datetime);
  static t_time getTimePart(t_datetime);
  static t_datetime getDateTime();
  static t_datetime getDateTime(t_date d, t_time t);
  static t_datetime getDateTime(int32_t mm, int32_t dd, int32_t y1, int32_t y2, int32_t h, int32_t m, int32_t s, int32_t ms);
  static t_date getDate(int32_t mm, int32_t dd, int32_t y1, int32_t y2);
  static t_time getTime(int32_t hb, int32_t mm, int32_t ss, int32_t ms);

  static t_datetime fromUTime(time_t t);

  static t_date strToDate(const t_string& szDate, char separator = '/');
  static t_datetime strToDateTime(const t_string& szDateTime);
  static t_time strToTime(t_string& time, char separator = ':');
  static t_string dateTimeToStr(t_datetime d);    // - Convert DateTime to string.
  static t_string timeToStr(
    t_time t
    , t_string delim = t_string(":")
    , bool hour = true
    , bool minute = true
    , bool second = true
    , bool millisecond = true
  );
  static t_string dateTimeToStrNoDelim(t_datetime d);    // - Convert DateTime to string.
  static t_string timeToStrNoDelim(t_time t);

  static t_string dateGetYearStr(const t_date date);// Get the year as a string
  static t_string dateGetDayStr(const t_date date);// Get the day as a string
  static t_string dateGetMonthStr(const t_date date);//Get the month as a string.
  static t_string dateGetMonthName(t_date date);// Get the name of the month.
  static t_string dateGetMonthNameAbbr(t_date date);    // Get the name of the month abbreviated.
  static t_string dateToStrNoSlash(const t_date date);// Same as above but outputs the date as mmddyyyy

  static bool dtLessThan(t_datetime d1, t_datetime d2);
  static bool dateLessThan(t_date d1, t_date d2);
  static bool timeLessThan(t_time t1, t_time t2);
  static t_string dateToStr(const t_date date);// Get the date as a string,  as mm/dd/yyyy

  static int32_t dateGetDay(t_date d);// Return the integer number of the day
  static int32_t dateGetMonth(t_date d);// Return the integer number of the month
  static int32_t dateGetYear(t_date d);// return the integer number of the year.

  static uint32_t timeGetHour(t_time t);
  static uint32_t timeGetMinute(t_time t);
  static uint32_t timeGetSecond(t_time t);
  static uint32_t timeGetMS(t_time t);

  static t_time getTime();    // - Returns a system-dependant time with h,m,s,ms.  This can be converted to and from s,ms,m,h using system functions.
  static t_time getTime(t_timeval milliseconds);    // - Milliseconds to time
  static t_time getTime(uint32_t hh, uint32_t mm, uint32_t ss, uint32_t ms);    // - components to time
  static int hour();                // - Get the current hour. 
  static int minute();            // - Get the current minute.
  static int second();            // - Get the current second.
  static int ms();                // - Get the current millisecond. ( out of 1000 )
  static t_string getTimeString();//time as string. hh:mm:ss:uuuu
};

}//ns game



#endif
