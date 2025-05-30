/******************************************************************************/
/*! @file utils.cc
    @brief Utility functions.
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "apolloron.h"


namespace {

/*! Check whether string is valid as dot-atom.
   @param dot_atom dot-atom string
   @retval true  valid
   @retval false not valid
 */
static bool _is_email_dot_atom(const char *dot_atom) {
    const char ATEXT[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&\'*+-/=?^_`{|}~";
    char tmp_dot_atom[256];
    char *pend, *pstr;
    char c;
    bool isvalid;
    long i;

    if (dot_atom == NULL || dot_atom[0] == '\0') {
        return false;
    }

    strcpy(tmp_dot_atom, dot_atom);
    pstr = tmp_dot_atom;
    isvalid = true;

    while (pstr != NULL) {
        pend = strchr(pstr, '.');
        if (pend == NULL) {
            pend = pstr + strlen(pstr);
        }
        c = *pend;
        *pend = '\0';

        if (pstr[0] == '\0') {
            isvalid = false;
            break;
        }
        for (i = 0; pstr[i] != '\0'; i++) {
            if (strchr(ATEXT, pstr[i]) == NULL) {
                isvalid = false;
                break;
            }
        }
        if (isvalid == false) {
            break;
        }

        if (c == '\0') {
            break;
        } else {
            pstr = pend + 1;
        }
    }

    return isvalid;
}


/*! Check whether string is valid as quoted-string.
   @param quoted_string quoted-string
   @retval true  valid
   @retval false not valid
 */
static bool _is_email_quoted_string(const char *quoted_string) {
    const char WSP[] = " \t\v";
    char tmp_quoted_string[256];
    char *pstr, *tokptr;
    bool isvalid;
    long i;
    int c;

    if (quoted_string == NULL || quoted_string[0] == '\0') {
        return false;
    }

    if (quoted_string[0] != '\"' ||
            quoted_string[strlen(quoted_string) - 1] != '\"') {
        return false;
    }

    strcpy(tmp_quoted_string, quoted_string + 1);
    tmp_quoted_string[strlen(tmp_quoted_string) - 1] = '\0';
    pstr = tmp_quoted_string;
    isvalid = true;

    tokptr = NULL;
    while ((pstr = strtok_r(pstr, WSP, &tokptr)) != NULL) {
        for (i = 0; pstr[i] != '\0'; i++) {
            if (pstr[i] == '\\') {
                c = (unsigned char)pstr[i + 1];
                if (!((1 <= c && c <= 9) || c == 11 || c == 12 ||
                        (14 <= c && c <= 127))) {
                    isvalid = false;
                    break;
                }
                i++;
            } else {
                c = (unsigned char)pstr[i];
                if (!((1 <= c && c <= 8) || c == 11 || c == 12 ||
                        (14 <= c && c <= 31) || c == 33 || (35 <= c && c <= 91) ||
                        (93 <= c && c <= 127))) {
                    isvalid = false;
                    break;
                }
            }
        }
        if (isvalid == false) {
            break;
        }
        pstr = NULL;
    }

    return isvalid;
}

} // namespace


namespace apolloron {

/*!
   Check the file is exist or not
   @param  filename  file name
   @retval true  file is exist
   @retval false file is not exist
   @author tachibana
 */
bool isFileExist(const String &filename) {
    struct stat st;
    const char *fn;

    fn = filename.c_str();
    if (fn == NULL || fn[0] == '\0') {
        return false;
    }

    if (access(fn, F_OK) != 0) {
        // filename is not exist, or access error
        return false;
    }

    if (stat(fn, &st) == 0 && S_ISREG(st.st_mode)) {
        // found a normal file
        return true;
    }

    return false;
}


/*!
   Check the directory is exist or not
   @param  dirname  directory name
   @retval true  directory is exist
   @retval false directory is not exist
 */
bool isDirExist(const String &dirname) {
    struct stat st;
    const char *dn;

    dn = dirname.c_str();
    if (dn == NULL || dn[0] == '\0') {
        return false;
    }

    if (access(dn, F_OK) != 0) {
        // dirname is not exist, or access error
        return false;
    }

    if (stat(dn, &st) == 0 && S_ISDIR(st.st_mode)) {
        // found the directory
        return true;
    }

    return false;
}


/*! Check whether string is valid as email. ex. "user1@example.com"
   @param  email email string
   @retval true  valid
   @retval false not valid
   @return pointer of sub-string in target string
 */
bool isEmailAddress(const String &email) {
    char *local_part, *domain;
    long length;

    length = email.len();
    if (length <= 0 || 256 < length) {
        return false;
    }

    local_part = new char [length + 1];
    strcpy(local_part, email.c_str());
    domain = strchr(local_part, '@');
    if (domain == NULL) {
        delete [] local_part;
        return true;
    }
    *domain = '\0';
    domain++;

    if (local_part[0] != '\0' && isEmailLocalPart(local_part) &&
            domain[0] != '\0' && isEmailDomain(domain)) {
        delete [] local_part;
        return true;
    }

    delete [] local_part;
    return false;
}


/*! Check whether string is valid as local-part of email.
   @param  local_part local-part string
   @retval true  valid
   @retval false not valid
 */
bool isEmailLocalPart(const String &local_part) {
    long length;

    length = local_part.len();
    if (length <= 0 || 256 <= length) {
        return false;
    }

    if (0 <= local_part.searchChar('.')) {
        char *pstr;
        long i, j;

        pstr = new char [length + 1];
        j = 0;
        for (i = 0; local_part[i] != '\0'; i++) {
            if (local_part[i] != '.') {
                pstr[j] = local_part[i];
                j++;
            }
        }
        pstr[j] = '\0';
        if (_is_email_dot_atom(pstr) || _is_email_quoted_string(pstr)) {
            delete [] pstr;
            return true;
        }
        delete [] pstr;
    } else {
        if (_is_email_dot_atom(local_part.c_str()) ||
                _is_email_quoted_string(local_part.c_str())) {
            return true;
        }
    }

    return false;
}


/*! Check whether string is valid as domain of email.
   @param  domain domain string
   @retval true  valid
   @retval false not valid
 */
bool isEmailDomain(const String &domain) {
    long length;

    length = domain.len();
    if (length <= 0 || domain[0] == '.' || 256 <= length) {
        return false;
    }

    if (_is_email_dot_atom(domain.c_str())) {
        return true;
    }

    return false;
}

} // namespace apolloron
