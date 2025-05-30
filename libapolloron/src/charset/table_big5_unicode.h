/******************************************************************************/
/*! @file table_big5_unicode.h
    @brief Header file of BIG5/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_BIG5_UNICODE_H_
#define _TABLE_BIG5_UNICODE_H_

namespace apolloron {

#define BIG5_UNICODE_TABLE_MAX 13502

extern const short int index_big5_unicode[256];
extern const unsigned char big5_unicode[BIG5_UNICODE_TABLE_MAX][3];


} // namespace apolloron

#endif
