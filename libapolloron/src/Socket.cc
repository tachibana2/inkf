/******************************************************************************/
/*! @file Socket.cc
    @brief Socket class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/errno.h>

// socket
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// OpenSSL
#if __OPENSSL == 1
#include <openssl/ssl.h>
#endif

#include "apolloron.h"

namespace {

#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFF
#endif

#ifndef MSG_WAITALL
#define MSG_WAITALL 0x8
#endif

} // namespace


namespace apolloron {

/*! Constructor of Socket.
    @param recycle_dstSocket  socket number (-1 for new socket)
    @return void
 */
Socket::Socket(int recycle_dstSocket) {
    (*this).dstHost[0] = '\0';
    if (0 <= recycle_dstSocket) {
        (*this).dstSocket = recycle_dstSocket;
        (*this).bConnected = true;
    } else {
        (*this).dstSocket = -1;
        (*this).bConnected = false;
    }
    (*this).nTimeout = 300;
    (*this).nErrno = 0;
    (*this).tmpString = new String;
    (*this).bSSLStream = false;
    (*this).pSSL = NULL;
    (*this).pSSLCTX = NULL;
}


/*! Destructor of Socket.
    @param void
    @return void
 */
Socket::~Socket() {
    (*this).clear();
    if ((*this).tmpString != (String *)NULL) {
        delete (*this).tmpString;
        (*this).tmpString = (String *)NULL;
    }
}


/*! Clear Socket object.
    @param void
    @retval true  success
    @retval false failure
 */
bool Socket::clear() {
    (*this).dstHost[0] = '\0';
    if ((*this).bConnected) {
        (*this).disconnect();
        (*this).bConnected = false;
    }
    (*this).dstSocket = -1;
    (*this).nTimeout = 300;
    (*this).nErrno = 0;
    if ((*this).tmpString != NULL) {
        (*((*this).tmpString)).clear();
    }
#if __OPENSSL == 1
    (*this).bSSLStream = false;
    (*this).pSSL = NULL;
    (*this).pSSLCTX = NULL;
#endif

    return true;
}


/*! Connecting to server (for starting client process).
    @param host  host name or IP address
    @param port  port number or service name
    @param ssl   is connection SSL or not
    @retval true  success
    @retval false failure
 */
bool Socket::connect(const char *host, const char *port, bool ssl) {
    const char *destination; /* name or IP address */
#ifdef AF_INET6
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *ai;
#else
    struct sockaddr_in dstAddr;
    unsigned long addr;
    struct hostent *hp;
#endif
    struct timeval timeout;

    (*this).nErrno = 0;
    if ((*this).bConnected) {
        return false;
    }

    if (port == NULL || port[0] == '\0') {
        (*this).nErrno = -1;
        return false;
    }

#if __OPENSSL == 0
    if (ssl) {
        (*this).nErrno = -1;
        return false;
    }
#endif

    if (host != NULL) {
        destination = host;
    } else {
        destination = "localhost";
    }
    strncpy((*this).dstHost, destination, sizeof((*this).dstHost));
    (*this).dstHost[sizeof((*this).dstHost)-1] = '\0';

    timeout.tv_sec = (*this).nTimeout;
    timeout.tv_usec = 0;

#ifdef AF_INET6
    // init server structure
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // support IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;
#ifdef AI_NUMERICSERV
    {
        bool is_port_number;
        int i;
        is_port_number = true;
        for (i = 0; port[i] != '\0'; i++) {
            if (!isdigit(port[i])) {
                is_port_number = false;
                break;
            }
        }
        if (is_port_number) {
            hints.ai_flags = AI_NUMERICSERV;
        }
    }
#endif

    if (getaddrinfo(destination, port, &hints, &res) != 0) {
        (*this).nErrno = -1;
        return false;
    }

    // create socket and connect
    for (ai = res; ai; ai = ai->ai_next) {
        (*this).dstSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if ((*this).dstSocket < 0) {
            continue;
        }

        ::setsockopt((*this).dstSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
        ::setsockopt((*this).dstSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

        // try connect
        if (::connect((*this).dstSocket, ai->ai_addr, ai->ai_addrlen) == 0) {
            break;
        }

        (*this).nErrno = errno;
        close((*this).dstSocket);
        (*this).dstSocket = -1;
        continue;
    }
    freeaddrinfo(res);
    if ((*this).dstSocket < 0) {
        (*this).nErrno = -1;
        return false;
    }
    (*this).nErrno = 0;
#else
    // resolve server name
    addr = inet_addr(destination);
    if (addr != INADDR_NONE) {
        hp = gethostbyaddr((const char *)&addr , 4 , AF_INET);
        if (hp == NULL) {
            (*this).nErrno = -1;
            return false;
        }
    } else {
        hp = gethostbyname(destination);
        if (hp == NULL) {
            (*this).nErrno = -1;
            return false;
        }
    }

    // init server structure
    memset(&dstAddr, 0, sizeof(dstAddr));
    {
        bool is_port_number;
        int i;
        is_port_number = true;
        for (i = 0; port[i] != '\0'; i++) {
            if (!isdigit(port[i])) {
                is_port_number = false;
                break;
            }
        }
        if (is_port_number) {
            dstAddr.sin_port = htons(atoi(port));
        } else {
            struct servent *se;
            se = getservbyname(port, "tcp");
            if (se == NULL) {
                (*this).nErrno = -1;
                return false;
            }
            dstAddr.sin_port = se->s_port;
        }
    }
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_addr = *(struct in_addr*)hp->h_addr;

    // create socket
    if(((*this).dstSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        (*this).nErrno = -1;
        return false;
    }

    // connect
    ::setsockopt((*this).dstSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

    if (::connect((*this).dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr)) < 0) {
        (*this).nErrno = errno;
        close((*this).dstSocket);
        (*this).dstSocket = -1;
        return false;
    }
#endif

#if __OPENSSL == 1
    if (ssl) {
        SSL_CTX *pctx;
        SSL *pssl;

        SSL_library_init();

        pctx = SSL_CTX_new(SSLv23_client_method());
        if (pctx == (SSL_CTX *)NULL) {
            (*this).nErrno = -1;
            close((*this).dstSocket);
            (*this).dstSocket = -1;
            return false;
        }
        SSL_CTX_set_timeout(pctx, (long)((*this).nTimeout));

        pssl = SSL_new(pctx);
        if (pssl == (SSL *)NULL) {
            (*this).nErrno = -1;
            SSL_CTX_free(pctx);
            close((*this).dstSocket);
            (*this).dstSocket = -1;
            return false;
        }

        SSL_set_fd(pssl, (*this).dstSocket);
        SSL_set_tlsext_host_name(pssl, (*this).dstHost);
        if (SSL_connect(pssl) <= 0) {
            (*this).nErrno = -1;
            SSL_free(pssl);
            SSL_CTX_free(pctx);
            close((*this).dstSocket);
            (*this).dstSocket = -1;
            return false;
        }

        (*this).pSSL = (void *)pssl;
        (*this).pSSLCTX = (void *)pctx;
        (*this).bSSLStream = true;
    }
#endif

    (*this).bConnected = true;
    return true;
}


/*! Disconnecting socket.
    @param void
    @retval true  success
    @retval false failure
 */
bool Socket::disconnect() {
    // close socket
    if (0 <= (*this).dstSocket) {
#if __OPENSSL == 1
        if ((*this).bSSLStream) {
            SSL_free((SSL *)((*this).pSSL));
            SSL_CTX_free((SSL_CTX *)((*this).pSSLCTX));
            (*this).pSSL = NULL;
            (*this).pSSLCTX = NULL;
            (*this).bSSLStream = false;
        }
#endif
        if ((*this).bConnected) {
            close((*this).dstSocket);
        }
        (*this).dstSocket = -1;
    }

    (*this).bConnected = false;
    return true;
}


/*! Set timeout
    @param sec  timeout (second)
    @retval true  success
    @retval false failure
 */
bool Socket::setTimeout(int sec) {
    (*this).nTimeout = sec;
    return true;
}


/*! Refer to error number
    @param void
    @return error number of current connection
 */
int Socket::error() const {
    return (*this).nErrno;
}


/*! Connection status check
    @param void
    @retval true  success
    @retval false failure
 */
bool Socket::connected() const {
    return (*this).bConnected;
}


/*! Starting TLS (Transport Layer Security)
    @param void
    @retval true  success
    @retval false failure
 */
bool Socket::startTls() {
#if __OPENSSL == 1
    SSL_CTX *pctx;
    SSL *pssl;

    if ((*this).bConnected == false || (*this).dstSocket < 0) {
        (*this).nErrno = -1;
        return false;
    }
    if ((*this).bSSLStream) {
        return true;
    }

    SSL_library_init();

    pctx = SSL_CTX_new(SSLv23_client_method());
    if (pctx == (SSL_CTX *)NULL) {
        (*this).nErrno = -1;
        return false;
    }
    SSL_CTX_set_timeout(pctx, (long)((*this).nTimeout));

    pssl = SSL_new(pctx);
    if (pssl == (SSL *)NULL) {
        (*this).nErrno = -1;
        SSL_CTX_free(pctx);
        return false;
    }

    SSL_set_fd(pssl, (*this).dstSocket);
    SSL_set_tlsext_host_name(pssl, (*this).dstHost);
    if (SSL_connect(pssl) <= 0) {
        (*this).nErrno = -1;
        SSL_free(pssl);
        SSL_CTX_free(pctx);
        return false;
    }

    (*this).pSSL = (void *)pssl;
    (*this).pSSLCTX = (void *)pctx;
    (*this).bSSLStream = true;

    return true;
#else
    return false;
#endif
}


/*! Sending data
    @param str  data to send
    @retval true  success
    @retval false failure
 */
bool Socket::send(const String &str) {
    int ret = -1;

    if ((*this).bConnected == false || str.c_str() == NULL || str.c_str()[0] == '\0') {
        return false;
    }

#if __OPENSSL == 1
    if ((*this).bSSLStream) {
        ret = SSL_write((SSL *)((*this).pSSL), str.c_str(), str.len());
        if (ret <= 0) {
            (*this).nErrno = -1;
            return false;
        }
    } else {
        ret = ::send((*this).dstSocket, str.c_str(), str.len(), 0);
    }
#else
    ret = ::send((*this).dstSocket, str.c_str(), str.len(), 0);
#endif

    if (ret < 0) {
        (*this).nErrno = errno;
        (*this).disconnect();
        return false;
    }

    return true;
}


/*! Send data
    @param str  data to send
    @retval true  success
    @retval false failure
 */
bool Socket::send(const char *str) {
    int ret = -1;

    if ((*this).bConnected == false || str == NULL || str[0] == '\0') {
        return false;
    }

#if __OPENSSL == 1
    if ((*this).bSSLStream) {
        ret = SSL_write((SSL *)((*this).pSSL), str, strlen(str));
        if (ret <= 0) {
            (*this).nErrno = -1;
            return false;
        }
    } else {
        ret = ::send((*this).dstSocket, str, strlen(str), 0);
    }
#else
    ret = ::send((*this).dstSocket, str, strlen(str), 0);
#endif

    if (ret < 0) {
        (*this).nErrno = errno;
        return false;
    }

    return true;
}


/*! Receive data
    @param size  receiving size
    @return received data
 */
String& Socket::receive(long size) {
    struct timeval timeout;
    fd_set readOK, mask;
    int width;
    long total;
    char *buf;
    long numrcv;

    total = 0;
    (*((*this).tmpString)).useAsBinary(total);

    if ((*this).bConnected == false || (*this).dstSocket < 0 || size < 0) {
        (*this).nErrno = -1;
        return *((*this).tmpString);
    }

    (*this).nErrno = 0;
    if (size == 0) {
        return *((*this).tmpString);
    }

    (*((*this).tmpString)).setFixedLength(size);

#if __OPENSSL == 1
    if ((*this).bSSLStream) {
        buf = new char [size + 1];
        total = 0;
        do {
            numrcv = SSL_read((SSL *)((*this).pSSL), buf + total, size - total);
            if (numrcv < 0) {
                break;
            } else if (numrcv == 0 && total < size) {
                usleep(1000);
                continue;
            }
            total += numrcv;
        } while (0 < size - total);
        buf[total] = '\0';
        buf[size] = '\0';
        if (0 <= total) {
            (*((*this).tmpString)).setBinary(buf, total);
        }
        delete [] buf;
        if (total < size) {
            (*this).nErrno = errno;
            (*this).disconnect();
        }
        return *((*this).tmpString);
    }
#endif

    timeout.tv_sec = (*this).nTimeout;
    timeout.tv_usec = 0;

    FD_ZERO(&mask);
    FD_SET((*this).dstSocket, &mask);
    readOK = mask;
    width = (*this).dstSocket + 1;

    if (0 < select(width, &readOK, NULL, NULL, &timeout)) {
        if (FD_ISSET((*this).dstSocket, &readOK)) {
            buf = new char [size + 1];
            total = 0;
            do {
                numrcv = recv((*this).dstSocket, buf + total, size - total, MSG_WAITALL);
                if (numrcv < 0) {
                    break;
                } else if (numrcv == 0 && total < size) {
                    usleep(1000);
                    continue;
                }
                total += numrcv;
            } while (0 < size - total);
            buf[total] = '\0';
            buf[size] = '\0';
            if (0 <= total) {
                (*((*this).tmpString)).setBinary(buf, total);
            }
            delete [] buf;
        }
    }
    if (total < size) {
        (*this).nErrno = errno;
        (*this).disconnect();
    }

    return *((*this).tmpString);
}


/*! Receive one line string
    @param void
    @return received data
 */
String& Socket::receiveLine() {
    struct timeval timeout;
    fd_set readOK, mask;
    int width;
    char tmp[2];
    long numrcv;

    (*((*this).tmpString)).useAsBinary(0);

    if ((*this).dstSocket < 0 || (*this).bConnected == false) {
        (*this).nErrno = -1;
        (*this).bConnected = false;
        return *((*this).tmpString);
    }

    (*((*this).tmpString)).setFixedLength(1024);

    (*this).nErrno = 0;

    tmp[0] = '\0';
    tmp[1] = '\0';

#if __OPENSSL == 1
    if ((*this).bSSLStream) {
        do {
            numrcv = SSL_read((SSL *)((*this).pSSL), tmp, 1);
            if (numrcv == 1) {
                tmp[1] = '\0';
                (*((*this).tmpString)).addBinary(tmp, numrcv);
                if (tmp[0] == '\n' || tmp[0] == '\0') {
                    break;
                }
            } else if (numrcv < 0) {
                (*this).nErrno = -1;
                break;
            } else {
                usleep(100);
            }
        } while (numrcv != 0);
        if ((*((*this).tmpString)).binaryLength() == 0) {
            (*this).disconnect();
        }
        return *((*this).tmpString);
    }
#endif

    timeout.tv_sec = (*this).nTimeout;
    timeout.tv_usec = 0;

    FD_ZERO(&mask);
    FD_SET((*this).dstSocket, &mask);
    readOK = mask;
    width = (*this).dstSocket + 1;

    if (0 < select(width, &readOK, NULL, NULL, &timeout)) {
        if (FD_ISSET((*this).dstSocket, &readOK)) {
            do {
                numrcv = recv((*this).dstSocket, tmp, 1, MSG_WAITALL);
                if (0 < numrcv) {
                    tmp[1] = '\0';
                    (*((*this).tmpString)).addBinary(tmp, numrcv);
                    if (tmp[0] == '\n' || tmp[0] == '\0') {
                        break;
                    }
                } else if (numrcv < 0) {
                    (*this).nErrno = errno;
                    break;
                } else {
                    usleep(100);
                }
            } while (0 < numrcv);
        }
    }
    if ((*((*this).tmpString)).binaryLength() == 0) {
        (*this).disconnect();
    }

    return *((*this).tmpString);
}


/*! Receive one line string
    @param void
    @return received data
 */
String& Socket::receivedData() const {
    return *((*this).tmpString);
}


} // namespace apolloron
