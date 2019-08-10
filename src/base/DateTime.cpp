#include "../base/DateTime.h"

#include "../base/StringUtil.h"
#include "../base/Logger.h"

namespace Game {
;

bool DateTime::dtLessThan(t_datetime d1, t_datetime d2)
{
    return (
        dateLessThan(getDatePart(d1), getDatePart(d2)) &&
        timeLessThan(getTimePart(d1), getTimePart(d2))
        );
}
bool DateTime::dateLessThan(t_date a1, t_date a2)
{
    // - Year
    if (dateGetYear(a1) < dateGetYear(a2)) return true;
    else if (dateGetYear(a1) > dateGetYear(a2)) return false;
    // - Month
    else if (dateGetMonth(a1) < dateGetMonth(a2)) return true;
    else if (dateGetMonth(a1) > dateGetMonth(a2)) return false;
    // - Day
    else if (dateGetDay(a1) < dateGetDay(a2)) return true;
    else if (dateGetDay(a1) > dateGetDay(a2)) return false;
    // dont need to test final condition

    return false;// - Dates are Equal
}
/**
*
*/
bool DateTime::timeLessThan(t_time t1, t_time t2)
{
    if (DateTime::timeGetHour(t1) < DateTime::timeGetHour(t2)) return true;
    else if (DateTime::timeGetHour(t1) > DateTime::timeGetHour(t2)) return false;

    else if (DateTime::timeGetMinute(t1) < DateTime::timeGetMinute(t2)) return true;
    else if (DateTime::timeGetMinute(t1) > DateTime::timeGetMinute(t2)) return false;

    else if (DateTime::timeGetSecond(t1) < DateTime::timeGetSecond(t2)) return true;
    else if (DateTime::timeGetSecond(t1) > DateTime::timeGetSecond(t2)) return false;

    else if (DateTime::timeGetMS(t1) < DateTime::timeGetMS(t2)) return true;
    else if (DateTime::timeGetMS(t1) > DateTime::timeGetMS(t2)) return false;

    return false;// - times are equal
}
/**
*    Get the date and time parts of datetime.
*/
t_date DateTime::getDatePart(t_datetime dt)
{
    return (t_date)((dt >> 32) & 0xFFFFFFFF);
}
t_time DateTime::getTimePart(t_datetime dt)
{
    return (t_time)(((dt << 32) >> 32) & 0xFFFFFFFF);
}
t_string DateTime::dateTimeToStr(t_datetime d)
{
    t_string dt = dateToStr(getDatePart(d));
    t_string tm = timeToStr(getTimePart(d));
    t_string ret = Stz dt + " " + tm;

    return ret;
}
t_string DateTime::dateTimeToStrNoDelim(t_datetime d)
{
    t_string dt = dateToStrNoSlash(getDatePart(d));
    t_string tm = timeToStrNoDelim(getTimePart(d));
    t_string ret = Stz dt + tm;

    return ret;
}
t_string DateTime::timeToStrNoDelim(t_time t)
{
    return timeToStr(t, "");
}
t_string DateTime::timeToStr(
    t_time t
    , t_string delim
    , bool hour
    , bool minute
    , bool second
    , bool millisecond
)
{
    t_string ret = "";

    t_string h, m, s, ms, del;

    h = m = s = ms = del = "";

    if (hour)
    {
        h = TypeConv::intToStr(DateTime::timeGetHour(t), "%02i"); ;
        del = delim;
        ret += h + del;
    }
    if (minute)
    {
        m = TypeConv::intToStr(DateTime::timeGetMinute(t), "%02i");
        del = delim;
        ret += m + del;
    }
    if (second)
    {
        s = TypeConv::intToStr(DateTime::timeGetSecond(t), "%02i");
        del = delim;
        ret += s + del;
    }
    if (millisecond)
    {
        ms = TypeConv::intToStr(DateTime::timeGetMS(t), "%03i");
        ret += ms;
    }

    return ret;

}
t_time DateTime::strToTime(t_string& time, char separator)
{
    t_string ret;
    std::vector<t_string> vals = StringUtil::split(time, separator);

    int32_t hb, mm, ss, ms;

    if (vals.size() < 4)
    {
        BroLogWarn("Time had not 4 components: "+ time);
    }
    hb = TypeConv::strToInt(vals[0]);
    mm = TypeConv::strToInt(vals[1]);
    ss = TypeConv::strToInt(vals[2]);
    ms = TypeConv::strToInt(vals[3]);

    return getTime(hb, mm, ss, ms);
}
t_date DateTime::strToDate(const t_string& szDate, char separator)
{
    if (szDate.length() < 10)
        return 0;
    t_string copy(szDate);
    int m, d, y1, y2;
    char buf[3];
    buf[2] = '\0';
    buf[0] = szDate[0];
    buf[1] = szDate[1];
    m = TypeConv::strToInt(t_string((char*)buf));
    buf[0] = szDate[3];
    buf[1] = szDate[4];
    d = TypeConv::strToInt(t_string((char*)buf));
    buf[0] = szDate[6];
    buf[1] = szDate[7];
    y1 = TypeConv::strToInt(t_string((char*)buf));
    buf[0] = szDate[8];
    buf[1] = szDate[9];
    y2 = TypeConv::strToInt(t_string((char*)buf));
    t_date dt = (t_date)((m << (24)) | (d << (16)) | (y1 << (8)) | (y2));

    return dt;
}
t_datetime DateTime::strToDateTime(const t_string& szDateTime)
{
    std::vector<t_string> str = StringUtil::split(szDateTime, ' ');

    if (str.size() == 0 || str.size() == 1)
    {
        BroLogWarn("Date time could not be converted, value: "+ szDateTime);
        return getDateTime();
    }

    t_date dt = strToDate(str[0]);
    t_time tt = strToTime(str[1]);

    return getDateTime(dt, tt);
}

t_datetime DateTime::fromUTime(time_t tt)
{
    //struct tm* ptm = gmtime(&tt);
 //       ptm->tm_sec;     /* seconds after the minute - [0,59] */
 //       ptm->tm_min;     /* minutes after the hour - [0,59] */
 //       ptm->tm_hour;    /* hours since midnight - [0,23] */
 //       ptm->tm_mday;    /* day of the month - [1,31] */
 //       ptm->tm_mon;     /* months since January - [0,11] */
 //       ptm->tm_year;    /* years since 1900 */
 //       ptm->int tm_wday;    /* days since Sunday - [0,6] */
 //       
    //    ptm->tm_yday;    /* days since January 1 - [0,365] */
 //       ptm->tm_isdst;   /* daylight savings time flag */

    return 0;
    throw new NotImplementedException();
}
t_datetime DateTime::getDateTime(int32_t mm, int32_t dd, int32_t y1, int32_t y2, int32_t h, int32_t m, int32_t s, int32_t ms)
{
    return getDateTime(getDate(mm, dd, y1, y2), getTime(h, m, s, ms));
}
t_date DateTime::getDate(int32_t mm, int32_t dd, int32_t y1, int32_t y2)
{
    return (t_date)((mm << (24)) | (dd << (16)) | (y1 << (8)) | (y2));
}
t_time DateTime::getTime(int32_t hh, int32_t mm, int32_t ss, int32_t ms)
{
    return (t_time)(
        (hh << TIME_HH_LSH) |
        (mm << TIME_MM_LSH) |
        (ss << TIME_SS_LSH) |
        (ms << TIME_MS_LSH));
}
t_date DateTime::getDate()
{
    char buf[12];
    _strdate_s((char*)buf, 12);
    t_string s = t_string((char*)buf);
    s.insert(6, t_string((char*)"20"));
    return strToDate(s);
}
/**
*
*    Returns the Date / Time in the format
*    [m][d][y][y][h][m][s][ms]
*/
t_datetime DateTime::getDateTime()
{
    t_date dt = DateTime::getDate();
    t_time tm = DateTime::getTime();
    return (t_datetime)(static_cast<int64_t>(dt) << 32 | static_cast<int64_t>(tm));
}
t_datetime DateTime::getDateTime(t_date d, t_time t)
{
    return (t_datetime)(static_cast<int64_t>(d) << 32 | static_cast<int64_t>(t));
}
int32_t DateTime::dateGetDay(t_date d) {
    return (d >> 16) & 0xFF;
}
int32_t DateTime::dateGetMonth(t_date d) {
    return (d >> 24) & 0xFF;
}
int32_t DateTime::dateGetYear(t_date d) {
    return ((d << 24) >> 24) & 0xFFFF;
}
/**

*/

t_string DateTime::dateGetYearStr(const t_date date)
{
    t_date d = date;
    t_string ret;
    ret += TypeConv::intToStr((d >> 8) & 0xFF);
    ret += TypeConv::intToStr(d & 0xFF);
    return ret;
}
t_string DateTime::dateGetDayStr(const t_date date)
{
    t_date d = date;
    t_string ret;
    ret += TypeConv::intToStr((d >> 16) & 0xFF);
    return ret;
}
t_string DateTime::dateGetMonthStr(const t_date date)
{
    t_date d = date;
    t_string ret;
    ret += TypeConv::intToStr((d >> 24) & 0xFF);
    return ret;
}
t_string DateTime::dateGetMonthName(t_date date)
{
    int month = dateGetMonth(date);
    switch (month) {
    case 1: return "January"; break;
    case 2: return "February"; break;
    case 3: return "March"; break;
    case 4: return "April"; break;
    case 5: return "May"; break;
    case 6: return "June"; break;
    case 7: return "July"; break;
    case 8: return "August"; break;
    case 9: return "September"; break;
    case 10: return "October"; break;
    case 11: return "November"; break;
    case 12: return "December"; break;
    default: return "Invalid Month Number"; break;
    };
}
t_string DateTime::dateGetMonthNameAbbr(t_date date)
{
    int month = dateGetMonth(date);
    switch (month) {
    case 1: return "Jan."; break;
    case 2: return "Feb."; break;
    case 3: return "Mar."; break;
    case 4: return "Apr."; break;
    case 5: return "May"; break;
    case 6: return "Jun."; break;
    case 7: return "Jul."; break;
    case 8: return "Aug."; break;
    case 9: return "Sep."; break;
    case 10: return "Oct."; break;
    case 11: return "Nov."; break;
    case 12: return "Dec."; break;
    default: return "Inv."; break;
    };
}
t_string DateTime::dateToStr(const t_date date)
{
    t_date d = date;
    t_string ret;

    ret += TypeConv::intToStr((d >> 24) & 0xFF);
    ret += "/";
    ret += TypeConv::intToStr((d >> 16) & 0xFF);
    ret += "/";
    ret += TypeConv::intToStr((d >> 8) & 0xFF);
    ret += TypeConv::intToStr(d & 0xFF);
    return ret;
}
t_string DateTime::dateToStrNoSlash(const t_date date)
{
    t_date d = date;
    t_string ret;

    ret += TypeConv::intToStr((d >> 8) & 0xFF, "%02i");//year1
    ret += TypeConv::intToStr(d & 0xFF, "%02i");//year2
    ret += TypeConv::intToStr((d >> 24) & 0xFF, "%02i");//month
    ret += TypeConv::intToStr((d >> 16) & 0xFF, "%02i");//day
    return ret;
}

uint32_t DateTime::timeGetHour(t_time t)
{
    return (uint32_t)((t >> TIME_HH_LSH)&TIME_HH_CH_MASK);//fix
}
uint32_t DateTime::timeGetMinute(t_time t)
{
    return (uint32_t)((t >> TIME_MM_LSH)&TIME_MM_CH_MASK);
}
uint32_t DateTime::timeGetSecond(t_time t)
{
    return (uint32_t)((t >> TIME_SS_LSH)&TIME_SS_CH_MASK);
}
uint32_t DateTime::timeGetMS(t_time t)
{
    return (uint32_t)((t >> TIME_MS_LSH)&TIME_MS_CH_MASK);
}
t_time DateTime::getTime()
{
    uint32_t hh = (uint32_t)DateTime::hour();
    uint32_t mm = (uint32_t)DateTime::minute();
    uint32_t ss = (uint32_t)DateTime::second();
    uint32_t ms = (uint32_t)DateTime::ms();    //ms/1000

    return getTime(hh, mm, ss, ms);
}
t_time DateTime::getTime(t_timeval in_ms)
{
    uint32_t hh = in_ms % (60 * 60 * 60 * 1000) / (60 * 60 * 1000);
    uint32_t mm = in_ms % (60 * 60 * 1000) / (60 * 1000);
    uint32_t ss = in_ms % (60 * 1000) / (1000);
    uint32_t ms = in_ms % (1000);

    return getTime(hh, mm, ss, ms);
}
t_time DateTime::getTime(uint32_t hh, uint32_t mm, uint32_t ss, uint32_t ms)
{
    return (t_time)(
        (hh << TIME_HH_LSH) |
        (mm << TIME_MM_LSH) |
        (ss << TIME_SS_LSH) |
        (ms << TIME_MS_LSH));
}
//Windows specific stuff MEH - fix this later
//See http://stackoverflow.com/questions/24991358/c-equivalent-for-getlocaltime-in-linux-with-milliseconds
int DateTime::hour()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return (int)st.wHour;
}
int DateTime::minute()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return (int)st.wMinute;
}
int DateTime::second()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return (int)st.wSecond;
}
int DateTime::ms()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return (int)st.wMilliseconds;
}


}//ns game
