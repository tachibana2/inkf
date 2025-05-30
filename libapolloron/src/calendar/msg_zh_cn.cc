/******************************************************************************/
/*! @file msg_zh_cn.cc
    @brief DateTime messages of Chinese(generic).
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
const char *calendar_msg_zh_cn[][2] = {
    {"YEAR",              "%Y\xe5\xb9\xb4"},
    {"MONTH",             "%m\xe6\x9c\x88"},
    {"YEARMONTH",         "%Y\xe5\xb9\xb4%m\xe6\x9c\x88"},
    {"DAY",               "%d\xe6\x97\xa5"},
    {"DATE",              "%Y\xe5\xb9\xb4%m\xe6\x9c\x88%d\xe6\x97\xa5"},
    {"DATE_SHORT",        "%y/%m/%d"},
    {"DATE_WDAY",         "%Y\xe5\xb9\xb4%m\xe6\x9c\x88%d\xe6\x97\xa5(%a)"},
    {"DATE_SHORT_WDAY",   "%y/%m/%d (%a)"},
    {"MONTHDAY",          "%m\xe6\x9c\x88%d\xe6\x97\xa5"},
    {"MONTHDAY_WDAY",     "%m\xe6\x9c\x88%d\xe6\x97\xa5 (%a)"},
    {"MONTHDAY_SHORT",    "%m/%d"},
    {"MONTHDAY_SHORT_WDAY","%m/%d (%a)"},
    {"HOUR",              "%H\xe7\x82\xb9"},
    {"MINUTE",            "%M\xe5\x88\x86"},
    {"SECOND",            "%S\xe7\xa7\x92"},
    {"WDAY",              "%a"},
    {"WDAY_LONG",         "%A"},
    {"TIME",              "%H\xe7\x82\xb9%M\xe5\x88\x86"},
    {"TIME_SEC",          "%H\xe7\x82\xb9%M\xe5\x88\x86%S\xe7\xa7\x92"},
    {"DATETIME",          "%Y\xe5\xb9\xb4%m\xe6\x9c\x88%d\xe6\x97\xa5 %H:%M"},
    {"DATETIME_SEC",      "%Y\xe5\xb9\xb4%m\xe6\x9c\x88%d\xe6\x97\xa5 %H:%M:%S"},
    {"DATETIME_WDAY",     "%Y\xe5\xb9\xb4%m\xe6\x9c\x88%d\xe6\x97\xa5(%a) %H:%M"},
    {"DATETIME_WDAY_SEC", "%Y\xe5\xb9\xb4%m\xe6\x9c\x88%d\xe6\x97\xa5(%a) %H:%M:%S"},
    {"TODAY",             "\xe4\xbb\x8a\xe5\xa4\xa9"},
    {"YESTERDAY",         "\xe6\x98\xa8\xe5\xa4\xa9"},
    {"WDAY_SUN",          "\xe6\x97\xa5"},
    {"WDAY_MON",          "\xe4\xb8\x80"},
    {"WDAY_TUE",          "\xe4\xba\x8c"},
    {"WDAY_WED",          "\xe4\xb8\x89"},
    {"WDAY_THU",          "\xe5\x9b\x9b"},
    {"WDAY_FRI",          "\xe4\xba\x94"},
    {"WDAY_SAT",          "\xe5\x85\xad"},
    {"WDAY_SUN_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe6\x97\xa5"},
    {"WDAY_MON_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x80"},
    {"WDAY_TUE_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x8c"},
    {"WDAY_WED_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe4\xb8\x89"},
    {"WDAY_THU_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe5\x9b\x9b"},
    {"WDAY_FRI_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe4\xba\x94"},
    {"WDAY_SAT_LONG",     "\xe6\x98\x9f\xe6\x9c\x9f\xe5\x85\xad"},
    {"MONTH1",            "1\xe6\x9c\x88"},
    {"MONTH2",            "2\xe6\x9c\x88"},
    {"MONTH3",            "3\xe6\x9c\x88"},
    {"MONTH4",            "4\xe6\x9c\x88"},
    {"MONTH5",            "5\xe6\x9c\x88"},
    {"MONTH6",            "6\xe6\x9c\x88"},
    {"MONTH7",            "7\xe6\x9c\x88"},
    {"MONTH8",            "8\xe6\x9c\x88"},
    {"MONTH9",            "9\xe6\x9c\x88"},
    {"MONTH10",           "10\xe6\x9c\x88"},
    {"MONTH11",           "11\xe6\x9c\x88"},
    {"MONTH12",           "12\xe6\x9c\x88"},
    {"MONTH1_LONG",       "1\xe6\x9c\x88"},
    {"MONTH2_LONG",       "2\xe6\x9c\x88"},
    {"MONTH3_LONG",       "3\xe6\x9c\x88"},
    {"MONTH4_LONG",       "4\xe6\x9c\x88"},
    {"MONTH5_LONG",       "5\xe6\x9c\x88"},
    {"MONTH6_LONG",       "6\xe6\x9c\x88"},
    {"MONTH7_LONG",       "7\xe6\x9c\x88"},
    {"MONTH8_LONG",       "8\xe6\x9c\x88"},
    {"MONTH9_LONG",       "9\xe6\x9c\x88"},
    {"MONTH10_LONG",      "10\xe6\x9c\x88"},
    {"MONTH11_LONG",      "11\xe6\x9c\x88"},
    {"MONTH12_LONG",      "12\xe6\x9c\x88"},
    {(const char*)0,      (const char *)0}
};


} // namespace apolloron
