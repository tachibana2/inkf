/******************************************************************************/
/*! @file HTTPClient.cc
    @brief HTTPClient class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace {
static const int REDIRECT_MAX = 10;
}

namespace apolloron {

/*! Constructor of HTTPClient.
    @param void
    @return void
 */
HTTPClient::HTTPClient() {
    (*this).content.clear();
}


/*! Destructor of HTTPClient.
    @param void
    @return void
 */
HTTPClient::~HTTPClient() {
    (*this).clear();
}


/*! Delete instance of HTTPClient.
    @param void
    @retval true   success
    @retval false  failure
 */
bool HTTPClient::clear() {
    (*this).content.clear();
    (*this).origCharset.clear();

    return true;
}


/*! get content specified by URL
    @param url      URL
    @param timeout  timeout
    @return HTTP Content
 */
String& HTTPClient::getURL(const String &url, long timeout) {
    (*this).content = "";
    (*this).origCharset = "";
    char input_charset[32];
    bool charset_found = false;

    strcpy(input_charset, "AUTODETECT");

#if __OPENSSL == 1
    if (!strncasecmp(url.c_str(), "http://", 7) ||
            !strncasecmp(url.c_str(), "https://", 8)) {
#else
    if (!strncasecmp(url.c_str(), "http://", 7)) {
#endif
        char *url_str;
        Socket tmp_socket;
        char host[1024], port[20], path[1024], port_tmp[20];
        char *p;
        int host_base;
        bool ssl;
        bool header_end;
        bool ischunked;
        long chunk_size;
        int redirect_count;
        long j, k, l;

        redirect_count = 0;
        url_str = new char [url.len() + 1];
        strcpy(url_str, url.c_str());

HTTP_START:
#if __OPENSSL == 1
        if (!strncasecmp(url_str, "http://", 7)) {
#endif
            strcpy(port, "80");
            host_base = 7;
            ssl = false;
#if __OPENSSL == 1
        } else {
            strcpy(port, "443");
            host_base = 8;
            ssl = true;
        }
#endif
        strncpy(host, url_str + host_base, 1023);
        host[1023] = '\0';
        p = strchr(host, '/');
        if (p != NULL) {
            *p = '\0';
        }
        p = strchr(host, ':');
        if (p != NULL) {
            *p = '\0';
        }
        port_tmp[0] = '\0';
        p = url_str + host_base + strlen(host);
        if (*p == ':') {
            for (j = 0; p[1 + j] != '\0' && j < 19; j++) {
                if (p[1 + 1] == '/') {
                    break;
                }
                port_tmp[j] = p[1 + j];
            }
            port_tmp[j] = '\0';
        }
        if (port_tmp[0] != '\0') {
            strcpy(port, port_tmp);
        }
        p = strchr(url_str + host_base, '/');
        strncpy(path, p?p:"/", 1023);
        path[1023] = '\0';
        delete [] url_str;
        tmp_socket.setTimeout(timeout);
        tmp_socket.connect(host, port, ssl);
        tmp_socket.send("GET ");
        tmp_socket.send((*path)?path:"/");
        tmp_socket.send(" HTTP/1.1\r\n");
        tmp_socket.send("HOST: ");
        tmp_socket.send(host);
        tmp_socket.send("\r\n");
        tmp_socket.send("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko)\r\n");
        tmp_socket.send("CONNECTION: close\r\n\r\n");
        (*this).content.useAsBinary(0L);
        header_end = false;
        ischunked = false;
        chunk_size = -1;
        while (tmp_socket.error() == 0 && tmp_socket.connected()) {
            char *endptr;

            tmp_socket.receiveLine();
            if (tmp_socket.receivedData().binaryLength() == 0) {
                break;
            }

            // redirect
            if (!header_end &&
                    !strncasecmp(tmp_socket.receivedData().c_str(), "Location:", 9) &&
                    9 <= tmp_socket.receivedData().searchCase("http")) {
                url_str = new char [tmp_socket.receivedData().len() + 1];
                p = url_str;
                p[0] = '\0';
                j = 0;
                for (j = 9; j < tmp_socket.receivedData().len(); j++) {
                    if (url_str[0] == '\0') {
                        if (!isspace(tmp_socket.receivedData()[j]) && tmp_socket.receivedData()[j] != ':') {
                            *p = tmp_socket.receivedData()[j];
                            p++;
                        }
                    } else if (!isspace(tmp_socket.receivedData()[j])) {
                        *p = tmp_socket.receivedData()[j];
                        p++;
                    }
                }
                *p = '\0';

#if __OPENSSL == 1
                if (!strncasecmp(url_str, "http://", 7) ||
                        !strncasecmp(url_str, "https://", 8)) {
#else
                if (!strncasecmp(url_str, "http://", 7)) {
#endif
                    tmp_socket.clear();
                    redirect_count++;
                    if (redirect_count <= REDIRECT_MAX) {
                        goto HTTP_START;
                    }
                }

                delete [] url_str;
            }

            if (!header_end &&
                    !strncasecmp(tmp_socket.receivedData().c_str(), "Transfer-Encoding", 17) &&
                    17 <= tmp_socket.receivedData().search(":") &&
                    18 <= tmp_socket.receivedData().searchCase("chunked")) {
                ischunked = true;
            }

            if (header_end && ischunked) {
                if (tmp_socket.receivedData()[0] == '\n' || tmp_socket.receivedData() == "\r\n") {
                    continue;
                }
                chunk_size = strtol(tmp_socket.receivedData().c_str(), &endptr, 16);
                if (chunk_size <= 0) {
                    continue;
                }
                tmp_socket.receive(chunk_size);
            }

            if (charset_found == false && 0 <= (l = tmp_socket.receivedData().searchCase("charset="))) {
                char tmp_charset[32], c;
                k = 0;
                while (k < 31) {
                    c = tmp_socket.receivedData()[l+8+k];
                    if (!((k == 0 && (c == '"' || c == '\'')) || isalnum(c) || c == '-' || c == '_')) {
                        break;
                    }
                    tmp_charset[k] = (c == '\'')?'"':c;
                    k++;
                }
                tmp_charset[k] = '\0';
                if (0 < k - ((tmp_charset[0] == '"')?1:0)) {
                    strcpy(input_charset, tmp_charset + ((tmp_charset[0] == '"')?1:0));
                }
                charset_found = true;
            }

            if (header_end) {
                (*this).content.addBinary(tmp_socket.receivedData().c_str(),
                        tmp_socket.receivedData().binaryLength());
            }

            if (!header_end && (tmp_socket.receivedData()[0] == '\n' ||
                    tmp_socket.receivedData() == "\r\n")) {
                header_end = true;
            }
        }
        tmp_socket.clear();
    }
    (*this).origCharset = input_charset;
    return (*this).content;
}


/*! get original character-set
    @param void
    @return charset in original HTTP Content
 */
String& HTTPClient::getOrigCharset() {
    return (*this).origCharset;
}

} // namespace apolloron
