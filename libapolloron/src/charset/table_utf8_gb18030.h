/******************************************************************************/
/*! @file table_utf8_gb18030.h
    @brief UTF-8/GB18030 table.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#ifndef _TABLE_UTF8_GB18030_H_
#define _TABLE_UTF8_GB18030_H_

namespace apolloron {

#define UTF8_GB18030_TABLE_MAX 231

#define UTF8_GB18030_2BYTE_START 0
#define UTF8_GB18030_2BYTE_END   41
#define UTF8_GB18030_3BYTE_START 42
#define UTF8_GB18030_3BYTE_END   218
#define UTF8_GB18030_4BYTE_START 219
#define UTF8_GB18030_4BYTE_END   230

typedef struct {
    const char *utf8;
    const char *gb18030;
    int count;
} T_UTF8_GB18030;

extern const T_UTF8_GB18030 utf8_gb18030[UTF8_GB18030_TABLE_MAX];


} // namespace apolloron
#endif
