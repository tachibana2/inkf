/******************************************************************************/
/*! @file msg_es.cc
    @brief DateTime messages of Spanish(generic).
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
const char *calendar_msg_es[][2] = {
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
    {"TODAY",             "Hoy"},
    {"YESTERDAY",         "Ayer"},
    {"WDAY_SUN",          "Dom"},
    {"WDAY_MON",          "Lun"},
    {"WDAY_TUE",          "Mar"},
    {"WDAY_WED",          "Mi\xc3\xa9"},
    {"WDAY_THU",          "Jue"},
    {"WDAY_FRI",          "Vie"},
    {"WDAY_SAT",          "S\xc3\xa1\x62"},
    {"WDAY_SUN_LONG",     "Domingo"},
    {"WDAY_MON_LONG",     "Lunes"},
    {"WDAY_TUE_LONG",     "Martes"},
    {"WDAY_WED_LONG",     "Mi\xc3\xa9rcoles"},
    {"WDAY_THU_LONG",     "Jueves"},
    {"WDAY_FRI_LONG",     "Viernes"},
    {"WDAY_SAT_LONG",     "S\xc3\xa1\x62\x61\x61o"},
    {"MONTH1",            "Ene"},
    {"MONTH2",            "Feb"},
    {"MONTH3",            "Mar"},
    {"MONTH4",            "Abr"},
    {"MONTH5",            "May"},
    {"MONTH6",            "Jun"},
    {"MONTH7",            "Jul"},
    {"MONTH8",            "Ago"},
    {"MONTH9",            "Sep"},
    {"MONTH10",           "Oct"},
    {"MONTH11",           "Nov"},
    {"MONTH12",           "Dic"},
    {"MONTH1_LONG",       "Enero"},
    {"MONTH2_LONG",       "Febrero"},
    {"MONTH3_LONG",       "Marzo"},
    {"MONTH4_LONG",       "Abril"},
    {"MONTH5_LONG",       "Mayo"},
    {"MONTH6_LONG",       "Junio"},
    {"MONTH7_LONG",       "Julio"},
    {"MONTH8_LONG",       "Agosto"},
    {"MONTH9_LONG",       "Septiembre"},
    {"MONTH10_LONG",      "Octubre"},
    {"MONTH11_LONG",      "Noviembre"},
    {"MONTH12_LONG",      "Diciembre"},
    {(const char*)0,      (const char *)0}
};


} // namespace apolloron
