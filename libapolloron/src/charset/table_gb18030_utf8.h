/******************************************************************************/
/*! @file table_gb18030_utf8.h
    @brief GB18030/UTF-8 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_GB18030_UTF8_H_
#define _TABLE_GB18030_UTF8_H_

namespace apolloron {

#define GB18030_UTF8_TABLE_MAX 231
#define GB18030_UTF8_TABLE_90  219

typedef struct {
    const char *gb18030;
    const char *utf8;
    unsigned int count;
} T_GB18030_UTF8;

extern const T_GB18030_UTF8 gb18030_utf8[GB18030_UTF8_TABLE_MAX];


} // namespace apolloron
#endif
