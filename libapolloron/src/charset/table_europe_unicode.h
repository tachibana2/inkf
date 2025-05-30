/******************************************************************************/
/*! @file table_europe_unicode.h
    @brief Header file of Europe code/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_EUROPE_UNICODE_H_
#define _TABLE_EUROPE_UNICODE_H_

namespace apolloron {

#define EUROPE_UNICODE_TABLE_MAX 128

/* KOI8-R  0x80-0xFF */
extern const unsigned char koi8_r_unicode[EUROPE_UNICODE_TABLE_MAX][2];

/* KOI8-U  0x80-0xFF */
extern const unsigned char koi8_u_unicode[EUROPE_UNICODE_TABLE_MAX][2];

/* CP1251  0x80-0xFF */
extern const unsigned char cp1251_unicode[EUROPE_UNICODE_TABLE_MAX][2];

/* CP1252  0x80-0xFF */
extern const unsigned char cp1252_unicode[EUROPE_UNICODE_TABLE_MAX][2];

/* CP1258  0x80-0xFF */
extern const unsigned char cp1258_unicode[EUROPE_UNICODE_TABLE_MAX][2];


} // namespace apolloron
#endif
