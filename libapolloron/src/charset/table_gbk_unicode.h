/******************************************************************************/
/*! @file table_gbk_unicode.h
    @brief Header file of GBK/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_GBK_UNICODE_H_
#define _TABLE_GBK_UNICODE_H_

namespace apolloron {

#define GBK_UNICODE_TABLE_MAX 32256

extern const short int index_gbk_unicode[256];
extern const unsigned char gbk_unicode[GBK_UNICODE_TABLE_MAX][2];


} // namespace apolloron
#endif
