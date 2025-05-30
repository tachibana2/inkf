/******************************************************************************/
/*! @file IMAPSearch.cc
    @brief IMAPSearch class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace apolloron {

/*! Constructor of IMAPSearch.
    @param imap_stream instance of IMAPStream class
    @param mailbox mailbox name in modified UTF-7
    @param sort_cond sort condition (ex. "REVERSE DATE")
           ARRIVAL ... INTERNAL DATE
           DATE ... Header Date
           TO ... Header To
           CC ... Header Cc
           FROM ... Header From
           SUBJECT ... Subject
           SIZE ... Size
           REVERSE ... reverse XXX
    @param search_cond search condition (ex. "ALL")
           ALL
           BODY "Hello"
           SINCE 1-Jan-2008 BEFORE 1-May-2008
    @return void
 */
IMAPSearch::IMAPSearch(IMAPStream *imap_stream, const String &mailbox,
                       const String &sort_cond, const String &search_cond, const String &search_charset) {
    (*this).uids = (long *)NULL;
    (*this).clear();

    (*this).imapStream = imap_stream;
    (*this).mailbox = mailbox;
    (*this).sortCondition = sort_cond;
    (*this).searchCondition = search_cond;
    (*this).searchCharSet = search_charset;
}


/*! Destructor of IMAPSearch.
    @param void
    @return void
 */
IMAPSearch::~IMAPSearch() {
    (*this).imapStream = NULL;
    (*this).mailbox.clear();
    (*this).sortCondition.clear();
    (*this).searchCondition.clear();
    (*this).searchCharSet.clear();
    (*this).clear();
}


/*! Delete instance of IMAPSearch.
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPSearch::clear() {
    if ((*this).uids != (long *)NULL) {
        delete [] uids;
        uids = (long *)NULL;
    }

    return true;
}


/*! set UID list
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPSearch::setUIDs() {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;
    long i, tag;

    if ((*this).uids != (long *)NULL) {
        delete [] uids;
        uids = (long *)NULL;
    }

    if ((*this).imapStream == NULL || (*this).imapStream->isLoggedIn() == false ||
            (*this).imapStream->getCapability().searchCase("SORT") < 0L) {
        return false;
    }

    if ((*this).imapStream->getSelectedMailBox() != (*this).mailbox) {
        (*this).imapStream->examine((*this).mailbox);
    }

    // set uids by search and sort conditions
    tag = (*this).imapStream->getNextTag();
    send_line.sprintf("%05ld UID SORT (%s) %s %s\r\n", tag,
                      (*this).sortCondition.c_str(),
                      (*this).searchCharSet.c_str(),
                      (*this).searchCondition.c_str());
    ret = (*this).imapStream->send(send_line);
    send_line.clear();
    if (ret == false) {
        return false;
    }

    do {
        receive_line = (*this).imapStream->receiveLine();
        if (receive_line.left(7) == "* SORT ") {
            const char *s;
            long col, length;

            length = receive_line.len();

            if ((*this).uids != (long *)NULL) {
                delete [] uids;
                uids = (long *)NULL;
            }

            if (7 < length) {
                uids = new long [(length / 2) + 2];
                uids[0] = 0L;
            }

            s = receive_line.c_str();
            col = 7;
            i = 0;
            while (col < length) {
                uids[i++] = atol(s + col);
                uids[i] = 0L;
                while (col < length) {
                    if (s[col] == ' ') {
                        col++;
                        break;
                    }
                    col++;
                }
            }
            break;
        }
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", tag);
    if (receive_line.left(expect_line.len()) != expect_line) {
        expect_line.clear();
        receive_line.clear();
        return false;
    }

    expect_line.clear();
    receive_line.clear();

    return true;
}


/*! get UID list
    @param void
    @retval true   success
    @retval false  failure
 */
const long *IMAPSearch::getUIDs() {
    return (*this).uids;
}


/*! load search result from file
    @param filename file name to load
    @retval true   success
    @retval false  failure (even if mailbox is different)
 */
bool IMAPSearch::loadFile(const String &filename) {
    Keys keys;
    long i, j, length;
    String orig_mailbox;
    String orig_sortcond;
    String orig_searchcond;
    String orig_searchcharset;
    bool ret;

    orig_mailbox = (*this).mailbox;
    orig_sortcond = (*this).sortCondition;
    orig_searchcond = (*this).searchCondition;
    orig_searchcharset = (*this).searchCharSet;

    (*this).clear();

    // load from file
    ret = keys.loadFile(filename.c_str());
    if (ret == true && 0 < (length = keys["UIDS"].len())) {
        String str;
        (*this).mailbox = keys["MAILBOX"];
        (*this).sortCondition = keys["SORT_COND"];
        (*this).searchCondition = keys["SEARCH_COND"];
        (*this).searchCharSet = keys["SEARCH_CHARSET"];
        if ((*this).mailbox != orig_mailbox ||
                (*this).sortCondition != orig_sortcond ||
                (*this).searchCondition != orig_searchcond ||
                (*this).searchCharSet != orig_searchcharset) {
            (*this).mailbox = orig_mailbox;
            (*this).sortCondition = orig_sortcond;
            (*this).searchCondition = orig_searchcond;
            (*this).searchCharSet = orig_searchcharset;
            return false;
        }
        str = keys["UIDS"];
        if ((*this).uids == (long *)NULL) {
            (*this).uids = new long [(length/2)+10];
        }
        i = 0;
        j = 0;
        while (i < length) {
            (*this).uids[j] = atol(str.c_str() + i);
            if ((*this).uids[j] == 0L) {
                break;
            }
            j++;
            i = str.search(",", i);
            if (0 < i) {
                i++;
            } else {
                break;
            }
        }
        (*this).uids[j] = 0L;
        str.clear();
    }

    if (ret == false) {
        (*this).mailbox = orig_mailbox;
        (*this).sortCondition = orig_sortcond;
        (*this).searchCondition = orig_searchcond;
        (*this).searchCharSet = orig_searchcharset;
    }

    return ret;
}


/*! save search result to file
    @param filename file name to save
    @retval true   success
    @retval false  failure
 */
bool IMAPSearch::saveFile(const String &filename) {
    Keys keys;
    String str;
    long i;
    bool ret;

    keys["MAILBOX"] = (*this).mailbox;
    keys["SORT_COND"] = (*this).sortCondition;
    keys["SEARCH_COND"] = (*this).searchCondition;
    keys["SEARCH_CHARSET"] = (*this).searchCharSet;

    str = "";
    if ((*this).uids != (long *)NULL) {
        for (i = 0; i < (*this).uids[i]; i++) {
            if (i != 0) {
                str.add(",");
            }
            str.add(uids[i]);
        }
    }
    keys["UIDS"] = str;
    str.clear();

    // save to file
    ret = keys.saveFile(filename.c_str());

    keys.clear();

    return ret;
}


} // namespace
