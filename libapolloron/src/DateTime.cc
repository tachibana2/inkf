/******************************************************************************/
/*! @file DateTime.cc
    @brief DateTime class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "calendar/msg.h"
#include "apolloron.h"

namespace {

const static char *AMONTH[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
const static char *FMONTH[12] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
const static char *AWEEK[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
const static char *FWEEK[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};


/*! Geting server timezone offset (seconds)
    @param void
    @return GMT offset (seconds)
 */
static long _get_server_gmtoff() {
    time_t cur_time, gmt_time;
    struct tm gmt_tm;

    cur_time = time(NULL);

    gmtime_r(&cur_time, &gmt_tm);
    gmt_time = mktime(&gmt_tm);

    return (int)(((long)cur_time) - ((long)gmt_time));
}


/*! Convertion GMT offset value from string to long int.
    @param gmtoff GMT offset string
                  "+0900",
                  "+09:00",
                  "+09:00 (JST)"
                  "JST"
    @return GMT offset (seconds)
 */
static long _gmtoff_str_to_long(const char *gmtoff) {
    char sgmtoff[20];
    long lgmtoff;
    int sign;
    int len;

    lgmtoff = 0L;
    sign = 0;

    if (gmtoff == NULL || gmtoff[0] == '\0') {
        return lgmtoff;
    }

    if (gmtoff[0] == '+') {
        sign = 1;
    } else if (gmtoff[0] == '-') {
        sign = -1;
    }

    len = strlen(gmtoff);
    if (20 <= len) len = 19;
    memset(sgmtoff, 0, 20);
    if (sign != 0) {
        if (5 <= len) {
            sgmtoff[0] = gmtoff[0];
            sgmtoff[1] = gmtoff[1];
            sgmtoff[2] = gmtoff[2];
            if (gmtoff[3] == ':' && 6 <= len) {
                sgmtoff[3] = gmtoff[4];
                sgmtoff[4] = gmtoff[5];
            } else {
                sgmtoff[3] = gmtoff[3];
                sgmtoff[4] = gmtoff[4];
            }
            sgmtoff[5] = '\0';
        }
    } else {
        int ii;
        for (ii = 0; ii < len; ii++) {
            if (isspace(gmtoff[ii]) || gmtoff[ii] == '(') {
                continue;
            } else {
                break;
            }
        }
        if (!strcasecmp(gmtoff+ii, "IDLW")) {
            strcpy(sgmtoff, "-1200");
        } else if (!strcasecmp(gmtoff+ii, "NT")) {
            strcpy(sgmtoff, "-1100");
        } else if (!strcasecmp(gmtoff+ii, "HAST")) {
            strcpy(sgmtoff, "-1000");
        } else if (!strcasecmp(gmtoff+ii, "AKST")) {
            strcpy(sgmtoff, "-0900");
        } else if (!strcasecmp(gmtoff+ii, "YST")) {
            strcpy(sgmtoff, "-0900");
        } else if (!strcasecmp(gmtoff+ii, "AKDT")) {
            strcpy(sgmtoff, "-0800");
        } else if (!strcasecmp(gmtoff+ii, "YDT")) {
            strcpy(sgmtoff, "-0800");
        } else if (!strcasecmp(gmtoff+ii, "PST")) {
            strcpy(sgmtoff, "-0800");
        } else if (!strcasecmp(gmtoff+ii, "PDT")) {
            strcpy(sgmtoff, "-0700");
        } else if (!strcasecmp(gmtoff+ii, "MST")) {
            strcpy(sgmtoff, "-0700");
        } else if (!strcasecmp(gmtoff+ii, "MDT")) {
            strcpy(sgmtoff, "-0600");
        } else if (!strcasecmp(gmtoff+ii, "CST")) {
            strcpy(sgmtoff, "-0600");
        } else if (!strcasecmp(gmtoff+ii, "CDT")) {
            strcpy(sgmtoff, "-0500");
        } else if (!strcasecmp(gmtoff+ii, "EST")) {
            strcpy(sgmtoff, "-0500");
        } else if (!strcasecmp(gmtoff+ii, "EDT")) {
            strcpy(sgmtoff, "-0400");
        } else if (!strcasecmp(gmtoff+ii, "NST")) {
            strcpy(sgmtoff, "-0330");
        } else if (!strcasecmp(gmtoff+ii, "BST")) {
            strcpy(sgmtoff, "-0300");
        } else if (!strcasecmp(gmtoff+ii, "NDT")) {
            strcpy(sgmtoff, "-0230");
        } else if (!strcasecmp(gmtoff+ii, "WAT")) {
            strcpy(sgmtoff, "-0100");
        } else if (!strcasecmp(gmtoff+ii, "GMT")) {
            strcpy(sgmtoff, "+0000");
        } else if (!strcasecmp(gmtoff+ii, "WET")) {
            strcpy(sgmtoff, "+0000");
        } else if (!strcasecmp(gmtoff+ii, "BST")) {
            strcpy(sgmtoff, "+0100");
        } else if (!strcasecmp(gmtoff+ii, "IST")) {
            strcpy(sgmtoff, "+0100");
        } else if (!strcasecmp(gmtoff+ii, "WEST")) {
            strcpy(sgmtoff, "+0100");
        } else if (!strcasecmp(gmtoff+ii, "CET")) {
            strcpy(sgmtoff, "+0100");
        } else if (!strcasecmp(gmtoff+ii, "FWT")) {
            strcpy(sgmtoff, "+0100");
        } else if (!strcasecmp(gmtoff+ii, "SWT")) {
            strcpy(sgmtoff, "+0100");
        } else if (!strcasecmp(gmtoff+ii, "CEST")) {
            strcpy(sgmtoff, "+0200");
        } else if (!strcasecmp(gmtoff+ii, "FST")) {
            strcpy(sgmtoff, "+0200");
        } else if (!strcasecmp(gmtoff+ii, "EET")) {
            strcpy(sgmtoff, "+0200");
        } else if (!strcasecmp(gmtoff+ii, "SST")) {
            strcpy(sgmtoff, "+0200");
        } else if (!strcasecmp(gmtoff+ii, "EEST")) {
            strcpy(sgmtoff, "+0300");
        } else if (!strcasecmp(gmtoff+ii, "ADT")) {
            strcpy(sgmtoff, "+0300");
        } else if (!strcasecmp(gmtoff+ii, "BT")) {
            strcpy(sgmtoff, "+0300");
        } else if (!strcasecmp(gmtoff+ii, "MSK")) {
            strcpy(sgmtoff, "+0300");
        } else if (!strcasecmp(gmtoff+ii, "IT")) {
            strcpy(sgmtoff, "+0330");
        } else if (!strcasecmp(gmtoff+ii, "AST")) {
            strcpy(sgmtoff, "+0400");
        } else if (!strcasecmp(gmtoff+ii, "MSD")) {
            strcpy(sgmtoff, "+0400");
        } else if (!strcasecmp(gmtoff+ii, "PST")) {
            strcpy(sgmtoff, "+0500");
        } else if (!strcasecmp(gmtoff+ii, "IST")) {
            strcpy(sgmtoff, "+0530");
        } else if (!strcasecmp(gmtoff+ii, "BTT")) {
            strcpy(sgmtoff, "+0600");
        } else if (!strcasecmp(gmtoff+ii, "JT")) {
            strcpy(sgmtoff, "+0700");
        } else if (!strcasecmp(gmtoff+ii, "CST")) {
            strcpy(sgmtoff, "+0800");
        } else if (!strcasecmp(gmtoff+ii, "AWST")) {
            strcpy(sgmtoff, "+0800");
        } else if (!strcasecmp(gmtoff+ii, "MT")) {
            strcpy(sgmtoff, "+0800");
        } else if (!strcasecmp(gmtoff+ii, "AWCST")) {
            strcpy(sgmtoff, "+0845");
        } else if (!strcasecmp(gmtoff+ii, "JST")) {
            strcpy(sgmtoff, "+0900");
        } else if (!strcasecmp(gmtoff+ii, "KST")) {
            strcpy(sgmtoff, "+0900");
        } else if (!strcasecmp(gmtoff+ii, "ACST")) {
            strcpy(sgmtoff, "+0930");
        } else if (!strcasecmp(gmtoff+ii, "AEST")) {
            strcpy(sgmtoff, "+1000");
        } else if (!strcasecmp(gmtoff+ii, "GST")) {
            strcpy(sgmtoff, "+1000");
        } else if (!strcasecmp(gmtoff+ii, "ACDT")) {
            strcpy(sgmtoff, "+1030");
        } else if (!strcasecmp(gmtoff+ii, "AEDT")) {
            strcpy(sgmtoff, "+1100");
        } else if (!strcasecmp(gmtoff+ii, "IDLE")) {
            strcpy(sgmtoff, "+1200");
        } else if (!strcasecmp(gmtoff+ii, "NZST")) {
            strcpy(sgmtoff, "+1200");
        } else if (!strcasecmp(gmtoff+ii, "CHAST")) {
            strcpy(sgmtoff, "+1245");
        } else if (!strcasecmp(gmtoff+ii, "NZDT")) {
            strcpy(sgmtoff, "+1300");
        } else if (!strcasecmp(gmtoff+ii, "CHADT")) {
            strcpy(sgmtoff, "+1345");
        } else if (!strcasecmp(gmtoff+ii, "LINT")) {
            strcpy(sgmtoff, "+1400");
        }
    }

    if ((sgmtoff[0] == '+' || sgmtoff[0] == '-') &&
            isdigit(sgmtoff[1]) && isdigit(sgmtoff[2]) &&
            isdigit(sgmtoff[3]) && isdigit(sgmtoff[4])) {
        int a, b;
        if (sgmtoff[0] == '+') {
            sign = 1;
        } else {
            sign = -1;
        }
        a = (int)((((unsigned int)(sgmtoff[1])) - (unsigned int)'0') * 10 + (((unsigned int)(sgmtoff[2])) - (unsigned int)'0'));
        b = (int)((((unsigned int)(sgmtoff[3])) - (unsigned int)'0') * 10 + (((unsigned int)(sgmtoff[4])) - (unsigned int)'0'));
        lgmtoff = (a * 3600) + (b * 60);
    }

    return lgmtoff;
}


/*! mktime without local time zone
 * @param time_tm time of struct tm
 * @return time_t time (-1 if error)
 **/
static time_t _mkgmtime(const struct tm * time_tm) {
    int dir, bits, ssec;
    time_t t;
    struct tm ytm, mtm;

    ytm = *time_tm;
    ssec = ytm.tm_sec;
    ytm.tm_sec = 0;
    for (bits = 0, t = 1; t > 0; ++bits, t <<= 1);
    t = (t < 0) ? 0 : ((time_t) 1 << bits);
    if(bits > 40) bits = 40;

    while (1) {
        if(!gmtime_r(&t, &mtm)) return -1;
        dir = mtm.tm_year - ytm.tm_year;
        if (dir == 0 &&
                (dir = (mtm.tm_mon - ytm.tm_mon)) == 0 &&
                (dir = (mtm.tm_mday - ytm.tm_mday)) == 0 &&
                (dir = (mtm.tm_hour - ytm.tm_hour)) == 0 &&
                (dir = (mtm.tm_min - ytm.tm_min)) == 0) {
            dir = mtm.tm_sec - ytm.tm_sec;
        }
        if (dir != 0) {
            if (bits-- < 0) return -1;
            if (bits < 0) {
                --t;
            } else if (dir > 0) {
                t -= (time_t) 1 << bits;
            } else {
                t += (time_t) 1 << bits;
            }
            continue;
        }
        break;
    }
    t += ssec;
    return t;
}


/*! Set tm_wday(day of week) and tm_yday(days in year) to struct tm.
 * @param  [out]time_tm time of struct tm
 * @return time of struct tm (same address as time_tm)
 * @author ichisawa
 **/
static struct tm *_set_tm_wday_yday(struct tm *time_tm) {
    struct tm my_tm;
    time_t t;

    if (time_tm == (struct tm *)NULL) {
        return (struct tm *)NULL;
    }

    memset(&my_tm, 0, sizeof(struct tm));

    my_tm.tm_mday = time_tm->tm_mday;
    my_tm.tm_mon  = time_tm->tm_mon;
    my_tm.tm_year = time_tm->tm_year;

    t = mktime((struct tm *)&my_tm);

    localtime_r(&t, &my_tm);

    if ((long)t < (long)0) {
        // time < "1970-01-01 00:00:00 +0000"
        // do nothing
    } else {
        // set tm_wday and tm_yday.
        time_tm->tm_wday = my_tm.tm_wday;
        time_tm->tm_yday = my_tm.tm_yday;
    }

    return time_tm;
}

} // namespace


namespace apolloron {

/*! Constructor of DateTime.
    @param void
    @return void
 */
DateTime::DateTime() {
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;

    (*this).clear();
}


/*! Copy constructor of DateTime.
    @param datetime date and time to set
    @return void
 */
DateTime::DateTime(DateTime &datetime) {
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;

    (*this).clear();
    (*this).set(datetime);
}


/*! Copy constructor of DateTime.
    @param datetime date and time to set
    @return void
 */
DateTime::DateTime(TDateTime &datetime) {
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;

    (*this).clear();
    (*this).set(datetime);
}


/*! Copy constructor of DateTime.
    @param time_t_time date and time in time_t structure
    @param gmtoff      GMT offset (seconds)
    @return void
 */
DateTime::DateTime(time_t time_t_time, long gmtoff) {
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;

    (*this).clear();
    (*this).set(time_t_time, gmtoff);
}


/*! Copy constructor of DateTime.
    @param datetimestr String of date and time, like follows:
                   "2008-01-02T10:55:30+09:00"
                   "Wed, 02 Jan 2008 10:55:30 +0900"
                   "2008-01-02 10:55:30 +09:00"
                   "20080102105530+0900"
    @return void
 */
DateTime::DateTime(String &datetimestr) {
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;

    (*this).clear();
    (*this).set(datetimestr);
}


/*! Destructor of DateTime.
    @param void
    @return void
 */
DateTime::~DateTime() {
    clear();

    if ((*this).pTmp != NULL) {
        delete [] (*this).pTmp;
        (*this).pTmp = NULL;
    }
    if ((*this).pTmpLen != NULL) {
        delete (*this).pTmpLen;
        (*this).pTmpLen = NULL;
    }
}


/*! Acquisition of a temporary String
    @param void
    @return Pointer of a temporary String.
 */
String* DateTime::tmpStr() const {
    if (*pTmp == NULL) {
        *((*this).pTmp) = new (String *);
        **((*this).pTmp) = NULL;
    }
    if (**pTmp != NULL) {
        String **ptmptmp;
        ptmptmp = new String * [(*((*this).pTmpLen)) + 1];
        memcpy(ptmptmp, *((*this).pTmp), sizeof(String *) * (*((*this).pTmpLen)));
        delete [] *((*this).pTmp);
        *((*this).pTmp) = ptmptmp;
        (*((*this).pTmp))[*((*this).pTmpLen)] = new String;
        (*((*this).pTmpLen))++;
    } else {
        **pTmp = new String;
        *((*this).pTmpLen) = 1;
    }
    return **((*this).pTmp);
}


/*! Delete temporary Strings currently held
    @param void
    @retval true  success
    @retval false failure
 */
bool DateTime::clearTmpStr() const {
    if ((*this).pTmp != NULL) {
        if (*((*this).pTmp) != NULL) {
            if (**((*this).pTmp) != NULL) {
                long i;
                for (i = 0; i < *((*this).pTmpLen) ; i++) {
                    if ((*((*this).pTmp))[i] != NULL) {
                        delete ((*((*this).pTmp))[i]);
                    }
                }
                **pTmp = NULL;
            }
            delete [] *((*this).pTmp);
            *((*this).pTmp) = NULL;
        }
    }
    if ((*this).pTmpLen != NULL) {
        *((*this).pTmpLen) = 0;
    }
    return true;
}


/*! Clear DateTime object.
    @param void
    @retval true  success
    @retval false failure
 */
bool DateTime::clear() {
    memset(&((*this).tmpDateTime), 0, sizeof(TDateTime));

    // Set "1970-01-01 00:00:00 +0000"
    (*this).tmpDateTime.date.year = 1970;
    (*this).tmpDateTime.date.mon = 1;
    (*this).tmpDateTime.date.mday = 1;
    (*this).tmpDateTime.date.wday = 4;
    (*this).tmpDateTime.gmtoff = _get_server_gmtoff();

    clearTmpStr();

    return true;
}


/*! Copy date and time to DateTime.
    @param datetime date and time to set
    @return void
 */
DateTime &DateTime::operator = (const DateTime &datetime) {
    (*this).set(datetime);
    return (*this);
}


/*! Set current date and time.
    @param void
    @retval true  success
    @retval false failure
 */
bool DateTime::setNow() {
    long gmtoff;
    time_t tmp_time_t;

    gmtoff = _get_server_gmtoff();
    tmp_time_t = time(NULL);
    tmp_time_t += gmtoff;

    set(tmp_time_t, gmtoff);

    return true;
}


/*! Set date and time by copying from other DateTime object.
    @param  datetime DateTime object
    @retval true  success
    @retval false failure
 */
bool DateTime::set(const DateTime &datetime) {
    memcpy(&((*this).tmpDateTime), &(datetime.getDateTime()), sizeof(TDateTime));

    return true;
}


/*! Set date and time by String.
    @param datetimestr String of date and time, like follows:
                   "2008-01-02T10:55:30+09:00" (ISO8601)
                   "Wed, 02 Jan 2008 10:55:30 +0900" (RFC822)
                   " 8-Jul-2009 10:32:11 +0900" (INTERNALDATE)
                   "2008-01-02 10:55:30 +09:00" (ORIGINAL1)
                   "20080102105530+0900" (ORIGINAL2)
    @retval true  success
    @retval false failure
 */
bool DateTime::set(const String &datetimestr) {
    long gmtoff;
    enum {UNKNOWN, ISO8601, RFC822, INTERNALDATE, ORIGINAL1, ORIGINAL2} datetime_format;
    struct tm tmp_time_tm;
    char buf[100];
    const char *cpstr, *p;
    long i, len, col;
    int year;

    datetime_format = UNKNOWN;
    cpstr = datetimestr.trim().c_str();

    // detecting type of date and time format
    for (i = 0; i < 7; i++) {
        if (!strncasecmp(cpstr, AWEEK[i], strlen(AWEEK[i]))) {
            datetime_format = RFC822;
            break;
        }
    }
    if (datetime_format == UNKNOWN) {
        for (i = 0; i < 7; i++) {
            if (!strncasecmp(cpstr, FWEEK[i], strlen(FWEEK[i]))) {
                datetime_format = RFC822;
                break;
            }
        }
    }
    if (datetime_format == UNKNOWN) {
        for (i = 0; i < 12; i++) {
            char amon[6];
            amon[0] = '-';
            amon[1] = AMONTH[i][0];
            amon[2] = AMONTH[i][1];
            amon[3] = AMONTH[i][2];
            amon[4] = '-';
            amon[5] = '\0';
            if (strcasestr(cpstr, amon) != NULL) {
                datetime_format = INTERNALDATE;
                break;
            }
        }
    }
    if (datetime_format == UNKNOWN && isdigit(cpstr[0]) && isdigit(cpstr[1]) &&
            isdigit(cpstr[2]) && isdigit(cpstr[3])) {
        if (isdigit(cpstr[4])) {
            datetime_format = ORIGINAL2;
        } else if (cpstr[4] == '-' && isdigit(cpstr[5]) && isdigit(cpstr[6]) &&
                   cpstr[7] == '-' && isdigit(cpstr[8]) && isdigit(cpstr[9])) {
            if (cpstr[10] == 'T' || cpstr[10] == 't') {
                datetime_format = ISO8601;
            } else if (cpstr[10] == ' ') {
                datetime_format = ORIGINAL1;
            }
        }
    }
    if (datetime_format == UNKNOWN) {
        return false;
    }

    gmtoff = _get_server_gmtoff();
    if ((p = strchr(cpstr, '+')) != NULL || (p = strchr(cpstr, '-')) != NULL) {
        gmtoff = _gmtoff_str_to_long(p);
    }

    // Set "1970-01-01 00:00:00"
    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_year = 70;
    tmp_time_tm.tm_mon = 0;
    tmp_time_tm.tm_mday = 1;
    tmp_time_tm.tm_wday = 4;

    len = datetimestr.len();

    switch (datetime_format) {
        case ISO8601: // "2008-01-02T10:55:30+09:00"
            strncpy(buf, cpstr, 4);
            buf[4] = '\0';
            tmp_time_tm.tm_year = atoi(buf) -1900;
            strncpy(buf, cpstr+5, 2);
            buf[2] = '\0';
            tmp_time_tm.tm_mon = atoi(buf) - 1;
            strncpy(buf, cpstr+8, 2);
            buf[2] = '\0';
            tmp_time_tm.tm_mday = atoi(buf);
            if (13 <= len) {
                strncpy(buf, cpstr+11, 2);
                buf[2] = '\0';
                tmp_time_tm.tm_hour = atoi(buf);
            }
            if (16 <= len) {
                strncpy(buf, cpstr+14, 2);
                buf[2] = '\0';
                tmp_time_tm.tm_min = atoi(buf);
            }
            if (19 <= len) {
                strncpy(buf, cpstr+17, 2);
                buf[2] = '\0';
                tmp_time_tm.tm_sec = atoi(buf);
            }
            break;

        case RFC822: // "Wed, 02 Jan 2008 10:55:30 +0900"
            col = 0;
            for (i = 0; i < 7; i++) {
                if (!strncasecmp(cpstr, AWEEK[i], strlen(AWEEK[i]))) {
                    tmp_time_tm.tm_wday = i;
                    col = strlen(AWEEK[i]);
                    break;
                }
            }
            if (col == 0) {
                for (i = 0; i < 7; i++) {
                    if (!strncasecmp(cpstr, FWEEK[i], strlen(FWEEK[i]))) {
                        tmp_time_tm.tm_wday = i;
                        col = strlen(FWEEK[i]);
                        break;
                    }
                }
            }
            while (cpstr[col] == ',' || cpstr[col] == ' ') {
                col++;
            }
            tmp_time_tm.tm_mday = atoi(cpstr + col);
            while (isdigit(cpstr[col]) || cpstr[col] == ',' || cpstr[col] == ' ') {
                col++;
            }
            for (i = 0; i < 12; i++) {
                if (!strncasecmp(cpstr+col, AMONTH[i], strlen(AMONTH[i]))) {
                    tmp_time_tm.tm_mon = i;
                    col += strlen(AMONTH[i]);
                    break;
                }
            }
            if (12 <= i) {
                for (i = 0; i < 12; i++) {
                    if (!strncasecmp(cpstr+col, FMONTH[i], strlen(FMONTH[i]))) {
                        tmp_time_tm.tm_mon = i;
                        col += strlen(FMONTH[i]);
                        break;
                    }
                }
            }
            while (cpstr[col] == ',' || cpstr[col] == ' ') {
                col++;
            }
            year = atoi(cpstr + col);
            if (1900 < year) {
                year = year-1900;
            }
            tmp_time_tm.tm_year = year;
            while (isdigit(cpstr[col])) {
                col++;
            }
            while (cpstr[col] == ',' || cpstr[col] == ' ') {
                col++;
            }
            tmp_time_tm.tm_hour = atoi(cpstr + col);
            while (isdigit(cpstr[col])) {
                col++;
            }
            if (cpstr[col] == ':') {
                col++;
            }
            tmp_time_tm.tm_min = atoi(cpstr + col);
            while (isdigit(cpstr[col])) {
                col++;
            }
            if (cpstr[col] == ':') {
                col++;
            }
            tmp_time_tm.tm_sec = atoi(cpstr + col);
            break;

        case INTERNALDATE: // "8-Jul-2009 10:32:11 +0900"
            col = 0;
            tmp_time_tm.tm_mday = atoi(cpstr);
            while (cpstr[col] != '-' && cpstr[col] != '\0') {
                col++;
            }
            if (cpstr[col] == '\0') {
                break;
            }
            col++;
            for (i = 0; i < 12; i++) {
                if (!strncasecmp(cpstr+col, AMONTH[i], strlen(AMONTH[i]))) {
                    tmp_time_tm.tm_mon = i;
                    col += strlen(AMONTH[i]);
                    break;
                }
            }
            if (cpstr[col] == '-') {
                col++;
            }
            year = atoi(cpstr + col);
            if (1900 < year) {
                year = year-1900;
            }
            tmp_time_tm.tm_year = year;
            while (isdigit(cpstr[col])) {
                col++;
            }
            while (cpstr[col] == ',' || cpstr[col] == ' ') {
                col++;
            }
            tmp_time_tm.tm_hour = atoi(cpstr + col);
            while (isdigit(cpstr[col])) {
                col++;
            }
            if (cpstr[col] == ':') {
                col++;
            }
            tmp_time_tm.tm_min = atoi(cpstr + col);
            while (isdigit(cpstr[col])) {
                col++;
            }
            if (cpstr[col] == ':') {
                col++;
            }
            tmp_time_tm.tm_sec = atoi(cpstr + col);
            break;

        case ORIGINAL1: // "2008-01-02 10:55:30 +09:00"
            strncpy(buf, cpstr, 4);
            buf[4] = '\0';
            tmp_time_tm.tm_year = atoi(buf) -1900;
            strncpy(buf, cpstr+5, 2);
            buf[2] = '\0';
            tmp_time_tm.tm_mon = atoi(buf) - 1;
            strncpy(buf, cpstr+8, 2);
            buf[2] = '\0';
            tmp_time_tm.tm_mday = atoi(buf);

            col = 10;
            while (cpstr[col] == ' ' && cpstr[col] != '\0') {
                col++;
            }
            if (cpstr[col] == '\0') {
                break;
            }
            tmp_time_tm.tm_hour = atoi(cpstr + col);
            while (isdigit(cpstr[col])) {
                col++;
            }
            if (cpstr[col] == ':') {
                col++;
            }
            tmp_time_tm.tm_min = atoi(cpstr + col);
            while (isdigit(cpstr[col])) {
                col++;
            }
            if (cpstr[col] == ':') {
                col++;
            }
            tmp_time_tm.tm_sec = atoi(cpstr + col);
            break;

        case ORIGINAL2: // "20080102105530+0900"
            strncpy(buf, cpstr, 4);
            buf[4] = '\0';
            tmp_time_tm.tm_year = atoi(buf) -1900;
            strncpy(buf, cpstr+4, 2);
            buf[2] = '\0';
            tmp_time_tm.tm_mon = atoi(buf) - 1;
            strncpy(buf, cpstr+6, 2);
            buf[2] = '\0';
            tmp_time_tm.tm_mday = atoi(buf);

            if (10 <= len) {
                strncpy(buf, cpstr+8, 2);
                buf[2] = '\0';
                tmp_time_tm.tm_hour = atoi(buf);
            }
            if (12 <= len) {
                strncpy(buf, cpstr+10, 2);
                buf[2] = '\0';
                tmp_time_tm.tm_min = atoi(buf);
            }
            if (14 <= len) {
                strncpy(buf, cpstr+12, 2);
                buf[2] = '\0';
                tmp_time_tm.tm_sec = atoi(buf);
            }
            break;

        case UNKNOWN:
        default:
            return false;
    }

    _set_tm_wday_yday(&tmp_time_tm);
    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;
    (*this).tmpDateTime.gmtoff = gmtoff;

    return true;
}


/*! Set date and time by TDateTime object.
    @param  datetime TDateTime object
    @retval true  success
    @retval false failure
 */
bool DateTime::set(const TDateTime &datetime) {
    memcpy(&((*this).tmpDateTime), &datetime, sizeof(TDateTime));

    return true;
}


/*! Set date and time by time_t and GMT offset.
    @param  time_t_time time_t time
    @param  gmtoff      GMT offset (seconds)
    @retval true  success
    @retval false failure
 */
bool DateTime::set(const time_t time_t_time, long gmtoff) {
    struct tm tmp_time_tm;

    gmtime_r(&time_t_time, &tmp_time_tm);
    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;
    (*this).tmpDateTime.gmtoff = gmtoff;

    return true;
}


/*! Set date
    @param date TDate structure to set
    @retval true  success
    @retval false failure
 */
bool DateTime::setDate(const TDate &date) {
    memcpy(&((*this).tmpDateTime.date), &date, sizeof(TDate));

    return true;
}


/*! Set time
    @param time TTime structure to set
    @retval true  success
    @retval false failure
 */
bool DateTime::setTime(const TTime &time) {
    memcpy(&((*this).tmpDateTime.time), &time, sizeof(TTime));

    return true;
}


/*! Set GMT offset
    @param gmtoff GMT offset (seconds)
    @retval true  success
    @retval false failure
 */
bool DateTime::setGMTOffset(long gmtoff) {
    (*this).tmpDateTime.gmtoff = gmtoff;

    return true;
}


/*! Get date and/or time String by specified format
    @param format string format
             "%Y", "%04Y" :  year
             "%m", "%02m" :  month
             "%d", "%02d" :  day
             "%H", "%02H" :  hour
             "%M", "%02M" :  minute
             "%S", "%02S" :  seconds
             "%a"  :  abbreviated week ("Mon", "Tue", ...)
             "%b"  :  abbreviated month name ("Jan", "Feb", ...)
             "%y"  :  year(range 00 to 99)
             "%A"  :  The full weekday name ("Monday", "Tuesday", ...)
             "%B"  :  The full month name ("January", "February", ...)
             "%Z"  :  time-zone offset from GMT (range -24:00 to +24:00)
             "%z"  :  time-zone offset from GMT (range -2400 to +2400)
             "%%"  :  %
    @return formatted date and/or time String
 */
String &DateTime::toString(const String &format) const {
    String *tmp = (*this).tmpStr();
    const char *cformat;
    char buf[100];
    long i, len, gmtoff;

    cformat = format.c_str();
    len = format.len();

    *tmp = "";

    for (i = 0L; i < len; i++) {
        if (cformat[i] != '%') {
            buf[0] = cformat[i];
            buf[1] = '\0';
            *tmp += buf;
        } else if (cformat[i+1] == '%') {
            *tmp += "%";
            i++;
        } else if (cformat[i+1] == 'Y') {
            ::snprintf(buf, sizeof(buf), "%d", (*this).tmpDateTime.date.year);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == '0' && cformat[i+2] == '4' && cformat[i+3] == 'Y') {
            ::snprintf(buf, sizeof(buf), "%04d", (*this).tmpDateTime.date.year);
            *tmp += buf;
            i += 3;
        } else if (cformat[i+1] == 'm') {
            ::snprintf(buf, sizeof(buf), "%d", (*this).tmpDateTime.date.mon);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == '0' && cformat[i+2] == '2' && cformat[i+3] == 'm') {
            ::snprintf(buf, sizeof(buf), "%02d", (*this).tmpDateTime.date.mon);
            *tmp += buf;
            i += 3;
        } else if (cformat[i+1] == 'd') {
            ::snprintf(buf, sizeof(buf), "%d", (*this).tmpDateTime.date.mday);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == '0' && cformat[i+2] == '2' && cformat[i+3] == 'd') {
            ::snprintf(buf, sizeof(buf), "%02d", (*this).tmpDateTime.date.mday);
            *tmp += buf;
            i += 3;
        } else if (cformat[i+1] == 'H') {
            ::snprintf(buf, sizeof(buf), "%d", (*this).tmpDateTime.time.hour);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == '0' && cformat[i+2] == '2' && cformat[i+3] == 'H') {
            ::snprintf(buf, sizeof(buf), "%02d", (*this).tmpDateTime.time.hour);
            *tmp += buf;
            i += 3;
        } else if (cformat[i+1] == 'M') {
            ::snprintf(buf, sizeof(buf), "%d", (*this).tmpDateTime.time.min);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == '0' && cformat[i+2] == '2' && cformat[i+3] == 'M') {
            ::snprintf(buf, sizeof(buf), "%02d", (*this).tmpDateTime.time.min);
            *tmp += buf;
            i += 3;
        } else if (cformat[i+1] == 'S') {
            ::snprintf(buf, sizeof(buf), "%d", (*this).tmpDateTime.time.sec);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == '0' && cformat[i+2] == '2' && cformat[i+3] == 'S') {
            ::snprintf(buf, sizeof(buf), "%02d", (*this).tmpDateTime.time.sec);
            *tmp += buf;
            i += 3;
        } else if (cformat[i+1] == 'a') {
            if (0 <= (*this).tmpDateTime.date.wday && (*this).tmpDateTime.date.wday < 7) {
                *tmp += AWEEK[(*this).tmpDateTime.date.wday];
            } else {
                *tmp += "?";
            }
            i++;
        } else if (cformat[i+1] == 'b') {
            if (1 <= (*this).tmpDateTime.date.mon && (*this).tmpDateTime.date.mon <= 12) {
                *tmp += AMONTH[(*this).tmpDateTime.date.mon - 1];
            } else {
                *tmp += "?";
            }
            i++;
        } else if (cformat[i+1] == 'y') {
            ::snprintf(buf, sizeof(buf), "%02d", (*this).tmpDateTime.date.year-1900);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == 'A') {
            if (0 <= (*this).tmpDateTime.date.wday && (*this).tmpDateTime.date.wday < 7) {
                *tmp += FWEEK[(*this).tmpDateTime.date.wday];
            } else {
                *tmp += "?";
            }
            i++;
        } else if (cformat[i+1] == 'B') {
            if (1 <= (*this).tmpDateTime.date.mon && (*this).tmpDateTime.date.mon <= 12) {
                *tmp += FMONTH[(*this).tmpDateTime.date.mon - 1];
            } else {
                *tmp += "?";
            }
            i++;
        } else if (cformat[i+1] == 'Z') {
            gmtoff = (*this).tmpDateTime.gmtoff;
            if (gmtoff < 0) {
                *tmp += "-";
            } else {
                *tmp += "+";
            }
            ::snprintf(buf, sizeof(buf), "%02ld", gmtoff/3600);
            *tmp += buf;
            *tmp += ":";
            ::snprintf(buf, sizeof(buf), "%02ld", gmtoff%3600);
            *tmp += buf;
            i++;
        } else if (cformat[i+1] == 'z') {
            gmtoff = (*this).tmpDateTime.gmtoff;
            if (gmtoff < 0) {
                *tmp += "-";
            } else {
                *tmp += "+";
            }
            ::snprintf(buf, sizeof(buf), "%02ld", gmtoff/3600);
            *tmp += buf;
            ::snprintf(buf, sizeof(buf), "%02ld", gmtoff%3600);
            *tmp += buf;
            i++;
        }
    }

    return *tmp;
}


/*! Get multi-language date and/or time String by specified message type
    @param msgid message type
    @param lang language (ex: "en", "ja", "ko", "zh-cn")
    @return formatted date and/or time String
 */
String &DateTime::getMessage(const char * msgid, const char * lang, const char * charset) const {
    String *tmp = (*this).tmpStr();
    const char *langs[] = {"en", "ja", "ko", "zh-cn", "de", "es", "fr", NULL};
    const char *wday[7] = {NULL,}, *wday_long[7] = {NULL,};
    const char *month[12] = {NULL,}, *month_long[12] = {NULL,};
    const char *(*calendar_msg)[][2];
    String format;
    int i, detected;
    const char *cpstr;

    if (msgid == NULL) {
        *tmp = "";
        return *tmp;
    }

    detected = 0;
    for (i = 0; langs[i] != NULL; i++) {
        if (!strcasecmp(lang, langs[i])) {
            detected = 1;
            lang = langs[i];
            break;
        }
    }

    if (detected == 0) {
        lang = langs[0]; // default is "en"
    }

    calendar_msg = &calendar_msg_en;
    if (lang == langs[1]) {
        // ja
        calendar_msg = &calendar_msg_ja;
    } else if (lang == langs[2]) {
        // ko
        calendar_msg = &calendar_msg_ko;
    } else if (lang == langs[3]) {
        // zh-cn
        calendar_msg = &calendar_msg_zh_cn;
    } else if (lang == langs[4]) {
        // de
        calendar_msg = &calendar_msg_de;
    } else if (lang == langs[5]) {
        // es
        calendar_msg = &calendar_msg_es;
    } else if (lang == langs[6]) {
        // fr
        calendar_msg = &calendar_msg_fr;
    }

    format = "";
    for (i = 0; (cpstr = (*calendar_msg)[i][0]) != (const char *)0; i++) {
        if (!strcmp(cpstr, msgid)) {
            format = (*calendar_msg)[i][1];
            break;
        }
    }

    for (i = 0; (cpstr = (*calendar_msg)[i][0]) != (const char *)0; i++) {
        if (!strncmp(cpstr, "WDAY_", 5)) {
            if (!strcmp(cpstr, "WDAY_SUN")) {
                wday[0] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_MON")) {
                wday[1] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_TUE")) {
                wday[2] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_WED")) {
                wday[3] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_THU")) {
                wday[4] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_FRI")) {
                wday[5] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_SAT")) {
                wday[6] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_SUN_LONG")) {
                wday_long[0] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_MON_LONG")) {
                wday_long[1] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_TUE_LONG")) {
                wday_long[2] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_WED_LONG")) {
                wday_long[3] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_THU_LONG")) {
                wday_long[4] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_FRI_LONG")) {
                wday_long[5] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "WDAY_SAT_LONG")) {
                wday_long[6] = (*calendar_msg)[i][1];
            }
        } else if (!strncmp(cpstr, "MONTH", 5)) {
            if (!strcmp(cpstr, "MONTH1")) {
                month[0] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH2")) {
                month[1] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH3")) {
                month[2] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH4")) {
                month[3] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH5")) {
                month[4] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH6")) {
                month[5] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH7")) {
                month[6] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH8")) {
                month[7] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH9")) {
                month[8] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH10")) {
                month[9] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH11")) {
                month[10] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH12")) {
                month[11] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH1_LONG")) {
                month_long[0] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH2_LONG")) {
                month_long[1] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH3_LONG")) {
                month_long[2] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH4_LONG")) {
                month_long[3] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH5_LONG")) {
                month_long[4] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH6_LONG")) {
                month_long[5] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH7_LONG")) {
                month_long[6] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH8_LONG")) {
                month_long[7] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH9_LONG")) {
                month_long[8] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH10_LONG")) {
                month_long[9] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH11_LONG")) {
                month_long[10] = (*calendar_msg)[i][1];
            } else if (!strcmp(cpstr, "MONTH12_LONG")) {
                month_long[11] = (*calendar_msg)[i][1];
            }
        }
    }

    format = format.replace("%a", wday[(*this).tmpDateTime.date.wday]);
    format = format.replace("%A", wday_long[(*this).tmpDateTime.date.wday]);
    format = format.replace("%b", month[(*this).tmpDateTime.date.mon-1]);
    format = format.replace("%B", month_long[(*this).tmpDateTime.date.mon-1]);

    *tmp = (*this).toString(format).strconv("UTF-8", charset);

    format.clear();
    return *tmp;
}


/*! Get date and time
    @param void
    @return date and time (TDateTime structure)
*/
const TDateTime &DateTime::getDateTime() const {
    return (*this).tmpDateTime;
}


/*! Get date
    @param void
    @return date (TDate structure)
*/
const TDate &DateTime::getDate() const {
    return (*this).tmpDateTime.date;
}


/*! Get time
    @param void
    @return time (TTime structure)
*/
const TTime &DateTime::getTime() const {
    return (*this).tmpDateTime.time;
}


/*! Get GMT offset
    @param void
    @return GMT offset (seconds)
 */
long DateTime::getGMTOffset() const {
    return (*this).tmpDateTime.gmtoff;
}


/*! Adjust year
    @param year Number of year
    @retval true  success
    @retval false failure
 */
bool DateTime::adjustYear(int year) {
    struct tm tmp_time_tm;
    time_t time_t_time;

    (*this).tmpDateTime.date.year += year;

    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_mday = (*this).tmpDateTime.date.mday;
    tmp_time_tm.tm_mon = (*this).tmpDateTime.date.mon -1;
    tmp_time_tm.tm_year = (*this).tmpDateTime.date.year - 1900;
    tmp_time_tm.tm_wday = (*this).tmpDateTime.date.wday;
    tmp_time_tm.tm_yday = (*this).tmpDateTime.date.yday;
    tmp_time_tm.tm_sec = (*this).tmpDateTime.time.sec;
    tmp_time_tm.tm_min = (*this).tmpDateTime.time.min;
    tmp_time_tm.tm_hour = (*this).tmpDateTime.time.hour;
    _set_tm_wday_yday(&tmp_time_tm);

    time_t_time = _mkgmtime(&tmp_time_tm);
    gmtime_r(&time_t_time, &tmp_time_tm);

    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;

    return true;
}


/*! Adjust month
    @param month Number of month
    @retval true  success
    @retval false failure
 */
bool DateTime::adjustMonth(int month) {
    long m;
    struct tm tmp_time_tm;
    time_t time_t_time;

    m = (*this).tmpDateTime.date.mon - 1 + month;
    if (m < 0 || 11 < m) {
        (*this).tmpDateTime.date.year += (m / 12);
    }
    (*this).tmpDateTime.date.mon = m + 1;

    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_mday = (*this).tmpDateTime.date.mday;
    tmp_time_tm.tm_mon = (*this).tmpDateTime.date.mon -1;
    tmp_time_tm.tm_year = (*this).tmpDateTime.date.year - 1900;
    tmp_time_tm.tm_wday = (*this).tmpDateTime.date.wday;
    tmp_time_tm.tm_yday = (*this).tmpDateTime.date.yday;
    tmp_time_tm.tm_sec = (*this).tmpDateTime.time.sec;
    tmp_time_tm.tm_min = (*this).tmpDateTime.time.min;
    tmp_time_tm.tm_hour = (*this).tmpDateTime.time.hour;
    _set_tm_wday_yday(&tmp_time_tm);

    time_t_time = _mkgmtime(&tmp_time_tm);
    gmtime_r(&time_t_time, &tmp_time_tm);

    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;

    return true;
}


/*! Adjust date
    @param day Number of day
    @retval true  success
    @retval false failure
 */
bool DateTime::adjustDate(int day) {
    struct tm tmp_time_tm;
    time_t time_t_time;

    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_mday = (*this).tmpDateTime.date.mday;
    tmp_time_tm.tm_mon = (*this).tmpDateTime.date.mon -1;
    tmp_time_tm.tm_year = (*this).tmpDateTime.date.year - 1900;
    tmp_time_tm.tm_wday = (*this).tmpDateTime.date.wday;
    tmp_time_tm.tm_yday = (*this).tmpDateTime.date.yday;
    tmp_time_tm.tm_sec = (*this).tmpDateTime.time.sec;
    tmp_time_tm.tm_min = (*this).tmpDateTime.time.min;
    tmp_time_tm.tm_hour = (*this).tmpDateTime.time.hour;
    _set_tm_wday_yday(&tmp_time_tm);

    time_t_time = _mkgmtime(&tmp_time_tm);
    time_t_time += (day * 24 * 60 * 60);
    gmtime_r(&time_t_time, &tmp_time_tm);

    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;

    return true;
}


/*! Adjust hour
    @param hour Number of hour
    @retval true  success
    @retval false failure
 */
bool DateTime::adjustHour(int hour) {
    struct tm tmp_time_tm;
    time_t time_t_time;

    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_mday = (*this).tmpDateTime.date.mday;
    tmp_time_tm.tm_mon = (*this).tmpDateTime.date.mon -1;
    tmp_time_tm.tm_year = (*this).tmpDateTime.date.year - 1900;
    tmp_time_tm.tm_wday = (*this).tmpDateTime.date.wday;
    tmp_time_tm.tm_yday = (*this).tmpDateTime.date.yday;
    tmp_time_tm.tm_sec = (*this).tmpDateTime.time.sec;
    tmp_time_tm.tm_min = (*this).tmpDateTime.time.min;
    tmp_time_tm.tm_hour = (*this).tmpDateTime.time.hour;
    _set_tm_wday_yday(&tmp_time_tm);

    time_t_time = _mkgmtime(&tmp_time_tm);
    time_t_time += (hour * 60);
    gmtime_r(&time_t_time, &tmp_time_tm);

    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;

    return true;
}


/*! Adjust seconds
    @param sec Number of seconds
    @retval true  success
    @retval false failure
 */
bool DateTime::adjustSeconds(long sec) {
    struct tm tmp_time_tm;
    time_t time_t_time;

    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_mday = (*this).tmpDateTime.date.mday;
    tmp_time_tm.tm_mon = (*this).tmpDateTime.date.mon -1;
    tmp_time_tm.tm_year = (*this).tmpDateTime.date.year - 1900;
    tmp_time_tm.tm_wday = (*this).tmpDateTime.date.wday;
    tmp_time_tm.tm_yday = (*this).tmpDateTime.date.yday;
    tmp_time_tm.tm_sec = (*this).tmpDateTime.time.sec;
    tmp_time_tm.tm_min = (*this).tmpDateTime.time.min;
    tmp_time_tm.tm_hour = (*this).tmpDateTime.time.hour;
    _set_tm_wday_yday(&tmp_time_tm);

    time_t_time = _mkgmtime(&tmp_time_tm);
    time_t_time += sec;
    gmtime_r(&time_t_time, &tmp_time_tm);

    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;

    return true;
}


/*! change GMT offset
    @param gmtoff GMT offset (seconds) to change
    @retval true  success
    @retval false failure
 */
bool DateTime::changeGMTOffset(long gmtoff) {
    long gmtdiff;
    struct tm tmp_time_tm;
    time_t time_t_time;

    gmtdiff = gmtoff - (*this).tmpDateTime.gmtoff;

    memset(&tmp_time_tm, 0, sizeof(struct tm));
    tmp_time_tm.tm_mday = (*this).tmpDateTime.date.mday;
    tmp_time_tm.tm_mon = (*this).tmpDateTime.date.mon -1;
    tmp_time_tm.tm_year = (*this).tmpDateTime.date.year - 1900;
    tmp_time_tm.tm_wday = (*this).tmpDateTime.date.wday;
    tmp_time_tm.tm_yday = (*this).tmpDateTime.date.yday;
    tmp_time_tm.tm_sec = (*this).tmpDateTime.time.sec;
    tmp_time_tm.tm_min = (*this).tmpDateTime.time.min;
    tmp_time_tm.tm_hour = (*this).tmpDateTime.time.hour;
    _set_tm_wday_yday(&tmp_time_tm);

    time_t_time = _mkgmtime(&tmp_time_tm);
    time_t_time += gmtdiff;
    gmtime_r(&time_t_time, &tmp_time_tm);

    (*this).tmpDateTime.date.mday = tmp_time_tm.tm_mday;
    (*this).tmpDateTime.date.mon = tmp_time_tm.tm_mon + 1;
    (*this).tmpDateTime.date.year = tmp_time_tm.tm_year + 1900;
    (*this).tmpDateTime.date.wday = tmp_time_tm.tm_wday;
    (*this).tmpDateTime.date.yday = tmp_time_tm.tm_yday;
    (*this).tmpDateTime.time.sec = tmp_time_tm.tm_sec;
    (*this).tmpDateTime.time.min = tmp_time_tm.tm_min;
    (*this).tmpDateTime.time.hour = tmp_time_tm.tm_hour;
    (*this).tmpDateTime.gmtoff = gmtoff;

    return true;
}


/*! Compare date and time
    @param value to compare with
    @retval 1  datetime >  value
    @retval 0  datetime == value
    @retval -1 datetime <  value
 */
int DateTime::compare(const DateTime &value) const {
    int d;
    const TDateTime &datetime1 = (*this).tmpDateTime;
    const TDateTime &datetime2 = value.getDateTime();
    struct tm tmp_time_tm1, tmp_time_tm2;
    time_t time_t_time1, time_t_time2;

    memset(&tmp_time_tm1, 0, sizeof(struct tm));
    tmp_time_tm1.tm_mday = datetime1.date.mday;
    tmp_time_tm1.tm_mon = datetime1.date.mon -1;
    tmp_time_tm1.tm_year = datetime1.date.year - 1900;
    tmp_time_tm1.tm_wday = datetime1.date.wday;
    tmp_time_tm1.tm_yday = datetime1.date.yday;
    tmp_time_tm1.tm_sec = datetime1.time.sec;
    tmp_time_tm1.tm_min = datetime1.time.min;
    tmp_time_tm1.tm_hour = datetime1.time.hour;
    _set_tm_wday_yday(&tmp_time_tm1);
    time_t_time1 = _mkgmtime(&tmp_time_tm1);
    time_t_time1 -= datetime1.gmtoff;

    memset(&tmp_time_tm2, 0, sizeof(struct tm));
    tmp_time_tm2.tm_mday = datetime2.date.mday;
    tmp_time_tm2.tm_mon = datetime2.date.mon -1;
    tmp_time_tm2.tm_year = datetime2.date.year - 1900;
    tmp_time_tm2.tm_wday = datetime2.date.wday;
    tmp_time_tm2.tm_yday = datetime2.date.yday;
    tmp_time_tm2.tm_sec = datetime2.time.sec;
    tmp_time_tm2.tm_min = datetime2.time.min;
    tmp_time_tm2.tm_hour = datetime2.time.hour;
    _set_tm_wday_yday(&tmp_time_tm2);
    time_t_time2 = _mkgmtime(&tmp_time_tm2);
    time_t_time2 -= datetime2.gmtoff;

    d = ((long)time_t_time1) - ((long)time_t_time2);
    if (0 < d) {
        d = 1;
    } else if (d < 0) {
        d = -1;
    }

    return d;
}


/*! Compare date and time
    @param value date to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int DateTime::operator == (const DateTime &value) const {
    return ((*this).compare(value) == 0)?1:0;
}


/*! Compare date and time
    @param value date to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int DateTime::operator != (const DateTime &value) const {
    return ((*this).compare(value) != 0)?1:0;
}


/*! Compare date and time
    @param value date to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int DateTime::operator <= (const DateTime &value) const {
    return ((*this).compare(value) <= 0)?1:0;
}


/*! Compare date and time
    @param value date to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int DateTime::operator >= (const DateTime &value) const {
    return ((*this).compare(value) >= 0)?1:0;
}


/*! Compare date and time
    @param value date to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int DateTime::operator < (const DateTime &value) const {
    return ((*this).compare(value) < 0)?1:0;
}


/*! Compare date and time
    @param value date to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int DateTime::operator > (const DateTime &value) const {
    return ((*this).compare(value) > 0)?1:0;
}

} // namespace apolloron
