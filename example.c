#include <stdio.h>
#include <string.h>

#include "inkf.h"

int main() {
    int ret;
    char *dest_str;
    long dest_str_len;

    ret = inkf("こんにちは", strlen("こんにちは"),
               &dest_str, &dest_str_len, "-We");

    printf("This is EUC-JP text.\n");
    printf("ret=%d\n", ret);
    printf("dest_str_len=%ld\n", dest_str_len);
    printf("dest_str=[%s]\n", dest_str);

    inkf_free_str(dest_str);
    return 0;
}
