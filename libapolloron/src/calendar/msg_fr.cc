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
const char *calendar_msg_fr[][2] = {
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
    {"TODAY",             "Aujourd'hui"},
    {"YESTERDAY",         "Hier"},
    {"WDAY_SUN",          "Dim"},
    {"WDAY_MON",          "Lun"},
    {"WDAY_TUE",          "Mar"},
    {"WDAY_WED",          "Mer"},
    {"WDAY_THU",          "Jeu"},
    {"WDAY_FRI",          "Ven"},
    {"WDAY_SAT",          "Sam"},
    {"WDAY_SUN_LONG",     "Dimanche"},
    {"WDAY_MON_LONG",     "Lundi"},
    {"WDAY_TUE_LONG",     "Mardi"},
    {"WDAY_WED_LONG",     "Mercredi"},
    {"WDAY_THU_LONG",     "Jeudi"},
    {"WDAY_FRI_LONG",     "Vendredi"},
    {"WDAY_SAT_LONG",     "Samedi"},
    {"MONTH1",            "Jan"},
    {"MONTH2",            "F\xc3\xa9v"},
    {"MONTH3",            "Mar"},
    {"MONTH4",            "Avr"},
    {"MONTH5",            "Mai"},
    {"MONTH6",            "Juin"},
    {"MONTH7",            "Juil"},
    {"MONTH8",            "Ao\xc3\xbbt"},
    {"MONTH9",            "Sep"},
    {"MONTH10",           "Oct"},
    {"MONTH11",           "Nov"},
    {"MONTH12",           "D\xc3\xa9\x63"},
    {"MONTH1_LONG",       "Janvier"},
    {"MONTH2_LONG",       "F\xc3\xa9vrier"},
    {"MONTH3_LONG",       "Mars"},
    {"MONTH4_LONG",       "Avril"},
    {"MONTH5_LONG",       "Mai"},
    {"MONTH6_LONG",       "Juin"},
    {"MONTH7_LONG",       "Juillet"},
    {"MONTH8_LONG",       "Ao\xc3\xbbt"},
    {"MONTH9_LONG",       "Septembre"},
    {"MONTH10_LONG",      "Octobre"},
    {"MONTH11_LONG",      "Novembre"},
    {"MONTH12_LONG",      "D\xc3\xa9\x63\x65mbre"},
    {(const char*)0,      (const char *)0}
};


} // namespace apolloron
