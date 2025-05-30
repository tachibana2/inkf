/******************************************************************************/
/*! @file libinkf.c
    @brief yet another nkf with international language support
    @author Masashi Astro Tachibana
    @date 2002-2019
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inkf_common.h"


/*! inkf text conversion
    @param src_str       Source text to convert
    @param src_str_len   Length of src_str
    @param dest_str      Converted text (new alloced memory), NULL if error
    @param dest_str_len  Length of dest_str (without NULL terminater)
    @retval  0  success
    @retval -1  failure
 */
int inkf(const char *src_str, long src_str_len,
         char **dest_str, long *dest_str_len, const char *option_str) {
    int argc;
    char **argv;
    char *p, *pstr;
    long i, l;
    TOption option;
    int quote;
    int retval = 0;

    if (dest_str != NULL) {
        *dest_str = NULL;
    }
    if (dest_str_len != NULL) {
        *dest_str_len = 0;
    }

    if (src_str == (const char *)NULL || src_str_len <= 0) {
        src_str = "";
        src_str_len = 0;
    }
    if (option_str == (const char *)NULL) {
        option_str = "";
    }

    /* parse option */
    argc = 1;
    argv = (char **)malloc(sizeof(char *) * (1 + 1));
    argv[0] = malloc(5);
    strcpy(argv[0], "inkf");
    argv[argc] = NULL;

    l = strlen(option_str);
    pstr = (char *)malloc(strlen(option_str)+1);
    strcpy(pstr, option_str);
    p = pstr;
    i = 0;
    quote = 0;
    while (i < l) {
        if (pstr[i] == ' ') {
            i++;
            p = pstr + i;
            continue;
        }
        break;
    }
    while (i < l) {
        while (i < l) {
            if (pstr[i] == '"') {
                memmove(pstr + i, pstr + i + 1, l - i + 1);
                quote = 1 - quote;
            } else if (quote == 1 && pstr[i] == '\\') {
                memmove(pstr + i, pstr + i + 1, l - i + 1);
                l--;
            } else if (quote == 0 && pstr[i] == ' ') {
                pstr[i] = '\0';
                break;
            }
            i++;
        }
        if (pstr[i] == ' ' || pstr[i] == '\0' || l - 1 <= i) {
            pstr[i] = '\0';
            if (p[0] != '\0') {
                argc++;
                argv = (char **)realloc(argv, sizeof(char *) * (argc + 1));
                argv[argc-1] = p;
                argv[argc] = NULL;
            }
            if (i < l) {
                p = pstr + i;
            }
        }
        i++;
        while (i < l) {
            if (pstr[i] != ' ') {
                p = pstr + i;
                break;
            }
            i++;
        }
    }
    if (quote != 0) {
        retval = -1;
        goto libinkf_exit;
    }

    /* set option */
    if (inkf_set_option(&option, argc, (const char **)argv) != 0) {
        /* Invalid parameter(s). */
        retval = -1;
        goto libinkf_exit;
    }

    retval = inkf_lib_exec(src_str, src_str_len, dest_str, dest_str_len,
                           &option);

libinkf_exit:
    free(argv);
    free(pstr);
    inkf_free_option(&option);

    return retval;
}


/*! Free inkf dest_str
    @param str  Pointer of text to free
    @return void
 */
void inkf_free_str(char *str) {
    if (str != NULL) {
        inkf_free_output_str(str);
    }
}
