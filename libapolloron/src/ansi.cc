/******************************************************************************/
/*! @file ansi.cc
    @brief ANSI compatible functions.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/param.h>

#include "apolloron.h"


namespace apolloron {

/*! convert a integer to string
    @param buf Buffer to store converted value.
    @param value int value to convert
    @param s Signed(1) or unsigned(0)
    @param base Base above 36, or below 1 (default=10)
    @retval length of result string
 */
int itoa(char *buf, int value, int s, int base) {
    int col1, col2, len;
    unsigned int i;

    col1 = 0;

    if (s != 0 && value < 0) {
        buf[col1] = '-';
        col1++;
        i = (unsigned int)(0-value);
    } else {
        i = (unsigned int)value;
    }

    col2 = col1;
    do {
        buf[col1] = i%base > 9 ? 'A' + i%base - 10 : '0' + i%base;
        i = i / base;
        col1++;
    } while(i != 0);

    buf[col1] = '\0';
    len = col1;
    col1--;

    while (col2 < col1) {
        buf[col1] ^= buf[col2];
        buf[col2] ^= buf[col1];
        buf[col1] ^= buf[col2];
        col1--;
        col2++;
    }

    return len;
}


/*! convert a long int to string
    @param buf Buffer to store converted value.
    @param value long value to convert
    @param s Signed(1) or unsigned(0)
    @param base Base above 36, or below 1 (default=10)
    @retval length of result string
 */
int ltoa(char *buf, long value, int s, int base) {
    int col1, col2, len;
    unsigned long u;

    col1 = 0;

    if (s != 0 && value < 0) {
        buf[col1] = '-';
        col1++;
        u = (unsigned long)(0-value);
    } else {
        u = (unsigned long)value;
    }

    col2 = col1;
    do {
        buf[col1] = u%base > 9 ? 'A' + u%base - 10 : '0' + u%base;
        u = u / base;
        col1++;
    } while(u != 0);

    buf[col1] = '\0';
    len = col1;
    col1--;

    while (col2 < col1) {
        buf[col1] ^= buf[col2];
        buf[col2] ^= buf[col1];
        buf[col1] ^= buf[col2];
        col1--;
        col2++;
    }

    return len;
}


/*! convert a double to string
    @param buf Buffer to store converted value.
    @param value double value to convert
    @return length of result string
 */
int dtoa(char *buf, double value) {
    const int FRACT_DIGIT_MAX = 9; // max of fractional digits (limited to 9)
    int i, pre, zero_len, remainder, len;
    double flg, zeros, round;
    long  fract_part;

    if (value == 0.0) {
        buf[0] = '0';
        buf[1] = '.';
        buf[2] = '0';
        buf[3] = '\0';
        return 3;
    }

    len = 0;
    if (value < 0.0) {
        value = -value;
        buf[len++] = '-';
    }

    // This could also be done with an array 1.0, 10.0, 100.0 etc.
    zeros = 1.0;
    for (i = 0; i < FRACT_DIGIT_MAX; i++) {
        zeros *= 10;
    }

    round = 0.5/zeros;
    value += round;
    flg = (long)value;
    value = value - flg;

    len += ltoa(&buf[len], (long)flg);
    buf[len++] = '.';

    buf[len+FRACT_DIGIT_MAX] = '\0';
    fract_part = (long)(value * zeros);
    pre = 0;
    zero_len = 0;
    for (i = len+FRACT_DIGIT_MAX-1; len <= i; i--) {
        remainder = fract_part % 10;
        if (pre == 0 && remainder == 0) {
            buf[i] = '\0';
            zero_len++;
        } else {
            buf[i] = remainder + 0x30;
            pre = remainder;
        }
        fract_part /= 10;
    }

    len += FRACT_DIGIT_MAX - zero_len;
    return len;
}


/*! convert a double to string (ex. 12345678.123 to "12,345,678.123")
    @param buf Buffer to store converted value.
    @param value double value to convert
    @param figure display num under point (.xx)
    @param lang lang_id
    @return length of result string
 */
int dtoa_lang(char *buf, double value, int figure, const char *lang) {
    int i, len, n, commas;
    char *p;
    char big[64];
    char small[64];
    const char *delim;
    const char *point;

    if (figure < 0 || 16 < figure) figure = 2;

    /* 12345.678 */
    ::snprintf(big, sizeof(big), "%.*f", figure, value);
    p = strchr(big, '.');
    if (p) {
        strcpy(small, p+1);
        *p = '\0';
    } else {
        for (i = 0; i < figure; i++) {
            small[i] = '0';
        }
        small[i] = '\0';
    }

    // insert commas, "-123456789" to "-123,456,789"
    len = strlen(big);
    i = len - 1;
    if (big[0] == '+' || big[0] == '-') len--;
    commas = (len - 1) / 3;
    big[i+commas+1] = '\0';
    for ( ; commas > 0; commas--) {
        for (n = 0; n++ < 3; i--) {
            big[i+commas] = big[i];
        }
        big[i+commas] = ',';
    }

    /* delim, point */
    delim = ",";
    point = ".";
    if (lang == NULL || !strncasecmp(lang, "en", 2) ||
            !strncasecmp(lang, "ja", 2) || !strncasecmp(lang, "ko", 2) ||
            !strncasecmp(lang, "zh", 2) || !strncasecmp(lang, "es-sv", 5) ||
            !strncasecmp(lang, "es-gt", 5) || !strncasecmp(lang, "es-do", 5) ||
            !strncasecmp(lang, "es-ni", 5) || !strncasecmp(lang, "es-pa", 5) ||
            !strncasecmp(lang, "es-hn", 5) || !strncasecmp(lang, "es-mx", 5) ||
            !strncasecmp(lang, "es-pr", 5) || !strncasecmp(lang, "es-pe", 5)) {
        delim = ",";
        point = ".";
    } else if (!strncasecmp(lang, "it-ch", 5) || !strncasecmp(lang, "de-ch", 5) ||
               !strncasecmp(lang, "de-li", 5) || !strncasecmp(lang, "fr-ch", 5)) {
        delim = "'";
        point = ".";
    } else if (!strncasecmp(lang, "fr-be", 5)) {
        delim = ".";
        point = ",";
    } else if (!strncasecmp(lang, "uk", 2) || !strncasecmp(lang, "et", 2) ||
               !strncasecmp(lang, "sv", 2) || !strncasecmp(lang, "sk", 2) ||
               !strncasecmp(lang, "cs", 2) || !strncasecmp(lang, "no", 2) ||
               !strncasecmp(lang, "hu", 2) || !strncasecmp(lang, "fi", 2) ||
               !strncasecmp(lang, "fr", 2) || !strncasecmp(lang, "pl", 2) ||
               !strncasecmp(lang, "lv", 2) || !strncasecmp(lang, "ru", 2)) {
        delim = " ";
        point = ",";
    } else if (!strncasecmp(lang, "af", 2) || !strncasecmp(lang, "in", 2) ||
               !strncasecmp(lang, "is", 2) || !strncasecmp(lang, "it", 2) ||
               !strncasecmp(lang, "nl", 2) || !strncasecmp(lang, "ca", 2) ||
               !strncasecmp(lang, "el", 2) || !strncasecmp(lang, "es", 2) ||
               !strncasecmp(lang, "sl", 2) || !strncasecmp(lang, "da", 2) ||
               !strncasecmp(lang, "de", 2) || !strncasecmp(lang, "tr", 2) ||
               !strncasecmp(lang, "pt", 2) || !strncasecmp(lang, "lt", 2) ||
               !strncasecmp(lang, "ro", 2)) {
        delim = ".";
        point = ",";
    }

    /* make string */
    if (delim[0] != ',') for (i = 0; i < len; i++) {
            if (big[i] == ',') big[i] = delim[0];
        }
    strcpy(buf, big);
    if (0 < figure) {
        strcat(buf, point);
        strcat(buf, small);
    }

    return strlen(buf);
}


/*! make directory hierarchically (like "mkdir -p")
    @param pathname path name to create
    @param mode mode like 0755 (affect: mode & ~umask & 0777)
    @retval 0  success
    @retval -1 failure
 */
int mkdirp(const char *pathname, mode_t mode) {
    char *p;
    int i;

    if (pathname == NULL || pathname[0] == '\0') {
        return -1;
    }

    if (isDirExist(pathname) == true) {
        // already exist
        return 0;
    }

    p = new char [strlen(pathname) + 1];
    i = 0;
    while (1) {
        p[i] = pathname[i];
        if ((0 < i && pathname[i] == '/') || pathname[i] == '\0') {
            p[i] = '\0';
            if (0 < i && isDirExist(p) == false) {
                if (mkdir(p, mode) != 0) {
                    delete [] p;
                    return -1;
                }
            }
            if (pathname[i] == '\0') break;
            p[i] = '/';
        }
        i++;
    }

    delete [] p;
    return 0;
}


/*! remove directory hierarchically (like "rm -Rf")
    @param pathname path name to create
    @retval 0  success
    @retval -1 failure
 */
int rmrf(const char *pathname) {
    DIR *dp;
    struct dirent *ep;
    char buf[MAXPATHLEN + 1];
    char abs_filename[MAXPATHLEN + 1];
    struct stat st;
    char *p;
    int ret = 0;

    p = realpath(pathname, buf);
    if (p == NULL) return -1;
    if (stat(buf, &st) == 0 && S_ISREG(st.st_mode)) {
        return unlink(buf);
    }
    dp = opendir(pathname);
    if (dp != NULL) {
        while ((ep = readdir(dp)) != 0) {
            strcpy(abs_filename, buf);
            strcat(abs_filename, "/");
            strcat(abs_filename, ep->d_name);
            if (lstat(abs_filename, &st) < 0) {
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                if (strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) {
                    rmrf(abs_filename);
                }
            } else {
                ret = remove(abs_filename);
            }
        }
        closedir(dp);
    } else {
        return -1;
    }

    ret = remove(buf);
    return ret;
}


} // namespace apolloron
