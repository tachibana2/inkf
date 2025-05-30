/******************************************************************************/
/*! @file table_sjis_unicode.h
    @brief Header file of SHIFT_JIS/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_SJIS_UNICODE_H_
#define _TABLE_SJIS_UNICODE_H_

namespace apolloron {

#define SJIS_UNICODE_TABLE_MAX 9206

extern const short int index_sjis_unicode[256];
extern const unsigned char sjis_unicode[SJIS_UNICODE_TABLE_MAX][3];


} // namespace apolloron
#endif
