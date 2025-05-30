/******************************************************************************/
/*! @file table_unicode_euckr.h
    @brief Header file of UCS-2/EUC-KR table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_UNICODE_EUCKR_H_
#define _TABLE_UNICODE_EUCKR_H_

namespace apolloron {

#define UNICODE_EUCKR_TABLE_MAX 17048

extern const short int index_unicode_euckr[256];
extern const unsigned char unicode_euckr[UNICODE_EUCKR_TABLE_MAX][3];


} // namespace apolloron
#endif
