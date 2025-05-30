/******************************************************************************/
/*! @file inkf.h
    @brief yet another nkf with international language support
    @author Masashi Astro Tachibana
    @date 2002-2025
 ******************************************************************************/

#ifndef _INKF_H_
#define _INKF_H_

int inkf(const char *src_str, long src_str_len,
         char **dest_str, long *dest_str_len, const char *option_str);

void inkf_free_str(char *str);

#endif
