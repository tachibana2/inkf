/******************************************************************************/
/*! @file IMAPStream.cc
    @brief IMAPStream class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace apolloron {

/*! Constructor of IMAPStream.
    @param void
    @return void
 */
IMAPStream::IMAPStream() {
    (*this).clear();
}


/*! Destructor of IMAPStream.
    @param void
    @return void
 */
IMAPStream::~IMAPStream() {
    (*this).clear();
}


/*! Delete instance of IMAPStream.
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::clear() {
    (*this).host.clear();
    (*this).port.clear();
    (*this).user.clear();
    (*this).passwd.clear();
    (*this).loggedIn = false;
    (*this).mailbox.clear();
    (*this).readOnly = true;
    (*this).capability.clear();
    (*this).socket.clear(); // IMAP stream
    (*this).nextTag = 1;
    tmpString.clear();

    return true;
}


/*! set timeout of Socket
    @param sec timeout (second)
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::setTimeout(int sec) {
    return (*this).socket.setTimeout(sec);
}


/*! IMAP login
    @param user   user id
    @param passwd password
    @param host   host name
    @param port   port number or service name
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::login(const String &user, const String &passwd,
                       const String &host, const String &port,
                       bool ssl) {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

#if __OPENSSL == 1
    if (ssl && port == "143") {
        (*this).port = "993"; // default IMAPS port
    } else
#endif
    {
        (*this).port = port;
    }

    (*this).host = host;
    (*this).user = user;
    (*this).passwd = passwd;

#if __OPENSSL == 1
    if (ssl && port == "143") {
        (*this).port = "993"; // default IMAPS port
    } else
#endif
    {
        (*this).port = port;
    }

    ret = (*this).socket.connect(host.c_str(), port.c_str(), ssl);
    if (ret == false) {
        (*this).socket.clear();
        (*this).capability.clear();
        (*this).loggedIn = false;
        return false;
    }

    if ((*this).socket.receiveLine().left(4) != "* OK") {
        (*this).socket.disconnect();
        (*this).socket.clear();
        (*this).capability.clear();
        (*this).loggedIn = false;
        return false;
    }

    send_line.sprintf("%05ld CAPABILITY\r\n", (*this).nextTag++);
    ret = (*this).socket.send(send_line);
    if (ret == false) {
        (*this).socket.disconnect();
        (*this).socket.clear();
        (*this).loggedIn = false;
        return false;
    }

    receive_line = (*this).socket.receiveLine();
    if (receive_line.left(2) != "* ") {
        send_line.sprintf("%05ld LOGOUT\r\n", (*this).nextTag++);
        (*this).socket.send(send_line);
        (*this).socket.disconnect();
        (*this).socket.clear();
        (*this).loggedIn = false;
        return false;
    }
    (*this).capability = (receive_line.c_str()+2);

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if ((*this).socket.receiveLine().left(expect_line.len()) != expect_line) {
        send_line.sprintf("%05ld LOGOUT\r\n", (*this).nextTag++);
        (*this).socket.send(send_line);
        (*this).socket.disconnect();
        (*this).socket.clear();
        (*this).loggedIn = false;
        return false;
    }

    if (0 < (*this).capability.searchCase("AUTH=CRAM-MD5")) {
        // auth CRAM-MD5
        send_line.sprintf("%05ld AUTHENTICATE CRAM-MD5\r\n", (*this).nextTag++);
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line.left(2) != "+ ") {
            send_line.sprintf("%05ld LOGOUT\r\n", (*this).nextTag++);
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        ret = (*this).socket.send(passwd.cramMd5(user, receive_line.mid(2).trimR())+"\r\n");
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        do {
            receive_line = (*this).socket.receiveLine();
        } while (receive_line.left(2) == "* ");

        expect_line.sprintf("%05ld OK", (*this).nextTag-1);
        if (receive_line.left(expect_line.len()) != expect_line) {
            send_line.sprintf("%05ld LOGOUT\r\n", (*this).nextTag++);
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }
    } else {
        // auth LOGIN
        send_line.sprintf("%05ld LOGIN %s \"%s\"\r\n", (*this).nextTag++, user.c_str(), passwd.escapeQuote().c_str());
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        do {
            receive_line = (*this).socket.receiveLine();
        } while (receive_line.left(2) == "* ");

        expect_line.sprintf("%05ld OK", (*this).nextTag-1);
        if (receive_line.left(expect_line.len()) != expect_line) {
            send_line.sprintf("%05ld LOGOUT\r\n", (*this).nextTag++);
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }
    }

    (*this).loggedIn = true;

    receive_line.clear();
    send_line.clear();
    expect_line.clear();

    return true;
}


/*! logout and kill socket stream
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::logout() {
    String send_line;

    if ((*this).loggedIn == true && (*this).socket.connected()) {
        send_line.sprintf("%05ld LOGOUT\r\n", (*this).nextTag++);
        (*this).socket.send(send_line);
    }

    (*this).socket.disconnect();
    (*this).loggedIn = false;

    (*this).socket.clear();
    send_line.clear();

    return false;
}


/*! conrirm login status
    @param void
    @retval true   logged in
    @retval false  not logged in
 */
bool IMAPStream::isLoggedIn() const {
    if ((*this).loggedIn == true && (*this).socket.connected()) {
        return true;
    }

    return false;
}


/*! select mailbox
    @param mailbox mailbox name in modified UTF-7
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::select(const String &mailbox) {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

    if (!((*this).loggedIn == true && (*this).socket.connected())) {
        return false;
    }

    send_line.sprintf("%05ld SELECT %s\r\n", (*this).nextTag++, mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret != true) {
        return false;
    }

    do {
        receive_line = (*this).socket.receiveLine();
        if (0 <= receive_line.search(" OK [READ-WRITE]")) {
            (*this).readOnly = false;
        } else if (0 <= receive_line.search(" OK [READ-ONLY]")) {
            (*this).readOnly = true;
        }
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }
    (*this).mailbox = mailbox;

    return true;
}


/*! examine mailbox
    @param mailbox mailbox name in modified UTF-7
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::examine(const String &mailbox) {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

    (*this).readOnly = true;

    if (!((*this).loggedIn == true && (*this).socket.connected())) {
        return false;
    }

    send_line.sprintf("%05ld EXAMINE %s\r\n", (*this).nextTag++, mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret != true) {
        return false;
    }

    do {
        receive_line = (*this).socket.receiveLine();
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }
    (*this).mailbox = mailbox;

    return true;
}


/*! unselect mailbox
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::unselect() {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

    (*this).readOnly = true;

    if (!((*this).loggedIn == true && (*this).socket.connected())) {
        return false;
    }

    if ((*this).mailbox.len() == 0) {
        return true;
    }

    send_line.sprintf("%05ld UNSELECT %s\r\n", (*this).nextTag++, mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret != true) {
        return false;
    }

    receive_line = (*this).socket.receiveLine();
    while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }
    (*this).mailbox = "";

    return true;
}


/*! get IMAP Capability
    @param void
    @return IMAP Capability String
 */
const String& IMAPStream::getCapability() {
    if ((*this).loggedIn == false) {
        (*this).capability.clear();
    }

    return (*this).capability;
}


/*! reffer selected (or examined) IMAP mailbox in modified UTF-7
    @param void
    @return IMAP Capability String
 */
const String& IMAPStream::getSelectedMailBox() {
    if (!((*this).loggedIn == true && (*this).socket.connected())) {
        (*this).mailbox.clear();
    }
    return (*this).mailbox;
}


/*! confirm read only mailbox or not
    @param void
    @retval true   read only
    @retval false  read write
 */
bool IMAPStream::isReadOnly() {
    if (!((*this).loggedIn == true && (*this).socket.connected())) {
        (*this).readOnly = true;
    } else if (mailbox.len() <= 0) {
        (*this).readOnly = true;
    }

    return (*this).readOnly;
}


/*! append a mail to selected mailbox
    @param eml mail source
    @param internal_date date and time to append
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::append(const String &eml, const String &internal_date) {
    String strdate;
    DateTime datetime;
    long i, j, length, header_date_len;
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

    if ((*this).isReadOnly() || (*this).mailbox.len() <= 0 || eml.len() <= 4) {
        return false;
    }

    if (0 < internal_date.len()) {
        strdate = internal_date.trim();
    } else {
        strdate = "";
        length = eml.len();
        for (i = 0; i < length; i++) {
            if (i == 0 || (eml[i-1] == '\n' && !strncasecmp(eml.c_str()+i, "Date", 4))) {
                i += 4;
                while (eml[i] == ' ') i++;
                if(eml[i] == ':') i++;
                while (eml[i] == ' ') i++;
                for (j = i; j < length; j++) {
                    if (eml[j] == '\n' && (eml[j+1] != ' ' || eml[j+1] == '\t' || eml[j+1] == '\v')) {
                        continue;
                    } else if (eml[j] == '\n') {
                        break;
                    }
                }
                header_date_len = j - i;
                strdate = eml.mid(i, header_date_len).trimR();
                break;
            }
            if (eml[i] == '\n' && (eml[i+1] == '\n' || (eml[i+1] == '\r' && eml[i+2] == '\n'))) {
                break;
            }
        }
    }

    if (!datetime.set(strdate)) {
        datetime.setNow();
    }


    // append mail
    send_line.sprintf("%05ld APPEND %s () \"%s\" {%ld}\r\n",
                      (*this).nextTag++, (*this).mailbox.c_str(),
                      datetime.toString(DATEFORMAT_INTERNALDATE).c_str(), eml.len());
    ret = (*this).socket.send(send_line);
    if (ret == false) {
        return false;
    }

    if ((*this).socket.receiveLine().left(1) != "+") {
        return false;
    }

    ret = (*this).socket.send(eml);

    do {
        receive_line = (*this).socket.receiveLine();
    } while (receive_line.left(2) == "* ");


    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }

    return true;
}


/*! create mailbox
    @param mailbox mailbox name in modified UTF-7
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::createMailBox(const String &mailbox) {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

    if ((*this).isReadOnly() || (*this).mailbox.len() <= 0) {
        return false;
    }

    // Create
    send_line.sprintf("%05ld CREATE %s\r\n", (*this).nextTag++, (*this).mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret == false) {
        return false;
    }

    do {
        receive_line = (*this).socket.receiveLine();
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }

    // Subscribe
    send_line.sprintf("%05ld SUBSCRIBE %s\r\n", (*this).nextTag++, (*this).mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret == false) {
        return false;
    }

    do {
        receive_line = (*this).socket.receiveLine();
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }

    return true;
}


/*! delete mailbox
    @param mailbox mailbox name in modified UTF-7
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::deleteMailBox(const String &mailbox) {
    String receive_line;
    String send_line;
    String expect_line;
    bool ret;

    if ((*this).isReadOnly() || (*this).mailbox.len() <= 0) {
        return false;
    }

    // Unsubscribe
    send_line.sprintf("%05ld UNSUBSCRIBE %s\r\n", (*this).nextTag++, (*this).mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret == false) {
        return false;
    }

    do {
        receive_line = (*this).socket.receiveLine();
    } while (receive_line.left(2) == "* ");

    //expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    //if (receive_line.left(expect_line.len()) != expect_line) {
    //  return false;
    //}


    // Delete
    send_line.sprintf("%05ld DELETE %s\r\n", (*this).nextTag++, (*this).mailbox.c_str());
    ret = (*this).socket.send(send_line);
    if (ret == false) {
        return false;
    }

    do {
        receive_line = (*this).socket.receiveLine();
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", (*this).nextTag-1);
    if (receive_line.left(expect_line.len()) != expect_line) {
        return false;
    }

    return true;
}


/*! send String to IMAP stream
    @param line String for send
    @retval true   success
    @retval false  failure
 */
bool IMAPStream::send(const String& line) {
    return (*this).socket.send(line);
}


/*! get next tag (increment automatically)
    @param void
    @return next tag
 */
long IMAPStream::getNextTag() {
    return (*this).nextTag++;
}


/*! receive one line from IMAP stream
    @param void
    @return received line
 */
String& IMAPStream::receiveLine() {
    return (*this).socket.receiveLine();
}


/*! Receive data from IMAP stream
    @param size  receiving size
    @return received data
 */
String& IMAPStream::receive(long size) {
    return (*this).socket.receive(size);
}

} // namespace
