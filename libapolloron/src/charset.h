/******************************************************************************/
/*! @file charset.h
    @brief Header file of charset.cc.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _CHARSET_H_
#define _CHARSET_H_

namespace apolloron {

// Macro for character code processing
#define isjis(c) (0x21 <= (c) && (c) <= 0x7E)
#define iseuc(c) ((c) == 0x8E || (0xA1 <= (c) && (c) <= 0xFE))
#define issjis1(c) ((0x81 <= (c) && (c) <= 0x9F) || (0xE0 <= (c) && (c) <= 0xFC))
#define issjis2(c) (0x40 <= (c) && (c) <= 0xFC && (c) != 0x7F)
#define ishankana(c) (0xA0 <= (c) && (c) <= 0xDF)
#define utf8_len(c) ((0 <= (unsigned char)(c) && (unsigned char)(c) <= 0x7F)?1:((unsigned char)(0xC2 <= (unsigned char)(c) && (unsigned char)(c) <= (unsigned char)0xDF)?2:((unsigned char)0xE0 <= (unsigned char)(c) && (unsigned char)(c) <= (unsigned char)0xEF)?3:((unsigned char)0xF0 <= (unsigned char)(c) && (unsigned char)(c) <= (unsigned char)0xF7)?4:((unsigned char)0xF8 <= (unsigned char)(c) && (unsigned char)(c) <= (unsigned char)0xFB)?5:((unsigned char)0xFC <= (unsigned char)(c) && (unsigned char)(c) <= (unsigned char)0xFD)?6:1))
#define isutf8_1(c) ((((unsigned char)(c) & (unsigned char)0xF0) == 0xE0) || (((unsigned char)(c) & (unsigned char)0xE0) == (unsigned char)0xC0))
#define isutf8_2(c) ((((unsigned char)(c) & (unsigned char)0x80) == 0x80) && ((unsigned char)(c) <= (unsigned char)0xBF))
#define isutf8_3(c) ((((unsigned char)(c) & (unsigned char)0xC0) == 0x80) && ((unsigned char)(c) <= (unsigned char)0xBF))

extern const char *STR_ISO8859_1;
extern const char *STR_ISO8859_2;
extern const char *STR_ISO8859_3;
extern const char *STR_ISO8859_4;
extern const char *STR_ISO8859_5;
extern const char *STR_ISO8859_6;
extern const char *STR_ISO8859_7;
extern const char *STR_ISO8859_8;
extern const char *STR_ISO8859_9;
extern const char *STR_ISO8859_10;
extern const char *STR_ISO8859_11;
extern const char *STR_ISO8859_13;
extern const char *STR_ISO8859_14;
extern const char *STR_ISO8859_15;
extern const char *STR_ISO8859_16;
extern const char *STR_KOI8_R;
extern const char *STR_KOI8_U;
extern const char *STR_CP1251;
extern const char *STR_CP1252;
extern const char *STR_CP1258;
extern const char *STR_UTF8;
extern const char *STR_UTF7;
extern const char *STR_UTF7_IMAP;
extern const char *STR_UTF16LE;
extern const char *STR_UTF16BE;
extern const char *STR_UTF32LE;
extern const char *STR_UTF32BE;
extern const char *STR_SJIS;
extern const char *STR_JIS;
extern const char *STR_EUCJP;
extern const char *STR_EUCJPMS;
extern const char *STR_ASCII;
extern const char *STR_AUTOJP;
extern const char *STR_EUCKR;
extern const char *STR_GBK;
extern const char *STR_BIG5;

char* charset_convert(const char* str, const char *src_charset, const char *dest_charset);
const char* auto_detect(const char* str);
const char* jis_auto_detect(const char* str);
char* iso8859_to_utf8(const char* str, int iso8859_num);
char* utf8_to_iso8859(const char* str, int iso8859_num);
char* europe_to_utf8(const char* str, char ch);
char* utf8_to_europe(const char* str, char ch);
char* utf8_to_utf16(const char *str, long *length, char en='L');
char* utf16_to_utf8(const char *str, long length, char en='L');
char* utf8_to_utf32(const char *str, long *length, char en='L');
char* utf32_to_utf8(const char *str, long length, char en='L');
char* utf8_to_eucjpms(const char *str);
char* utf8_to_eucjpwin(const char* str);
char* sjis_to_eucjpms(const char* str);
char* sjis_to_eucjpwin(const char* str);
char* jis_to_eucjpms(const char* str);
char* jis_to_eucjpwin(const char* str);
char* eucjpms_to_sjis(const char* str);
char* eucjpwin_to_sjis(const char* str);
char* eucjpms_to_jis(const char* str);
char* eucjpwin_to_jis(const char* str);
char* eucjpms_to_utf8(const char *str);
char* eucjpwin_to_utf8(const char* str);
char* eucjpms_to_eucjpwin(const char *str);
char* eucjpwin_to_eucjpms(const char *str);
char* utf8_to_sjis(const char* str);
char* utf8_to_jis(const char* str);
char* sjis_to_utf8(const char* str);
char* jis_to_utf8(const char* str);
char* sjis_to_jis(const char* str);
char* jis_to_sjis(const char* str);
char* autojp_to_eucjpms(const char *str);
char* autojp_to_eucjpwin(const char* str);
char* autojp_to_sjis(const char* str);
char* autojp_to_jis(const char* str);
char* autojp_to_utf8(const char* str);
char* euckr_to_utf8(const char* str);
char* utf8_to_euckr(const char* str);
char* gbk_to_utf8(const char* str);
char* utf8_to_gbk(const char* str);
char* big5_to_utf8(const char* str);
char* utf8_to_big5(const char* str);
char* utf7_to_utf8(const char* str);
char* utf8_to_utf7(const char* str);
char* modutf7_to_utf8(const char* str);
char* utf8_to_modutf7(const char* str);

long utf8_width(const char* str);
char* utf8_change_width(const char* str, const char* options);
char* utf8_clean(char* str);

} // namespace apolloron

#endif
