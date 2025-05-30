/******************************************************************************/
/*! @file msg_de.cc
    @brief DateTime messages of Deutsch(generic).
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
const char *calendar_msg_de[][2] = {
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
    {"TODAY",             "Heute"},
    {"YESTERDAY",         "Gestern"},
    {"WDAY_SUN",          "Son"},
    {"WDAY_MON",          "Mon"},
    {"WDAY_TUE",          "Die"},
    {"WDAY_WED",          "Mit"},
    {"WDAY_THU",          "Don"},
    {"WDAY_FRI",          "Fre"},
    {"WDAY_SAT",          "Sam"},
    {"WDAY_SUN_LONG",     "Sonntag"},
    {"WDAY_MON_LONG",     "Mon"},
    {"WDAY_TUE_LONG",     "Dienstag"},
    {"WDAY_WED_LONG",     "Mittwoch"},
    {"WDAY_THU_LONG",     "Donnerstag"},
    {"WDAY_FRI_LONG",     "Freitag"},
    {"WDAY_SAT_LONG",     "Samstag"},
    {"MONTH1",            "Jan"},
    {"MONTH2",            "Feb"},
    {"MONTH3",            "M\xc3\xa4r"},
    {"MONTH4",            "Apr"},
    {"MONTH5",            "Mai"},
    {"MONTH6",            "Jun"},
    {"MONTH7",            "Jul"},
    {"MONTH8",            "Aug"},
    {"MONTH9",            "Sep"},
    {"MONTH10",           "Okt"},
    {"MONTH11",           "Nov"},
    {"MONTH12",           "Dez"},
    {"MONTH1_LONG",       "Januar"},
    {"MONTH2_LONG",       "Februar"},
    {"MONTH3_LONG",       "M\xc3\xa4rz"},
    {"MONTH4_LONG",       "April"},
    {"MONTH5_LONG",       "Mai"},
    {"MONTH6_LONG",       "Juni"},
    {"MONTH7_LONG",       "Juli"},
    {"MONTH8_LONG",       "August"},
    {"MONTH9_LONG",       "September"},
    {"MONTH10_LONG",      "Oktober"},
    {"MONTH11_LONG",      "November"},
    {"MONTH12_LONG",      "Dezember"},
    {(const char*)0,      (const char *)0}
};


} // namespace apolloron
