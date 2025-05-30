/******************************************************************************/
/*! @file table_euckr_unicode.h
    @brief Header file of EUC-KR/UCS-2 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_EUCKR_UNICODE_H_
#define _TABLE_EUCKR_UNICODE_H_

namespace apolloron {

#define EUCKR_UNICODE_TABLE_MAX 17048

extern const short int index_euckr_unicode[256];
extern const unsigned char euckr_unicode[EUCKR_UNICODE_TABLE_MAX][3];


} // namespace apolloron
#endif
