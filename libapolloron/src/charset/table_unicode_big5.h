/******************************************************************************/
/*! @file table_unicode_big5.h
    @brief Header file of UCS-2/BIG5 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _H_TABLE_UNICODE_BIG5_
#define _H_TABLE_UNICODE_BIG5_

namespace apolloron {

#define UNICODE_BIG5_TABLE_MAX 13493

extern const short int index_unicode_big5[256];
extern const unsigned char unicode_big5[UNICODE_BIG5_TABLE_MAX][3];


} // namespace apolloron
#endif
