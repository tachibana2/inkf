/******************************************************************************/
/*! @file charset.cc
    @brief charset functions
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __ICONV == 1
#include <errno.h>
#include <sys/errno.h>
#include <iconv.h>
#endif

#include "apolloron.h"
#include "charset/table_unicode_sjis.h"
#include "charset/table_sjis_unicode.h"
#include "charset/table_unicode_euckr.h"
#include "charset/table_euckr_unicode.h"
#include "charset/table_unicode_gbk.h"
#include "charset/table_gbk_unicode.h"
#include "charset/table_unicode_big5.h"
#include "charset/table_big5_unicode.h"
#include "charset/table_iso8859_unicode.h"
#include "charset/table_europe_unicode.h"
#include "charset/table_gb18030_utf8.h"
#include "charset/table_utf8_gb18030.h"
#include "charset/table_charwidth.h"
#include "charset.h"

namespace {
const char *SJIS_EUCJP_FA40[16*12] = { /* SJIS FA40-FAFF */
    "\xF3\xF3", "\xF3\xF4", "\xF3\xF5", "\xF3\xF6", "\xF3\xF7", "\xF3\xF8", "\xF3\xF9", "\xF3\xFA",
    "\xF3\xFB", "\xF3\xFC",         "",         "",         "",         "",         "",         "",
            "",         "",         "",         "",         "", "\xA2\xC3", "\xF4\xA9", "\xF4\xAA",
            "",         "",         "",         "", "\xD4\xE3", "\xDC\xDF", "\xE4\xE9", "\xE3\xF8",
    "\xD9\xA1", "\xB1\xBB", "\xF4\xAE", "\xC2\xAD", "\xC3\xFC", "\xE4\xD0", "\xC2\xBF", "\xBC\xF4",
    "\xB0\xA9", "\xB0\xC8", "\xF4\xAF", "\xB0\xD2", "\xB0\xD4", "\xB0\xE3", "\xB0\xEE", "\xB1\xA7",
    "\xB1\xA3", "\xB1\xAC", "\xB1\xA9", "\xB1\xBE", "\xB1\xDF", "\xB1\xD8", "\xB1\xC8", "\xB1\xD7",
    "\xB1\xE3", "\xB1\xF4", "\xB1\xE1", "\xB2\xA3", "\xF4\xB0", "\xB2\xBB", "\xB2\xE6",         "",
    "\xB2\xED", "\xB2\xF5", "\xB2\xFC", "\xF4\xB1", "\xB3\xB5", "\xB3\xD8", "\xB3\xDB", "\xB3\xE5",
    "\xB3\xEE", "\xB3\xFB", "\xF4\xB2", "\xF4\xB3", "\xB4\xC0", "\xB4\xC7", "\xB4\xD0", "\xB4\xDE",
    "\xF4\xB4", "\xB5\xAA", "\xF4\xB5", "\xB5\xAF", "\xB5\xC4", "\xB5\xE8", "\xF4\xB6", "\xB7\xC2",
    "\xB7\xE4", "\xB7\xE8", "\xB7\xE7", "\xF4\xB7", "\xF4\xB8", "\xF4\xB9", "\xB8\xCE", "\xB8\xE1",
    "\xB8\xF5", "\xB8\xF7", "\xB8\xF8", "\xB8\xFC", "\xB9\xAF", "\xB9\xB7", "\xBA\xBE", "\xBA\xDB",
    "\xCD\xAA", "\xBA\xE1", "\xF4\xBA", "\xBA\xEB", "\xBB\xB3", "\xBB\xB8", "\xF4\xBB", "\xBB\xCA",
    "\xF4\xBC", "\xF4\xBD", "\xBB\xD0", "\xBB\xDE", "\xBB\xF4", "\xBB\xF5", "\xBB\xF9", "\xBC\xE4",
    "\xBC\xED", "\xBC\xFE", "\xF4\xBE", "\xBD\xC2", "\xBD\xE7", "\xF4\xBF", "\xBD\xF0", "\xBE\xB0",
    "\xBE\xAC", "\xF4\xC0", "\xBE\xB3", "\xBE\xBD", "\xBE\xCD", "\xBE\xC9", "\xBE\xE4", "\xBF\xA8",
    "\xBF\xC9", "\xC0\xC4", "\xC0\xE4", "\xC0\xF4", "\xC1\xA6", "\xF4\xC1", "\xC1\xF5", "\xC1\xFC",
    "\xF4\xC2", "\xC1\xF8", "\xC2\xAB", "\xC2\xA1", "\xC2\xA5", "\xF4\xC3", "\xC2\xB8", "\xC2\xBA",
    "\xF4\xC4", "\xC2\xC4", "\xC2\xD2", "\xC2\xD7", "\xC2\xDB", "\xC2\xDE", "\xC2\xED", "\xC2\xF0",
    "\xF4\xC5", "\xC3\xA1", "\xC3\xB5", "\xC3\xC9", "\xC3\xB9", "\xF4\xC6", "\xC3\xD8", "\xC3\xFE",
    "\xF4\xC7", "\xC4\xCC", "\xF4\xC8", "\xC4\xD9", "\xC4\xEA", "\xC4\xFD", "\xF4\xC9", "\xC5\xA7",
    "\xC5\xB5", "\xC5\xB6", "\xF4\xCA", "\xC5\xD5", "\xC6\xB8", "\xC6\xD7", "\xC6\xE0", "\xC6\xEA",
    "\xC6\xE3", "\xC7\xA1", "\xC7\xAB", "\xC7\xC7", "\xC7\xC3",         "",         "",         ""
};

const char *SJIS_EUCJP_FB40[16*12] = { /* SJIS FB40-FBFF */
    "\xC7\xCB", "\xC7\xCF", "\xC7\xD9", "\xF4\xCB", "\xF4\xCC", "\xC7\xE6", "\xC7\xEE", "\xC7\xFC",
    "\xC7\xEB", "\xC7\xF0", "\xC8\xB1", "\xC8\xE5", "\xC8\xF8", "\xC9\xA6", "\xC9\xAB", "\xC9\xAD",
    "\xF4\xCD", "\xC9\xCA", "\xC9\xD3", "\xC9\xE9", "\xC9\xE3", "\xC9\xFC", "\xC9\xF4", "\xC9\xF5",
    "\xF4\xCE", "\xCA\xB3", "\xCA\xBD", "\xCA\xEF", "\xCA\xF1", "\xCB\xAE", "\xF4\xCF", "\xCB\xCA",
    "\xCB\xE6", "\xCB\xEA", "\xCB\xF0", "\xCB\xF4", "\xCB\xEE", "\xCC\xA5", "\xCB\xF9", "\xCC\xAB",
    "\xCC\xAE", "\xCC\xAD", "\xCC\xB2", "\xCC\xC2", "\xCC\xD0", "\xCC\xD9", "\xF4\xD0", "\xCD\xBB",
    "\xF4\xD1", "\xCE\xBB", "\xF4\xD2", "\xCE\xBA", "\xCE\xC3", "\xF4\xD3", "\xCE\xF2", "\xB3\xDD",
    "\xCF\xD5", "\xCF\xE2", "\xCF\xE9", "\xCF\xED", "\xF4\xD4", "\xF4\xD5", "\xF4\xD6",         "",
    "\xF4\xD7", "\xD0\xE5", "\xF4\xD8", "\xD0\xE9", "\xD1\xE8", "\xF4\xD9", "\xF4\xDA", "\xD1\xEC",
    "\xD2\xBB", "\xF4\xDB", "\xD3\xE1", "\xD3\xE8", "\xD4\xA7", "\xF4\xDC", "\xF4\xDD", "\xD4\xD4",
    "\xD4\xF2", "\xD5\xAE", "\xF4\xDE", "\xD7\xDE", "\xF4\xDF", "\xD8\xA2", "\xD8\xB7", "\xD8\xC1",
    "\xD8\xD1", "\xD8\xF4", "\xD9\xC6", "\xD9\xC8", "\xD9\xD1", "\xF4\xE0", "\xF4\xE1", "\xF4\xE2",
    "\xF4\xE3", "\xF4\xE4", "\xDC\xD3", "\xDD\xC8", "\xDD\xD4", "\xDD\xEA", "\xDD\xFA", "\xDE\xA4",
    "\xDE\xB0", "\xF4\xE5", "\xDE\xB5", "\xDE\xCB", "\xF4\xE6", "\xDF\xB9", "\xF4\xE7", "\xDF\xC3",
    "\xF4\xE8", "\xF4\xE9", "\xE0\xD9", "\xF4\xEA", "\xF4\xEB", "\xE1\xE2", "\xF4\xEC", "\xF4\xED",
    "\xF4\xEE", "\xE2\xC7", "\xE3\xA8", "\xE3\xA6", "\xE3\xA9", "\xE3\xAF", "\xE3\xB0", "\xE3\xAA",
    "\xE3\xAB", "\xE3\xBC", "\xE3\xC1", "\xE3\xBF", "\xE3\xD5", "\xE3\xD8", "\xE3\xD6", "\xE3\xDF",
    "\xE3\xE3", "\xE3\xE1", "\xE3\xD4", "\xE3\xE9", "\xE4\xA6", "\xE3\xF1", "\xE3\xF2", "\xE4\xCB",
    "\xE4\xC1", "\xE4\xC3", "\xE4\xBE", "\xF4\xEF", "\xE4\xC0", "\xE4\xC7", "\xE4\xBF", "\xE4\xE0",
    "\xE4\xDE", "\xE4\xD1", "\xF4\xF0", "\xE4\xDC", "\xE4\xD2", "\xE4\xDB", "\xE4\xD4", "\xE4\xFA",
    "\xE4\xEF", "\xE5\xB3", "\xE5\xBF", "\xE5\xC9", "\xE5\xD0", "\xE5\xE2", "\xE5\xEA", "\xE5\xEB",
    "\xF4\xF1", "\xF4\xF2", "\xF4\xF3", "\xE6\xE8", "\xE6\xEF", "\xE7\xAC", "\xF4\xF4", "\xE7\xAE",
    "\xF4\xF5", "\xE7\xB1", "\xF4\xF6", "\xE7\xB2", "\xE8\xB1", "\xE8\xB6", "\xF4\xF7", "\xF4\xF8",
    "\xE8\xDD", "\xF4\xF9", "\xF4\xFA", "\xE9\xD1", "\xF4\xFB",         "",         "",         ""
};

const char *SJIS_EUCJP_FC40[16] = { /* SJIS FC40-FC4F */
    "\xE9\xED", "\xEA\xCD", "\xF4\xFC", "\xEA\xDB", "\xEA\xE6", "\xEA\xEA", "\xEB\xA5", "\xEB\xFB",
    "\xEB\xFA", "\xF4\xFD", "\xEC\xD6", "\xF4\xFE",         "",         "",         "",         ""
};

#if defined(__GNUC__)
#ifdef str2cmp
#undef str2cmp
#endif
#define str2cmp(x, y) ({ \
  int _str2cmp_n; \
  _str2cmp_n = (unsigned char)(*x) - (unsigned char)(*y); \
  if (_str2cmp_n == 0) { \
    _str2cmp_n = (unsigned char)(x[1]) - (unsigned char)(y[1]); \
  } \
  (_str2cmp_n); \
})
#else
#define str2cmp(x, y) strncmp((x), (y), 2)
#endif

#if defined(__GNUC__)
#ifdef str3cmp
#undef str3cmp
#endif
#define str3cmp(x, y) ({ \
  int _str3cmp_n; \
  _str3cmp_n = (unsigned char)(*x) - (unsigned char)(*y); \
  if (_str3cmp_n == 0) { \
    _str3cmp_n = (unsigned char)(x[1]) - (unsigned char)(y[1]); \
    if (_str3cmp_n == 0) { \
      _str3cmp_n = (unsigned char)(x[2]) - (unsigned char)(y[2]); \
    } \
  } \
  (_str3cmp_n); \
})
#else
#define str3cmp(x, y) strncmp((x), (y), 3)
#endif

#if defined(__GNUC__)
#ifdef str4cmp
#undef str4cmp
#endif
#define str4cmp(x, y) ({ \
  int _str4cmp_n; \
  _str4cmp_n = (unsigned char)(*x) - (unsigned char)(*y); \
  if (_str4cmp_n == 0) { \
    _str4cmp_n = (unsigned char)(x[1]) - (unsigned char)(y[1]); \
    if (_str4cmp_n == 0) { \
      _str4cmp_n = (unsigned char)(x[2]) - (unsigned char)(y[2]); \
      if (_str4cmp_n == 0) { \
        _str4cmp_n = (unsigned char)(x[3]) - (unsigned char)(y[3]); \
      } \
    } \
  } \
  (_str4cmp_n); \
})
#else
#define str4cmp(x, y) strncmp((x), (y), 4)
#endif

}

namespace apolloron {

const char *STR_ISO8859_1  = "ISO-8859-1";
const char *STR_ISO8859_2  = "ISO-8859-2";
const char *STR_ISO8859_3  = "ISO-8859-3";
const char *STR_ISO8859_4  = "ISO-8859-4";
const char *STR_ISO8859_5  = "ISO-8859-5";
const char *STR_ISO8859_6  = "ISO-8859-6";
const char *STR_ISO8859_7  = "ISO-8859-7";
const char *STR_ISO8859_8  = "ISO-8859-8";
const char *STR_ISO8859_9  = "ISO-8859-9";
const char *STR_ISO8859_10 = "ISO-8859-10";
const char *STR_ISO8859_11 = "ISO-8859-11";
const char *STR_ISO8859_13 = "ISO-8859-13";
const char *STR_ISO8859_14 = "ISO-8859-14";
const char *STR_ISO8859_15 = "ISO-8859-15";
const char *STR_ISO8859_16 = "ISO-8859-16";
const char *STR_KOI8_R     = "KOI8-R";
const char *STR_KOI8_U     = "KOI8-U";
const char *STR_CP1251     = "CP1251";
const char *STR_CP1252     = "CP1252";
const char *STR_CP1258     = "CP1258";
const char *STR_UTF8       = "UTF-8";
const char *STR_UTF7       = "UTF-7";
const char *STR_UTF7_IMAP  = "UTF-7-IMAP";
const char *STR_UTF16LE    = "UTF-16LE";
const char *STR_UTF16BE    = "UTF-16BE";
const char *STR_UTF32LE    = "UTF-32LE";
const char *STR_UTF32BE    = "UTF-32BE";
const char *STR_SJIS       = "CP932";
const char *STR_JIS        = "ISO-2022-JP";
const char *STR_EUCJP      = "EUC-JP";
const char *STR_EUCJPMS    = "EUCJP-MS";
const char *STR_ASCII      = "US-ASCII";
const char *STR_AUTOJP     = "AUTODETECT_JP";
const char *STR_EUCKR      = "CP949";
const char *STR_BIG5       = "CP950";
const char *STR_GBK        = "GB18030";
const char *STR_AUTO       = "AUTODETECT";


/*! Multi-character set converter
    @param str          Source text.
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Converted text
 */
char* charset_convert(const char* str, const char *src_charset, const char *dest_charset) {
    char *buf;
    int converted;
    const char *src_ch, *dest_ch;
    long length;
#if __ICONV == 0
    char *buf1, *utf8_str;
    buf1 = NULL;
    utf8_str = NULL;
#else
    iconv_t cd;
#endif

    if (str == (const char *)NULL) {
        return NULL;
    }

    length = strlen(str);

    buf = NULL;
    converted = 0;

    if (src_charset == NULL || src_charset[0] == '\0') {
        src_ch = STR_AUTO;
    } else if (!strncasecmp(src_charset, "ASCII", 5) || !strncasecmp(src_charset, "US-ASCII", 8)) {
        src_ch = STR_ASCII;
    } else if (!strncasecmp(src_charset, "EUC-JP-MS", 9) || !strncasecmp(src_charset, "EUCJP-MS", 8)) {
        src_ch = STR_EUCJPMS;
    } else if (!strncasecmp(src_charset, "EUC-JP", 6) || !strncasecmp(src_charset, "X-EUC-JP", 8) ||
               !strncasecmp(src_charset, "EUCJP", 5) || !strcasecmp(src_charset, "CP51932") ||
               !strncasecmp(src_charset, "EUC-JIS", 7)) {
        src_ch = STR_EUCJP;
    } else if (!strncasecmp(src_charset, "SHIFT_JIS", 9) || !strncasecmp(src_charset, "SHIFT-JIS", 9) ||
               !strcasecmp(src_charset, "CP932") || !strcasecmp(src_charset, "WINDOWS-932") ||
               !strcasecmp(src_charset, "WINDOWS-31J") || !strcasecmp(src_charset, "MS932") ||
               !strncasecmp(src_charset, "X-SJIS", 6)) {
        src_ch = STR_SJIS;
    } else if (!strncasecmp(src_charset, "ISO-2022-JP", 11) || !strncasecmp(src_charset, "X-WINDOWS-ISO2022JP", 19)) {
        src_ch = STR_JIS;
    } else if (!strncasecmp(src_charset, "UTF-8", 5) || !strncasecmp(src_charset, "UTF8", 4)) {
        src_ch = STR_UTF8;
    } else if (!strncasecmp(src_charset, "UTF-7-IMAP", 10) || !strncasecmp(src_charset, "UTF7-IMAP", 9)) {
        src_ch = STR_UTF7_IMAP;
    } else if (!strncasecmp(src_charset, "UTF-7", 5) || !strncasecmp(src_charset, "UTF7", 4)) {
        src_ch = STR_UTF7;
    } else if (!strncasecmp(src_charset, "AUTODETECT_JP", 13)) {
        src_ch = jis_auto_detect(str);
        if (src_ch == (const char *)NULL) {
            src_ch = src_charset;
        }
    } else if (!strncasecmp(src_charset, "AUTODETECT", 10)) {
        src_ch = auto_detect(str);
        if (src_ch == (const char *)NULL) {
            src_ch = src_charset;
        }
    } else if (!strncasecmp(src_charset, "EUC-KR", 6) || !strncasecmp(src_charset, "EUCKR", 5) ||
               !strcasecmp(src_charset, "CP949") || !strcasecmp(src_charset, "WINDOWS-949") ||
               !strcasecmp(src_charset, "MS949") || !strncasecmp(src_charset, "X-EUC-KR", 8) ||
               !strcasecmp(src_charset, "KS_C_5601-1987")) {
        src_ch = STR_EUCKR;
    } else if (!strncasecmp(src_charset, "GB", 2) ||
               !strncasecmp(src_charset, "EUC-CN", 6) || !strncasecmp(src_charset, "EUCCN", 5) ||
               !strcasecmp(src_charset, "CP936") || !strcasecmp(src_charset, "WINDOWS-936") ||
               !strcasecmp(src_charset, "MS936") || !strncasecmp(src_charset, "X-EUC-CN", 8)) {
        src_ch = STR_GBK;
    } else if (!strcasecmp(src_charset, "BIG5") || !strncasecmp(src_charset, "EUC-TW", 6) ||
               !strncasecmp(src_charset, "EUCTW", 5) || !strcasecmp(src_charset, "CP950") ||
               !strcasecmp(src_charset, "WINDOWS-950") || !strcasecmp(src_charset, "MS950")) {
        src_ch = STR_BIG5;
    } else if (!strcasecmp(src_charset, "CP1251") || !strcasecmp(src_charset, "WINDOWS-1251") ||
               !strcasecmp(src_charset, "MS1251")) {
        src_ch = STR_CP1251;
    } else if (!strcasecmp(src_charset, "CP1252") || !strcasecmp(src_charset, "CP-1252") ||
               !strcasecmp(src_charset, "WINDOWS-1252") || !strcasecmp(src_charset, "MS1252") ||
               !strcasecmp(src_charset, "LATIN-1") || !strcasecmp(src_charset, "LATIN1")) {
        src_ch = STR_CP1252;
    } else if (!strcasecmp(src_charset, "CP1258") || !strcasecmp(src_charset, "CP-1258") ||
               !strcasecmp(src_charset, "WINDOWS-1258") || !strcasecmp(src_charset, "MS1258")) {
        src_ch = STR_CP1258;
    } else if (!strcasecmp(src_charset, "ISO-8859-1")) {
        src_ch = STR_ISO8859_1;
    } else if (!strcasecmp(src_charset, "ISO-8859-2") || !strcasecmp(src_charset, "LATIN-2") ||
               !strcasecmp(src_charset, "LATIN2") || !strcasecmp(src_charset, "CP28592") ||
               !strcasecmp(src_charset, "CP-28592") || !strcasecmp(src_charset, "WINDOWS-28592")) {
        src_ch = STR_ISO8859_2;
    } else if (!strcasecmp(src_charset, "ISO-8859-3") || !strcasecmp(src_charset, "LATIN-3") ||
               !strcasecmp(src_charset, "LATIN3") || !strcasecmp(src_charset, "CP28593") ||
               !strcasecmp(src_charset, "CP-28593") || !strcasecmp(src_charset, "WINDOWS-28593")) {
        src_ch = STR_ISO8859_3;
    } else if (!strcasecmp(src_charset, "ISO-8859-4") || !strcasecmp(src_charset, "LATIN-4") ||
               !strcasecmp(src_charset, "LATIN4") || !strcasecmp(src_charset, "CP28594") ||
               !strcasecmp(src_charset, "CP-28594") || !strcasecmp(src_charset, "WINDOWS-28594")) {
        src_ch = STR_ISO8859_4;
    } else if (!strcasecmp(src_charset, "ISO-8859-5") || !strcasecmp(src_charset, "CP28595") ||
               !strcasecmp(src_charset, "CP-28595") || !strcasecmp(src_charset, "WINDOWS-28595")) {
        src_ch = STR_ISO8859_5;
    } else if (!strcasecmp(src_charset, "ISO-8859-6") || !strcasecmp(src_charset, "iso-ir-127") ||
               !strcasecmp(src_charset, "ECMA-114") || !strcasecmp(src_charset, "ASMO-708") ||
               !strcasecmp(src_charset, "Arabic") || !strcasecmp(src_charset, "csISOLatinArabic") ||
               !strcasecmp(src_charset, "CP28596") || !strcasecmp(src_charset, "CP-28596") ||
               !strcasecmp(src_charset, "WINDOWS-28596")) {
        src_ch = STR_ISO8859_6;
    } else if (!strcasecmp(src_charset, "ISO-8859-7") || !strcasecmp(src_charset, "iso-ir-126") ||
               !strcasecmp(src_charset, "ELOT_928") || !strcasecmp(src_charset, "ECMA-118") ||
               !strncasecmp(src_charset, "greek", 5) || !strcasecmp(src_charset, "csISOLatinGreek") ||
               !strcasecmp(src_charset, "CP28597") || !strcasecmp(src_charset, "CP-28597") ||
               !strcasecmp(src_charset, "WINDOWS-28597")) {
        src_ch = STR_ISO8859_7;
    } else if (!strncasecmp(src_charset, "ISO-8859-8", 10) || !strcasecmp(src_charset, "CP28598") ||
               !strcasecmp(src_charset, "CP-28598") || !strcasecmp(src_charset, "WINDOWS-28598")) {
        src_ch = STR_ISO8859_8;
    } else if (!strcasecmp(src_charset, "ISO-8859-9") || !strcasecmp(src_charset, "LATIN-5") ||
               !strcasecmp(src_charset, "LATIN5") || !strcasecmp(src_charset, "CP28599") ||
               !strcasecmp(src_charset, "CP-28599") || !strcasecmp(src_charset, "WINDOWS-28599")) {
        src_ch = STR_ISO8859_9;
    } else if (!strcasecmp(src_charset, "ISO-8859-10") || !strcasecmp(src_charset, "LATIN-6") ||
               !strcasecmp(src_charset, "LATIN6")) {
        src_ch = STR_ISO8859_10;
    } else if (!strcasecmp(src_charset, "ISO-8859-11") || !strcasecmp(src_charset, "CP874") ||
               !strcasecmp(src_charset, "CP-874") || !strcasecmp(src_charset, "WINDOWS-874") ||
               !strcasecmp(src_charset, "TIS-620")) {
        src_ch = STR_ISO8859_11;
    } else if (!strcasecmp(src_charset, "ISO-8859-13") || !strcasecmp(src_charset, "LATIN-7") ||
               !strcasecmp(src_charset, "LATIN7")) {
        src_ch = STR_ISO8859_13;
    } else if (!strcasecmp(src_charset, "ISO-8859-14") || !strcasecmp(src_charset, "LATIN-8") ||
               !strcasecmp(src_charset, "LATIN8")) {
        src_ch = STR_ISO8859_14;
    } else if (!strcasecmp(src_charset, "ISO-8859-15") || !strcasecmp(src_charset, "LATIN-9") ||
               !strcasecmp(src_charset, "LATIN9") || !strcasecmp(src_charset, "CP28605") ||
               !strcasecmp(src_charset, "CP-28605") || !strcasecmp(src_charset, "WINDOWS-28605")) {
        src_ch = STR_ISO8859_15;
    } else if (!strcasecmp(src_charset, "ISO-8859-16") || !strcasecmp(src_charset, "LATIN-10") ||
               !strcasecmp(src_charset, "LATIN10")) {
        src_ch = STR_ISO8859_16;
    } else if (!strncasecmp(src_charset, "KOI8-U", 6) ||
               !strcasecmp(src_charset, "CP21866") || !strcasecmp(src_charset, "WINDOWS-21866") ||
               !strcasecmp(src_charset, "MS21866")) {
        src_ch = STR_KOI8_U;
    } else if (!strncasecmp(src_charset, "KOI8", 4) ||
               !strcasecmp(src_charset, "CP20866") || !strcasecmp(src_charset, "WINDOWS-20866") ||
               !strcasecmp(src_charset, "MS20866")) {
        src_ch = STR_KOI8_R;
    } else {
        src_ch = src_charset;
    }

    if (dest_charset == NULL) {
        dest_ch = "";
    } else if (!strncasecmp(dest_charset, "ASCII", 5) || !strncasecmp(dest_charset, "US-ASCII", 8)) {
        dest_ch = STR_ASCII;
    } else if (!strncasecmp(dest_charset, "EUC-JP-MS", 9) || !strncasecmp(dest_charset, "EUCJP-MS", 8)) {
        dest_ch = STR_EUCJPMS;
    } else if (!strncasecmp(dest_charset, "EUC-JP", 6) || !strncasecmp(dest_charset, "X-EUC-JP", 8) ||
               !strncasecmp(dest_charset, "EUCJP", 5) || !strcasecmp(dest_charset, "CP51932") ||
               !strncasecmp(dest_charset, "EUC-JIS", 7)) {
        dest_ch = STR_EUCJP;
    } else if (!strncasecmp(dest_charset, "SHIFT_JIS", 9) || !strncasecmp(dest_charset, "SHIFT-JIS", 9) ||
               !strcasecmp(dest_charset, "CP932") || !strcasecmp(dest_charset, "WINDOWS-932") ||
               !strcasecmp(dest_charset, "WINDOWS-31J") || !strcasecmp(dest_charset, "MS932") ||
               !strncasecmp(dest_charset, "X-SJIS", 6)) {
        dest_ch = STR_SJIS;
    } else if (!strncasecmp(dest_charset, "ISO-2022-JP", 11) || !strncasecmp(dest_charset, "X-WINDOWS-ISO2022JP", 19)) {
        dest_ch = STR_JIS;
    } else if (!strncasecmp(dest_charset, "UTF-8", 5) || !strncasecmp(dest_charset, "UTF8", 4)) {
        dest_ch = STR_UTF8;
    } else if (!strncasecmp(dest_charset, "UTF-7-IMAP", 10) || !strncasecmp(dest_charset, "UTF7-IMAP", 9)) {
        dest_ch = STR_UTF7_IMAP;
    } else if (!strncasecmp(dest_charset, "UTF-7", 5) || !strncasecmp(dest_charset, "UTF7", 4)) {
        dest_ch = STR_UTF7;
    } else if (!strncasecmp(dest_charset, "EUC-KR", 6) || !strncasecmp(dest_charset, "EUCKR", 5) ||
               !strcasecmp(dest_charset, "CP949") || !strcasecmp(dest_charset, "WINDOWS-949") ||
               !strcasecmp(dest_charset, "MS949") || !strncasecmp(dest_charset, "X-EUC-KR", 8) ||
               !strcasecmp(dest_charset, "KS_C_5601-1987")) {
        dest_ch = STR_EUCKR;
    } else if (!strncasecmp(dest_charset, "GB", 2) ||
               !strncasecmp(dest_charset, "EUC-CN", 6) || !strncasecmp(src_charset, "EUCCN", 5) ||
               !strcasecmp(dest_charset, "CP936") || !strcasecmp(dest_charset, "WINDOWS-936") ||
               !strcasecmp(dest_charset, "MS936") || !strncasecmp(dest_charset, "X-EUC-CN", 8)) {
        dest_ch = STR_GBK;
    } else if (!strcasecmp(dest_charset, "BIG5") || !strncasecmp(dest_charset, "EUC-TW", 6) ||
               !strncasecmp(dest_charset, "EUCTW", 5) || !strcasecmp(dest_charset, "CP950") ||
               !strcasecmp(dest_charset, "WINDOWS-950") || !strcasecmp(dest_charset, "MS950")) {
        dest_ch = STR_BIG5;
    } else if (!strcasecmp(dest_charset, "CP1251") || !strcasecmp(dest_charset, "WINDOWS-1251") ||
               !strcasecmp(dest_charset, "MS1251")) {
        dest_ch = STR_CP1251;
    } else if (!strcasecmp(dest_charset, "CP1252") || !strcasecmp(dest_charset, "CP-1252") ||
               !strcasecmp(dest_charset, "WINDOWS-1252") || !strcasecmp(dest_charset, "MS1252") ||
               !strcasecmp(dest_charset, "LATIN-1") || !strcasecmp(dest_charset, "LATIN1")) {
        dest_ch = STR_CP1252;
    } else if (!strcasecmp(dest_charset, "CP1258") || !strcasecmp(dest_charset, "CP-1258") ||
               !strcasecmp(dest_charset, "WINDOWS-1258") || !strcasecmp(dest_charset, "MS1258")) {
        dest_ch = STR_CP1258;
    } else if (!strcasecmp(dest_charset, "ISO-8859-1")) {
        dest_ch = STR_ISO8859_1;
    } else if (!strcasecmp(dest_charset, "ISO-8859-2") || !strcasecmp(dest_charset, "LATIN-2") ||
               !strcasecmp(dest_charset, "LATIN2") || !strcasecmp(dest_charset, "CP28592") ||
               !strcasecmp(dest_charset, "CP-28592") || !strcasecmp(dest_charset, "WINDOWS-28592")) {
        dest_ch = STR_ISO8859_2;
    } else if (!strcasecmp(dest_charset, "ISO-8859-3") || !strcasecmp(dest_charset, "LATIN-3") ||
               !strcasecmp(dest_charset, "LATIN3") || !strcasecmp(dest_charset, "CP28593") ||
               !strcasecmp(dest_charset, "CP-28593") || !strcasecmp(dest_charset, "WINDOWS-28593")) {
        dest_ch = STR_ISO8859_3;
    } else if (!strcasecmp(dest_charset, "ISO-8859-4") || !strcasecmp(dest_charset, "LATIN-4") ||
               !strcasecmp(dest_charset, "LATIN4") || !strcasecmp(dest_charset, "CP28594") ||
               !strcasecmp(dest_charset, "CP-28594") || !strcasecmp(dest_charset, "WINDOWS-28594")) {
        dest_ch = STR_ISO8859_4;
    } else if (!strcasecmp(dest_charset, "ISO-8859-5") || !strcasecmp(dest_charset, "CP28595") ||
               !strcasecmp(dest_charset, "CP-28595") || !strcasecmp(dest_charset, "WINDOWS-28595")) {
        dest_ch = STR_ISO8859_5;
    } else if (!strcasecmp(dest_charset, "ISO-8859-6") || !strcasecmp(dest_charset, "iso-ir-127") ||
               !strcasecmp(dest_charset, "ECMA-114") || !strcasecmp(dest_charset, "ASMO-708") ||
               !strcasecmp(dest_charset, "Arabic") || !strcasecmp(dest_charset, "csISOLatinArabic") ||
               !strcasecmp(dest_charset, "CP28596") || !strcasecmp(dest_charset, "CP-28596") ||
               !strcasecmp(dest_charset, "WINDOWS-28596")) {
        dest_ch = STR_ISO8859_6;
    } else if (!strcasecmp(dest_charset, "ISO-8859-7") || !strcasecmp(dest_charset, "iso-ir-126") ||
               !strcasecmp(dest_charset, "ELOT_928") || !strcasecmp(dest_charset, "ECMA-118") ||
               !strncasecmp(dest_charset, "greek", 5) || !strcasecmp(dest_charset, "csISOLatinGreek") ||
               !strcasecmp(dest_charset, "CP28597") || !strcasecmp(dest_charset, "CP-28597") ||
               !strcasecmp(dest_charset, "WINDOWS-28597")) {
        dest_ch = STR_ISO8859_7;
    } else if (!strncasecmp(dest_charset, "ISO-8859-8", 10) || !strcasecmp(dest_charset, "CP28598") ||
               !strcasecmp(dest_charset, "CP-28598") || !strcasecmp(dest_charset, "WINDOWS-28598")) {
        dest_ch = STR_ISO8859_8;
    } else if (!strcasecmp(dest_charset, "ISO-8859-9") || !strcasecmp(dest_charset, "LATIN-5") ||
               !strcasecmp(dest_charset, "LATIN5") || !strcasecmp(dest_charset, "CP28599") ||
               !strcasecmp(dest_charset, "CP-28599") || !strcasecmp(dest_charset, "WINDOWS-28599")) {
        dest_ch = STR_ISO8859_9;
    } else if (!strcasecmp(dest_charset, "ISO-8859-10") || !strcasecmp(dest_charset, "LATIN-6") ||
               !strcasecmp(dest_charset, "LATIN6")) {
        dest_ch = STR_ISO8859_10;
    } else if (!strcasecmp(dest_charset, "ISO-8859-11") || !strcasecmp(dest_charset, "CP874") ||
               !strcasecmp(dest_charset, "CP-874") || !strcasecmp(dest_charset, "WINDOWS-874") ||
               !strcasecmp(dest_charset, "TIS-620")) {
        dest_ch = STR_ISO8859_11;
    } else if (!strcasecmp(dest_charset, "ISO-8859-13") || !strcasecmp(dest_charset, "LATIN-7") ||
               !strcasecmp(dest_charset, "LATIN7")) {
        dest_ch = STR_ISO8859_13;
    } else if (!strcasecmp(dest_charset, "ISO-8859-14") || !strcasecmp(dest_charset, "LATIN-8") ||
               !strcasecmp(dest_charset, "LATIN8")) {
        dest_ch = STR_ISO8859_14;
    } else if (!strcasecmp(dest_charset, "ISO-8859-15") || !strcasecmp(dest_charset, "LATIN-9") ||
               !strcasecmp(dest_charset, "LATIN9")) {
        dest_ch = STR_ISO8859_15;
    } else if (!strcasecmp(dest_charset, "ISO-8859-16") || !strcasecmp(dest_charset, "LATIN-10") ||
               !strcasecmp(dest_charset, "LATIN10")) {
        dest_ch = STR_ISO8859_16;
    } else if (!strncasecmp(dest_charset, "KOI8-U", 6) ||
               !strcasecmp(dest_charset, "CP21866") || !strcasecmp(dest_charset, "WINDOWS-21866") ||
               !strcasecmp(dest_charset, "MS21866")) {
        dest_ch = STR_KOI8_U;
    } else if (!strncasecmp(dest_charset, "KOI8", 4) ||
               !strcasecmp(dest_charset, "CP20866") || !strcasecmp(dest_charset, "WINDOWS-20866") ||
               !strcasecmp(dest_charset, "MS20866")) {
        dest_ch = STR_KOI8_R;
    } else {
        dest_ch = "";
    }

#if __ICONV == 1
    // get characterset converter from src_ch to dest_ch
    cd = iconv_open(dest_ch, src_ch);

    if (cd != (iconv_t)-1) {
#if __ICONV_CONST == 1
        const char *src;
        const char *ibuf, *ip;
#else
        char *src;
        char *ibuf, *ip;
#endif
        char *obuf, *op;
        size_t ileft, oleft;
        size_t olen, osize;
        int r;

#if __ICONV_CONST == 1
        src = (const char *)str;
#else
        src = (char *)str;
#endif

        osize = 1024 + 1;
        obuf = new char [osize];
        olen = 0;
        obuf[olen] = '\0';

        ibuf = src;
        ileft = (size_t)length;
        ip = ibuf;
        do {
            char *obuf_orig;

            oleft = 1024;
            osize = olen + oleft + 1;

            obuf_orig = obuf;
            obuf = new char [osize];
            memcpy(obuf, obuf_orig, olen + 1);
            delete [] obuf_orig;

            op = obuf + olen;

            r = iconv(cd, &ip, &ileft, &op, &oleft);

            if (r < 0 && errno == EILSEQ) {
                /* error */
                if (0 < oleft) {
                    *op = '?';
                    op++;
                    oleft--;
                    ip++;
                    ileft--;
                } else {
                    break;
                }
            }
            olen += (1024 - oleft);
        } while (r < 0 && errno == E2BIG);
        obuf[olen] = '\0';

        iconv_close(cd);

        if (!(r < 0 && errno == EILSEQ)) {
            buf = obuf;
            converted = 1;
        } else {
            delete [] obuf;
        }
    }

#else
    if (!strcasecmp(src_ch, STR_AUTOJP)) {
        src_ch = jis_auto_detect(str);
    } else if (!strcasecmp(src_ch, STR_AUTO)) {
        src_ch = auto_detect(str);
    }

    if (dest_ch[0] == '\0' || !strcasecmp(src_ch, dest_ch)) {
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
        converted = 1;
    } else if (!strcasecmp(src_ch, STR_EUCJPMS)) {
        if (!strcasecmp(dest_ch, STR_EUCJP)) {
            buf = eucjpms_to_eucjpwin(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_SJIS)) {
            buf = eucjpms_to_sjis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_JIS)) {
            buf = eucjpms_to_jis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = eucjpms_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = eucjpms_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_EUCJP)) {
        if (!strcasecmp(dest_ch, STR_EUCJPMS)) {
            buf = eucjpwin_to_eucjpms(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_SJIS)) {
            buf = eucjpwin_to_sjis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_JIS)) {
            buf = eucjpwin_to_jis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = eucjpwin_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = eucjpwin_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_SJIS)) {
        if (!strcasecmp(dest_ch, STR_EUCJPMS)) {
            buf = sjis_to_eucjpms(str);
            converted = 1;
        } if (!strcasecmp(dest_ch, STR_EUCJP)) {
            buf = sjis_to_eucjpwin(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_JIS)) {
            buf = sjis_to_jis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = sjis_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = sjis_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_JIS)) {
        if (!strcasecmp(dest_ch, STR_EUCJPMS)) {
            buf = jis_to_eucjpms(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_EUCJP)) {
            buf = jis_to_eucjpwin(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_SJIS)) {
            buf = jis_to_sjis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = jis_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = eucjpwin_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_UTF8)) {
        utf8_str = new char [length + 1];
        memcpy(utf8_str, str, length);
        utf8_str[length] = '\0';
        utf8_clean(utf8_str);
        if (!strcasecmp(dest_ch, STR_EUCJPMS)) {
            buf = utf8_to_eucjpms(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_EUCJP)) {
            buf = utf8_to_eucjpwin(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_SJIS)) {
            buf = utf8_to_sjis(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_JIS)) {
            buf = utf8_to_jis(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_EUCKR)) {
            buf = utf8_to_euckr(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF7)) {
            buf = utf8_to_utf7(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF7_IMAP)) {
            buf = utf8_to_modutf7(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_GBK)) {
            buf = utf8_to_gbk(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_BIG5)) {
            buf = utf8_to_big5(utf8_str);
            converted = 1;
        } else if (!strncasecmp(dest_ch, "ISO-8859-", 9)) {
            int num;
            num = atoi(dest_ch + 9);
            buf = utf8_to_iso8859(utf8_str, num);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_KOI8_U)) {
            buf = utf8_to_europe(utf8_str, 'U');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_KOI8_R)) {
            buf = utf8_to_europe(utf8_str, 'R');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_CP1251)) {
            buf = utf8_to_europe(utf8_str, '1');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_CP1252)) {
            buf = utf8_to_europe(utf8_str, '2');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_CP1258)) {
            buf = utf8_to_europe(utf8_str, '8');
            converted = 1;
        }
        delete [] utf8_str;
        utf8_str = NULL;
    } else if (!strcasecmp(src_ch, STR_AUTOJP) || !strcasecmp(src_ch, STR_AUTO)) {
        if (!strcasecmp(dest_ch, STR_EUCJPMS)) {
            buf = autojp_to_eucjpms(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_EUCJP)) {
            buf = autojp_to_eucjpwin(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_SJIS)) {
            buf = autojp_to_sjis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_JIS)) {
            buf = autojp_to_jis(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = autojp_to_utf8(str);
            utf8_clean(buf);
            converted = 1;
        }
    } else if (!strcasecmp(src_ch, STR_EUCKR)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = euckr_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = euckr_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_UTF7)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = utf7_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = utf7_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_UTF7_IMAP)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = modutf7_to_utf8(str);
            converted = 1;
        } else {
            utf8_str = modutf7_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_GBK)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = gbk_to_utf8(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_BIG5)) {
            buf1 = gbk_to_utf8(str);
            buf = utf8_to_big5(buf1);
            if (buf1 != NULL) {
                delete [] buf1;
            }
            converted = 1;
        } else {
            utf8_str = gbk_to_utf8(str);
        }
    } else if (!strcasecmp(src_ch, STR_BIG5)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = big5_to_utf8(str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_GBK)) {
            buf1 = big5_to_utf8(str);
            buf = utf8_to_gbk(buf1);
            if (buf1 != NULL) {
                delete [] buf1;
            }
            converted = 1;
        } else {
            utf8_str = big5_to_utf8(str);
        }
    } else if (!strncasecmp(src_ch, "ISO-8859-", 9)) {
        int num;
        num = atoi(src_ch + 9);
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = iso8859_to_utf8(str, num);
            converted = 1;
        } else {
            utf8_str = iso8859_to_utf8(str, num);
        }
    } else if (!strcasecmp(src_ch, STR_KOI8_U)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = europe_to_utf8(str, 'U');
            converted = 1;
        } else {
            utf8_str = europe_to_utf8(str, 'U');
        }
    } else if (!strcasecmp(src_ch, STR_KOI8_R)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = europe_to_utf8(str, 'R');
            converted = 1;
        } else {
            utf8_str = europe_to_utf8(str, 'R');
        }
    } else if (!strcasecmp(src_ch, STR_CP1251)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = europe_to_utf8(str, '1');
            converted = 1;
        } else {
            utf8_str = europe_to_utf8(str, '1');
        }
    } else if (!strcasecmp(src_ch, STR_CP1252)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = europe_to_utf8(str, '2');
            converted = 1;
        } else {
            utf8_str = europe_to_utf8(str, '2');
        }
    } else if (!strcasecmp(src_ch, STR_CP1258)) {
        if (!strcasecmp(dest_ch, STR_UTF8)) {
            buf = europe_to_utf8(str, '8');
            converted = 1;
        } else {
            utf8_str = europe_to_utf8(str, '8');
        }
    }
#endif

#if __ICONV == 0
    if (converted == 0 && utf8_str != NULL) {
        if (!strcasecmp(dest_ch, STR_EUCJPMS)) {
            buf = utf8_to_eucjpms(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_EUCJP)) {
            buf = utf8_to_eucjpwin(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_SJIS)) {
            buf = utf8_to_sjis(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_JIS)) {
            buf = utf8_to_jis(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_EUCKR)) {
            buf = utf8_to_euckr(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF7)) {
            buf = utf8_to_utf7(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_UTF7_IMAP)) {
            buf = utf8_to_modutf7(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_GBK)) {
            buf = utf8_to_gbk(utf8_str);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_BIG5)) {
            buf = utf8_to_big5(utf8_str);
            converted = 1;
        } else if (!strncasecmp(dest_ch, "ISO-8859-", 9)) {
            int num;
            num = atoi(dest_ch + 9);
            buf = utf8_to_iso8859(utf8_str, num);
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_KOI8_U)) {
            buf = utf8_to_europe(utf8_str, 'U');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_KOI8_R)) {
            buf = utf8_to_europe(utf8_str, 'R');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_CP1251)) {
            buf = utf8_to_europe(utf8_str, '1');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_CP1252)) {
            buf = utf8_to_europe(utf8_str, '2');
            converted = 1;
        } else if (!strcasecmp(dest_ch, STR_CP1258)) {
            buf = utf8_to_europe(utf8_str, '8');
            converted = 1;
        }
    }

    if (utf8_str) {
        delete [] utf8_str;
    }
#endif

    if (converted == 0 || buf == NULL) {
        long length;
        length = strlen(str);
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
        converted = 1;
    }

    if (!strcasecmp(src_ch, STR_UTF8) &&
            (!strcasecmp(dest_ch, STR_UTF8) || dest_ch[0] == '\0')) {
        utf8_clean(buf);
    }

    if (buf && dest_charset && !strcasecmp(dest_ch, STR_UTF8) && strcasestr(dest_charset, "BOM")) {
        long length;
        length = strlen(buf);
        utf8_str = new char [length + 4];
        utf8_str[0] = '\xEF';
        utf8_str[1] = '\xBB';
        utf8_str[2] = '\xBF';
        memcpy(utf8_str + 3, buf, length);
        utf8_str[length + 3] = '\0';
        delete [] buf;
        buf = utf8_str;
    }

    return buf;
}


/*! Auto detect character set
    @param str  text for character set auto detection
    @return Character set
 */
const char* auto_detect(const char *str) {
    const char *charcode;
    long i, length, cp1252Pattern, badEUCKR;
    unsigned char c0, c1, c2, c3;

    if (str == NULL || str[0] == '\0') {
        charcode = STR_ASCII;
        return charcode;
    }

    length = strlen(str);

    if (20 < length) {
        cp1252Pattern = 0;
        for (i = 1; i < length-3; i++) {
            c0 = str[i-1];
            c1 = str[i];
            c2 = str[i+1];
            c3 = str[i+2];
            if (0x20 <= c0 && c0 <= 0x7E && (unsigned char)0x80 <= c1 &&
                    'a' <= c2 && c2 <= 'z' && 'a' <= c3 && c3 <= 'z') {
                cp1252Pattern++;
            } else if ((unsigned char)0x80 <= c0 && (unsigned char)0x80 <= c1) {
                cp1252Pattern = 0;
                break;
            }
        }
        if (0 < cp1252Pattern && cp1252Pattern <= length/60+1) {
            charcode = STR_CP1252;
            return charcode;
        }
    }

    charcode = jis_auto_detect(str);

    if (!strcmp(charcode, STR_EUCJP)) {
        // Judgment of EUC-KR
        badEUCKR = 0;
        for (i = 0; i < length; i++) {
            c0 = str[i];
            c1 = str[i + 1];
            if (c1 == (unsigned char)'\0') {
                if ((unsigned char)0x80 <= c0) {
                    badEUCKR++;
                    break;
                }
            } else if (((unsigned char)0xB0 <= c0 && c0 <= (unsigned char)0xC8) &&
                       ((unsigned char)0xA1 <= c1 && c1 <= (unsigned char)0xFE)) {
                i++;
            } else if ((unsigned char)0x80 <= c0) {
                badEUCKR++;
                break;
            }
        }
        if (badEUCKR == 0) {
            charcode = STR_EUCKR;
        }
    }
    if (!strcmp(charcode, STR_EUCJP) || !strcmp(charcode, STR_SJIS)|| !strcmp(charcode, STR_UTF8)) {
        // Judgment of UTF-8, GB18030, CP950
        int u8len, gb18030_4byte, gb_char, kana, big5_char;
        long badGBK, badUTF8, badBIG5;
        unsigned char c2, c3;

        badUTF8 = 0;
        for (i = 0; i < length; i++) {
            u8len = utf8_len(str[i]);
            c0 = str[i];
            c1 = str[i + 1];
            if (u8len == 1) {
                continue;
            } else if (u8len == 2) {
                if (0xC2 <= c0 && c0 <= 0xDF && 0x80 <= c1 && c1 <= 0xBF) {
                    i++;
                    continue;
                }
                badUTF8++;
                break;
            } else if (u8len == 3) {
                if (0xE0 <= c0 && c0 <= 0xEF && 0x80 <= c1 && c1 <= 0xBF &&
                        0x80 <= (unsigned char)str[i+2] && (unsigned char)str[i+2] <= 0xBF) {
                    i += 2;
                    continue;
                }
                badUTF8++;
                break;
            } else if (u8len == 4) {
                if (0xF0 <= c0 && c0 <= 0xF7 && 0x80 <= c1 && c1 <= 0xBF &&
                        0x80 <= (unsigned char)str[i+2] && (unsigned char)str[i+2] <= 0xBF &&
                        0x80 <= (unsigned char)str[i+3] && (unsigned char)str[i+3] <= 0xBF) {
                    i += 3;
                    continue;
                }
                badUTF8++;
                break;
            } else if (u8len == 5) {
                if (0xF8 <= c0 && c0 <= 0xFB && 0x80 <= c1 && c1 <= 0xBF &&
                        0x80 <= (unsigned char)str[i+2] && (unsigned char)str[i+2] <= 0xBF &&
                        0x80 <= (unsigned char)str[i+3] && (unsigned char)str[i+3] <= 0xBF &&
                        0x80 <= (unsigned char)str[i+4] && (unsigned char)str[i+4] <= 0xBF) {
                    i += 4;
                    continue;
                }
                badUTF8++;
                break;
            } else if (u8len == 6) {
                if (0xFC <= c0 && c0 <= 0xFD && 0x80 <= c1 && c1 <= 0xBF &&
                        0x80 <= (unsigned char)str[i+2] && (unsigned char)str[i+2] <= 0xBF &&
                        0x80 <= (unsigned char)str[i+3] && (unsigned char)str[i+3] <= 0xBF &&
                        0x80 <= (unsigned char)str[i+4] && (unsigned char)str[i+4] <= 0xBF &&
                        0x80 <= (unsigned char)str[i+5] && (unsigned char)str[i+5] <= 0xBF) {
                    i += 5;
                    continue;
                }
                badUTF8++;
                break;
            } else {
                badUTF8++;
                break;
            }
        }

        if (badUTF8 == 0) {
            return charcode;
        }

        gb18030_4byte = 0;
        gb_char = 0;
        kana = 0;
        badGBK = 0;
        for (i = 0; i < length; i++) {
            c0 = str[i];
            c1 = str[i + 1];
            if (c0 < (unsigned char)0x80) {
                continue;
            } else if (c1 == (unsigned char)'\0') {
                if ((unsigned char)0x80 <= c0) {
                    badGBK++;
                    break;
                }
            } else if (((unsigned char)0x81 <= c0 && c0 <= (unsigned char)0xFE) &&
                    ((unsigned char)0x40 <= c1 && c1 <= (unsigned char)0xFE &&
                    c1 != (unsigned char)0x7F)) {
                if (gb_char == 0 && (
                        (c0 == (unsigned char)0xCE && c1 == (unsigned char)0xD2) ||
                        (c0 == (unsigned char)0xBB && c1 == (unsigned char)0xF2) ||
                        (c0 == (unsigned char)0xB5 && c1 == (unsigned char)0xC4) ||
                        (c0 == (unsigned char)0xB2 && c1 == (unsigned char)0xFA) ||
                        (c0 == (unsigned char)0xD6 && c1 == (unsigned char)0xB7) ||
                        (c0 == (unsigned char)0xCE && c1 == (unsigned char)0xF1) ||
                        (c0 == (unsigned char)0xA3 && c1 == (unsigned char)0xAC) ||
                        (c0 == (unsigned char)0xA3 && c1 == (unsigned char)0xBB))) {
                    gb_char = 1;
                }
                if ((unsigned char)0xA4 <= c0 && c0 <= (unsigned char)0xA5 &&
                        (unsigned char)0xA1 <= c1 && c1 <= (unsigned char)0xF6) {
                    kana = 1;
                }
                i++;
            } else if (((unsigned char)0x81 <= c0 && c0 <= (unsigned char)0xFE) &&
                       ((unsigned char)0x30 <= c1 && c1 <= (unsigned char)0x39)) {
                c2 = str[i + 2];
                if (c2 == (unsigned char)'\0') {
                    badGBK++;
                    break;
                }
                c3 = str[i + 3];
                if (c3 == (unsigned char)'\0') {
                    badGBK++;
                    break;
                }
                if (((unsigned char)0x81 <= c2 && c2 <= (unsigned char)0xFE) &&
                        ((unsigned char)0x30 <= c3 && c3 <= (unsigned char)0x39)) {
                    gb18030_4byte = 1;
                    i += 3;
                } else {
                    badGBK++;
                    break;
                }
            } else {
                badGBK++;
                break;
            }
        }
        if (badGBK == 0 && (gb18030_4byte == 1 || gb_char == 1) && kana == 0) {
            charcode = STR_GBK;
        } else if (badGBK == 0 && kana == 1) {
            charcode = STR_EUCJP;
        }

        big5_char = 0;
        badBIG5 = 0;
        for (i = 0; i < length; i++) {
            c0 = str[i];
            c1 = str[i + 1];
            if (c0 < 0x7F) {
                continue;
            } else if (( /* (0x88 <= c0 && c0 <= 0xA0) ||  is Big5-HKSCS */
                    (0xA1 <= c0 && c0 <= 0xC6) || (0xC9 <= c0 && c0 <= 0xF9)) ||
                    ((0x40 <= c1 && c1 <= 0x7E) || (0xA1 <= c1 && c1 <= 0xFE))) {
                if (0x40 <= c1 && c1 <= 0x7E) big5_char = 1;
                i++;
                continue;
            } else {
                badBIG5++;
                break;
            }
        }

        if (strcmp(charcode, STR_SJIS) != 0 && badBIG5 == 0 && big5_char == 1) {
            charcode = STR_BIG5;
        }
    }

    return charcode;
}


/*! Auto detect japanese character set
    @param str  text for character set auto detection
    @return Character set ("CP932", "ISO-2022-JP", "EUC-JP", or "UTF-8").
 */
const char* jis_auto_detect(const char *str) {
    long length;
    const char *charcode;

    if (str != NULL && str[0] != '\0') {
        length = strlen(str);
    } else {
        length = 0;
    }

    if (0 < length) {
        long i, badEUCJP, badSJIS, badJIS, badUTF8, max, isJIS, isEUCJPMS;

        badEUCJP = 0;
        badSJIS = 0;
        badJIS = 0;
        badUTF8 = 0;

        max = length; // Processing will be omitted if comparison is fully possible

        isJIS = 0;
        isEUCJPMS = 0;

        // Judgment of EUC-JP
        for (i = 0; i < length; i++) {
            if (((unsigned char)str[i] == 0x8E) && (++i < length)) {
                if (ishankana((unsigned char)str[i])) {
                    badEUCJP++;
                } else {
                    badEUCJP += 10;
                    i--;
                }
            } else if (((unsigned char)str[i] == 0x8F) && (++i < length - 1)) {
                if (iseuc((unsigned char)str[i]) && iseuc((unsigned char)str[i + 1])) {
                    // 3byte EUC-JP
                    isEUCJPMS++;
                    i++;
                } else {
                    badEUCJP += 10;
                    i--;
                }
            } else if (iseuc((unsigned char)str[i]) && (++i < length)) {
                if (!iseuc((unsigned char)str[i])) {
                    badEUCJP += 10;
                    i--;
                } else if (0x50 <= str[i-1]) {
                    badEUCJP++;
                }
            } else if ((str[i] == 0x1B) && (++i < length)) {
                if ((str[i] == '$') || (str[i] == '(')) {
                    badEUCJP += 20;
                } else {
                    i--;
                }
            } else if ((str[i] == '$') || (str[i] == '%')) {
                badEUCJP++;
            } else if (0x80 <= (unsigned char)str[i]) {
                badEUCJP++;
            }
            if (1000 < badEUCJP) {
                max = i;
                break;
            }
        }

        // Judgment of CP932 and ISO-2022-JP
        for (i = 0; i < length && i < max; i++) {
            unsigned char c, d;
            c = (unsigned char)str[i];
            if (c == 0x1B && ((unsigned char)str[i+1] == '$' || (unsigned char)str[i+1] == '(')) {
                badSJIS += 160;
                badUTF8 += 200;
                badEUCJP += 200;
                isJIS++;
                i++;
            } else if (issjis1(c) && (i + 1 < length)) {
                d = (unsigned char)str[i+1];
                if (issjis2(d)) {
                    badJIS += 50;
                    i++;
                } else if (0 < i && 0x989F <= (((unsigned int)str[i-1] << 8) | (unsigned int)str[i])) {
                    badSJIS++;
                    badJIS++;
                }
            } else if (0x80 <= (unsigned char)str[i]) {
                badJIS += 10;
                if (ishankana((unsigned char)str[i])) {
                    badSJIS++;
                } else {
                    badSJIS += 10;
                }
            }
            if (1000 < badSJIS) {
                max = i;
                break;
            }
        }

        // Judgment of UTF-8
        if (3 <= length && (unsigned char)str[0] == 0xEF &&
                (unsigned char)str[1] == 0xBB && (unsigned char)str[2] == 0xBF) {
            badEUCJP += 10;
            badSJIS += 10;
            badJIS += 10;
        }
        for (i = 0; i < length && i < max; i++) {
            if ((str[i] == 0x1B) && (i+1 < length)) {
                if ((str[i+1] == '$') || (str[i+1] == '(')) {
                    badUTF8 += 20;
                }
            } else if ((str[i] == '$') || (str[i] == '%')) {
                badUTF8++;
            }
            if (isutf8_1((unsigned char)str[i]) && (++i < length)) {
                if (isutf8_2((unsigned char)str[i]) && (++i < length)) {
                    if (3 <= utf8_len((unsigned char)str[i-2])) {
                        if (!isutf8_3((unsigned char)str[i])) {
                            badUTF8 += 10;
                            i -= 2;
                        }
                    } else {
                        i--;
                    }
                } else {
                    badUTF8 += 5;
                    i--;
                }
            } else if (iseuc((unsigned char)str[i])) {
                badUTF8 += 10;
            }
            if (1000 < badUTF8) {
                max = i;
                break;
            }
        }

        if (badSJIS < badEUCJP && badSJIS <= badUTF8) {
            // ISO-2022-JP or CP932
            if (badSJIS < badJIS) {
                charcode = STR_SJIS;
            } else if (0 < isJIS) {
                charcode = STR_JIS;
            } else {
                charcode = STR_ASCII;
            }
        } else if (2 <= length && badUTF8 < badEUCJP && badUTF8 < badSJIS) {
            // UTF-8
            charcode = STR_UTF8;
        } else if (!(badEUCJP == 0 && badSJIS == 0 && badUTF8 == 0)) {
            // EUC-JP
            if (0 < isEUCJPMS) {
                charcode = STR_EUCJPMS;
            } else {
                charcode = STR_EUCJP;
            }
        } else {
            // ASCII
            charcode = STR_ASCII;
        }
    } else {
        // NULL
        charcode = STR_ASCII;
    }

    return charcode;
}


#if __ICONV == 0
/*! Convert character set from ISO-8859-x to UTF-8
    @param str          ISO-8859-x text
    @param iso8859_num  x of ISO-8859-x.
    @return Converted text  (UTF-8 text)
 */
char* iso8859_to_utf8(const char* str, int iso8859_num) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1;
    const unsigned char (*iso8859_map)[ISO8859_UNICODE_TABLE_MAX][2];

    if (iso8859_num <= 0 || iso8859_num == 12 || 17 <= iso8859_num) {
        return NULL;
    }

    switch (iso8859_num) {
        case 1:
            iso8859_map = &iso8859_1_unicode;
            break;
        case 2:
            iso8859_map = &iso8859_2_unicode;
            break;
        case 3:
            iso8859_map = &iso8859_3_unicode;
            break;
        case 4:
            iso8859_map = &iso8859_4_unicode;
            break;
        case 5:
            iso8859_map = &iso8859_5_unicode;
            break;
        case 6:
            iso8859_map = &iso8859_6_unicode;
            break;
        case 7:
            iso8859_map = &iso8859_7_unicode;
            break;
        case 8:
            iso8859_map = &iso8859_8_unicode;
            break;
        case 9:
            iso8859_map = &iso8859_9_unicode;
            break;
        case 10:
            iso8859_map = &iso8859_10_unicode;
            break;
        case 11:
            iso8859_map = &iso8859_11_unicode;
            break;
        case 13:
            iso8859_map = &iso8859_13_unicode;
            break;
        case 14:
            iso8859_map = &iso8859_14_unicode;
            break;
        case 15:
            iso8859_map = &iso8859_15_unicode;
            break;
        case 16:
            iso8859_map = &iso8859_16_unicode;
            break;
        default:
            return NULL;
            break;
    }

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*3;
        buf = new char[length2 + 1];

        // ISO-8859-x to UTF-8
        i = 0;
        j = 0;
        while (i < length) {
            if ((unsigned char)0x80 <= (unsigned char)str[i]) {
                a0 = (*iso8859_map)[(unsigned char)str[i] - (unsigned char)0x80][0];
                a1 = (*iso8859_map)[(unsigned char)str[i] - (unsigned char)0x80][1];
                if (a0 == (unsigned char)0x00 && (a1 & (unsigned char)0x80) == (unsigned char)0x00) {
                    buf[j++] = a1;
                } else if (a0 <= (unsigned char)0x07) {
                    buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                    buf[j++] = 0x80 | (0x3F & a1);
                } else {
                    buf[j++] = 0xE0 | (a0 >> 4);
                    buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                    buf[j++] = 0x80 | (0x3F & a1);
                }
            } else {
                buf[j++] = str[i];
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-8 to ISO-8859-x
    @param str          UTF-8 text
    @param iso8859_num  x of ISO-8859-x
    @return Converted text (ISO-8859-x text)
 */
char* utf8_to_iso8859(const char* str, int iso8859_num) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1, a2;
    const unsigned char (*iso8859_map)[ISO8859_UNICODE_TABLE_MAX][2];

    if (iso8859_num <= 0 || iso8859_num == 12 || 17 <= iso8859_num) {
        return NULL;
    }

    switch (iso8859_num) {
        case 1:
            iso8859_map = &iso8859_1_unicode;
            break;
        case 2:
            iso8859_map = &iso8859_2_unicode;
            break;
        case 3:
            iso8859_map = &iso8859_3_unicode;
            break;
        case 4:
            iso8859_map = &iso8859_4_unicode;
            break;
        case 5:
            iso8859_map = &iso8859_5_unicode;
            break;
        case 6:
            iso8859_map = &iso8859_6_unicode;
            break;
        case 7:
            iso8859_map = &iso8859_7_unicode;
            break;
        case 8:
            iso8859_map = &iso8859_8_unicode;
            break;
        case 9:
            iso8859_map = &iso8859_9_unicode;
            break;
        case 10:
            iso8859_map = &iso8859_10_unicode;
            break;
        case 11:
            iso8859_map = &iso8859_11_unicode;
            break;
        case 13:
            iso8859_map = &iso8859_13_unicode;
            break;
        case 14:
            iso8859_map = &iso8859_14_unicode;
            break;
        case 15:
            iso8859_map = &iso8859_15_unicode;
            break;
        case 16:
            iso8859_map = &iso8859_16_unicode;
            break;
        default:
            return NULL;
            break;
    }

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*3;
        buf = new char[length2 + 1];

        // UTF-8 to ISO-8859-x
        i = 0;
        j = 0;
        while (i < length && j <= length2) {
            a0 = str[i];
            if (isutf8_1(a0) && i+utf8_len(a0)-1 < length) {
                unsigned short len1, n;

                a1 = str[i+1];
                a2 = str[i+2];
                if (a0 == 0xEF && a1 == 0xBB && a2 == 0xBF) {
                    i += 3;
                    continue;
                }

                if (a0 == 0xE2 && a1 == 0x80 && a2 == 0xBE) {
                    buf[j++] = 0x7E;
                    i += 3;
                    continue;
                }

                len1 = utf8_len(str[i]);
                if (len1 == 2) {
                    a0 = 0x07 & (str[i] >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else if (len1 == 3) {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                } else if (len1 == 1) {
                    buf[j++] = str[i];
                    i++;
                    continue;
                } else {
                    buf[j++] = '?';
                    i += len1;
                    continue;
                }

                n = 0;
                while (n < ISO8859_UNICODE_TABLE_MAX) {
                    if ((*iso8859_map)[n][0] == a0 && (*iso8859_map)[n][1] == a1) {
                        buf[j++] = (unsigned char)(0x80 + n);
                        i += (utf8_len(str[i])-1);
                        break;
                    }
                    n++;
                }
                if (ISO8859_UNICODE_TABLE_MAX <= n) {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = str[i];
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from KOI8-x/CP1251/CP1252/CP1258 to UTF-8
    @param str          KOI-8-x text
    @param ch           x of KOI8-x. '1' for CP1251, '2' for CP1252, '8' for CP1258
    @return Converted text  (UTF-8 text)
 */
char* europe_to_utf8(const char* str, char ch) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1;
    const unsigned char (*eur_map)[EUROPE_UNICODE_TABLE_MAX][2];

    if (ch == '1') {
        eur_map = &cp1251_unicode;
    } else if (ch == '2') {
        eur_map = &cp1252_unicode;
    } else if (ch == '8') {
        eur_map = &cp1258_unicode;
    } else if (ch == 'U' || ch == 'u') {
        eur_map = &koi8_u_unicode;
    } else if (ch == 'R' || ch == 'r') {
        eur_map = &koi8_r_unicode;
    } else {
        eur_map = &cp1252_unicode;
    }

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*3;
        buf = new char[length2 + 1];

        // KOI8-x/CP1251/CP1252/CP1258 to UTF-8
        i = 0;
        j = 0;
        while (i < length) {
            if ((unsigned char)0x80 <= (unsigned char)str[i]) {
                a0 = (*eur_map)[(unsigned char)str[i] - (unsigned char)0x80][0];
                a1 = (*eur_map)[(unsigned char)str[i] - (unsigned char)0x80][1];
                if (a0 == (unsigned char)0x00 && (a1 & (unsigned char)0x80) == (unsigned char)0x00) {
                    buf[j++] = a1;
                } else if (a0 <= (unsigned char)0x07) {
                    buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                    buf[j++] = 0x80 | (0x3F & a1);
                } else {
                    buf[j++] = 0xE0 | (a0 >> 4);
                    buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                    buf[j++] = 0x80 | (0x3F & a1);
                }
            } else {
                buf[j++] = str[i];
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-8 to KOI8-x/CP1251/CP1252/CP1258
    @param str          UTF-8 text
    @param ch           x of KOI8-x. '1' for CP1251, '2' for CP1252, '8' for CP1258
    @return Converted text (KOI8-x/CP1251/CP1252/CP1258 text)
 */
char* utf8_to_europe(const char* str, char ch) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1, a2;
    const unsigned char (*eur_map)[EUROPE_UNICODE_TABLE_MAX][2];

    if (ch == '1') {
        eur_map = &cp1251_unicode;
    } else if (ch == '2') {
        eur_map = &cp1252_unicode;
    } else if (ch == '8') {
        eur_map = &cp1258_unicode;
    } else if (ch == 'U' || ch == 'u') {
        eur_map = &koi8_u_unicode;
    } else if (ch == 'R' || ch == 'r') {
        eur_map = &koi8_r_unicode;
    } else {
        eur_map = &cp1252_unicode;
    }

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*3;
        buf = new char[length2 + 1];

        // UTF-8 to KOI8-x/CP1251/CP1252/CP1258
        i = 0;
        j = 0;
        while (i < length && j <= length2) {
            a0 = str[i];
            if (isutf8_1(a0) && i+utf8_len(a0)-1 < length) {
                unsigned short len1, n;

                a1 = str[i+1];
                a2 = str[i+2];
                if (a0 == 0xEF && a1 == 0xBB && a2 == 0xBF) {
                    i += 3;
                    continue;
                }

                if (a0 == 0xE2 && a1 == 0x80 && a2 == 0xBE) {
                    buf[j++] = 0x7E;
                    i += 3;
                    continue;
                }

                len1 = utf8_len(str[i]);
                if (len1 == 2) {
                    a0 = 0x07 & (str[i] >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else if (len1 == 3) {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                } else if (len1 == 1) {
                    buf[j++] = str[i];
                    i++;
                    continue;
                } else {
                    buf[j++] = '?';
                    i += len1;
                    continue;
                }

                n = 0;
                while (n < EUROPE_UNICODE_TABLE_MAX) {
                    if ((*eur_map)[n][0] == a0 && (*eur_map)[n][1] == a1) {
                        buf[j++] = (unsigned char)(0x80 + n);
                        i += (utf8_len(str[i])-1);
                        break;
                    }
                    n++;
                }
                if (EUROPE_UNICODE_TABLE_MAX <= n) {
                    buf[j++] = '?';
                    i += (len1 - 1);
                }
            } else {
                buf[j++] = (a0 & (unsigned char)0x80)?'?':a0;
            }
            i++;
        }
        buf[j] = '\0';
    }
    return buf;
}


/*! Convert character set from UTF-8 to UTF-16
    @param str    UTF-8 text
    @param length byte size of UTF-16 binary text
    @param en     'b' for big endian, 'l' for little endian, 'B' or 'L' for adding BOM
    @return Converted text (UTF-16 binary text)
 */
char* utf8_to_utf16(const char *str, long *length, char en) {
    char *buf;
    unsigned long ucs4, n, n1, n2;
    const unsigned char *p;
    unsigned char c;
    long i, j, l;
    int bom;

    if (length != NULL) *length = 0;
    if (str == NULL || length == NULL) return NULL;

    // Memory allocation
    l = strlen(str);
    buf = new char[l * 4 + 4];
    buf[0] = '\0';
    buf[1] = '\0';

    i = 0;
    j = 0;
    if (en == 'L') {
        buf[j++] = '\xFF';
        buf[j++] = '\xFE';
    } else if (en == 'B') {
        buf[j++] = '\xFE';
        buf[j++] = '\xFF';
    }
    while (i < l) {
        p = (const unsigned char *)(str + i);
        c = p[0];
        bom = 0;
        switch (utf8_len(c)) {
            case 1:
                ucs4 = (unsigned long)c;
                i++;
                break;
            case 2:
                if (p[1] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 2;
                    break;
                }
                ucs4 = c & 0x1F;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                i += 2;
                break;
            case 3:
                if (p[1] == '\0' || p[2] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 3;
                    break;
                }
                if (c == 0xEF && p[1] == 0xBB && p[2] == 0xBF) bom = 1;
                ucs4 = c & 0x0F;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                i += 3;
                break;
            case 4:
                if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 4;
                    break;
                }
                ucs4 = c & 7;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[3] & 0x3F);
                i += 4;
                break;
            case 5:
                if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0' || p[4] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 5;
                    break;
                }
                ucs4 = c & 3;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[3] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[4] & 0x3F);
                i += 5;
                break;
            case 6:
                if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0' || p[4] == '\0' || p[5] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 6;
                    break;
                }
                ucs4 = c & 1;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[3] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[4] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[5] & 0x3F);
                i += 6;
                break;
            default:
                ucs4 = (unsigned long)'?';
                i++;
                break;
        }

        if (bom) continue;

        if (en == 'l' || en == 'L') {
            if (0x10FFFFUL < ucs4) {
                buf[j++] = '?';
                buf[j++] = '\0';
            } else if (0x10000UL <= ucs4 && ucs4 <= 0x10FFFFUL) {
                n = ucs4 - 0x10000;
                n1 = n / 0x400 + 0xD800;
                n2 = n % 0x400 + 0xDC00;
                buf[j++] = n1 & 0xFF;
                buf[j++] = n1 / 256;
                buf[j++] = n2 & 0xFF;
                buf[j++] = n2 / 256;
            } else {
                buf[j++] = ucs4 & 0xFF;
                buf[j++] = (ucs4 >> 8) & 0xFF;
            }
        } else {
            if (0x10FFFFUL < ucs4) {
                buf[j++] = '\0';
                buf[j++] = '?';
            } else if (0x10000UL <= ucs4 && ucs4 <= 0x10FFFFUL) {
                n = ucs4 - 0x10000;
                n1 = n / 0x400 + 0xD800;
                n2 = n % 0x400 + 0xDC00;
                buf[j++] = n1 / 256;
                buf[j++] = n1 & 0xFF;
                buf[j++] = n2 / 256;
                buf[j++] = n2 & 0xFF;
            } else {
                buf[j++] = (ucs4 >> 8) & 0xFF;
                buf[j++] = ucs4 & 0xFF;
            }
        }
    }
    *length = j;

    return buf;
}


/*! Convert character set from UTF-16 to UTF-8
    @param str    UTF-16 binary text
    @param length byte size of UTF-16 binary text
    @param en     'b' for big endian, 'l' for little endian
    @return Converted text (UTF-8 text)
 */
char* utf16_to_utf8(const char *str, long length, char en) {
    char *buf;
    long i, j;
    unsigned long ucs4, n1, n2;

    if (str == NULL) return NULL;

    // Memory allocation
    buf = new char[length * 2 + 1];

    if (2 <= length) {
        if ((const unsigned char)str[0] == (const unsigned char)'\xFF' &&
            (const unsigned char)str[1] == (const unsigned char)'\xFE') {
            en = 'L';
        } else if ((const unsigned char)str[0] == (const unsigned char)'\xFE' &&
            (const unsigned char)str[1] == (const unsigned char)'\xFF') {
            en = 'B';
        }
    }

    j = 0;
    for (i = 0; i < length; i += 2) {
        if (length <= i + 1) {
            buf[j++] = '?';
            buf[j] = '\0';
        } else {
            if (en == 'l' || en == 'L') {
                // skip BOM
                if ((const unsigned char)str[i] == 0xFF &&
                        (const unsigned char)str[i+1] == 0xFE) continue;

                if (i + 3 < length &&
                        0xD8 <= (const unsigned char)str[i+1] &&
                        (const unsigned char)str[i+1] <= 0xDB &&
                        0xDC <= (const unsigned char)str[i+3] &&
                        (const unsigned char)str[i+3] <= 0xDF) {
                    n1 = ((const unsigned char)str[i+1] - 0xD8) * 256 +
                            (const unsigned char)str[i];
                    n2 = ((const unsigned char)str[i+3] - 0xDC) * 256 +
                            (const unsigned char)str[i+2];
                    ucs4 = (n1 << 10) + n2 + 0x10000UL;
                    i += 2;
                } else {
                    ucs4 = ((const unsigned char)str[i+1] * 256UL) +
                            (const unsigned char)str[i];
                }
            } else {
                // skip BOM
                if ((const unsigned char)str[i] == 0xFE &&
                        (const unsigned char)str[i+1] == 0xFF) continue;

                if (i + 3 < length &&
                        0xD8 <= (const unsigned char)str[i] &&
                        (const unsigned char)str[i] <= 0xDB &&
                        0xDC <= (const unsigned char)str[i+2] &&
                        (const unsigned char)str[i+2] <= 0xDF) {
                    n1 = ((const unsigned char)str[i] - 0xD8) * 256UL +
                            (const unsigned char)str[i+1];
                    n2 = ((const unsigned char)str[i+2] - 0xDC) * 256UL +
                            (const unsigned char)str[i+3];
                    ucs4 = (n1 << 10) + n2 + 0x10000UL;
                    i += 2;
                } else {
                    ucs4 = ((const unsigned char)str[i] * 256UL) +
                            (const unsigned char)str[i+1];
                }
            }

            // UCS4 to UTF-8
            if (ucs4 == 0UL) {
                buf[j++] = '?';
            } else if (ucs4 <= 0x7FUL) {
                buf[j++] = (unsigned char)ucs4;
            } else if (ucs4 <= 0x7FFUL) {
                buf[j++] = 0xC0 | (ucs4 >> 6);
                buf[j++] = 0x80 | (ucs4 & 0x3F);
            } else if (ucs4 <= 0xFFFFUL) {
                buf[j++] = 0xE0 | (ucs4 >> 12);
                buf[j++] = 0x80 | ((ucs4 >> 6) & 0x3F);
                buf[j++] = 0x80 | (ucs4 & 0x3F);
            } else {
                buf[j++] = 0xF0 | (ucs4 >> 18);
                buf[j++] = 0x80 | ((ucs4 >> 12) & 0x3F);
                buf[j++] = 0x80 | ((ucs4 >> 6) & 0x3F);
                buf[j++] = 0x80 | (ucs4 & 0x3F);
            }
        }
    }
    buf[j] = '\0';

    return buf;
}


/*! Convert character set from UTF-8 to UTF-32
    @param str    UTF-8 text
    @param length byte size of UTF-32 binary text
    @param en     'b' for big endian, 'l' for little endian, 'B' or 'L' for adding BOM
    @return Converted text (UTF-32 binary text)
 */
char* utf8_to_utf32(const char *str, long *length, char en) {
    char *buf;
    unsigned long ucs4;
    const unsigned char *p;
    unsigned char c;
    long i, j, l;
    int bom;

    if (length != NULL) *length = 0;
    if (str == NULL || length == NULL) return NULL;

    // Memory allocation
    l = strlen(str);
    buf = new char[l * 4 + 8];
    buf[0] = '\0';
    buf[1] = '\0';
    buf[2] = '\0';
    buf[3] = '\0';

    i = 0;
    j = 0;
    if (en == 'L') {
        buf[j++] = '\xFF';
        buf[j++] = '\xFE';
        buf[j++] = '\x00';
        buf[j++] = '\x00';
    } else if (en == 'B') {
        buf[j++] = '\x00';
        buf[j++] = '\x00';
        buf[j++] = '\xFE';
        buf[j++] = '\xFF';
    }
    while (i < l) {
        p = (const unsigned char *)(str + i);
        c = p[0];
        bom = 0;
        switch (utf8_len(c)) {
            case 1:
                ucs4 = (unsigned long)c;
                i++;
                break;
            case 2:
                if (p[1] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 2;
                    break;
                }
                ucs4 = c & 0x1F;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                i += 2;
                break;
            case 3:
                if (p[1] == '\0' || p[2] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 3;
                    break;
                }
                if (c == 0xEF && p[1] == 0xBB && p[2] == 0xBF) bom = 1;
                ucs4 = c & 0x0F;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                i += 3;
                break;
            case 4:
                if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 4;
                    break;
                }
                ucs4 = c & 7;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[3] & 0x3F);
                i += 4;
                break;
            case 5:
                if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0' || p[4] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 5;
                    break;
                }
                ucs4 = c & 3;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[3] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[4] & 0x3F);
                i += 5;
                break;
            case 6:
                if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0' || p[4] == '\0' || p[5] == '\0') {
                    ucs4 = (unsigned long)'?';
                    i += 6;
                    break;
                }
                ucs4 = c & 1;
                ucs4 = (ucs4 << 6) + (p[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[3] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[4] & 0x3F);
                ucs4 = (ucs4 << 6) + (p[5] & 0x3F);
                i += 6;
                break;
            default:
                ucs4 = (unsigned long)'?';
                i++;
                break;
        }

        if (bom) continue;

        if (en == 'l' || en == 'L') {
            buf[j++] = ucs4 & 0xFF;
            buf[j++] = (ucs4 >> 8) & 0xFF;
            buf[j++] = (ucs4 >> 16) & 0xFF;
            buf[j++] = (ucs4 >> 24) & 0xFF;
        } else {
            buf[j++] = (ucs4 >> 24) & 0xFF;
            buf[j++] = (ucs4 >> 16) & 0xFF;
            buf[j++] = (ucs4 >> 8) & 0xFF;
            buf[j++] = ucs4 & 0xFF;
        }
    }
    *length = j;

    return buf;
}


/*! Convert character set from UTF-32 to UTF-8
    @param str    UTF-32 binary text
    @param length byte size of UTF-32 binary text
    @param en     'b' for big endian, 'l' for little endian
    @return Converted text (UTF-8 text)
 */
char* utf32_to_utf8(const char *str, long length, char en) {
    char *buf;
    long i, j;
    unsigned long ucs4;

    if (str == NULL) return NULL;

    // Memory allocation
    buf = new char[length + 1];

    if (4 <= length) {
        if ((const unsigned char)str[0] == (const unsigned char)'\xFF' &&
            (const unsigned char)str[1] == (const unsigned char)'\xFE' &&
            (const unsigned char)str[2] == (const unsigned char)'\x00' &&
            (const unsigned char)str[3] == (const unsigned char)'\x00') {
            en = 'L';
        } else if ((const unsigned char)str[0] == (const unsigned char)'\x00' &&
            (const unsigned char)str[2] == (const unsigned char)'\x00' &&
            (const unsigned char)str[2] == (const unsigned char)'\xFE' &&
            (const unsigned char)str[1] == (const unsigned char)'\xFF') {
            en = 'B';
        }
    }

    j = 0;
    for (i = 0; i < length; i += 4) {
        if (length <= i + 3) {
            buf[j++] = '?';
            buf[j] = '\0';
        } else {
            if (en == 'l' || en == 'L') {
                ucs4 = ((const unsigned char)str[i+3] * 16777216UL) +
                        ((const unsigned char)str[i+2] * 65536UL) +
                        ((const unsigned char)str[i+1] * 256UL) +
                        (const unsigned char)str[i];
            } else {
                ucs4 = ((const unsigned char)str[i] * 16777216UL) +
                        ((const unsigned char)str[i+1] * 65536UL) +
                        ((const unsigned char)str[i+2] * 256UL) +
                        (const unsigned char)str[i+3];
            }

            // skip BOM
            if (ucs4 == 0xFEUL * 256UL + 0xFFUL) continue;

            // UCS4 to UTF-8
            if (ucs4 == 0UL) {
                buf[j++] = '?';
            } else if (ucs4 <= 0x7FUL) {
                buf[j++] = (unsigned char)ucs4;
            } else if (ucs4 <= 0x7FFUL) {
                buf[j++] = 0xC0 | (ucs4 >> 6);
                buf[j++] = 0x80 | (ucs4 & 0x3F);
            } else if (ucs4 <= 0xFFFFUL) {
                buf[j++] = 0xE0 | (ucs4 >> 12);
                buf[j++] = 0x80 | ((ucs4 >> 6) & 0x3F);
                buf[j++] = 0x80 | (ucs4 & 0x3F);
            } else {
                buf[j++] = 0xF0 | (ucs4 >> 18);
                buf[j++] = 0x80 | ((ucs4 >> 12) & 0x3F);
                buf[j++] = 0x80 | ((ucs4 >> 6) & 0x3F);
                buf[j++] = 0x80 | (ucs4 & 0x3F);
            }
        }
    }
    buf[j] = '\0';

    return buf;
}


/*! Convert character set from UTF-8 to EUCJP-MS
    @param str  UTF-8 text
    @return Converted text (EUC-JP text)
 */
char* utf8_to_eucjpms(const char *str) {
    char *buf1, *buf2;

    buf1 = utf8_to_sjis(str);
    buf2 = sjis_to_eucjpms(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from UTF-8 to EUCJP-WIN
    @param str  UTF-8 text
    @return Converted text (EUC-JP text)
 */
char* utf8_to_eucjpwin(const char *str) {
    char *buf1, *buf2;

    buf1 = utf8_to_sjis(str);
    buf2 = sjis_to_eucjpwin(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from CP932 to EUCJP-MS
    @param str  CP932 text
    @return Converted text (EUC-JP text)
 */
char* sjis_to_eucjpms(const char *str) {
    char *buf;
    long i, j, hankana, length;
    unsigned char c, d;
    const char *p;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // CP932 to EUC-JP
        hankana = 0;
        i = 0;
        j = 0;
        while (i < length) {
            c = str[i];
            d = c?(str[i+1]):0;
            if (c == 0xFA && 0x40 <= d && d <= 0xFF) {
                p = SJIS_EUCJP_FA40[d - 0x40];
                if (p[0] == '\0') {
                    i += 2;
                    continue;
                }
                buf[j++] = '\x8F';
                buf[j++] = (unsigned char)(p[0]);
                buf[j++] = (unsigned char)(p[1]);
                i++;
            } else if (c == 0xFB && 0x40 <= d && d <= 0xFF) {
                p = SJIS_EUCJP_FB40[d - 0x40];
                if (p[0] == '\0') {
                    i += 2;
                    continue;
                }
                buf[j++] = '\x8F';
                buf[j++] = (unsigned char)(p[0]);
                buf[j++] = (unsigned char)(p[1]);
                i++;
            } else if (c == 0xFC && 0x40 <= d && d <= 0x4F) {
                p = SJIS_EUCJP_FC40[d - 0x40];
                if (p[0] == '\0') {
                    i += 2;
                    continue;
                }
                buf[j++] = '\x8F';
                buf[j++] = (unsigned char)(p[0]);
                buf[j++] = (unsigned char)(p[1]);
                i++;
            } else if (issjis1(c) && issjis2(d)) {
                if (c <= 0x9F) {
                    if (d < 0x9F) {
                        c = (c << 1) - 0xE1;
                    } else {
                        c = (c << 1) - 0xE0;
                    }
                } else if (0xF0 <= c && c < 0xF5) {
                    if (d < 0x9F) {
                        c = (c << 1) - 0x161 - (0xFF - 0xF5);
                    } else {
                        c = (c << 1) - 0x160 - (0xFF - 0xF5);
                    }
                } else if (0xF5 <= c && c < 0xFA) {
                    // 3byte EUC
                    buf[j++] = '\x8F';
                    if (d < 0x9F) {
                        c = (c << 1) - 0x161 + (0xF5 - 0x89);
                    } else {
                        c = (c << 1) - 0x160 + (0xF5 - 0x89);
                    }
                } else {
                    if (d < 0x9F) {
                        c = (c << 1) - 0x161;
                    } else {
                        c = (c << 1) - 0x160;
                    }
                }
                if (d < 0x7F) {
                    d -= 0x1F;
                } else if (d < 0x9F) {
                    d -= 0x20;
                } else {
                    d -= 0x7E;
                }
                buf[j++] = (unsigned char)(c | 0x80);
                buf[j++] = (unsigned char)(d | 0x80);
                i++;
            } else if (ishankana((unsigned char)str[i])) {
                if (j + (++hankana) < length*3) {
                    buf[j++] = 0x8E;
                    buf[j++] = str[i];
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = str[i];
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from CP932 to EUCJP-WIN
    @param str  CP932 text
    @return Converted text (EUC-JP text)
 */
char* sjis_to_eucjpwin(const char *str) {
    char *buf;
    long i, j, hankana, length;
    unsigned char c, d;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // CP932 to EUC-JP
        hankana = 0;
        i = 0;
        j = 0;
        while (i < length) {
            c = str[i];
            d = c?(str[i+1]):0;
            if (issjis1(c) && issjis2(d)) {
                if (c <= 0x9F) {
                    if (d < 0x9F) {
                        c = (c << 1) - 0xE1;
                    } else {
                        c = (c << 1) - 0xE0;
                    }
                    if (d < 0x7F) {
                        d -= 0x1F;
                    } else if (d < 0x9F) {
                        d -= 0x20;
                    } else {
                        d -= 0x7E;
                    }
                } else if (c <= 0xEA) {
                    if (d < 0x9F) {
                        c = (c << 1) - 0x161;
                    } else {
                        c = (c << 1) - 0x160;
                    }
                    if (d < 0x7F) {
                        d -= 0x1F;
                    } else if (d < 0x9F) {
                        d -= 0x20;
                    } else {
                        d -= 0x7E;
                    }
                } else if (0xF0 <= c && c < 0xFA) {
                    buf[j++] = '?';
                    buf[j++] = '?';
                    i += 2;
                    continue;
                } else if (c == 0xFA && 0x40 <= d && d < 0x5C) {
                    if (d < 0x4A) {
                        c = 0xFC;
                        d = 0xF1 + (d-0x40);
                    } else if (0x55 <= d && d <= 0x57) {
                        c = 0xFC;
                        d = 0xFC + (d-0x55);
                    } else {
                        c = '?';
                        d = '?';
                    }
                } else if (c == 0xFA && 0x7B <= d && d <= 0x7F) {
                    c = 0xF9;
                    d = 0xC0 + (d-0x7B);
                } else if (c == 0xFA && 0x9F <= d && d <= 0xBA) {
                    c = 0xF9;
                    d = 0xE3 + (d-0x9F);
                } else if (c == 0xFB && 0x40 <= d && d <= 0x5B) {
                    c = 0xFA;
                    d = 0xE3 + (d-0x40);
                } else if (c == 0xFB && 0x5C <= d && d <= 0x5F) {
                    c = 0xFB;
                    d = 0xA1 + (d-0x5C);
                } else if (c == 0xFB && 0x9F <= d && d <= 0xBA) {
                    c = 0xFB;
                    d = 0xE3 + (d-0x9F);
                } else if (c == 0xFC && 0x40 <= d) {
                    c = 0xFC;
                    d = 0xE3 + (d-0x40);
                } else {
                    if (d < 0x9F) {
                        c = (c << 1) - 0xFB;
                    } else {
                        c = (c << 1) - 0xFA;
                    }
                    if (d < 0x7F) {
                        d += 0x45;
                    } else if (d < 0x9F) {
                        d += 0x44;
                    } else {
                        d -= 0x1A;
                    }
                }
                buf[j++] = (unsigned char)(c | 0x80);
                buf[j++] = (unsigned char)(d | 0x80);
                i++;
            } else if (ishankana((unsigned char)str[i])) {
                if (j + (++hankana) < length*3) {
                    buf[j++] = 0x8E;
                    buf[j++] = str[i];
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = str[i];
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from ISO-2022-JP to EUCJP-MS
    @param str  ISO-2022-JP text
    @return Converted text (EUC-JP text)
 */
char* jis_to_eucjpms(const char *str) {
    char *buf1, *buf2;

    buf1 = jis_to_sjis(str);
    buf2 = sjis_to_eucjpms(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from ISO-2022-JP to EUCJP-WIN
    @param str  ISO-2022-JP text
    @return Converted text (EUC-JP text)
 */
char* jis_to_eucjpwin(const char *str) {
    char *buf1, *buf2;

    buf1 = jis_to_sjis(str);
    buf2 = sjis_to_eucjpwin(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;

#if 0
    char *buf;
    long i, j, hankana, length;
    unsigned char c, d;
    enum {NORMAL, KANJI, HANKANA} mode = NORMAL;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // ISO-2022-JP to EUC-JP
        mode = NORMAL;
        hankana = 0;
        i = 0;
        j = 0;
        while (i < length) {
            c = str[i];
            d = c?(str[i+1]):0;
            if (str[i] == 0x1B) {
                if (str[i+1] == '$') {
                    if (i+2 < length && (str[i+2] == '@' || str[i+2] == 'B')) {
                        mode = KANJI;
                        i += 2;
                    } else {
                        buf[j++] = 0x1B;
                    }
                } else if (str[i+1] == '(') {
                    if (i+2 < length && (str[i+2] == 'B' || str[i+2] == 'J')) {
                        mode = NORMAL;
                        i += 2;
                    } else if (i+2 < length && (str[i+2] == 'I')) {
                        mode = HANKANA;
                        i += 2;
                    } else {
                        buf[j++] = 0x1B;
                        i++;
                    }
                } else {
                    buf[j++] = 0x1B;
                }
            } else if (str[i] == 0x0E) {
                mode = HANKANA;
            } else if (str[i] == 0x0F) {
                mode = NORMAL;
            } else if ((mode == KANJI) && (isjis(str[i]) && isjis(str[i+1]))) {
                buf[j++] = (str[i] | 0x80);
                buf[j++] = (str[i+1] | 0x80);
                i++;
            } else if ((mode == HANKANA) && (0x20 <= str[i] && str[i] <= 0x5F)) {
                buf[j++] = 0x8E;
                buf[j++] = (str[i] | 0x80);
            } else if (issjis1(c) && issjis2(d)) {
                if (c <= 0x9F) {
                    if (d < 0x9F) {
                        c = (c << 1) - 0xE1;
                    } else {
                        c = (c << 1) - 0xE0;
                    }
                } else {
                    if (d < 0x9F) {
                        c = (c << 1) - 0x161;
                    } else {
                        c = (c << 1) - 0x160;
                    }
                }
                if (d < 0x7F) {
                    d -= 0x1F;
                } else if (d < 0x9F) {
                    d -= 0x20;
                } else {
                    d -= 0x7E;
                }
                buf[j++] = (unsigned char)(c | 0x80);
                buf[j++] = (unsigned char)(d | 0x80);
                i++;
            } else if (ishankana((unsigned char)str[i])) {
                if (j + (++hankana) < length*3) {
                    buf[j++] = 0x8E;
                    buf[j++] = str[i];
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = str[i];
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
#endif
}


/*! Convert character set from EUCJP-MS to CP932
    @param str  EUC-JP text
    @return Converted text (CP932 text)
 */
char* eucjpms_to_sjis(const char *str) {
    char *buf;
    long i, j, k, length;
    unsigned char a0, a1, *e;
    int found;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length + 1];

        // EUC-JP to CP932
        i = 0;
        j = 0;
        while (i < length) {
            a0 = str[i];
            a1 = str[i+1];
            if (a0 == 0x8E && ishankana(a1)) {
                buf[j++] = a1;
                i++;
            } else if (a0 == 0x8F && a1 != 0 && i + 2 < length) {
                // 3byte EUC
                a0 = str[i+1];
                a1 = str[i+2];

                found = 0;
                for (k = 0; k < 16*12; k++) {
                    e = (unsigned char *)SJIS_EUCJP_FA40[k];
                    if (e[0] == '\0') continue;
                    if (e[0] == a0 && e[1] == a1) {
                        buf[j++] = '\xFA';
                        buf[j++] = '\x40' + k;
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    i += 3;
                    continue;
                }
                for (k = 0; k < 16*12; k++) {
                    e = (unsigned char *)SJIS_EUCJP_FB40[k];
                    if (e[0] == '\0') continue;
                    if (e[0] == a0 && e[1] == a1) {
                        buf[j++] = '\xFB';
                        buf[j++] = '\x40' + k;
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    i += 3;
                    continue;
                }
                for (k = 0; k < 16; k++) {
                    e = (unsigned char *)SJIS_EUCJP_FC40[k];
                    if (e[0] == '\0') continue;
                    if (e[0] == a0 && e[1] == a1) {
                        buf[j++] = '\xFC';
                        buf[j++] = '\x40' + k;
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    i += 3;
                    continue;
                }

                a0 &= 0x7F;
                a1 &= 0x7F;
                if ((a0 & 0x01) != 0) {
                    a0 = ((a0 + 1) / 2) + 0x70;
                    a1 = a1 + 0x1F;
                } else {
                    a0 = (a0 / 2) + 0x70;
                    a1 = a1 + 0x7D;
                }
                if (0xA0 <= a0) a0 += 0x40;
                if (0x7F <= a1) a1 += 0x01;
                a0 += (0xF5 - 0xEB);
                buf[j++] = a0;
                buf[j++] = a1;
                i += 2;
            } else if (iseuc(a0) && iseuc(a1)) {
                a0 &= 0x7F;
                a1 &= 0x7F;
                if ((a0 & 0x01) != 0) {
                    a0 = ((a0 + 1) / 2) + 0x70;
                    a1 = a1 + 0x1F;
                } else {
                    a0 = (a0 / 2) + 0x70;
                    a1 = a1 + 0x7D;
                }
                if (0xA0 <= a0) a0 += 0x40;
                if (0xEB <= a0) a0 += (0xF0 - 0xEB);
                if (0x7F <= a1) a1 += 0x01;
                buf[j++] = a0;
                buf[j++] = a1;
                i++;
            } else {
                buf[j++] = a0;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from EUCJP-WIN to CP932
    @param str  EUC-JP text
    @return Converted text (CP932 text)
 */
char* eucjpwin_to_sjis(const char *str) {
    char *buf;
    long i, j, k, length;
    unsigned char a0, a1, *e;
    int found;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length + 1];

        // EUC-JP to CP932
        i = 0;
        j = 0;
        while (i < length) {
            a0 = str[i];
            a1 = str[i+1];
            if (a0 == 0x8E && ishankana(a1)) {
                buf[j++] = a1;
                i++;
            } else if (a0 == 0x8F && a1 != 0 && i + 2 < length) {
                // 3byte EUC
                a0 = str[i+1];
                a1 = str[i+2];

                found = 0;
                for (k = 0; k < 16*12; k++) {
                    e = (unsigned char *)SJIS_EUCJP_FA40[k];
                    if (e[0] == '\0') continue;
                    if (e[0] == a0 && e[1] == a1) {
                        buf[j++] = '\xFA';
                        buf[j++] = '\x40' + k;
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    i += 3;
                    continue;
                }
                for (k = 0; k < 16*12; k++) {
                    e = (unsigned char *)SJIS_EUCJP_FB40[k];
                    if (e[0] == '\0') continue;
                    if (e[0] == a0 && e[1] == a1) {
                        buf[j++] = '\xFB';
                        buf[j++] = '\x40' + k;
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    i += 3;
                    continue;
                }
                for (k = 0; k < 16; k++) {
                    e = (unsigned char *)SJIS_EUCJP_FC40[k];
                    if (e[0] == '\0') continue;
                    if (e[0] == a0 && e[1] == a1) {
                        buf[j++] = '\xFC';
                        buf[j++] = '\x40' + k;
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    i += 3;
                    continue;
                }

                a0 &= 0x7F;
                a1 &= 0x7F;
                if ((a0 & 0x01) != 0) {
                    a0 = ((a0 + 1) / 2) + 0x70;
                    a1 = a1 + 0x1F;
                } else {
                    a0 = (a0 / 2) + 0x70;
                    a1 = a1 + 0x7D;
                }
                if (0xA0 <= a0) a0 += 0x40;
                if (0x7F <= a1) a1 += 0x01;
                a0 += (0xF5 - 0xEB);
                buf[j++] = a0;
                buf[j++] = a1;
                i += 2;
            } else if (iseuc(a0) && iseuc(a1)) {
                if (a0 < 0xF9) {
                    a0 &= 0x7F;
                    a1 &= 0x7F;
                    if (a0 & 1) {
                        if (a1 < 0x60) {
                            a1 += 0x1F;
                        } else {
                            a1 += 0x20;
                        }
                    } else {
                        a1 += 0x7E;
                    }
                    if (a0 < 0x5F) {
                        a0 = (a0 + 0xE1) >> 1;
                    } else {
                        a0 = (unsigned char)((a0 + 0x161) >> 1);
                    }
                } else if (a0 == 0xF9 && 0xC0 <= a1 && a1 <= 0xC3) {
                    a0 = 0xFA;
                    a1 = 0x7B + (a1-0xC0);
                } else if (a0 == 0xFA && 0xE3 <= a1 && a1 <= 0xFE) {
                    a0 = 0xFB;
                    a1 = 0x40 + (a1-0xE3);
                } else if (a0 == 0xFB && 0xA1 <= a1 && a1 <= 0xA4) {
                    a0 = 0xFB;
                    a1 = 0x5C + (a1-0xA1);
                } else if (a0 == 0xFB && 0xC0 <= a1 && a1 <= 0xC3) {
                    a0 = 0xFB;
                    a1 = 0x7B + (a1-0xC0);
                } else if (a0 == 0xFC && 0xE3 <= a1 && a1 <= 0xEF) {
                    a0 = 0xFC;
                    a1 = 0x40 + (a1-0xE3);
                } else if (a0 == 0xFC && 0xF1 <= a1 && a1 <= 0xFE && a1 != 0xFB) {
                    if (a1 <= 0xFA) {
                        a0 = 0xFA;
                        a1 = 0x40 + (a1-0xF1);
                    } else {
                        a0 = 0xFA;
                        a1 = 0x55 + (a1-0xFC);
                    }
                } else {
                    a0 &= 0x7F;
                    a1 &= 0x7F;
                    if (a0 & 1) {
                        if (a1 < 0x40) {
                            a1 += 0x3B;
                        } else {
                            a1 += 0x3C;
                        }
                    } else {
                        a1 += 0x9A;
                    }
                    if (a0 < 0x5F) {
                        a0 = (unsigned char)((a0 + 0xFA) >> 1) + 0x40;
                    } else {
                        a0 = (unsigned char)(((a0 + 0xFB) >> 1) + 0x40);
                    }
                }
                if (!(issjis1(a0) && issjis2(a1))) {
                    a0 = '?';
                    a1 = '?';
                }
                buf[j++] = a0;
                buf[j++] = a1;
                i++;
            } else {
                buf[j++] = a0;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from EUCJP-MS to ISO-2022-JP
    @param str  EUC-JP text
    @return Converted text (ISO-2022-JP text)
 */
char* eucjpms_to_jis(const char *str) {
    char *buf1, *buf2;

    buf1 = eucjpms_to_sjis(str);
    buf2 = sjis_to_jis(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from EUCJP-WIN to ISO-2022-JP
    @param str  EUC-JP text
    @return Converted text (ISO-2022-JP text)
 */
char* eucjpwin_to_jis(const char *str) {
    char *buf1, *buf2;

    buf1 = eucjpwin_to_sjis(str);
    buf2 = sjis_to_jis(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;

#if 0
    char *buf;
    long i, j, buf_size, length;
    unsigned char a0, a1;
    enum {NORMAL, KANJI, HANKANA} mode = NORMAL;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[(buf_size = length*3) + 1];

        // EUC-JP to ISO-2022-JP
        i = 0;
        j = 0;
        while (i < length) {
            if (iseuc(a0 = str[i]) && iseuc(a1 = str[i+1]) && j+1 < buf_size) {
                if (mode != KANJI && j+4 < buf_size) {
                    if (mode == HANKANA && j+5 < buf_size) {
                        buf[j++] = 0x0F;
                    }
                    mode = KANJI;
                    buf[j++] = 0x1B;
                    buf[j++] = '$';
                    buf[j++] = 'B';
                }
                buf[j++] = a0 & 0x7F;
                buf[j++] = a1 & 0x7F;
                i++;
            } else if (a0 == 0x8E && j+1 < buf_size) {
                if (ishankana(a1 = str[i+1])) {
                    if (mode != HANKANA && j+2 < buf_size) {
                        if (mode == KANJI && j+5 < buf_size) {
                            buf[j++] = 0x1B;
                            buf[j++] = '(';
                            buf[j++] = 'B';
                        }
                        mode = HANKANA;
                    }
                    buf[j++] = 0x0e;
                    buf[j++] = a1 & 0x7F;
                    i++;
                }
            } else {
                if (mode == KANJI && j+3 < buf_size) {
                    buf[j++] = 0x1B;
                    buf[j++] = '(';
                    buf[j++] = 'B';
                } else if (mode == HANKANA && j+1 < buf_size) {
                    buf[j++] = 0x0F;
                }
                mode = NORMAL;
                buf[j++] = a0;
            }
            i++;
        }
        if (mode == KANJI && j+2 < buf_size) {
            buf[j++] = 0x1B;
            buf[j++] = '(';
            buf[j++] = 'B';
        } else if (mode == HANKANA && j < buf_size) {
            buf[j++] = 0x0F;
        }
        buf[j] = '\0';
    }

    return buf;
#endif
}


/*! Convert character set from EUCJP-MS to UTF-8
    @param str  EUC-JP text.
    @return Converted text (UTF-8 text)
 */
char* eucjpms_to_utf8(const char *str) {
    char *buf1, *buf2;

    buf1 = eucjpms_to_sjis(str);
    buf2 = sjis_to_utf8(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from EUCJP-WIN to UTF-8
    @param str  EUC-JP text.
    @return Converted text (UTF-8 text)
 */
char* eucjpwin_to_utf8(const char *str) {
    char *buf1, *buf2;

    buf1 = eucjpwin_to_sjis(str);
    buf2 = sjis_to_utf8(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from EUCJP-MS to EUCJP-WIN
    @param str  EUCJP-MS text.
    @return Converted text (EUCJP-WIN text)
 */
char* eucjpms_to_eucjpwin(const char *str) {
    char *buf1, *buf2;

    buf1 = eucjpms_to_sjis(str);
    buf2 = sjis_to_eucjpwin(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from EUCJP-WIN to EUCJP-MS
    @param str  EUCJP-WIN text.
    @return Converted text (EUCJP-MS text)
 */
char* eucjpwin_to_eucjpms(const char *str) {
    char *buf1, *buf2;

    buf1 = eucjpwin_to_sjis(str);
    buf2 = sjis_to_eucjpms(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from UTF-8 to CP932
    @param str  UTF-8 text
    @return Converted text (CP932 text)
 */
char* utf8_to_sjis(const char *str) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1, a2;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*2;
        buf = new char[length2 + 1];

        // UTF-8 to CP932
        i = 0;
        j = 0;
        while (i < length && j <= length2) {
            a0 = str[i];
            if (isutf8_1(a0) && i+utf8_len(a0)-1 < length) {
                int pos, old_pos, min, max, s;
                unsigned short len1;

                a1 = str[i+1];
                a2 = str[i+2];
                if (a0 == 0xEF && a1 == 0xBB && a2 == 0xBF) {
                    i += 3;
                    continue;
                }

                if (a0 == 0xC2 && a1 == 0xA5) {
                    buf[j++] = 0x5C;
                    i += 2;
                    continue;
                }

                if (a0 == 0xE2 && a1 == 0x80 && a2 == 0xBE) {
                    buf[j++] = 0x7E;
                    i += 3;
                    continue;
                }

                len1 = utf8_len(str[i]);
                if (len1 == 2) {
                    a0 = 0x07 & (str[i] >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else if (len1 == 3) {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                } else {
                    buf[j++] = '?';
                    i += len1;
                    continue;
                }

                if (a0 == 0x30) {
                    if (0x41 <= a1 && a1 <= (unsigned char)0x93) {
                        // hiragana
                        buf[j++] = 0x82;
                        buf[j++] = a1 + ((0x9F)-(0x41));
                        i += 3;
                        continue;
                    } else if ((unsigned char)0xA1 <= a1 && a1 <= (unsigned char)0xDF) {
                        // katakana 1
                        buf[j++] = 0x83;
                        buf[j++] = a1 - ((0xA1)-(0x40));
                        i += 3;
                        continue;
                    } else if ((unsigned char)0xE0 <= a1 && a1 <= (unsigned char)0xF6) {
                        // katakana 2
                        buf[j++] = 0x83;
                        buf[j++] = a1 - ((0xE0)-(0x80));
                        i += 3;
                        continue;
                    }
                }

                // hankana
                if (a0 == (unsigned char)0xFF && 0x60 <= a1 && a1 <= (unsigned char)0x9F) {
                    if (a1 == '\x60') {
                        buf[j++] = 0x20;
                    } else {
                        buf[j++] = a1 + ((0xA1)-(0x61));
                    }
                    i += len1;
                    continue;
                }

                min = index_unicode_sjis[a0];
                if (min < 0) {
                    max = min;
                } else if (a0 != 255) {
                    max = min + 255;
                    for (s = a0+1; s <= 255; s++) {
                        if (0 <= index_unicode_sjis[s]) {
                            max = index_unicode_sjis[s]-1;
                            break;
                        }
                    }
                    if (UNICODE_SJIS_TABLE_MAX-1 < max) {
                        max = UNICODE_SJIS_TABLE_MAX-1;
                    }
                } else {
                    max = UNICODE_SJIS_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (unicode_sjis[pos][0] == a1) {
                            buf[j++] = unicode_sjis[pos][1];
                            buf[j++] = unicode_sjis[pos][2];
                            i += (utf8_len(str[i])-1);
                            break;
                        }

                        old_pos = pos;
                        if (unicode_sjis[old_pos][0] < a1) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);

                if (min < 0 || unicode_sjis[pos][0] != a1) {
                    buf[j++] = '?';
                    i += (len1 - 1);
                }
            } else {
                buf[j++] = (a0 & (unsigned char)0x80)?'?':a0;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-8 to ISO-2022-JP
    @param str  UTF-8 text
    @return Converted text (ISO-2022-JP text)
 */
char* utf8_to_jis(const char *str) {
    char *buf1, *buf2;

    buf1 = utf8_to_sjis(str);
    buf2 = sjis_to_jis(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from CP932 to UTF-8
    @param str  CP932 text
    @return Converted text (UTF-8 text)
 */
char* sjis_to_utf8(const char *str) {
    char *buf;
    long i, j, l, length;
    unsigned char c, d, a0, a1;

    /* SJIS A0-DF */
    const char *hankaku_kana[64] = {
        "\x20",         "\xEF\xBD\xA1", "\xEF\xBD\xA2", "\xEF\xBD\xA3",
        "\xEF\xBD\xA4", "\xEF\xBD\xA5", "\xEF\xBD\xA6", "\xEF\xBD\xA7",
        "\xEF\xBD\xA8", "\xEF\xBD\xA9", "\xEF\xBD\xAA", "\xEF\xBD\xAB",
        "\xEF\xBD\xAC", "\xEF\xBD\xAD", "\xEF\xBD\xAE", "\xEF\xBD\xAF",
        "\xEF\xBD\xB0", "\xEF\xBD\xB1", "\xEF\xBD\xB2", "\xEF\xBD\xB3",
        "\xEF\xBD\xB4", "\xEF\xBD\xB5", "\xEF\xBD\xB6", "\xEF\xBD\xB7",
        "\xEF\xBD\xB8", "\xEF\xBD\xB9", "\xEF\xBD\xBA", "\xEF\xBD\xBB",
        "\xEF\xBD\xBC", "\xEF\xBD\xBD", "\xEF\xBD\xBE", "\xEF\xBD\xBF",
    
        "\xEF\xBE\x80", "\xEF\xBE\x81", "\xEF\xBE\x82", "\xEF\xBE\x83",
        "\xEF\xBE\x84", "\xEF\xBE\x85", "\xEF\xBE\x86", "\xEF\xBE\x87",
        "\xEF\xBE\x88", "\xEF\xBE\x89", "\xEF\xBE\x8A", "\xEF\xBE\x8B",
        "\xEF\xBE\x8C", "\xEF\xBE\x8D", "\xEF\xBE\x8E", "\xEF\xBE\x8F",
    
        "\xEF\xBE\x90", "\xEF\xBE\x91", "\xEF\xBE\x92", "\xEF\xBE\x93",
        "\xEF\xBE\x94", "\xEF\xBE\x95", "\xEF\xBE\x96", "\xEF\xBE\x97",
        "\xEF\xBE\x98", "\xEF\xBE\x99", "\xEF\xBE\x9A", "\xEF\xBE\x9B",
        "\xEF\xBE\x9C", "\xEF\xBE\x9D", "\xEF\xBE\x9E", "\xEF\xBE\x9F"
    };

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // CP932 to UTF-8
        i = 0;
        j = 0;
        while (i < length) {
            c = str[i];
            if ((unsigned char)0xA0 <= c && c <= (unsigned char)0xDF) {
                l = (c == (unsigned char)0xA0)?1:3;
                strncpy(buf+j, hankaku_kana[c-(unsigned char)0xA0], l);
                j += l;
                i++;
                continue;
            }
            d = c?(str[i+1]):0;
            if (issjis1(c) && issjis2(d)) {
                int pos, old_pos, min, max, s;

                if (c == 0x82) {
                    if ((unsigned char)0x9F <= d && d <= (unsigned char)0xDD) {
                        // hiragana 1
                        buf[j++] = 0xE3;
                        buf[j++] = 0x81;
                        buf[j++] = d - ((0x9F)-(0x81));
                        i += 2;
                        continue;
                    } else if ((unsigned char)0xDE <= d && d <= (unsigned char)0xF1) {
                        // hiragana 2
                        buf[j++] = 0xE3;
                        buf[j++] = 0x82;
                        buf[j++] = d - ((0xDE)-(0x80));
                        i += 2;
                        continue;
                    }
                } else if (c == 0x83) {
                    if (0x40 <= d && d <= 0x5E) {
                        // katakana 1
                        buf[j++] = 0xE3;
                        buf[j++] = 0x82;
                        buf[j++] = d + ((0xA1)-(0x40));
                        i += 2;
                        continue;
                    } else if (0x5F <= d && d <= (unsigned char)0x7E) {
                        // katakana 2
                        buf[j++] = 0xE3;
                        buf[j++] = 0x83;
                        buf[j++] = d + ((0x80)-(0x5F));
                        i += 2;
                        continue;
                    } else if ((unsigned char)0x80 <= d && d <= (unsigned char)0x96) {
                        // katakana 3
                        buf[j++] = 0xE3;
                        buf[j++] = 0x83;
                        buf[j++] = d + ((0xA0)-(0x80));
                        i += 2;
                        continue;
                    }
                }

                min = index_sjis_unicode[c];
                if (min < 0) {
                    max = min;
                } else if (c != 255) {
                    max = min + 255;
                    for (s = c+1; s <= 255; s++) {
                        if (0 <= index_sjis_unicode[s]) {
                            max = index_sjis_unicode[s]-1;
                            break;
                        }
                    }
                    if (SJIS_UNICODE_TABLE_MAX-1 < max) {
                        max = SJIS_UNICODE_TABLE_MAX-1;
                    }
                } else {
                    max = SJIS_UNICODE_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (sjis_unicode[pos][0] == d) {
                            break;
                        }

                        old_pos = pos;
                        if (sjis_unicode[old_pos][0] < d) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);

                if (0 <= min && pos != old_pos && min <= max) {
                    a0 = sjis_unicode[pos][1];
                    a1 = sjis_unicode[pos][2];
                    if (a0 == (unsigned char)0x00 && (a1 & (unsigned char)0x80) == (unsigned char)0x00) {
                        buf[j++] = a1;
                    } else if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                    i++;
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = c;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from ISO-2022-JP to UTF-8
    @param str  ISO-2022-JP text
    @return Converted text (UTF-8 text)
 */
char* jis_to_utf8(const char *str) {
    char *buf1, *buf2;

    buf1 = jis_to_sjis(str);
    buf2 = sjis_to_utf8(buf1);

    if (buf1 != NULL) {
        delete [] buf1;
    }
    return buf2;
}


/*! Convert character set from CP932 to ISO-2022-JP
    @param str  CP932 text
    @return Converted text (ISO-2022-JP text)
 */
char* sjis_to_jis(const char *str) {
    char *buf;
    long i, j, jiskanji, length;
    unsigned char a0, a1;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*7 + 100];

        // CP932 to ISO-2022-JP
        jiskanji = 0;
        i = 0;
        j = 0;
        while (i < length) {
            a0 = (unsigned char)str[i];
            if (0x81 <= (unsigned char)str[i] && (unsigned char)str[i] <= 0x9F) {
                // Kanji
                // From division 1 to division 62
                a1 = str[i+1];
                if (a1 <= 0x9E) {
                    // Oddth division
                    a0 = str[i] - 0x80;
                    a0 = (a0 << 1) -1;
                    a1 = (a1 < 0x7F)?(a1-63):(a1-64);
                } else {
                    // Eventh division
                    a0 = ((unsigned char)str[i] - 0x80) << 1;
                    a1 = a1 - 0x9E;
                }
                // to iso_2022_jp
                a0 += 0x20;
                a1 += 0x20;

                if (jiskanji == 0 || jiskanji == 2) {
                    buf[j++] = 0x1B;
                    buf[j++] = 0x24;
                    buf[j++] = 0x42;
                    jiskanji = 1;
                }
                buf[j++] = a0;
                buf[j] = a1;
                i += 2;
            } else if (0xA0 <= (unsigned char)str[i] && (unsigned char)str[i] <= 0xDF) {
                // Half-wdth kana (JIS7)
                a0 = str[i] - 0x80;
                if (jiskanji == 0 || jiskanji == 1) {
                    buf[j++] = 0x1B;
                    buf[j++] = 0x28;
                    buf[j++] = 0x49;
                    jiskanji = 2;
                }
                buf[j] = a0;
                i++;
            } else if (0xE0 <= (unsigned char)str[i] && (unsigned char)str[i] <= 0xF9) {
                // From division 63 to division 94
                a0 = (str[i] - 0xE0) << 1;
                a1 = str[i+1];
                if (a1 <= 0x9E) {
                    // Oddth division
                    a0 += 0x3F;
                    a1 = (a1 < 0x7F)?(a1-63):(a1-64);
                } else {
                    // Eventh division
                    a0 += 0x40;
                    a1 = a1 - 0x9E;
                }
                a0 += 0x20;
                a1 += 0x20;
                if (jiskanji == 0 || jiskanji == 2) {
                    buf[j++] = 0x1B;
                    buf[j++] = 0x24;
                    buf[j++] = 0x42;
                    jiskanji = 1;
                }
                buf[j++] = a0;
                buf[j] = a1;
                i += 2;
            } else if (0xFA <= (unsigned char)str[i] && (unsigned char)str[i] <= 0xFC) {
                // From division 115 to division 119 to CP50220
                unsigned char s1;
                unsigned char s2;
                unsigned short linear;

                s1 = (unsigned char)(str[i]);
                s2 = (unsigned char)(str[i+1]);

                linear = 188 * s1 + (s2 < 0x7f ? s2 - 0x40 : s2 - 0x41);
                if (0xB7B4 <= linear) {
                    linear -= 0xB7B4 - 0xAE0C;
                } else if (0xB7AD <= linear) {
                    linear -= 0xB7AD - 0xAF81;
                } else {
                    linear -= 0xB798 - 0xAF76;
                }

                if (jiskanji == 0 || jiskanji == 2) {
                    buf[j++] = 0x1B;
                    buf[j++] = 0x24;
                    buf[j++] = 0x42;
                    jiskanji = 1;
                }
                buf[j++] = (linear - 0x819E) / 94;
                buf[j] = (linear % 94) + 0x21;
                i += 2;
            } else {
                // Non-Kanji
                if (jiskanji) {
                    buf[j++] = 0x1B;
                    buf[j++] = 0x28;
                    buf[j++] = 0x42;
                    jiskanji = 0;
                }
                buf[j] = str[i];
                i++;
            }
            j++;
        }

        if (jiskanji) {
            buf[j++] = 0x1B;
            buf[j++] = 0x28;
            buf[j++] = 0x42;
        }

        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from ISO-2022-JP to CP932
    @param str  ISO-2022-JP text
    @return Converted text (CP932 text)
 */
char *jis_to_sjis(const char *str) {
    char *buf;
    long i, j, length;
    const char *from;
    enum {IS_ROMAN, IS_KANJI, IS_KANA} shifted;
    int c, normal_flg;
    int hi, lo;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length * 3 + 2048];

        // ISO-2022-JP to CP932
        shifted = IS_ROMAN;
        i = 0;
        j = 0;
        from = str;
        while (from[i] != '\0' && i < length) {
            normal_flg = 0;
            if ((unsigned char)(from[i]) == 0x1B) {
                if (((unsigned char)(from[i+1]) == '$') &&
                        (((unsigned char)(from[i+2])) == 'B' ||
                         ((unsigned char)(from[i+2])) == '@')) {
                    shifted = IS_KANJI;
                    i += 3;
                } else if ((unsigned char)(from[i+1]) == '(' &&
                           (unsigned char)(from[i+2]) == 'I') {
                    shifted = IS_KANA;
                    i += 3;
                } else if ((unsigned char)(from[i+1]) == '(' &&
                           (((unsigned char)(from[i+2])) == 'J' ||
                            ((unsigned char)(from[i+2])) == 'B' ||
                            ((unsigned char)(from[i+2])) == 'H')) {
                    shifted = IS_ROMAN;
                    i += 3;
                } else {             // sequence error
                    normal_flg = 1;
                }
            } else if ((unsigned char)(from[i]) == 0x0E) {
                shifted = IS_KANA;   // to KANA
                i++;
            } else if ((unsigned char)(from[i]) == 0x0F) {
                shifted = IS_ROMAN;  // to ROMAN
                i++;
            } else {
                normal_flg = 1;
            }
            if (normal_flg) {
                switch (shifted) {
                    case IS_KANJI:
                        hi = (int) from[i] & 0xFF;
                        lo = (int) from[i+1] & 0xFF;
                        if (hi & 1) {
                            c = ((hi / 2 + (hi < 0x5F ? 0x71 : 0xB1)) << 8) |
                                (lo + (lo >= 0x60 ? 0x20 : 0x1F));
                        } else {
                            c = ((hi / 2 + (hi < 0x5F ? 0x70 : 0xB0)) << 8) | (lo + 0x7E);
                        }
                        if (0xED40 <= c && c <= 0xEEFC) {
                            if (0xEEEF <= c && c <= 0xEEF8) {
                                c += (0xFA40-0xEEEF);
                            } else if (0xEEFA <= c && c <= 0xEEFC) {
                                c += (0xFA55-0xEEFA);
                            } else if (0xED40 <= c && c <= 0xED43) {
                                c += (0xFA5C-0xED40);
                            } else if (0xED44 <= c && c <= 0xED53) {
                                c += (0xFA60-0xED44);
                            } else if (0xED54 <= c && c <= 0xED62) {
                                c += (0xFA70-0xED54);
                            } else if (0xED63 <= c && c <= 0xED72) {
                                c += (0xFA80-0xED63);
                            } else if (0xED73 <= c && c <= 0xED7E) {
                                c += (0xFA90-0xED73);
                            } else if (0xED80 <= c && c <= 0xED83) {
                                c += (0xFA9C-0xED80);
                            } else if (0xED84 <= c && c <= 0xED93) {
                                c += (0xFAA0-0xED84);
                            } else if (0xED94 <= c && c <= 0xEDA3) {
                                c += (0xFAB0-0xED94);
                            } else if (0xEDA4 <= c && c <= 0xEDB3) {
                                c += (0xFAC0-0xEDA4);
                            } else if (0xEDB4 <= c && c <= 0xEDC3) {
                                c += (0xFAD0-0xEDB4);
                            } else if (0xEDC4 <= c && c <= 0xEDD3) {
                                c += (0xFAE0-0xEDC4);
                            } else if (0xEDD4 <= c && c <= 0xEDE0) {
                                c += (0xFAF0-0xEDD4);
                            } else if (0xEDE1 <= c && c <= 0xEDF0) {
                                c += (0xFB40-0xEDE1);
                            } else if (0xEDF1 <= c && c <= 0xEDFC) {
                                c += (0xFB50-0xEDF1);
                            } else if (0xEE40 <= c && c <= 0xEE43) {
                                c += (0xFB5C-0xEE40);
                            } else if (0xEE44 <= c && c <= 0xEE53) {
                                c += (0xFB60-0xEE44);
                            } else if (0xEE54 <= c && c <= 0xEE62) {
                                c += (0xFB70-0xEE54);
                            } else if (0xEE63 <= c && c <= 0xEE72) {
                                c += (0xFB80-0xEE63);
                            } else if (0xEE73 <= c && c <= 0xEE7E) {
                                c += (0xFB90-0xEE73);
                            } else if (0xEE80 <= c && c <= 0xEE83) {
                                c += (0xFB9C-0xEE80);
                            } else if (0xEE84 <= c && c <= 0xEE93) {
                                c += (0xFBA0-0xEE84);
                            } else if (0xEE94 <= c && c <= 0xEEA3) {
                                c += (0xFBB0-0xEE94);
                            } else if (0xEEA4 <= c && c <= 0xEEB3) {
                                c += (0xFBC0-0xEEA4);
                            } else if (0xEEB4 <= c && c <= 0xEEC3) {
                                c += (0xFBD0-0xEEB4);
                            } else if (0xEEC4 <= c && c <= 0xEED3) {
                                c += (0xFBE0-0xEEC4);
                            } else if (0xEED4 <= c && c <= 0xEEE0) {
                                c += (0xFBF0-0xEED4);
                            } else if (0xEEE1 <= c && c <= 0xEEEC) {
                                c += (0xFC40-0xEEE1);
                            }
                        }
                        buf[j++] = (c >> 8) & 0xFF;
                        buf[j++] = c;
                        i += 2;
                        break;
                    case IS_KANA:
                        buf[j++] = ((int) from[i]) + 0x80;
                        i++;
                        break;
                    case IS_ROMAN:
                    default:
                        buf[j++] = from[i];
                        i++;
                        break;
                }
            }
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Auto detect japanese character set, and convert text to EUCJP-MS
    @param str  text for convertion)
    @return Converted text (EUC-JP text)
 */
char* autojp_to_eucjpms(const char *str) {
    char *buf;
    const char *charset;
    long length;

    if (str == NULL) {
        return NULL;
    }

    charset = jis_auto_detect(str);
    if (charset == (const char *)NULL) {
        charset = "";
    }
    if (!strcasecmp(charset, STR_SJIS)) {
        buf = sjis_to_eucjpms(str);
    } else if (!strcasecmp(charset, STR_JIS)) {
        buf = jis_to_eucjpms(str);
    } else if (!strcasecmp(charset, STR_UTF8)) {
        buf = utf8_to_eucjpms(str);
    } else {
        length = strlen(str);
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
    }

    return buf;
}


/*! Auto detect japanese character set, and convert text to EUCJP-WIN
    @param str  text for convertion)
    @return Converted text (EUC-JP text)
 */
char* autojp_to_eucjpwin(const char *str) {
    char *buf;
    const char *charset;
    long length;

    if (str == NULL) {
        return NULL;
    }

    charset = jis_auto_detect(str);
    if (charset == (const char *)NULL) {
        charset = "";
    }
    if (!strcasecmp(charset, STR_SJIS)) {
        buf = sjis_to_eucjpwin(str);
    } else if (!strcasecmp(charset, STR_JIS)) {
        buf = jis_to_eucjpwin(str);
    } else if (!strcasecmp(charset, STR_UTF8)) {
        buf = utf8_to_eucjpwin(str);
    } else {
        length = strlen(str);
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
    }

    return buf;
}


/*! Auto detect japanese character set, and convert text to CP932.
    @param str  text for convertion
    @return Converted text (CP932 text)
 */
char* autojp_to_sjis(const char *str) {
    char *buf;
    const char *charset;
    long length;

    if (str == NULL) {
        return NULL;
    }

    charset = jis_auto_detect(str);
    if (charset == (const char *)NULL) {
        charset = "";
    }
    if (!strcasecmp(charset, STR_EUCJPMS)) {
        buf = eucjpms_to_sjis(str);
    } else if (!strcasecmp(charset, STR_EUCJP)) {
        buf = eucjpwin_to_sjis(str);
    } else if (!strcasecmp(charset, STR_JIS)) {
        buf = jis_to_sjis(str);
    } else if (!strcasecmp(charset, STR_UTF8)) {
        buf = utf8_to_sjis(str);
    } else {
        length = strlen(str);
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
    }

    return buf;
}


/*! Auto detect japanese character set, and convert text to ISO-2022-JP
    @param str  text for convertion
    @return Converted text (ISO-2022-JP text)
 */
char* autojp_to_jis(const char *str) {
    char *buf;
    const char *charset;
    long length;

    if (str == NULL) {
        return NULL;
    }

    charset = jis_auto_detect(str);
    if (charset == (const char *)NULL) {
        charset = "";
    }
    if (!strcasecmp(charset, STR_EUCJPMS)) {
        buf = eucjpms_to_jis(str);
    } else if (!strcasecmp(charset, STR_EUCJP)) {
        buf = eucjpwin_to_jis(str);
    } else if (!strcasecmp(charset, STR_SJIS)) {
        buf = sjis_to_jis(str);
    } else if (!strcasecmp(charset, STR_UTF8)) {
        buf = utf8_to_jis(str);
    } else {
        length = strlen(str);
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
    }

    return buf;
}


/*! Auto detect japanese character set, and convert text to UTF-8
    @param str  text for convertion
    @return Converted text: UTF-8 text.
 */
char* autojp_to_utf8(const char *str) {
    char *buf;
    const char *charset;
    long length;

    if (str == NULL) {
        return NULL;
    }

    charset = jis_auto_detect(str);
    if (charset == (const char *)NULL) {
        charset = "";
    }
    if (!strcasecmp(charset, STR_EUCJP)) {
        buf = eucjpwin_to_utf8(str);
    } else if (!strcasecmp(charset, STR_SJIS)) {
        buf = sjis_to_utf8(str);
    } else if (!strcasecmp(charset, STR_JIS)) {
        buf = jis_to_utf8(str);
    } else {
        length = strlen(str);
        buf = new char [length + 1];
        memcpy(buf, str, length);
        buf[length] = '\0';
    }

    return buf;
}


/*! Convert character set from CP949 to UTF-8
    @param str  CP949 text.
    @return Converted text (UTF-8 text)
 */
char* euckr_to_utf8(const char* str) {
    char *buf;
    long i, j, length;
    unsigned char c, d, a0, a1;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // CP949 to UTF-8
        i = 0;
        j = 0;
        while (i < length) {
            c = str[i];
            d = str[i+1];
            if ((unsigned char)0x80 <= c) {
                int pos, old_pos, min, max, s;

                min = index_euckr_unicode[c];
                if (min < 0) {
                    max = min;
                } else if (c != 255) {
                    max = min + 255;
                    for (s = c+1; s <= 255; s++) {
                        if (0 <= index_euckr_unicode[s]) {
                            max = index_euckr_unicode[s]-1;
                            break;
                        }
                    }
                    if (UNICODE_EUCKR_TABLE_MAX-1 < max) {
                        max = EUCKR_UNICODE_TABLE_MAX-1;
                    }
                } else {
                    max = EUCKR_UNICODE_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (euckr_unicode[pos][0] == d) {
                            break;
                        }

                        old_pos = pos;
                        if (euckr_unicode[old_pos][0] < d) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);

                if (0 <= min && pos != old_pos && min <= max) {
                    a0 = euckr_unicode[pos][1];
                    a1 = euckr_unicode[pos][2];
                    if (a0 == (unsigned char)0x00 && (a1 & (unsigned char)0x80) == (unsigned char)0x00) {
                        buf[j++] = a1;
                    } else if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                    i++;
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = c;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-8 to CP949
    @param str  UTF-8 text
    @return Converted text (CP949 text)
 */
char* utf8_to_euckr(const char* str) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1, a2;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*2;
        buf = new char[length2 + 1];

        // UTF-8 to CP949
        i = 0;
        j = 0;
        while (i < length && j <= length2) {
            a0 = str[i];
            if (isutf8_1(a0) && i+utf8_len(a0)-1 < length) {
                int pos, old_pos, min, max, s;
                unsigned short len1;

                a1 = str[i+1];
                a2 = str[i+2];
                if (a0 == 0xEF && a1 == 0xBB && a2 == 0xBF) {
                    i += 3;
                    continue;
                }

                if (a0 == 0xE2 && a1 == 0x80 && a2 == 0xBE) {
                    buf[j++] = 0x7E;
                    i += 3;
                    continue;
                }

                len1 = utf8_len(str[i]);
                if (len1 == 2) {
                    a0 = 0x07 & (str[i] >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else if (len1 == 3) {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                } else {
                    buf[j++] = '?';
                    i += len1;
                    continue;
                }

                min = index_unicode_euckr[a0];
                if (min < 0) {
                    max = min;
                } else if (a0 != 255) {
                    max = min + 255;
                    for (s = a0+1; s <= 255; s++) {
                        if (0 <= index_unicode_euckr[s]) {
                            max = index_unicode_euckr[s]-1;
                            break;
                        }
                    }
                    if (UNICODE_EUCKR_TABLE_MAX-1 < max) {
                        max = UNICODE_EUCKR_TABLE_MAX-1;
                    }
                } else {
                    max = UNICODE_EUCKR_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (unicode_euckr[pos][0] == a1) {
                            buf[j++] = unicode_euckr[pos][1];
                            buf[j++] = unicode_euckr[pos][2];
                            i += (utf8_len(str[i])-1);
                            break;
                        }

                        old_pos = pos;
                        if (unicode_euckr[old_pos][0] < a1) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);

                if (min < 0 || unicode_euckr[pos][0] != a1) {
                    buf[j++] = '?';
                    i += (len1 - 1);
                }
            } else {
                buf[j++] = (a0 & (unsigned char)0x80)?'?':a0;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


static inline char *_utf8_char_to_gb18030(const char *utf8_char, char *gb18030_buf) {
    int x, length, count;
    int xx, xy, o;
    unsigned long ucs4_base, ucs4_find;
    unsigned char c;
    const unsigned char *p;

    if (gb18030_buf == NULL) return NULL;
    gb18030_buf[0] = '\0';

    length = utf8_len(utf8_char[0]);
    if (length < 2) return NULL;
    if (1 <= length && utf8_char[0] == '\0') return NULL;
    if (2 <= length && utf8_char[1] == '\0') return NULL;
    if (3 <= length && utf8_char[2] == '\0') return NULL;
    if (4 <= length && utf8_char[3] == '\0') return NULL;
    if (5 <= length) return NULL;

    if (length == 2) {
        if (0 < str2cmp(utf8_gb18030[UTF8_GB18030_2BYTE_START].utf8, utf8_char)) {
            return NULL;
        }
        xx = UTF8_GB18030_2BYTE_START;
        xy = UTF8_GB18030_2BYTE_END + 1;
        o = xx + (xy - xx)/2;
        while (xx < o && o < xy) {
            x = str2cmp(utf8_gb18030[o].utf8, utf8_char);
            if (x < 0) {
                xx = o;
                o = o + (xy - xx)/2;
            } else if (0 < x) {
                xy = o;
                o = o - (xy - xx)/2;
            } else {
                break;
            }
        }
        x = str2cmp(utf8_gb18030[o].utf8, utf8_char);
        if (0 < x && 0 < o) o--;
    } else if (length == 3) {
        if (0 < str3cmp(utf8_gb18030[UTF8_GB18030_3BYTE_START].utf8, utf8_char)) {
            return NULL;
        }
        xx = UTF8_GB18030_3BYTE_START;
        xy = UTF8_GB18030_3BYTE_END + 1;
        o = xx + (xy - xx)/2;
        while (xx < o && o < xy) {
            x = str3cmp(utf8_gb18030[o].utf8, utf8_char);
            if (x < 0) {
                xx = o;
                o = o + (xy - xx)/2;
            } else if (0 < x) {
                xy = o;
                o = o - (xy - xx)/2;
            } else {
                break;
            }
        }
        x = str3cmp(utf8_gb18030[o].utf8, utf8_char);
        if (0 < x && 0 < o) o--;
    } else {
        if (0 < str4cmp(utf8_gb18030[UTF8_GB18030_4BYTE_START].utf8, utf8_char)) {
            return NULL;
        }
        xx = UTF8_GB18030_4BYTE_START;
        xy = UTF8_GB18030_4BYTE_END + 1;
        o = xx + (xy - xx)/2;
        while (xx < o && o < xy) {
            x = str4cmp(utf8_gb18030[o].utf8, utf8_char);
            if (x < 0) {
                xx = o;
                o = o + (xy - xx)/2;
            } else if (0 < x) {
                xy = o;
                o = o - (xy - xx)/2;
            } else {
                break;
            }
        }
        x = str4cmp(utf8_gb18030[o].utf8, utf8_char);
        if (0 < x && 0 < o) o--;
    }

    length = utf8_len(utf8_gb18030[o].utf8[0]);
    if (length < 2) {
        return NULL;
    }

    // find UTF-8
    p = (const unsigned char *)utf8_gb18030[o].utf8;
    c = p[0];
    switch (utf8_len(c)) {
        case 1:
            ucs4_base = (unsigned long)c;
            break;
        case 2:
            ucs4_base = c & 0x1F;
            ucs4_base = (ucs4_base << 6) + (p[1] & 0x3F);
            break;
        case 3:
            ucs4_base = c & 0x0F;
            ucs4_base = (ucs4_base << 6) + (p[1] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[2] & 0x3F);
            break;
        case 4:
            ucs4_base = c & 7;
            ucs4_base = (ucs4_base << 6) + (p[1] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[2] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[3] & 0x3F);
            break;
        case 5:
            ucs4_base = c & 3;
            ucs4_base = (ucs4_base << 6) + (p[1] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[2] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[3] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[4] & 0x3F);
            break;
        case 6:
            ucs4_base = c & 1;
            ucs4_base = (ucs4_base << 6) + (p[1] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[2] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[3] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[4] & 0x3F);
            ucs4_base = (ucs4_base << 6) + (p[5] & 0x3F);
            break;
        default:
            return NULL;
            break;
    }

    c = (unsigned char)utf8_char[0];
    switch (utf8_len(c)) {
        case 1:
            ucs4_find = (unsigned long)c;
            break;
        case 2:
            ucs4_find = c & 0x1F;
            ucs4_find = (ucs4_find << 6) + (utf8_char[1] & 0x3F);
            break;
        case 3:
            ucs4_find = c & 0x0F;
            ucs4_find = (ucs4_find << 6) + (utf8_char[1] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[2] & 0x3F);
            break;
        case 4:
            ucs4_find = c & 7;
            ucs4_find = (ucs4_find << 6) + (utf8_char[1] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[2] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[3] & 0x3F);
            break;
        case 5:
            ucs4_find = c & 3;
            ucs4_find = (ucs4_find << 6) + (utf8_char[1] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[2] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[3] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[4] & 0x3F);
            break;
        case 6:
            ucs4_find = c & 1;
            ucs4_find = (ucs4_find << 6) + (utf8_char[1] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[2] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[3] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[4] & 0x3F);
            ucs4_find = (ucs4_find << 6) + (utf8_char[5] & 0x3F);
            break;
        default:
            return NULL;
            break;
    }

    count = ucs4_find - ucs4_base;
    if (utf8_gb18030[o].count <= count) return NULL;

    if (count == 0) {
        strcpy(gb18030_buf, utf8_gb18030[o].gb18030);
        return gb18030_buf;
    } else if (0 < count) {
        const unsigned char *gb18030_char = (const unsigned char *)utf8_gb18030[o].gb18030;
        unsigned char *buf = (unsigned char *)gb18030_buf;
        unsigned long n;

        n = gb18030_char[3] - 0x30;
        n += (gb18030_char[2] - 0x81) * (0x39 - 0x30 + 1);
        n += (gb18030_char[1] - 0x30) * (0x39 - 0x30 + 1) * (0xFE - 0x81 + 1);
        n += (gb18030_char[0] - 0x81) * (0x39 - 0x30 + 1) * (0xFE - 0x81 + 1) * (0x39 - 0x30 + 1);

        n += count;

        buf[4] = '\0';
        buf[3] = (n % (0x39 - 0x30 + 1)) + 0x30;
        n = n / (0x39 - 0x30 + 1);
        buf[2] = (n % (0xFE - 0x81 + 1)) + 0x81;
        n = n / (0xFE - 0x81 + 1);
        buf[1] = (n % (0x39 - 0x30 + 1)) + 0x30;
        n = n / (0x39 - 0x30 + 1);
        buf[0] = (n % (0xFE - 0x81 + 1)) + 0x81;

        return gb18030_buf;
    } else {
        return NULL;
    }

    return NULL;
}


static inline char *_gb18030_char_to_utf8(const char *gb18030_char, char *utf8_buf) {
    int x, count;
    int xx, xy, o;
    unsigned long l, m;
    const unsigned char *gb18030, *p;

    if (utf8_buf == NULL) return NULL;
    utf8_buf[0] = '\0';

    if ((unsigned char)0x90 <= (unsigned char)gb18030_char[0]) {
        xx = GB18030_UTF8_TABLE_90;
        xy = GB18030_UTF8_TABLE_MAX-1;
        o = xx + (xy - xx)/2;
    } else {
        xx = 0;
        xy = GB18030_UTF8_TABLE_90;
        o = xy / 2;
    }
    while (xx < o && o < xy) {
        x = str4cmp(gb18030_utf8[o].gb18030, gb18030_char);
        if (x < 0) {
            xx = o;
            o = o + (xy - xx)/2;
        } else if (0 < x) {
            xy = o;
            o = o - (xy - xx)/2;
        } else {
            break;
        }
    }

    x = str4cmp(gb18030_utf8[o].gb18030, gb18030_char);
    if (0 < x && 0 < o) o--;

    // find gb18030
    gb18030 = (const unsigned char *)gb18030_utf8[o].gb18030;
    p = (const unsigned char *)gb18030_char;

    l = gb18030[3] - 0x30;
    l += (gb18030[2] - 0x81) * (0x39 - 0x30 + 1);
    l += (gb18030[1] - 0x30) * (0x39 - 0x30 + 1) * (0xFE - 0x81 + 1);
    l += (gb18030[0] - 0x81) * (0x39 - 0x30 + 1) * (0xFE - 0x81 + 1) * (0x39 - 0x30 + 1);

    m = p[3] - 0x30;
    m += (p[2] - 0x81) * (0x39 - 0x30 + 1);
    m += (p[1] - 0x30) * (0x39 - 0x30 + 1) * (0xFE - 0x81 + 1);
    m += (p[0] - 0x81) * (0x39 - 0x30 + 1) * (0xFE - 0x81 + 1) * (0x39 - 0x30 + 1);

    if (gb18030_utf8[o].count <= m - l) {
        count = -1;
    } else {
        count = m - l;
    }

    if (0 == count) {
        strcpy(utf8_buf, gb18030_utf8[o].utf8);
        return utf8_buf;
    } else if (0 < count) {
        const unsigned char *utf8_char = (const unsigned char *)gb18030_utf8[o].utf8;
        unsigned char *buf = (unsigned char *)utf8_buf; // to be unsigned
        int length;
        unsigned char c;
        unsigned long ucs4;

        strncpy((char *)buf, (const char *)utf8_char, 7);
        c = utf8_char[0];
        length = utf8_len(c);
        buf[length] = '\0';
        if (length < 2) {
            return utf8_buf;
        }

        // UTF-8 to UCS4
        switch (length) {
            case 1:
                ucs4 = (unsigned long)c;
                break;
            case 2:
                ucs4 = c & 0x1F;
                ucs4 = (ucs4 << 6) + (utf8_char[1] & 0x3F);
                break;
            case 3:
                ucs4 = c & 0x0F;
                ucs4 = (ucs4 << 6) + (utf8_char[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[2] & 0x3F);
                break;
            case 4:
                ucs4 = c & 7;
                ucs4 = (ucs4 << 6) + (utf8_char[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[3] & 0x3F);
                break;
            case 5:
                ucs4 = c & 3;
                ucs4 = (ucs4 << 6) + (utf8_char[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[3] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[4] & 0x3F);
                break;
            case 6:
                ucs4 = c & 1;
                ucs4 = (ucs4 << 6) + (utf8_char[1] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[2] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[3] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[4] & 0x3F);
                ucs4 = (ucs4 << 6) + (utf8_char[5] & 0x3F);
                break;
            default:
                ucs4 = 0;
                break;
        }

        ucs4 += count;

        // UCS4 to UTF-8
        if (ucs4 <= 0x7FUL) {
            buf[0] = (unsigned char)ucs4;
            buf[1] = '\0';
        } else if (ucs4 <= 0x7FFUL) {
            buf[0] = 0xC0 | (ucs4 >> 6);
            buf[1] = 0x80 | (ucs4 & 0x3F);
            buf[2] = '\0';
        } else if (ucs4 <= 0xFFFFUL) {
            buf[0] = 0xE0 | (ucs4 >> 12);
            buf[1] = 0x80 | ((ucs4 >> 6) & 0x3F);
            buf[2] = 0x80 | (ucs4 & 0x3F);
            buf[3] = '\0';
        } else {
            buf[0] = 0xF0 | (ucs4 >> 18);
            buf[1] = 0x80 | ((ucs4 >> 12) & 0x3F);
            buf[2] = 0x80 | ((ucs4 >> 6) & 0x3F);
            buf[3] = 0x80 | (ucs4 & 0x3F);
            buf[4] = '\0';
        }

        return utf8_buf;
    } else {
        return NULL;
    }

    return NULL;
}


/*! Convert character set from GBK to UTF-8
    @param str  GBK text
    @return Converted text (UTF-8 text)
 */
char* gbk_to_utf8(const char* str) {
    char *buf;
    char buf2[10], *p;
    long i, j, k, length;
    unsigned char c, d, a0, a1;
    int pos;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // gbk to UTF-8
        i = 0;
        j = 0;
        while (i < length) {
            // 4byte GB18030
            if (0x81 <= (const unsigned char)str[i] &&
                    (const unsigned char)str[i] <= 0xFE &&
                    0x30 <= (const unsigned char)str[i+1] &&
                    (const unsigned char)str[i+1] <= 0x39 &&
                    0x81 <= (const unsigned char)str[i+2] &&
                    (const unsigned char)str[i+2] <= 0xFE &&
                    0x30 <= (const unsigned char)str[i+3] &&
                    (const unsigned char)str[i+3] <= 0x39) {
                p = _gb18030_char_to_utf8(str+i, buf2);
                if (p != NULL) {
                    k = strlen(p);
                    strncpy(buf+j, p, k);
                    j += k;
                } else {
                    buf[j++] = '?';
                    buf[j++] = '?';
                    buf[j++] = '?';
                    buf[j++] = '?';
                }
                i += 4;
                continue;
            }
            c = str[i];
            d = str[i+1];
            if ((unsigned char)0x80 <= c) {
                pos = index_gbk_unicode[c];
                if (0 <= pos) {
                    pos += d;
                    a0 = gbk_unicode[pos][0];
                    a1 = gbk_unicode[pos][1];
                    if (!(a0 == (unsigned char)0x00 && a1 == (unsigned char)0x00)) {
                        if (a0 == (unsigned char)0x00 && (a1 & (unsigned char)0x80) == (unsigned char)0x00) {
                            buf[j++] = a1;
                        } else if (a0 <= (unsigned char)0x07) {
                            buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                            buf[j++] = 0x80 | (0x3F & a1);
                        } else {
                            buf[j++] = 0xE0 | (a0 >> 4);
                            buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                            buf[j++] = 0x80 | (0x3F & a1);
                        }
                        i++;
                    } else {
                        buf[j++] = '?';
                    }
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = c;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-8 to GBK
    @param str  UTF-8 text
    @return Converted text (GBK text)
 */
char* utf8_to_gbk(const char* str) {
    char *buf, buf2[10];
    long i, j, length, length3;
    unsigned char c, d, a0, a1, a2;
    int a0len, pos;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length3 = length*3;
        buf = new char[length3 + 1];

        // UTF-8 to gbk
        i = 0;
        j = 0;
        while (i < length && j <= length3) {
            a0 = str[i];
            a0len = utf8_len(a0);
            if (a0len == 1) {
                buf[j++] = a0;
                i++;
                continue;
            }
            if (_utf8_char_to_gb18030(str+i, buf2) != NULL) {
                strncpy(buf+j, buf2, 4);
                j += 4;
                i += a0len;
                continue;
            }
            if (isutf8_1(a0) && i+a0len-1 < length) {
                a1 = str[i+1];
                a2 = str[i+2];
                if (a0 == 0xEF && a1 == 0xBB && a2 == 0xBF) {
                    i += 3;
                    continue;
                }

                if (a0 == 0xE2 && a1 == 0x80 && a2 == 0xBE) {
                    buf[j++] = 0x7E;
                    i += 3;
                    continue;
                }

                if (a0len == 2) {
                    a0 = 0x07 & (a0 >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else if (a0len == 3) {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                } else {
                    buf[j++] = '?';
                    i += a0len;
                    continue;
                }

                pos = index_unicode_gbk[a0];
                if (0 <= pos) pos += a1;
                if (0 <= pos && (c = unicode_gbk[pos][0]) != 0 && (d = unicode_gbk[pos][1]) != 0) {
                    buf[j++] = c;
                    buf[j++] = d;
                    i += (a0len - 1);
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = (a0 & (unsigned char)0x80)?'?':a0;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from BIG5 to UTF-8
    @param str  BIG5 text.
    @return Converted text (UTF-8 text)
 */
char* big5_to_utf8(const char* str) {
    char *buf;
    long i, j, length;
    unsigned char c, d, a0, a1;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        buf = new char[length*3 + 1];

        // BIG5 to UTF-8
        i = 0;
        j = 0;
        while (i < length) {
            c = str[i];
            d = str[i+1];
            if ((unsigned char)0x80 <= c) {
                int pos, old_pos, min, max, s;

                min = index_big5_unicode[c];
                if (min < 0) {
                    max = min;
                } else if (c != 255) {
                    max = min + 255;
                    for (s = c+1; s <= 255; s++) {
                        if (0 <= index_big5_unicode[s]) {
                            max = index_big5_unicode[s]-1;
                            break;
                        }
                    }
                    if (UNICODE_BIG5_TABLE_MAX-1 < max) {
                        max = BIG5_UNICODE_TABLE_MAX-1;
                    }
                } else {
                    max = BIG5_UNICODE_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (big5_unicode[pos][0] == d) {
                            break;
                        }

                        old_pos = pos;
                        if (big5_unicode[old_pos][0] < d) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);

                if (0 <= min && pos != old_pos && min <= max) {
                    a0 = big5_unicode[pos][1];
                    a1 = big5_unicode[pos][2];
                    if (a0 == (unsigned char)0x00 && (a1 & (unsigned char)0x80) == (unsigned char)0x00) {
                        buf[j++] = a1;
                    } else if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                    i++;
                } else {
                    buf[j++] = '?';
                }
            } else {
                buf[j++] = c;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-8 to BIG5
    @param str  UTF-8 text
    @return Converted text (BIG5 text)
 */
char* utf8_to_big5(const char* str) {
    char *buf;
    long i, j, length, length2;
    unsigned char a0, a1, a2;

    buf = NULL;

    if (str != NULL) {
        // Memory allocation
        length = strlen(str);
        length2 = length*2;
        buf = new char[length2 + 1];

        // UTF-8 to BIG5
        i = 0;
        j = 0;
        while (i < length && j <= length2) {
            a0 = str[i];
            if (isutf8_1(a0) && i+utf8_len(a0)-1 < length) {
                int pos, old_pos, min, max, s;
                unsigned short len1;

                a1 = str[i+1];
                a2 = str[i+2];
                if (a0 == 0xEF && a1 == 0xBB && a2 == 0xBF) {
                    i += 3;
                    continue;
                }

                if (a0 == 0xE2 && a1 == 0x80 && a2 == 0xBE) {
                    buf[j++] = 0x7E;
                    i += 3;
                    continue;
                }

                len1 = utf8_len(str[i]);
                if (len1 == 2) {
                    a0 = 0x07 & (str[i] >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else if (len1 == 3) {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                } else {
                    buf[j++] = '?';
                    i += len1;
                    continue;
                }

                min = index_unicode_big5[a0];
                if (min < 0) {
                    max = min;
                } else if (a0 != 255) {
                    max = min + 255;
                    for (s = a0+1; s <= 255; s++) {
                        if (0 <= index_unicode_big5[s]) {
                            max = index_unicode_big5[s]-1;
                            break;
                        }
                    }
                    if (UNICODE_BIG5_TABLE_MAX-1 < max) {
                        max = UNICODE_BIG5_TABLE_MAX-1;
                    }
                } else {
                    max = UNICODE_BIG5_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (unicode_big5[pos][0] == a1) {
                            buf[j++] = unicode_big5[pos][1];
                            buf[j++] = unicode_big5[pos][2];
                            i += (utf8_len(str[i])-1);
                            break;
                        }

                        old_pos = pos;
                        if (unicode_big5[old_pos][0] < a1) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);

                if (min < 0 || unicode_big5[pos][0] != a1) {
                    buf[j++] = '?';
                    i += (len1 - 1);
                }
            } else {
                buf[j++] = (a0 & (unsigned char)0x80)?'?':a0;
            }
            i++;
        }
        buf[j] = '\0';
    }

    return buf;
}


/*! Convert character set from UTF-7 to UTF-8
    @param str  UTF-7 text
    @return Converted text (UTF-8 text)
 */
char* utf7_to_utf8(const char* str) {
    unsigned char c, i, bitcount;
    unsigned long ucs4, utf16, bitbuf;
    unsigned char base64[256] = {
        //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 0
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 1
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 63, 64, 64, 63, // 2
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64, // 3
        64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, // 4
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, // 5
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 6
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, // 7
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 8
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 9
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // A
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // B
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // C
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // D
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // E
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64  // F
    };
    unsigned char utf8[7] = {0,};
    const char hex[] = "0123456789ABCDEF"; // hexadecimal lookup table
    const int undefined_num = 64;
    const unsigned long UTF16SHIFT     = 10;
    const unsigned long UTF16BASE      = 0x10000UL;
    const unsigned long UTF16HIGHSTART = 0xD800UL;
    const unsigned long UTF16HIGHEND   = 0xDBFFUL;
    const unsigned long UTF16LOSTART   = 0xDC00UL;
    const unsigned long UTF16LOEND   = 0xDFFFUL;
    const int malloc_base_size = 4096;
    int str_malloced_size;
    char *utf8_str;
    int col;

    if (str == NULL) {
        return NULL;
    }

    str_malloced_size = malloc_base_size;
    utf8_str = new char [str_malloced_size];
    col = 0;
    utf8_str[col] = '\0';

    // loop until end of string
    while (*str != '\0') {
        c = *str++;
        // deal with literal characters and &-, +-
        if (c != '+' || *str == '-') {
            if (c < ' ' || c > '~') {
                // hex encode if necessary
                if (str_malloced_size - col - 1 < 3) {
                    char *tmp_utf8_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf8_str = new char [str_malloced_size];
                    memcpy(tmp_utf8_str, utf8_str, col);
                    delete [] utf8_str;
                    utf8_str = tmp_utf8_str;
                }
                if (c != '\r' && c != '\n') {
                    utf8_str[col] = '%';
                    utf8_str[col+1] = hex[c >> 4];
                    utf8_str[col+2] = hex[c & 0x0f];
                    col += 3;
                } else {
                    utf8_str[col] = (char)c;
                    col++;
                }
            } else {
                // encode literally
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_utf8_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf8_str = new char [str_malloced_size];
                    memcpy(tmp_utf8_str, utf8_str, col);
                    delete [] utf8_str;
                    utf8_str = tmp_utf8_str;
                }
                utf8_str[col] = c;
                col++;
            }
        } else {
            // convert modified UTF-7 -> UTF-16 -> UCS-4 -> UTF-8 -> HEX
            bitbuf = 0;
            bitcount = 0;
            ucs4 = 0;
            while ((c = base64[(unsigned char) *str]) != undefined_num) {
                str++;
                bitbuf = (bitbuf << 6) | c;
                bitcount += 6;
                // enough bits for a UTF-16 character?
                if (bitcount >= 16) {
                    bitcount -= 16;
                    utf16 = (bitcount ? bitbuf >> bitcount
                             : bitbuf) & 0xFFFF;
                    // convert UTF16 to UCS4
                    if
                    (utf16 >= UTF16HIGHSTART && utf16 <= UTF16HIGHEND) {
                        ucs4 = (utf16 - UTF16HIGHSTART) << UTF16SHIFT;
                        continue;
                    } else if
                    (utf16 >= UTF16LOSTART && utf16 <= UTF16LOEND) {
                        ucs4 += utf16 - UTF16LOSTART + UTF16BASE;
                    } else {
                        ucs4 = utf16;
                    }
                    // convert UTF-16 range of UCS4 to UTF-8
                    if (ucs4 <= 0x7FUL) {
                        utf8[0] = ucs4;
                        i = 1;
                    } else if (ucs4 <= 0x7FFUL) {
                        utf8[0] = 0xC0 | (ucs4 >> 6);
                        utf8[1] = 0x80 | (ucs4 & 0x3F);
                        i = 2;
                    } else if (ucs4 <= 0xFFFFUL) {
                        utf8[0] = 0xE0 | (ucs4 >> 12);
                        utf8[1] = 0x80 | ((ucs4 >> 6) & 0x3F);
                        utf8[2] = 0x80 | (ucs4 & 0x3F);
                        i = 3;
                    } else {
                        utf8[0] = 0xF0 | (ucs4 >> 18);
                        utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3F);
                        utf8[2] = 0x80 | ((ucs4 >> 6) & 0x3F);
                        utf8[3] = 0x80 | (ucs4 & 0x3F);
                        i = 4;
                    }

                    for (c = 0; c < i; c++) {
                        if (str_malloced_size - col - 1 < 1) {
                            char *tmp_utf8_str;
                            str_malloced_size += malloc_base_size;
                            tmp_utf8_str = new char [str_malloced_size];
                            memcpy(tmp_utf8_str, utf8_str, col);
                            delete [] utf8_str;
                            utf8_str = tmp_utf8_str;
                        }
                        utf8_str[col] = utf8[c];
                        col++;
                    }
                }
            }
            // skip over trailing '-' in modified UTF-7 encoding
            if (*str == '-') str++;
        }
    }

    utf8_str[col] = '\0';
    return utf8_str;
}


/*! Convert character set from UTF-8 to UTF-7
    @param str  UTF-8 text
    @return Converted text (UTF-7 text)
 */
char* utf8_to_utf7(const char* str) {
    unsigned int utf8pos, utf8total, c, utf7mode, bitstogo, utf16flag;
    unsigned long ucs4, bitbuf;
    const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // base64
    const unsigned long UTF16MASK      = 0x03FFUL;
    const unsigned long UTF16SHIFT     = 10;
    const unsigned long UTF16BASE      = 0x10000UL;
    const unsigned long UTF16HIGHSTART = 0xD800UL;
    const unsigned long UTF16LOSTART   = 0xDC00UL;
    const int malloc_base_size = 4096;
    int str_malloced_size;
    char *utf7_str;
    int col;

    if (str == NULL) {
        return NULL;
    }

    utf8pos = 0;
    ucs4 = 0;
    bitbuf = 0;

    str_malloced_size = malloc_base_size;
    utf7_str = new char [str_malloced_size];
    col = 0;
    utf7_str[col] = '\0';

    utf7mode = 0;
    utf8total = 0;
    bitstogo = 0;
    while ((c = (unsigned char)*str) != '\0') {
        str++;
        // normal character?
        if (isalnum(c) || isspace(c) || c == '(' || c == ')' || c == '/' || c == ',') {
            // switch out of UTF-7 mode
            if (utf7mode) {
                if (bitstogo) {
                    if (str_malloced_size - col - 1 < 1) {
                        char *tmp_utf7_str;
                        str_malloced_size += malloc_base_size;
                        tmp_utf7_str = new char [str_malloced_size];
                        memcpy(tmp_utf7_str, utf7_str, col);
                        delete [] utf7_str;
                        utf7_str = tmp_utf7_str;
                    }
                    utf7_str[col] = base64chars[(bitbuf << (6 - bitstogo)) & 0x3F];
                    col++;
                }
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_utf7_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf7_str = new char [str_malloced_size];
                    memcpy(tmp_utf7_str, utf7_str, col);
                    delete [] utf7_str;
                    utf7_str = tmp_utf7_str;
                }
                utf7_str[col] = '-';
                col++;
                utf7mode = 0;
            }
            if (str_malloced_size - col - 1 < 1) {
                char *tmp_utf7_str;
                str_malloced_size += malloc_base_size;
                tmp_utf7_str = new char [str_malloced_size];
                memcpy(tmp_utf7_str, utf7_str, col);
                delete [] utf7_str;
                utf7_str = tmp_utf7_str;
            }
            utf7_str[col] = c;
            col++;
            // encode '+' as '+-'
            if (c == '+') {
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_utf7_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf7_str = new char [str_malloced_size];
                    memcpy(tmp_utf7_str, utf7_str, col);
                    delete [] utf7_str;
                    utf7_str = tmp_utf7_str;
                }
                utf7_str[col] = '-';
                col++;
            }
            continue;
        }
        // switch to UTF-7 mode
        if (!utf7mode) {
            if (str_malloced_size - col - 1 < 1) {
                char *tmp_utf7_str;
                str_malloced_size += malloc_base_size;
                tmp_utf7_str = new char [str_malloced_size];
                memcpy(tmp_utf7_str, utf7_str, col);
                delete [] utf7_str;
                utf7_str = tmp_utf7_str;
            }
            utf7_str[col] = '+';
            col++;
            utf7mode = 1;
            bitbuf = 0;
            bitstogo = 0;
        }
        // Encode US-ASCII characters as themselves
        if (c < 0x80) {
            ucs4 = c;
            utf8total = 1;
        } else if (utf8total) {
            // save UTF8 bits into UCS4
            ucs4 = (ucs4 << 6) | (c & 0x3FUL);
            if (++utf8pos < utf8total) {
                continue;
            }
        } else {
            utf8pos = 1;
            if (c < 0xE0) {
                utf8total = 2;
                ucs4 = c & 0x1F;
            } else if (c < 0xF0) {
                utf8total = 3;
                ucs4 = c & 0x0F;
            } else {
                // NOTE: can't convert UTF8 sequences longer than 4
                utf8total = 4;
                ucs4 = c & 0x03;
            }
            continue;
        }
        // loop to split ucs4 into two utf16 chars if necessary
        utf8total = 0;
        do {
            if (ucs4 >= UTF16BASE) {
                ucs4 -= UTF16BASE;
                bitbuf = (((bitbuf << 16) | ((ucs4 >> UTF16SHIFT))) + UTF16HIGHSTART);
                ucs4 = (ucs4 & UTF16MASK) + UTF16LOSTART;
                utf16flag = 1;
            } else {
                bitbuf = (bitbuf << 16) | ucs4;
                utf16flag = 0;
            }
            bitstogo += 16;
            // spew out base64
            while (bitstogo >= 6) {
                bitstogo -= 6;
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_utf7_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf7_str = new char [str_malloced_size];
                    memcpy(tmp_utf7_str, utf7_str, col);
                    delete [] utf7_str;
                    utf7_str = tmp_utf7_str;
                }
                utf7_str[col] = base64chars[(bitstogo ? (bitbuf >> bitstogo) : bitbuf) & 0x3F];
                col++;
            }
        } while (utf16flag);
    }

    // if in UTF-7 mode, finish in ASCII
    if (utf7mode) {
        if (bitstogo) {
            if (str_malloced_size - col - 1 < 1) {
                char *tmp_utf7_str;
                str_malloced_size += malloc_base_size;
                tmp_utf7_str = new char [str_malloced_size];
                memcpy(tmp_utf7_str, utf7_str, col);
                delete [] utf7_str;
                utf7_str = tmp_utf7_str;
            }
            utf7_str[col] = base64chars[(bitbuf << (6 - bitstogo)) & 0x3F];
            col++;
        }
        if (str_malloced_size - col - 1 < 1) {
            char *tmp_utf7_str;
            str_malloced_size += malloc_base_size;
            tmp_utf7_str = new char [str_malloced_size];
            memcpy(tmp_utf7_str, utf7_str, col);
            delete [] utf7_str;
            utf7_str = tmp_utf7_str;
        }
        utf7_str[col] = '-';
        col++;
    }

    utf7_str[col] = '\0';
    return utf7_str;
}


/*! Convert character set from IMAP4 modified UTF-7 to UTF-8
    @param str  IMAP4 modified UTF-7 text
    @return Converted text (UTF-8 text)
 */
char* modutf7_to_utf8(const char* str) {
    unsigned char c, i, bitcount;
    unsigned long ucs4, utf16, bitbuf;
    unsigned char base64[256] = {
        //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 0
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 1
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 63, 64, 64, 63, // 2
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64, // 3
        64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, // 4
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, // 5
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 6
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, // 7
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 8
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 9
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // A
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // B
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // C
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // D
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // E
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64  // F
    };
    unsigned char utf8[7] = {0,};
    const char hex[] = "0123456789ABCDEF"; // hexadecimal lookup table
    const int undefined_num = 64;
    const unsigned long UTF16SHIFT     = 10;
    const unsigned long UTF16BASE      = 0x10000UL;
    const unsigned long UTF16HIGHSTART = 0xD800UL;
    const unsigned long UTF16HIGHEND   = 0xDBFFUL;
    const unsigned long UTF16LOSTART   = 0xDC00UL;
    const unsigned long UTF16LOEND   = 0xDFFFUL;
    const int malloc_base_size = 4096;
    int str_malloced_size;
    char *utf8_str;
    int col;

    if (str == NULL) {
        return NULL;
    }

    str_malloced_size = malloc_base_size;
    utf8_str = new char [str_malloced_size];
    col = 0;
    utf8_str[col] = '\0';

    // loop until end of string
    while (*str != '\0') {
        c = *str++;
        // deal with literal characters and &-, +-
        if (c != '&' || *str == '-') {
            if (c < ' ' || c > '~') {
                // hex encode if necessary
                if (str_malloced_size - col - 1 < 3) {
                    char *tmp_utf8_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf8_str = new char [str_malloced_size];
                    memcpy(tmp_utf8_str, utf8_str, col);
                    delete [] utf8_str;
                    utf8_str = tmp_utf8_str;
                }
                if (c != '\r' && c != '\n') {
                    utf8_str[col] = '%';
                    utf8_str[col+1] = hex[c >> 4];
                    utf8_str[col+2] = hex[c & 0x0f];
                    col += 3;
                } else {
                    utf8_str[col] = (char)c;
                    col++;
                }
            } else {
                // encode literally
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_utf8_str;
                    str_malloced_size += malloc_base_size;
                    tmp_utf8_str = new char [str_malloced_size];
                    memcpy(tmp_utf8_str, utf8_str, col);
                    delete [] utf8_str;
                    utf8_str = tmp_utf8_str;
                }
                utf8_str[col] = c;
                col++;
            }
        } else {
            // convert modified UTF-7 -> UTF-16 -> UCS-4 -> UTF-8 -> HEX
            bitbuf = 0;
            bitcount = 0;
            ucs4 = 0;
            while ((c = base64[(unsigned char) *str]) != undefined_num) {
                str++;
                bitbuf = (bitbuf << 6) | c;
                bitcount += 6;
                // enough bits for a UTF-16 character?
                if (bitcount >= 16) {
                    bitcount -= 16;
                    utf16 = (bitcount ? bitbuf >> bitcount
                             : bitbuf) & 0xFFFF;
                    // convert UTF16 to UCS4
                    if
                    (utf16 >= UTF16HIGHSTART && utf16 <= UTF16HIGHEND) {
                        ucs4 = (utf16 - UTF16HIGHSTART) << UTF16SHIFT;
                        continue;
                    } else if
                    (utf16 >= UTF16LOSTART && utf16 <= UTF16LOEND) {
                        ucs4 += utf16 - UTF16LOSTART + UTF16BASE;
                    } else {
                        ucs4 = utf16;
                    }
                    // convert UTF-16 range of UCS4 to UTF-8
                    if (ucs4 <= 0x7fUL) {
                        utf8[0] = ucs4;
                        i = 1;
                    } else if (ucs4 <= 0x7FFUL) {
                        utf8[0] = 0xC0 | (ucs4 >> 6);
                        utf8[1] = 0x80 | (ucs4 & 0x3F);
                        i = 2;
                    } else if (ucs4 <= 0xFFFFUL) {
                        utf8[0] = 0xE0 | (ucs4 >> 12);
                        utf8[1] = 0x80 | ((ucs4 >> 6) & 0x3F);
                        utf8[2] = 0x80 | (ucs4 & 0x3F);
                        i = 3;
                    } else {
                        utf8[0] = 0xF0 | (ucs4 >> 18);
                        utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3F);
                        utf8[2] = 0x80 | ((ucs4 >> 6) & 0x3F);
                        utf8[3] = 0x80 | (ucs4 & 0x3F);
                        i = 4;
                    }

                    for (c = 0; c < i; c++) {
                        if (str_malloced_size - col - 1 < 1) {
                            char *tmp_utf8_str;
                            str_malloced_size += malloc_base_size;
                            tmp_utf8_str = new char [str_malloced_size];
                            memcpy(tmp_utf8_str, utf8_str, col);
                            delete [] utf8_str;
                            utf8_str = tmp_utf8_str;
                        }
                        utf8_str[col] = utf8[c];
                        col++;
                    }
                }
            }
            // skip over trailing '-' in modified UTF-7 encoding
            if (*str == '-') str++;
        }
    }

    utf8_str[col] = '\0';
    return utf8_str;
}


/*! Convert character set from UTF-8 to IMAP4 modified UTF-7
    @param str  UTF-8 text
    @return Converted text (IMAP4 modified UTF-7 text)
 */
char* utf8_to_modutf7(const char* str) {
    unsigned int utf8pos=0, utf8total, c, utf7mode, bitstogo, utf16flag;
    unsigned long ucs4=0, bitbuf=0;
    const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,"; // UTF-7 modified base64
    const unsigned long UTF16MASK      = 0x03FFUL;
    const unsigned long UTF16SHIFT     = 10;
    const unsigned long UTF16BASE      = 0x10000UL;
    const unsigned long UTF16HIGHSTART = 0xD800UL;
    const unsigned long UTF16LOSTART   = 0xDC00UL;
    const int malloc_base_size = 4096;
    int str_malloced_size;
    char *modutf7_str;
    int col;

    if (str == NULL) {
        return NULL;
    }

    str_malloced_size = malloc_base_size;
    modutf7_str = new char [str_malloced_size];
    col = 0;
    modutf7_str[col] = '\0';

    utf7mode = 0;
    utf8total = 0;
    bitstogo = 0;
    while ((c = (unsigned char)*str) != '\0') {
        str++;
        // normal character?
        if (0x20 <= c && c < 0x80 && c != '/' && c != '\\' && c != '~' && c != '&' && c != '-') {
            // switch out of UTF-7 mode
            if (utf7mode) {
                if (bitstogo) {
                    if (str_malloced_size - col - 1 < 1) {
                        char *tmp_modutf7_str;
                        str_malloced_size += malloc_base_size;
                        tmp_modutf7_str = new char [str_malloced_size];
                        memcpy(tmp_modutf7_str, modutf7_str, col);
                        delete [] modutf7_str;
                        modutf7_str = tmp_modutf7_str;
                    }
                    modutf7_str[col] = base64chars[(bitbuf << (6 - bitstogo)) & 0x3F];
                    col++;
                }
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_modutf7_str;
                    str_malloced_size += malloc_base_size;
                    tmp_modutf7_str = new char [str_malloced_size];
                    memcpy(tmp_modutf7_str, modutf7_str, col);
                    delete [] modutf7_str;
                    modutf7_str = tmp_modutf7_str;
                }
                modutf7_str[col] = '-';
                col++;
                utf7mode = 0;
            }
            if (str_malloced_size - col - 1 < 1) {
                char *tmp_modutf7_str;
                str_malloced_size += malloc_base_size;
                tmp_modutf7_str = new char [str_malloced_size];
                memcpy(tmp_modutf7_str, modutf7_str, col);
                delete [] modutf7_str;
                modutf7_str = tmp_modutf7_str;
            }
            modutf7_str[col] = c;
            col++;
            // encode '&' as '&-'
            if (c == '&') {
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_modutf7_str;
                    str_malloced_size += malloc_base_size;
                    tmp_modutf7_str = new char [str_malloced_size];
                    memcpy(tmp_modutf7_str, modutf7_str, col);
                    delete [] modutf7_str;
                    modutf7_str = tmp_modutf7_str;
                }
                modutf7_str[col] = '-';
                col++;
            }
            continue;
        }
        // switch to UTF-7 mode
        if (!utf7mode) {
            if (str_malloced_size - col - 1 < 1) {
                char *tmp_modutf7_str;
                str_malloced_size += malloc_base_size;
                tmp_modutf7_str = new char [str_malloced_size];
                memcpy(tmp_modutf7_str, modutf7_str, col);
                delete [] modutf7_str;
                modutf7_str = tmp_modutf7_str;
            }
            modutf7_str[col] = '&';
            col++;
            utf7mode = 1;
            bitbuf = 0;
            bitstogo = 0;
        }
        // Encode US-ASCII characters as themselves
        if (c < 0x80) {
            ucs4 = c;
            utf8total = 1;
        } else if (utf8total) {
            // save UTF8 bits into UCS4
            ucs4 = (ucs4 << 6) | (c & 0x3FUL);
            if (++utf8pos < utf8total) {
                continue;
            }
        } else {
            utf8pos = 1;
            if (c < 0xE0) {
                utf8total = 2;
                ucs4 = c & 0x1F;
            } else if (c < 0xF0) {
                utf8total = 3;
                ucs4 = c & 0x0F;
            } else {
                // NOTE: can't convert UTF8 sequences longer than 4
                utf8total = 4;
                ucs4 = c & 0x03;
            }
            continue;
        }
        // loop to split ucs4 into two utf16 chars if necessary
        utf8total = 0;
        do {
            if (ucs4 >= UTF16BASE) {
                ucs4 -= UTF16BASE;
                bitbuf = (((bitbuf << 16) | ((ucs4 >> UTF16SHIFT))) + UTF16HIGHSTART);
                ucs4 = (ucs4 & UTF16MASK) + UTF16LOSTART;
                utf16flag = 1;
            } else {
                bitbuf = (bitbuf << 16) | ucs4;
                utf16flag = 0;
            }
            bitstogo += 16;
            // spew out base64
            while (bitstogo >= 6) {
                bitstogo -= 6;
                if (str_malloced_size - col - 1 < 1) {
                    char *tmp_modutf7_str;
                    str_malloced_size += malloc_base_size;
                    tmp_modutf7_str = new char [str_malloced_size];
                    memcpy(tmp_modutf7_str, modutf7_str, col);
                    delete [] modutf7_str;
                    modutf7_str = tmp_modutf7_str;
                }
                modutf7_str[col] = base64chars[(bitstogo ? (bitbuf >> bitstogo) : bitbuf) & 0x3F];
                col++;
            }
        } while (utf16flag);
    }

    // if in UTF-7 mode, finish in ASCII
    if (utf7mode) {
        if (bitstogo) {
            if (str_malloced_size - col - 1 < 1) {
                char *tmp_modutf7_str;
                str_malloced_size += malloc_base_size;
                tmp_modutf7_str = new char [str_malloced_size];
                memcpy(tmp_modutf7_str, modutf7_str, col);
                delete [] modutf7_str;
                modutf7_str = tmp_modutf7_str;
            }
            modutf7_str[col] = base64chars[(bitbuf << (6 - bitstogo)) & 0x3F];
            col++;
        }
        if (str_malloced_size - col - 1 < 1) {
            char *tmp_modutf7_str;
            str_malloced_size += malloc_base_size;
            tmp_modutf7_str = new char [str_malloced_size];
            memcpy(tmp_modutf7_str, modutf7_str, col);
            delete [] modutf7_str;
            modutf7_str = tmp_modutf7_str;
        }
        modutf7_str[col] = '-';
        col++;
    }

    modutf7_str[col] = '\0';
    return modutf7_str;
}


#endif


/*! Get display width of text
    @param str     UTF-8 text
    @return Width of string
 */
long utf8_width(const char* str) {
    long i, width;
    unsigned short u8len;
    unsigned char a0, a1, b0 /*, b1 */;
    int pos, old_pos, min, max, s;
    char buf[7];

    if (str == NULL) {
        return 0;
    }

    width = 0;

    i = 0;
    while (str[i] != '\0') {
        u8len = utf8_len(str[i]);
        if ((unsigned char)(str[i]) == (unsigned char)0xEF &&
                (unsigned char)(str[i+1]) == (unsigned char)0xBB &&
                (unsigned char)(str[i+2]) == (unsigned char)0xBF) {
            i += 3;
        } else if (u8len <= 1) {
            width++;
            i++;
        } else if (u8len == 2 || u8len == 3) {
            // UTF-8 to SJIS
            if (u8len == 2) {
                a0 = 0x07 & (str[i] >> 2);
                a1 = (str[i] << 6) | (0x3F & str[i+1]);
            } else {
                a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
            }

            buf[0] = '\0';
 
            // hankana
            if (a0 == (unsigned char)0xFF && 0x60 <= a1 && a1 <= 0x9F) {
                if (a1 == '\x60') {
                    buf[0] = 0x20;
                } else {
                    buf[0] = a1 + (0xA1-0x61);
                }
                buf[1] = '\0';
            } else {
                buf[0] = '\0';
                min = index_unicode_sjis[a0];
                if (min < 0) {
                    max = min;
                } else if (a0 != 255) {
                    max = min + 255;
                    for (s = a0+1; s <= 255; s++) {
                        if (0 <= index_unicode_sjis[s]) {
                            max = index_unicode_sjis[s]-1;
                            break;
                        }
                    }
                    if (UNICODE_SJIS_TABLE_MAX-1 < max) {
                        max = UNICODE_SJIS_TABLE_MAX-1;
                    }
                } else {
                    max = UNICODE_SJIS_TABLE_MAX-1;
                }
                old_pos = -1;
                pos = (min+max)/2;
                if (0 <= min) do {
                        if (unicode_sjis[pos][0] == a1) {
                            buf[0] = unicode_sjis[pos][1];
                            buf[1] = unicode_sjis[pos][2];
                            buf[2] = '\0';
                            break;
                        }

                        old_pos = pos;
                        if (unicode_sjis[old_pos][0] < a1) {
                            pos = (old_pos+1+max)/2;
                            min = old_pos+1;
                        } else {
                            pos = (min+old_pos-1)/2;
                            max = old_pos-1;
                        }
                    } while (pos != old_pos && min <= max);
            }

            if (buf[0] != '\0') {
                // check width
                b0 = buf[0];
                //b1 = buf[1];
                if (ishankana(b0) || (a0 == '\0' && isascii(a1))) {
                    width++;
                } else {
                    width += 2;
                //} else if (issjis1(b0) && issjis2(b1)) {
                //    width += 2;
                //} else {
                //    width++;
                }
            } else {
                if (u8len == 2) {
                    a0 = 0x07 & (str[i] >> 2);
                    a1 = (str[i] << 6) | (0x3F & str[i+1]);
                } else {
                    a0 = (str[i] << 4) | (0x0F & (str[i+1] >> 2));
                    a1 = (str[i+1] << 6) | (0x3F & str[i+2]);
                }

                if (a0 == 0) {
                    width++;
                } else {
                    unsigned long ucs4;
                    ucs4 = (a0 * 256) + a1;
                    if (0x1100 <= ucs4 &&
                            (ucs4 <= 0x115F || ucs4 == 0x2329 || ucs4 == 0x232A ||
                             (0x2E80 <= ucs4 && ucs4 <= 0xA4CF && ucs4 != 0x303F) ||
                             (0xAC00 <= ucs4 && ucs4 <= 0xD7A3) ||
                             (0xF900 <= ucs4 && ucs4 <= 0xFAFF) ||
                             (0xFE30 <= ucs4 && ucs4 <= 0xFE6F) ||
                             (0xFF00 <= ucs4 && ucs4 <= 0xFF60) ||
                             (0xFFE0 <= ucs4 && ucs4 <= 0xFFE6) ||
                             (0x20000 <= ucs4 && ucs4 <= 0x2FFFD) ||
                             (0x30000 <= ucs4 && ucs4 <= 0x3FFFD))) {
                        width += 2;
                    } else {
                        width++;
                    }
                }
            }
            i += u8len;

        } else {
            width += 2;
            i += u8len;
        }
    }

    return width;
}


/*! Change text width
    @param str     UTF-8 text
    @param options converting options (ex. "AHI")
                   'A': ASCII to full width
                   'a': ASCII to half width
                   'H': half width katakana to full width hiragana
                   'K': half width katakana to full width katakana
                   'J': full width hiragana to full width katakana
                   'j': full width hiragana to half width katakana
                   'I': full width katakana to full width hiragana
                   'k': full width katakana to half width katakana
    @return Converted text (UTF-8 text)
 */
char* utf8_change_width(const char* str, const char *options) {
    char *buf;
    long i, j, length, length2;
    int OPTION_A, OPTION_a, OPTION_H, OPTION_K, OPTION_J, OPTION_j, OPTION_I, OPTION_k;

    buf = NULL;

    if (str != NULL && options != NULL) {
        // set options
        OPTION_A = strchr(options, 'A')?1:0;
        OPTION_a = strchr(options, 'a')?1:0;
        OPTION_H = strchr(options, 'H')?1:0;
        OPTION_K = strchr(options, 'K')?1:0;
        OPTION_J = strchr(options, 'J')?1:0;
        OPTION_j = strchr(options, 'j')?1:0;
        OPTION_I = strchr(options, 'I')?1:0;
        OPTION_k = strchr(options, 'k')?1:0;

        // Memory allocation
        length = strlen(str);
        length2 = length*6;
        buf = new char[length2 + 1];

        // change width
        i = 0;
        j = 0;
        while (i < length && j <= length2) {
            int mblen, converted, convlen, num;
            mblen = utf8_len(str[i]);
            if (length < i + mblen) {
                break;
            }
            converted = 0;

            // 'A': ASCII to full width
            if (OPTION_A && mblen == 1 && ((unsigned char)str[i] & (unsigned char)0x80) == (unsigned char)0) {
                for (num = 0; HALF_ASCII[num] != NULL; num++) {
                    convlen = strlen(HALF_ASCII[num]);
                    if (!strncmp(&str[i], HALF_ASCII[num], convlen)) {
                        strcpy(&buf[j], FULL_ASCII[num]);
                        i += convlen;
                        j += strlen(FULL_ASCII[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'a': ASCII to half width
            if (OPTION_a && converted == 0 && mblen == 3 && ((unsigned char)str[i] & (unsigned char)0xE0) == (unsigned char)0xE0) {
                for (num = 0; FULL_ASCII[num] != NULL; num++) {
                    convlen = strlen(FULL_ASCII[num]);
                    if (!strncmp(&str[i], FULL_ASCII[num], convlen)) {
                        strcpy(&buf[j], HALF_ASCII[num]);
                        i += convlen;
                        j += strlen(HALF_ASCII[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'H': half width katakana to full width hiragana
            if (OPTION_H && converted == 0 && 3 <= mblen && (unsigned char)str[i] == (unsigned char)0xEF) {
                for (num = 0; HALF_KANA[num] != NULL; num++) {
                    convlen = strlen(HALF_KANA[num]);
                    if (!strncmp(&str[i+1], HALF_KANA[num], convlen)) {
                        buf[j++] = (char)0xE3;
                        strcpy(&buf[j], FULL_HIRA[num]);
                        i += (convlen + 1);
                        j += strlen(FULL_HIRA[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'K': half width katakana to full width katakana
            if (OPTION_K && converted == 0 && 3 <= mblen && (unsigned char)str[i] == (unsigned char)0xEF) {
                for (num = 0; HALF_KANA[num] != NULL; num++) {
                    convlen = strlen(HALF_KANA[num]);
                    if (!strncmp(&str[i+1], HALF_KANA[num], convlen)) {
                        buf[j++] = (char)0xE3;
                        strcpy(&buf[j], FULL_KANA[num]);
                        i += (convlen + 1);
                        j += strlen(FULL_KANA[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'J': full width hiragana to full width katakana
            if (OPTION_J && converted == 0 && 3 <= mblen && (unsigned char)str[i] == (unsigned char)0xE3) {
                for (num = 0; FULL_HIRA[num] != NULL; num++) {
                    convlen = strlen(FULL_HIRA[num]);
                    if (!strncmp(&str[i+1], FULL_HIRA[num], convlen)) {
                        buf[j++] = (char)0xE3;
                        strcpy(&buf[j], FULL_KANA[num]);
                        i += (convlen + 1);
                        j += strlen(FULL_KANA[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'j': full width hiragana to half width katakana
            if (OPTION_j && converted == 0 && 3 <= mblen && (unsigned char)str[i] == (unsigned char)0xE3) {
                for (num = 0; FULL_HIRA[num] != NULL; num++) {
                    convlen = strlen(FULL_HIRA[num]);
                    if (!strncmp(&str[i+1], FULL_HIRA[num], convlen)) {
                        buf[j++] = (char)0xEF;
                        strcpy(&buf[j], HALF_KANA[num]);
                        i += (convlen + 1);
                        j += strlen(HALF_KANA[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'I': full width katakana to full width hiragana
            if (OPTION_I && converted == 0 && 3 <= mblen && (unsigned char)str[i] == (unsigned char)0xE3) {
                for (num = 0; FULL_KANA[num] != NULL; num++) {
                    convlen = strlen(FULL_KANA[num]);
                    if (!strncmp(&str[i+1], FULL_KANA[num], convlen)) {
                        buf[j++] = (char)0xE3;
                        strcpy(&buf[j], FULL_HIRA[num]);
                        i += (convlen + 1);
                        j += strlen(FULL_HIRA[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            // 'k': full width katakana to half width katakana
            if (OPTION_k && converted == 0 && 3 <= mblen && (unsigned char)str[i] == (unsigned char)0xE3) {
                for (num = 0; FULL_KANA[num] != NULL; num++) {
                    convlen = strlen(FULL_KANA[num]);
                    if (!strncmp(&str[i+1], FULL_KANA[num], convlen)) {
                        buf[j++] = (char)0xEF;
                        strcpy(&buf[j], HALF_KANA[num]);
                        i += (convlen + 1);
                        j += strlen(HALF_KANA[num]);
                        converted = 1;
                        break;
                    }
                }
            }

            if (converted == 0) {
                strncpy(&buf[j], &str[i], mblen);
                i += mblen;
                j += mblen;
            }
        }
        buf[j] = '\0';
    } else if (options != NULL) {
        length = strlen(str);
        buf = new char[length + 1];
        strcpy(buf, str);
    }

    return buf;
}


/*! Clean up UTF-8 string (remove invalid characters)
    @param str     string for cleaning up
    @return Pointer of cleaned up string (Same address of str)
 */
char* utf8_clean(char* str) {
    int ucs4_code;
    int utf8_size;
    int i;
    int len;
    unsigned char* pstr;
    unsigned char* peob;

    if (str == NULL) {
        return NULL;
    }

    pstr = (unsigned char *)str;
    peob = pstr + strlen((char *)pstr);

    while (pstr < peob) {
        utf8_size = utf8_len(pstr[0]);

        if (peob < pstr + utf8_size) {
            pstr[0] = '\0';
            break;
        }

        switch (utf8_size) {
            case 1:
                ucs4_code = pstr[0];
                len = 0;
                break;
            case 2:
                ucs4_code = pstr[0] & 0x1F;
                len = 2;
                break;
            case 3:
                ucs4_code = pstr[0] & 0x0F;
                len = 3;
                break;
            case 4:
                ucs4_code = pstr[0] & 0x07;
                len = 4;
                break;
            default:
                ucs4_code = -1;
                len = 0;
                break;
        }

        for (i = 1; i < len; i++) {
            if ((pstr[i] & 0xC0) != 0x80) {
                ucs4_code = -1;
                break;
            }
            ucs4_code = (ucs4_code << 6) | (pstr[i] & 0x3F);
        }

        switch (utf8_size) {
            case 1:
                if (!((0x00 <= ucs4_code) && (ucs4_code <= 0x7F))) {
                    ucs4_code = -1;
                }
                break;
            case 2:
                if (!((0x80 <= ucs4_code) && (ucs4_code <= 0x7FF))) {
                    ucs4_code = -1;
                }
                break;
            case 3:
                if (!(((0x800 <= ucs4_code) && (ucs4_code <= 0xD7FF)) ||
                        ((0xE000 <= ucs4_code) && (ucs4_code <= 0xFFFF)))) {
                    ucs4_code = -1;
                }
                break;
            case 4:
                if (!((0x1000 <= ucs4_code) && (ucs4_code <= 0x10FFFF))) {
                    ucs4_code = -1;
                }
                break;
            default:
                ucs4_code = -1;
                break;
        }

        if (ucs4_code < 0) {
            for (i = 0; i < utf8_size; i++) {
                pstr[i] = '?';
            }
        }
        pstr += utf8_size;
    }

    return str;
}

} // namespace apolloron
