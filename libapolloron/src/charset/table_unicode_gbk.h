/******************************************************************************/
/*! @file table_unicode_gbk.h
    @brief Header file of UCS-2/GBK table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_UNICODE_GBK_H_
#define _TABLE_UNICODE_GBK_H_

namespace apolloron {

#define UNICODE_GBK_TABLE_MAX 33280

extern const int index_unicode_gbk[256];
extern const unsigned char unicode_gbk[UNICODE_GBK_TABLE_MAX][2];


} // namespace apolloron
#endif
