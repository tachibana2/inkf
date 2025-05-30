/******************************************************************************/
/*! @file table_unicode_eucjp.h
    @brief Header file of UCS-2/SHIFT_JIS table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_UNICODE_SJIS_H_
#define _TABLE_UNICODE_SJIS_H_

namespace apolloron {

#define UNICODE_SJIS_TABLE_MAX 9206

extern const short int index_unicode_sjis[256];
extern const unsigned char unicode_sjis[UNICODE_SJIS_TABLE_MAX][3];


} // namespace apolloron
#endif
