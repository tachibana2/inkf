/******************************************************************************/
/*! @file table_iso8859_unicode.h
    @brief Header file of ISO-8859/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_ISO8859_UNICODE_H_
#define _TABLE_ISO8859_UNICODE_H_

namespace apolloron {

#define ISO8859_UNICODE_TABLE_MAX 128

/* ISO-8859-1  0x80-0xFF */
extern const unsigned char iso8859_1_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-2  0x80-0xFF */
extern const unsigned char iso8859_2_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-3  0x80-0xFF */
extern const unsigned char iso8859_3_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-4  0x80-0xFF */
extern const unsigned char iso8859_4_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-5  0x80-0xFF */
extern const unsigned char iso8859_5_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-6  0x80-0xFF */
extern const unsigned char iso8859_6_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-7  0x80-0xFF */
extern const unsigned char iso8859_7_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-8  0x80-0xFF */
extern const unsigned char iso8859_8_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-9  0x80-0xFF */
extern const unsigned char iso8859_9_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-10  0x80-0xFF */
extern const unsigned char iso8859_10_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-11(Windows-874)  0x80-0xFF */
extern const unsigned char iso8859_11_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-13  0x80-0xFF */
extern const unsigned char iso8859_13_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-14  0x80-0xFF */
extern const unsigned char iso8859_14_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-15  0x80-0xFF */
extern const unsigned char iso8859_15_unicode[ISO8859_UNICODE_TABLE_MAX][2];

/* ISO-8859-16  0x80-0xFF */
extern const unsigned char iso8859_16_unicode[ISO8859_UNICODE_TABLE_MAX][2];


} // namespace apolloron
#endif
