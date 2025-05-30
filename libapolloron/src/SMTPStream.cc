/******************************************************************************/
/*! @file SMTPStream.cc
    @brief SMTPStream class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace apolloron {

/*! Constructor of SMTPStream.
    @param void
    @return void
 */
SMTPStream::SMTPStream() {
    (*this).clear();
}


/*! Destructor of SMTPStream.
    @param void
    @return void
 */
SMTPStream::~SMTPStream() {
    (*this).clear();
}


/*! Delete instance of SMTPStream.
    @param void
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::clear() {
    (*this).host.clear();
    (*this).port.clear();
    (*this).socket.clear();
    (*this).responseList.clear();

    return true;
}


/*! set timeout of Socket
    @param sec timeout (second)
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::setTimeout(int sec) {
    return (*this).socket.setTimeout(sec);
}


/*! connect to SMTP server
    @param host host name
    @param port port number or service name
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::connect(const String &host, const String &port) {
    bool ret;
    String line;
    long i;

    if ((*this).socket.connected()) {
        return false;
    }

    // connect
    ret = (*this).socket.connect(host, port);
    if (ret == false) {
        return false;
    }

    ret = false;
    i = 0;
    while (i < 1000) {
        line = (*this).socket.receiveLine();
        (*this).responseList.add(line);
        if (4 <= line.len() && line[3] == ' ') {
            if (line.left(4) == "220 ") {
                ret = true;
            }
            break;
        }
        i++;
    }

    line.clear();

    return ret;
}


/*! send QUIT and kill socket stream
    @param void
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::disconnect() {
    bool ret;

    if ((*this).socket.connected() == false) {
        return true;
    }

    ret = (*this).socket.send("QUIT\r\n");
    if (ret == false) {
        (*this).socket.disconnect();
        (*this).socket.clear();
        return false;
    }

    // disconnect
    ret = (*this).socket.disconnect();
    (*this).socket.clear();

    return ret;
}


/*! send EHLO
    @param fqdn  FQDN of EHLO
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::sendEHLO(const String &fqdn) {
    bool ret;
    String line;
    long i;

    if ((*this).socket.connected() == false) {
        return false;
    }

    // send EHLO
    line = "EHLO ";
    line += fqdn+"\r\n";
    ret = (*this).socket.send(line);
    if (ret == false) {
        return false;
    }

    ret = false;
    i = 0;
    while (i < 1000) {
        line = (*this).socket.receiveLine();
        (*this).responseList.add(line);
        if (4 <= line.len() && line[3] == ' ') {
            if (line.left(4) == "250 ") {
                ret = true;
            }
            break;
        }
        i++;
    }

    line.clear();

    return ret;
}


/*! send Mail From
    @param mail_from Mail From String
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::sendMailFrom(const apolloron::String &mail_from) {
    bool ret;
    String line;
    long i;

    if ((*this).socket.connected() == false) {
        return false;
    }

    // send Mail From
    line = "MAIL FROM:<";
    line += mail_from+">\r\n";
    ret = (*this).socket.send(line);
    if (ret == false) {
        return false;
    }

    ret = false;
    i = 0;
    while (i < 1000) {
        line = (*this).socket.receiveLine();
        (*this).responseList.add(line);
        if (4 <= line.len() && line[3] == ' ') {
            if (line.left(4) == "250 ") {
                ret = true;
            }
            break;
        }
        i++;
    }

    line.clear();

    return ret;
}


/*! send Rcpt To
    @param rcpt_to receipt email address
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::sendRcptTo(const apolloron::String &rcpt_to) {
    bool ret;
    String line;
    long i;

    if ((*this).socket.connected() == false) {
        return false;
    }

    // send Mail From
    line = "RCPT TO:<";
    line += rcpt_to+">\r\n";
    ret = (*this).socket.send(line);
    if (ret == false) {
        return false;
    }

    ret = false;
    i = 0;
    while (i < 1000) {
        line = (*this).socket.receiveLine();
        (*this).responseList.add(line);
        if (4 <= line.len() && line[3] == ' ') {
            if (line.left(4) == "250 ") {
                ret = true;
            }
            break;
        }
        i++;
    }

    line.clear();

    return ret;
}


/*! send DATA
    @param eml mail source
    @retval true   success
    @retval false  failure
 */
bool SMTPStream::sendData(const apolloron::String &eml) {
    bool ret;
    String line, mod_eml;
    long i;

    if ((*this).socket.connected() == false) {
        return false;
    }

    if (eml.len() <= 0) {
        return false;
    }

    // send DATA (eml)
    ret = (*this).socket.send("DATA\r\n");
    if (ret == false) {
        return false;
    }

    ret = false;
    i = 0;
    while (i < 1000) {
        line = (*this).socket.receiveLine();
        (*this).responseList.add(line);
        if (4 <= line.len() && line[3] == ' ') {
            if (line.left(4) == "354 ") {
                ret = true;
            }
            break;
        }
        i++;
    }

    // send EML
    mod_eml = eml.changeReturnCode("\r\n");
    if (mod_eml[0] == '.') {
        mod_eml.insert(0L, ".");
    }
    for (i = mod_eml.len()-1; 0 < i; i++) {
        if (mod_eml[i-1] == '\n' && mod_eml[i] == '.') {
            mod_eml.insert(i, ".");
        }
    }

    if (mod_eml.len() <= 0) {
        line.clear();
        mod_eml.clear();
        return false;
    }

    if (mod_eml[mod_eml.len()-1] != '\n') {
        ret = (*this).socket.send("\r\n");
        if (ret == false) {
            line.clear();
            mod_eml.clear();
            return false;
        }
    }
    ret = (*this).socket.send(".\r\n");
    if (ret == false) {
        return false;
    }

    ret = false;
    i = 0;
    while (i < 1000) {
        line = (*this).socket.receiveLine();
        (*this).responseList.add(line);
        if (4 <= line.len() && line[3] == ' ') {
            if (line.left(4) == "250 ") {
                ret = true;
            }
            break;
        }
        i++;
    }

    line.clear();
    mod_eml.clear();

    return false;
}


/*! get list of SMTP responce
    @param void
    @retval true   success
    @retval false  failure
 */
const List& SMTPStream::getResponseList() const {
    return (*this).responseList;
}

} // namespace apolloron
