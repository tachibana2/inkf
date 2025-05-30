/******************************************************************************/
/*! @file msg_en.cc
    @brief DateTime messages of English(generic).
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

namespace apolloron {

/*
  Date and time messages in UTF-8
  %Y=year, %m=month, %d=day, %H=hour, %M=minute, %S=second,
  %y=year(range 00 to 99),
  %B=the full month name, %b=abbreviated month name,
  %A=the full weekday name, %a=abbreviated week
*/
const char *calendar_msg_en[][2] = {
    {"YEAR",              "%Y"},
    {"MONTH",             "%B"},
    {"YEARMONTH",         "%B %Y"},
    {"DAY",               "%d"},
    {"DATE",              "%b %d, %Y"},
    {"DATE_SHORT",        "%b %d, %y"},
    {"DATE_WDAY",         "%a %B %d, %Y"},
    {"DATE_SHORT_WDAY",   "%b %d, %y (%a)"},
    {"MONTHDAY",          "%m %d"},
    {"MONTHDAY_WDAY",     "%m %d (%a)"},
    {"MONTHDAY_SHORT",    "%b %d"},
    {"MONTHDAY_SHORT_WDAY","%b %d (%a)"},
    {"HOUR",              "%H"},
    {"MINUTE",            "%M"},
    {"SECOND",            "%S"},
    {"WDAY",              "%a"},
    {"WDAY_LONG",         "%A"},
    {"TIME",              "%H:%M"},
    {"TIME_SEC",          "%H:%M:%S"},
    {"DATETIME",          "%H:%M %b %d, %Y"},
    {"DATETIME_SEC",      "%H:%M:%S %b %d, %Y"},
    {"DATETIME_WDAY",     "%H:%M %a %B %d, %Y"},
    {"DATETIME_WDAY_SEC", "%H:%M:%S %a %B %d, %Y"},
    {"TODAY",             "Today"},
    {"YESTERDAY",         "Yesterday"},
    {"WDAY_SUN",          "Sun"},
    {"WDAY_MON",          "Mon"},
    {"WDAY_TUE",          "Tue"},
    {"WDAY_WED",          "Wed"},
    {"WDAY_THU",          "Thu"},
    {"WDAY_FRI",          "Fri"},
    {"WDAY_SAT",          "Sat"},
    {"WDAY_SUN_LONG",     "Sunday"},
    {"WDAY_MON_LONG",     "Monday"},
    {"WDAY_TUE_LONG",     "Tuesday"},
    {"WDAY_WED_LONG",     "Wednesday"},
    {"WDAY_THU_LONG",     "Thursday"},
    {"WDAY_FRI_LONG",     "Friday"},
    {"WDAY_SAT_LONG",     "Saturday"},
    {"MONTH1",            "Jan"},
    {"MONTH2",            "Feb"},
    {"MONTH3",            "Mar"},
    {"MONTH4",            "Apr"},
    {"MONTH5",            "May"},
    {"MONTH6",            "Jun"},
    {"MONTH7",            "Jul"},
    {"MONTH8",            "Aug"},
    {"MONTH9",            "Sep"},
    {"MONTH10",           "Oct"},
    {"MONTH11",           "Nov"},
    {"MONTH12",           "Dec"},
    {"MONTH1_LONG",       "January"},
    {"MONTH2_LONG",       "February"},
    {"MONTH3_LONG",       "March"},
    {"MONTH4_LONG",       "April"},
    {"MONTH5_LONG",       "May"},
    {"MONTH6_LONG",       "June"},
    {"MONTH7_LONG",       "July"},
    {"MONTH8_LONG",       "August"},
    {"MONTH9_LONG",       "September"},
    {"MONTH10_LONG",      "October"},
    {"MONTH11_LONG",      "November"},
    {"MONTH12_LONG",      "December"},
    {(const char*)0,      (const char *)0}
};


} // namespace apolloron
