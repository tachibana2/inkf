/******************************************************************************/
/*! @file IMAPMailBoxList.cc
    @brief IMAPMailBoxList class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace apolloron {

/*! Constructor of IMAPMailBoxList.
    @param imap_stream instance of IMAPStream class
    @return void
 */
IMAPMailBoxList::IMAPMailBoxList(IMAPStream *imap_stream) {
    (*this).clear();
    (*this).imapStream = imap_stream;
}


/*! Destructor of IMAPMailBoxList.
    @param void
    @return void
 */
IMAPMailBoxList::~IMAPMailBoxList() {
    (*this).clear();
}


/*! Delete instance of IMAPMailBoxList.
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPMailBoxList::clear() {
    (*this).imapStream = NULL;
    (*this).mailboxListUtf7.clear();
    (*this).mailboxList.clear();
    (*this).namespaceDelimList.clear();

    return true;
}


/*! set mailbox list
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPMailBoxList::setMailBoxList() {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;
    long tag;
    String delim;
    String utf7name;

    (*this).mailboxListUtf7.clear();
    (*this).mailboxList.clear();
    (*this).namespaceDelimList.clear();

    if ((*this).imapStream == NULL || (*this).imapStream->isLoggedIn() == false) {
        return false;
    }

    (*this).imapStream->unselect();

    // set mailbox list
    tag = (*this).imapStream->getNextTag();
    send_line.sprintf("%05ld LIST \"\" *\r\n", tag);
    ret = (*this).imapStream->send(send_line);
    send_line.clear();
    if (ret == false) {
        return false;
    }

    // 00004 LIST "" *
    // * LIST (\HasChildren) "." "INBOX"
    // * LSUB () "." "INBOX.Drafts"
    // 00004 OK Completed
    receive_line = "";
    while(1) {
        delim = "";
        utf7name = "";
        receive_line = (*this).imapStream->receiveLine();
        if (receive_line.left(2) == "* ") {
            if (receive_line.left(7) == "* LIST " || receive_line.left(7) == "* LSUB ") {
                long col1, col2;
                col2 = 0;
                col1 = receive_line.search("\"", 7);
                if (0 <= col1) {
                    col2 = receive_line.search("\"", col1+1);
                    if (0 < col2) {
                        delim = receive_line.mid(col1+1, col2-col1-1).unescapeQuote();
                    }
                }
                if (0 < col1 && 0 < col2 && 0 < delim.len()) {
                    col1 = receive_line.search("\"", col2+1);
                    if (0 < col1) {
                        col2 = receive_line.search("\"", col1+1);
                        if (0 < col2) {
                            utf7name = receive_line.mid(col1+1, col2-col1-1).unescapeQuote();
                            (*this).mailboxListUtf7.add(utf7name);
                            (*this).mailboxList.add(utf7name.strconv("UTF-7-IMAP", "UTF-8").replace(delim, "/"));
                            (*this).namespaceDelimList.add(delim);
                        }
                    }
                }
            }
        } else {
            break;
        }
    }

    expect_line.sprintf("%05ld OK", tag);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }

    delim.clear();
    utf7name.clear();
    expect_line.clear();
    receive_line.clear();

    return true;
}


/*! reset mailbox list
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPMailBoxList::reload() {
    return (*this).setMailBoxList();
}


/*! get mailbox list in UTF-8
    @param void
    @return mailbox list in UTF-8
 */
const List &IMAPMailBoxList::getMailBoxList() {
    if ((*this).mailboxList.max() <= 0) {
        (*this).setMailBoxList();
    }

    return (*this).mailboxList;
}


/*! get mailbox list in modified UTF-7
    @param void
    @return mailbox list in modified UTF-7
 */
const List &IMAPMailBoxList::getMailBoxListUtf7() {
    if ((*this).mailboxListUtf7.max() <= 0) {
        (*this).setMailBoxList();
    }

    return (*this).mailboxListUtf7;
}


/*! get namespace delimiter list ('.', '/' or '\\')
    @param void
    @return namespace list ("personal", "other" or "shared")
 */
const List &IMAPMailBoxList::getNamespaceDelimList() {
    if ((*this).namespaceDelimList.max() <= 0) {
        (*this).setMailBoxList();
    }

    return (*this).namespaceDelimList;
}


/*! load mailbox from file
    @param filename file name to load
    @retval true   success
    @retval false  failure
 */
bool IMAPMailBoxList::loadFile(const String &filename) {
    String str, line, value1, value2, value3, tmp_str;
    long len, i, j, k, l;

    (*this).mailboxListUtf7.clear();
    (*this).mailboxList.clear();
    (*this).namespaceDelimList.clear();

    // load from file
    len = str.loadFile(filename.c_str());
    if (len < 0L) {
        return false;
    }

    for (i = 0L; i < len; i++) {
        for (j = 0L; i + j < len; j++) {
            if (str[i + j] == '\n') {
                break;
            }
        }
        if (0L < j) {
            line = str.mid(i, j).trim();

            if (0L < line.len() && line[0] != '#') {
                if (1L <= line.len() && line[0] == '"') {
                    for (k = 1L; k < line.len(); k++) {
                        if (line[k-1] != '\\' && line[k] == '"') {
                            break;
                        }
                    }
                    tmp_str = line.mid(1, k-2);
                    value1 = tmp_str.unescapeQuote();
                    tmp_str.clear();
                    l = line.len();
                    k++;
                    while (k < l) {
                        if (line[k] == ',') {
                            k++;
                            break;
                        }
                        k++;
                    }
                } else {
                    k = line.searchChar(',');
                    if (k < 0) {
                        k = line.len();
                    }
                    if (0 <= k) {
                        value1 = line.left(k);
                    } else {
                        value1 = "";
                    }
                    k++;
                }
                line = line.mid(k);

                if (1L <= line.len() && line[0] == '"') {
                    for (k = 1L; k < line.len(); k++) {
                        if (line[k-1] != '\\' && line[k] == '"') {
                            break;
                        }
                    }
                    tmp_str = line.mid(1, k-2);
                    value2 = tmp_str.unescapeQuote();
                    tmp_str.clear();
                    l = line.len();
                    k++;
                    while (k < l) {
                        if (line[k] == ',') {
                            k++;
                            break;
                        }
                        k++;
                    }
                } else {
                    k = line.searchChar(',');
                    if (k < 0) {
                        k = line.len();
                    }
                    if (0 <= k) {
                        value2 = line.left(k);
                    } else {
                        value2 = "";
                    }
                    k++;
                }
                line = line.mid(k);

                if (1L <= line.len() && line[0] == '"') {
                    for (k = 1L; k < line.len(); k++) {
                        if (line[k-1] != '\\' && line[k] == '"') {
                            break;
                        }
                    }
                    tmp_str = line.mid(1, k-2);
                    value3 = tmp_str.unescapeQuote();
                    tmp_str.clear();
                    l = line.len();
                } else {
                    k = line.searchChar(',');
                    if (k < 0) {
                        k = line.len();
                    }
                    if (0 <= k) {
                        value3 = line.left(k);
                    } else {
                        value3 = "";
                    }
                }

                (*this).mailboxListUtf7.add(value1);
                (*this).mailboxList.add(value2);
                (*this).namespaceDelimList.add(value2);
            }
            line.clear();
            i += j;
        }
    }

    str.clear();

    return false;
}


/*! save mailbox list to file
    @param filename file name to save
    @retval true   success
    @retval false  failure
 */
bool IMAPMailBoxList::saveFile(const String &filename) {
    long m, i;
    String str, tmp_str1, tmp_str2;
    bool ret;

    if ((*this).mailboxListUtf7.max() <= 0 || (*this).mailboxList.max() <= 0 ||
            (*this).namespaceDelimList.max() <= 0) {
        (*this).setMailBoxList();
    }


    // save to file
    m = (*this).mailboxListUtf7.max();
    str.sprintf("# %ld mailboxlist\n", m);
    for (i = 0; i < m; i++) {
        tmp_str1 = (*this).mailboxListUtf7[i].trim();
        tmp_str2 = (*this).mailboxListUtf7[i].escapeQuote();
        if (tmp_str1 == tmp_str2 && 0L < tmp_str1.len()) {
            str += tmp_str1;
        } else {
            str += "\",";
            str += tmp_str2;
            str += "\",";
        }
        tmp_str1 = (*this).mailboxList[i].trim();
        tmp_str2 = (*this).mailboxList[i].escapeQuote();
        if (tmp_str1 == tmp_str2) {
            str += tmp_str1;
        } else {
            str += "\",";
            str += tmp_str2;
            str += "\",";
        }
        tmp_str1 = (*this).namespaceDelimList[i].trim();
        tmp_str2 = (*this).namespaceDelimList[i].escapeQuote();
        if (tmp_str1 == tmp_str2) {
            str += tmp_str1;
            str += "\n";
        } else {
            str += "\"";
            str += tmp_str2;
            str += "\"\n";
        }
    }

    ret = str.saveFile(filename);

    tmp_str1.clear();
    tmp_str2.clear();
    str.clear();

    return ret;
}


} // namespace
