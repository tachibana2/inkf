/******************************************************************************/
/*! @file POP3Stream.cc
    @brief POP3Stream class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace apolloron {

/*! Constructor of POP3Stream.
    @param void
    @return void
 */
POP3Stream::POP3Stream() {
    (*this).clear();
}


/*! Destructor of POP3Stream.
    @param void
    @return void
 */
POP3Stream::~POP3Stream() {
    (*this).clear();
}


/*! Delete instance of POP3Stream.
    @param void
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::clear() {
    (*this).host.clear();
    (*this).port.clear();
    (*this).user.clear();
    (*this).passwd.clear();
    (*this).loggedIn = false;
    (*this).socket.clear(); // POP3 stream
    tmpString.clear();

    return true;
}


/*! set timeout of Socket
    @param sec timeout (second)
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::setTimeout(int sec) {
    return (*this).socket.setTimeout(sec);
}


/*! POP3 login
    @param user   user id
    @param passwd password
    @param host   host name
    @param port   port number or service name
    @param apop   APOP mode
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::login(const String &user, const String &passwd,
                       const String &host, const String &port,
                       bool apop, bool ssl) {
    String start_line;
    String receive_line;
    String send_line;
    bool ret;

    (*this).host = host;
    (*this).port = port;
    (*this).user = user;
    (*this).passwd = passwd;

#if __OPENSSL == 1
    if (ssl && (*this).port == "110") {
        (*this).port = "995"; // default POP3S port
    }
#endif

    ret = (*this).socket.connect(host.c_str(), port.c_str(), ssl);
    if (ret == false) {
        (*this).socket.clear();
        (*this).loggedIn = false;
        return false;
    }

    start_line = (*this).socket.receiveLine();
    if (start_line[0] != '+') {
        send_line = "QUIT\r\n";
        (*this).socket.send(send_line);
        (*this).socket.disconnect();
        (*this).socket.clear();
        (*this).loggedIn = false;
        return false;
    }

    if (apop == true) {
        // auth APOP
        long pos1, pos2 = 0L;
        String auth_str;
        String md5;

        if ((pos1 = start_line.search("<")) < 0L || (pos2 = start_line.search(">", pos1 + 1)) < 0L) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }
        auth_str = start_line.mid(pos1, pos2 - pos1 + 1) + passwd;
        md5 = auth_str.md5();

        send_line.sprintf("APOP %s %s\r\n", user.c_str(), md5.c_str());
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            send_line = "QUIT\r\n";
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

    } else {
        // auth POP3
        send_line.sprintf("USER %s\r\n", user.c_str());
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            send_line = "QUIT\r\n";
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        send_line.sprintf("PASS %s\r\n", passwd.c_str());
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            send_line = "QUIT\r\n";
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }
    }

    (*this).loggedIn = true;

    start_line.clear();
    receive_line.clear();
    send_line.clear();

    return true;
}


/*! logout and kill socket stream
    @param void
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::logout() {
    String send_line;

    if ((*this).loggedIn == true && (*this).socket.connected()) {
        send_line.sprintf("QUIT\r\n");
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
bool POP3Stream::isLoggedIn() {
    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        bool ret;

        // NOOP
        send_line = "NOOP\r\n";
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            send_line = "QUIT\r\n";
            (*this).socket.send(send_line);
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        return true;
    } else {
        return false;
    }

    return false;
}


/*! STAT command
    @param [out]total_num  total message number
    @param [out]total_size total message size
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::stat(long &total_num, long &total_size) {
    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        long pos1, pos2 = 0L;
        bool ret;

        send_line = "STAT\r\n";
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            return false;
        }
        if ((pos1 = receive_line.search(" ")) < 0L || (pos2 = receive_line.search(" ", pos1 + 1)) < 0L) {
            return false;
        }

        total_num = atol(receive_line.c_str() + pos1 + 1);
        total_size = atol(receive_line.c_str() + pos2 + 1);
    } else {
        return false;
    }

    return true;
}


/*! LIST command
    @param [out]size  sizelist
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::size(List &size) {
    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        long pos;
        long i;
        bool ret;

        send_line = "LIST\r\n";
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            return false;
        }

        size.clear();
        i = 0;
        while(1) {
            receive_line = (*this).socket.receiveLine();
            if (receive_line == "" || receive_line[0] == '.') {
                break;
            }

            if ((pos = receive_line.search(" ")) < 0L) {
                return false;
            }

            if (atol(receive_line.c_str()) == i + 1) {
                size.add(atol(receive_line.c_str() + pos + 1));
            }
            i++;
        }
    } else {
        return false;
    }

    return true;
}

/*! TOP n 0 command
    @param n number of mail
    @return mail header string
 */
String& POP3Stream::fetchHeader(long n) {
    tmpString.clear();

    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        bool ret;

        send_line.sprintf("TOP %ld 0\r\n", n);
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return tmpString;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            return tmpString;
        }

        while(1) {
            receive_line = (*this).socket.receiveLine();
            if (receive_line == "" || receive_line == "." || (receive_line[0] == '.' && ((receive_line[1] == '\r' && receive_line[2] == '\n') || receive_line[1] == '\n'))) {
                break;
            }
            if (receive_line[0] == '.') {
                tmpString += (receive_line.c_str() + 1);
            } else {
                tmpString += receive_line;
            }
        }
    }

    return tmpString;
}


/*! RETR n command
    @param n number of mail
    @return mail source string
 */
String& POP3Stream::fetch(long n) {
    tmpString.clear();

    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        bool ret;

        send_line.sprintf("RETR %ld\r\n", n);
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return tmpString;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            return tmpString;
        }

        while(1) {
            receive_line = (*this).socket.receiveLine();
            if (receive_line == "" || receive_line == "." || (receive_line[0] == '.' && ((receive_line[1] == '\r' && receive_line[2] == '\n') || receive_line[1] == '\n'))) {
                break;
            }
            if (receive_line[0] == '.') {
                tmpString += (receive_line.c_str() + 1);
            } else {
                tmpString += receive_line;
            }
        }
    }

    return tmpString;
}


/*! UIDL n command
    @param n number of mail
    @return mail UIDL
 */
String& POP3Stream::uidl(long n) {
    tmpString.clear();

    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        long pos1, pos2 = 0L;
        bool ret;

        send_line.sprintf("UIDL %ld\r\n", n);
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return tmpString;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            return tmpString;
        }

        receive_line = receive_line.trim();
        if ((pos1 = receive_line.search(" ")) < 0L || (pos2 = receive_line.search(" ", pos1 + 1)) < 0L) {
            return tmpString;
        }

        tmpString = (receive_line.c_str() + pos2 + 1);
    }

    return tmpString;
}


/*! DELE n command
    @param n number of mail
    @retval true   success
    @retval false  failure
 */
bool POP3Stream::dele(long n) {
    if ((*this).loggedIn == true && (*this).socket.connected()) {
        String receive_line;
        String send_line;
        bool ret;

        send_line.sprintf("DELE %ld\r\n", n);
        ret = (*this).socket.send(send_line);
        if (ret == false) {
            (*this).socket.disconnect();
            (*this).socket.clear();
            (*this).loggedIn = false;
            return false;
        }

        receive_line = (*this).socket.receiveLine();
        if (receive_line[0] != '+') {
            return false;
        }
    } else {
        return false;
    }

    return true;
}


} // namespace
