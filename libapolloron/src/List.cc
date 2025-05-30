/******************************************************************************/
/*! @file List.cc
    @brief List class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/param.h>
#include <limits.h>
#include <stdlib.h>

#include "apolloron.h"

namespace apolloron {

/*! Constructor of List.
    @param void
    @return void
 */
List::List() {
    (*this).pList = NULL;
    (*this).pListSize = 0;
    (*this).pListCapacity = 0;
}


/*! Copy constructor of List.
    @param list
    @return void
 */
List::List(const List & list) {
    (*this).pList = NULL;
    (*this).pListSize = 0;
    set(list);
}


/*! Destructor of List.
    @param void
    @return void
 */
List::~List() {
    (*this).clear();
}


/*! Clear List.
    @param void
    @retval true  success
    @retval false failure
 */
bool List::clear() {
    long i;

    i = 0;
    while (i < (*this).pListSize) {
        delete (String *)((*this).pList[i]);
        i++;
    }
    if ((*this).pList != NULL) {
        delete [] (*this).pList;
    }
    (*this).pList = NULL;
    (*this).pListSize = 0;
    (*this).pListCapacity = 0;

    return true;
}


/*! Array operator of List.
    @param index index of array
    @return String Object
 */
String& List::operator [] (long index) {
    return *((*this).pList[index]);
}


/*! Array operator of List.
    @param index index of array
    @return String Object
 */
String& List::operator [] (int index) {
    return *((*this).pList[index]);
}


/*! Array operator of List.
    @param index index of array
    @return content of String Object
 */
const char* List::read(long index) const {
    if (index < 0 || (*this).pListSize <= index) return NULL;
    return (*((*this).pList[index])).c_str();
}


/*! Array operator of List.
    @param index index of array
    @return content of String Object
 */
const char* List::read(int index) const {
    if (index < 0 || (*this).pListSize <= index) return NULL;
    return (*((*this).pList[index])).c_str();
}


/*! Append String into List object
    @param value  object
    @retval true  success
    @retval false failure
 */
bool List::addString(const String & value) {
    // check if current capacity is insufficient
    if ((*this).pListSize >= (*this).pListCapacity) {
        // calculate new capacity (current size + 128)
        long new_capacity = (*this).pListSize + 128;
        String **new_pList = new String * [new_capacity];

        // copy existing data
        if ((*this).pList != NULL) {
            memcpy(new_pList, (*this).pList, sizeof(String *) * (*this).pListSize);
            delete [] (*this).pList;
        }
        (*this).pList = new_pList;
        (*this).pListCapacity = new_capacity;
    }

    // add new element
    (*this).pList[(*this).pListSize] = new String;
    *((*this).pList[(*this).pListSize]) = value;
    (*this).pListSize++;

    return true;
}


/*! Delete String from List.
    @param index index of array
    @retval true  success
    @retval false failure
 */
bool List::delString(long index) {
    if (index < 0 || (*this).pListSize <= index) return false;

    // Shift one after number of array index
    (*this).pListSize--;
    delete (String *)((*this).pList[index]);

    if ((*this).pListSize <= 0) {
        delete [] (*this).pList;
        (*this).pList = NULL;
    } else {
        String **new_pList;

        new_pList = new String * [(*this).pListSize];
        if (0 < index) {
            memcpy(new_pList, (*this).pList, sizeof(String *) * index);
        }
        if (index < (*this).pListSize) {
            memcpy(&(new_pList[index]), &((*this).pList[index + 1]), sizeof(String *) * ((*this).pListSize - index));
        }
        delete [] (*this).pList;
        (*this).pList = new_pList;
    }

    return true;
}


/*! Insert String into List.
    @param index  index of array
    @param value  object
    @retval true  success
    @retval false failure
 */
bool List::insertString(long index, const String & value) {
    if (index < 0 || (*this).pListSize < index) {
        return false;
    } else if ((*this).pListSize == index) {
        (*this).addString(value);
    } else {
        String **new_pList;

        // Create new array, then copy before number of array index
        (*this).pListSize++;
        new_pList = new String * [(*this).pListSize];
        if (0 < index) {
            memcpy(new_pList, (*this).pList, sizeof(String *) * index);
        }

        // Copy string
        new_pList[index] = new String;
        *(new_pList[index]) = value;

        // Copy of continuing array
        memcpy(&(new_pList[index + 1]), &((*this).pList[index]), sizeof(String *) * ((*this).pListSize - 1 - index));

        // Replace array
        if ((*this).pList != NULL) {
            delete [] (*this).pList;
        }
        (*this).pList = new_pList;

    }

    return true;
}


/*! Get length of rows of List.
    @param void
    @return length of rows
 */
long List::max() const {
    return (*this).pListSize;
}


/*! Copy List object.
    @param list  List object
    @retval true  success
    @retval false failed
 */
bool List::set(const List & list) {
    long i;

    (*this).clear();

    i = 0;
    while (i < list.pListSize) {
        (*this).addString(*(list.pList[i]));
        i++;
    }

    return true;
}


/*! Set operator of List.
    @param list  List object
    @return List object
 */
const List& List::operator = (const List &list) {
    (*this).set(list);

    return *this;
}


/*! Set directory and file list
    @param dirname directory name
    @param wildcard match pattern
    @param fullpath return fullpath or not
    @retval true  success
    @retval false failed
 */
bool List::setDirFileList(const String & dirname, const char *wildcard, bool fullpath) {
    DIR *dir;
    struct dirent *dirent;
    const char *cpstr;
    String filename;
    String fullpathname;
    char buf[MAXPATHLEN + 1];

    (*this).clear();

    if (wildcard == NULL) {
        return false;
    }

    dir = opendir(dirname);
    if (dir == NULL) {
        return false;
    }

    while ((dirent = readdir(dir)) != NULL) {
        cpstr = dirent->d_name;

        // ignore ".", ".."
        if (cpstr[0] == '.' && (cpstr[1] == '\0' || (cpstr[1] == '.' && cpstr[2] == '\0'))) {
            continue;
        }

        filename = cpstr;
        if (filename.isWcMatchAll(wildcard) == false) {
            filename.clear();
            continue;
        }
        filename.clear();

        if (fullpath == true) {
            fullpathname.sprintf("%s/%s", dirname.c_str(), cpstr);
            if (realpath(fullpathname.c_str(), buf) != NULL) {
                fullpathname = buf;
            }

            (*this).addString(fullpathname);
            fullpathname.clear();
        } else {
            (*this).addString(cpstr);
        }
    }

    closedir(dir);

    return true;
}


/*! Set file list
    @param dirname directory name
    @param wildcard match pattern
    @param fullpath return fullpath or not
    @retval true  success
    @retval false failed
 */
bool List::setFileList(const String & dirname, const char *wildcard, bool fullpath) {
    DIR *dir;
    struct dirent *dirent;
    const char *cpstr;
    String filename;
    String fullpathname;
    char buf[MAXPATHLEN + 1];

    (*this).clear();

    if (wildcard == NULL) {
        return false;
    }

    dir = opendir(dirname);
    if (dir == NULL) {
        return false;
    }

    while ((dirent = readdir(dir)) != NULL) {
        cpstr = dirent->d_name;

        // ignore ".", ".."
        if (cpstr[0] == '.' && (cpstr[1] == '\0' || (cpstr[1] == '.' && cpstr[2] == '\0'))) {
            continue;
        }

        filename = cpstr;
        if (filename.isWcMatchAll(wildcard) == false) {
            filename.clear();
            continue;
        }
        filename.clear();

        fullpathname.sprintf("%s/%s", dirname.c_str(), cpstr);
        if (realpath(fullpathname.c_str(), buf) != NULL) {
            fullpathname = buf;
        }
        if (isFileExist(fullpathname) == false) {
            fullpathname.clear();
            continue;
        }
        if (fullpath == true) {
            (*this).addString(fullpathname);
        } else {
            (*this).addString(cpstr);
        }
        fullpathname.clear();
    }

    closedir(dir);

    return true;
}


/*! Set directory list
    @param dirname directory name
    @param wildcard match pattern
    @param fullpath return fullpath or not
    @retval true  success
    @retval false failed
 */
bool List::setDirList(const String & dirname, const char *wildcard, bool fullpath) {
    DIR *dir;
    struct dirent *dirent;
    const char *cpstr;
    String filename;
    String fullpathname;
    char buf[MAXPATHLEN + 1];

    (*this).clear();

    if (wildcard == NULL) {
        return false;
    }

    dir = opendir(dirname);
    if (dir == NULL) {
        return false;
    }

    while ((dirent = readdir(dir)) != NULL) {
        cpstr = dirent->d_name;

        // ignore ".", ".."
        if (cpstr[0] == '.' && (cpstr[1] == '\0' || (cpstr[1] == '.' && cpstr[2] == '\0'))) {
            continue;
        }

        filename = cpstr;
        if (filename.isWcMatchAll(wildcard) == false) {
            filename.clear();
            continue;
        }
        filename.clear();

        fullpathname.sprintf("%s/%s", dirname.c_str(), cpstr);
        if (realpath(fullpathname.c_str(), buf) != NULL) {
            fullpathname = buf;
        }
        if (isFileExist(fullpathname) == false) {
            fullpathname.clear();
            continue;
        }
        if (fullpath == true) {
            (*this).addString(fullpathname);
        } else {
            (*this).addString(cpstr);
        }
        fullpathname.clear();
    }

    closedir(dir);

    return true;
}


/*! Set emails to List object (without MIME decoding).
    @param emails_str  comma separated emails string
           "Tarou Sasaki" <tarou@example.com>, example2
           --> List[0] = "Tarou Sasaki" <tarou@example.com>
               List[1] = example2
    @retval true  success
    @retval false failed
 */
bool List::setEmails(const String & emails_str) {
    long i, j, k, length;
    char quote, c;
    String tmp_email;

    (*this).clear();

    length = emails_str.len();
    if (length <= 0) {
        return true;
    }

    quote = 0;
    i = 0;
    j = 0;
    while (i < length) {
        if (emails_str[i] == '"') {
            quote = 1 - quote;
        }

        if (quote == 0 && emails_str[i] == ',') {
            tmp_email = emails_str.mid(j, i - j).trim();

            // ignore address group
            if (tmp_email[0] != '\0' && tmp_email[tmp_email.len()-1] == ';') {
                tmp_email = tmp_email.left(tmp_email.len()-1).trimR();
            }
            if (tmp_email[0] != '\0') {
                for (k = 0; tmp_email[k] != '\0'; k++) {
                    c = tmp_email[k];
                    if (c == '"' || c == '<' || c == '@') {
                        break;
                    }
                    if (c == ':') {
                        tmp_email = tmp_email.mid(k + 1).trimL();
                        break;
                    }
                }
            }

            if (tmp_email[0] != '\0') {
                (*this).addString(tmp_email);
            }
            j = i + 1;
        } else if (i == length - 1 && quote == 0) {
            tmp_email = emails_str.mid(j, i - j + 1).trim();

            // ignore address group
            if (tmp_email[0] != '\0' && tmp_email[tmp_email.len()-1] == ';') {
                tmp_email = tmp_email.left(tmp_email.len()-1).trimR();
            }
            if (tmp_email[0] != '\0') {
                for (k = 0; tmp_email[k] != '\0'; k++) {
                    c = tmp_email[k];
                    if (c == '"' || c == '<' || c == '@') {
                        break;
                    }
                    if (c == ':') {
                        tmp_email = tmp_email.mid(k + 1).trimL();
                        break;
                    }
                }
            }

            if (tmp_email[0] != '\0') {
                (*this).addString(tmp_email);
            }
            break;
        }

        i++;
    }

    tmp_email.clear();

    if (quote != 0) {
        return false;
    }

    return true;
}


/*! Set operator of List.
    @param value  List object
    @return List object
 */
const List& List::operator += (const String & value) {
    (*this).addString(value);
    return *this;
}


/*! Append String into List object.
    @param value  List object
    @retval true  success
    @retval false failed
 */
bool List::add(const String & value) {
    return (*this).addString(value);
}


/*! Insert String into List object.
    @param index  index of array
    @param value  List object
    @retval true  success
    @retval false failed
 */
bool List::insert(long index, const String & value) {
    return (*this).insertString(index, value);
}


/*! Remove String from List object.
    @param index  index of array
    @retval true  success
    @retval false failed
 */
bool List::remove(long index) {
    return (*this).delString(index);
}


} // namespace apolloron
