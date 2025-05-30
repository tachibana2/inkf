/******************************************************************************/
/*! @file msg_ko.cc
    @brief DateTime messages of Korean(generic).
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
const char *calendar_msg_ko[][2] = {
    {"YEAR",              "%Y\xeb\x85\x84"},
    {"MONTH",             "%m\xec\x9b\x94"},
    {"YEARMONTH",         "%Y\xeb\x85\x84%m\xec\x9b\x94"},
    {"DAY",               "%d\xec\x9d\xbc"},
    {"DATE",              "%Y\xeb\x85\x84%m\xec\x9b\x94%d\xec\x9d\xbc"},
    {"DATE_SHORT",        "%y/%m/%d"},
    {"DATE_WDAY",         "%Y\xeb\x85\x84%m\xec\x9b\x94%d\xec\x9d\xbc(%a)"},
    {"DATE_SHORT_WDAY",   "%y/%m/%d (%a)"},
    {"MONTHDAY",          "%m\xec\x9b\x94%d\xec\x9d\xbc"},
    {"MONTHDAY_WDAY",     "%m\xec\x9b\x94%d\xec\x9d\xbc (%a)"},
    {"MONTHDAY_SHORT",    "%m/%d"},
    {"MONTHDAY_SHORT_WDAY","%m/%d (%a)"},
    {"HOUR",              "%H\xec\x8b\x9c"},
    {"MINUTE",            "%M\xeb\xb6\x84"},
    {"SECOND",            "%S\xec\xb4\x88"},
    {"WDAY",              "%a"},
    {"WDAY_LONG",         "%A"},
    {"TIME",              "%H\xec\x8b\x9c%M\xeb\xb6\x84"},
    {"TIME_SEC",          "%H\xec\x8b\x9c%M\xeb\xb6\x84%S\xec\xb4\x88"},
    {"DATETIME",          "%Y\xeb\x85\x84%m\xec\x9b\x94%d\xec\x9d\xbc %H:%M"},
    {"DATETIME_SEC",      "%Y\xeb\x85\x84%m\xec\x9b\x94%d\xec\x9d\xbc %H:%M:%S"},
    {"DATETIME_WDAY",     "%Y\xeb\x85\x84%m\xec\x9b\x94%d\xec\x9d\xbc(%a) %H:%M"},
    {"DATETIME_WDAY_SEC", "%Y\xeb\x85\x84%m\xec\x9b\x94%d\xec\x9d\xbc(%a) %H:%M:%S"},
    {"TODAY",             "\xec\x98\xa4\xeb\x8a\x98"},
    {"YESTERDAY",         "\xec\x96\xb4\xec\xa0\x9c"},
    {"WDAY_SUN",          "\xec\x9d\xbc"},
    {"WDAY_MON",          "\xec\x9b\x94"},
    {"WDAY_TUE",          "\xed\x99\x94"},
    {"WDAY_WED",          "\xec\x88\x98"},
    {"WDAY_THU",          "\xeb\xaa\xa9"},
    {"WDAY_FRI",          "\xea\xb8\x88"},
    {"WDAY_SAT",          "\xed\x86\xa0"},
    {"WDAY_SUN_LONG",     "\xec\x9d\xbc\xec\x9a\x94\xec\x9d\xbc"},
    {"WDAY_MON_LONG",     "\xec\x9b\x94\xec\x9a\x94\xec\x9d\xbc"},
    {"WDAY_TUE_LONG",     "\xed\x99\x94\xec\x9a\x94\xec\x9d\xbc"},
    {"WDAY_WED_LONG",     "\xec\x88\x98\xec\x9a\x94\xec\x9d\xbc"},
    {"WDAY_THU_LONG",     "\xeb\xaa\xa9\xec\x9a\x94\xec\x9d\xbc"},
    {"WDAY_FRI_LONG",     "\xea\xb8\x88\xec\x9a\x94\xec\x9d\xbc"},
    {"WDAY_SAT_LONG",     "\xed\x86\xa0\xec\x9a\x94\xec\x9d\xbc"},
    {"MONTH1",            "1\xec\x9b\x94"},
    {"MONTH2",            "2\xec\x9b\x94"},
    {"MONTH3",            "3\xec\x9b\x94"},
    {"MONTH4",            "4\xec\x9b\x94"},
    {"MONTH5",            "5\xec\x9b\x94"},
    {"MONTH6",            "6\xec\x9b\x94"},
    {"MONTH7",            "7\xec\x9b\x94"},
    {"MONTH8",            "8\xec\x9b\x94"},
    {"MONTH9",            "9\xec\x9b\x94"},
    {"MONTH10",           "10\xec\x9b\x94"},
    {"MONTH11",           "11\xec\x9b\x94"},
    {"MONTH12",           "12\xec\x9b\x94"},
    {"MONTH1_LONG",       "1\xec\x9b\x94"},
    {"MONTH2_LONG",       "2\xec\x9b\x94"},
    {"MONTH3_LONG",       "3\xec\x9b\x94"},
    {"MONTH4_LONG",       "4\xec\x9b\x94"},
    {"MONTH5_LONG",       "5\xec\x9b\x94"},
    {"MONTH6_LONG",       "6\xec\x9b\x94"},
    {"MONTH7_LONG",       "7\xec\x9b\x94"},
    {"MONTH8_LONG",       "8\xec\x9b\x94"},
    {"MONTH9_LONG",       "9\xec\x9b\x94"},
    {"MONTH10_LONG",      "10\xec\x9b\x94"},
    {"MONTH11_LONG",      "11\xec\x9b\x94"},
    {"MONTH12_LONG",      "12\xec\x9b\x94"},
    {(const char*)0,      (const char *)0}
};


} // namespace apolloron
