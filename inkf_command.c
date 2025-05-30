/******************************************************************************/
/*! @file inkf.c
    @brief yet another nkf with international language support
    @author Masashi Astro Tachibana
    @date 2002-2025
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inkf_common.h"


int main(int argc, char *argv[]) {
    TOption option;
    int retval = 0;

    /* set option */
    if (inkf_set_option(&option, argc, (const char **)argv) != 0) {
        fprintf(stderr, "Invalid parameter(s).\n");
        retval = -1;
        goto main_exit;
    }

    retval = inkf_command_exec(&option);

main_exit:
    inkf_free_option(&option);

    return retval;
}
