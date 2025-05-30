/******************************************************************************/
/*! @file FCGI.cc
 *  @brief FCGI class
 *  @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include "fcgiapp.h"
#include "apolloron.h"

namespace apolloron {

#define getenv(name) FCGX_GetParam((name), (*this).request->envp)


/*! Constructor of String.
 *  @param void
 *  @return void
 */
FCGI::FCGI(FCGX_Request *request, WebSocket *customWebSocket) {
    String str;
    char buf[1024 + 1];
    const char *cpstr;
    char *pstr;
    const char *http_accept_language;

    (*this).clear();
    str.clear();

    (*this).request = request;

    (*this).isWebSocketReq = false;
    if (customWebSocket != NULL) {
        (*this).webSocket = *customWebSocket;
        (*this).webSocket.setParent(this, false);  // Set this FCGI instance as parent
    }

    if ((cpstr = getenv("REQUEST_METHOD")) != NULL) {
        char *p;
        strncpy(buf, cpstr, 1024);
        buf[1024]='\0';
        p = buf;
        while (*p != '\0') {
            *p = toupper(*p);
            p++;
        }

        if (!strcasecmp(buf, "GET") || !strcasecmp(buf, "DELETE")) {
            /* GET, DELETE Method */
            if ((cpstr = getenv("QUERY_STRING")) != NULL) {
                str = cpstr;
            }
        } else if (!strcasecmp(buf, "POST") || !strcasecmp(buf, "PUT") || !strcasecmp(buf, "PATCH")) {
            /* POST, PUT Method */
            if ((cpstr = getenv("CONTENT_LENGTH")) != NULL) {
                int len, tmplen, getlen, getlen_pre;
                strncpy(buf, cpstr, 1024);
                buf[1024]='\0';
                len = atoi(buf);
                getlen = 0;
                pstr = new char [len + 1];
                pstr[len] = '\0';
                while (getlen < len) {
                    getlen_pre = getlen;
                    tmplen = FCGX_GetStr(buf, (len - getlen <= 1024)?(len - getlen):1024, request->in);
                    if (tmplen < 0) {
                        break;
                    }
                    getlen += tmplen;
                    buf[getlen - getlen_pre] = '\0';
                    if (0 < (getlen - getlen_pre)) {
                        memcpy(pstr + getlen_pre, buf, getlen - getlen_pre);
                    } else if (getlen == getlen_pre) {
                        sleep(1);
                    }
                    if (request->in->isClosed) {
                        break;
                    } else if (FCGX_HasSeenEOF(request->in)) {
                        break;
                    }
                }
                str.setBinary(pstr, getlen);
                delete [] pstr;
            }
        }
    }

    if (0 < str.binaryLength()) {
        long i, j, k, l, len;
        len = str.binaryLength();
        cpstr = getenv("CONTENT_TYPE");
        if (cpstr != NULL && !strncasecmp(cpstr, "multipart/form-data", 19)) {
            // multipart request
            char *boundary;
            int ret;
            const char *head_start;
            int boundary_len, col;
            bool is_text;
            String esc_tmp_key, esc_tmp_filename;

            cpstr = strstr(cpstr, "boundary=");
            if (cpstr != NULL) {
                l = strlen(cpstr + strlen("boundary=")) + 3;
                boundary = new char [l];
                ::snprintf(boundary, l, "--%s", cpstr + strlen("boundary="));
                pstr = strchr(boundary, ';');
                if (pstr != NULL) {
                    *pstr = '\0';
                }
            } else {
                boundary = new char [1];
                boundary[0] = '\0';
            }
            boundary_len = strlen(boundary);

            if (0 < boundary_len) {
                cpstr = str.c_str();
                col = 0;
                while (!strncmp(cpstr+col, boundary, boundary_len)) {
                    char *head, *body;
                    const char *cp;
                    long body_len;

                    if (!strncmp(cpstr+col+boundary_len, "--", 2)) {
                        break;
                    }

                    head = NULL;
                    body = NULL;
                    body_len = 0;

                    col += boundary_len;
                    if (cpstr[col] == '\r') col++;
                    if (cpstr[col] == '\n') col++;

                    esc_tmp_key = "";
                    esc_tmp_filename = "";
                    is_text = true;

                    head_start = cpstr + col;
                    ret = 0;
                    for (i = 0; head_start[i] != '\0'; i++) {
                        if (ret == 2) {
                            break;
                        }
                        if (head_start[i] == '\n') {
                            ret++;
                        } else if (head_start[i] == '\r' && head_start[i+1] == '\n') {
                            i++;
                            ret++;
                        } else {
                            ret = 0;
                        }
                    }
                    if (ret == 2) {
                        head = new char [i+1];
                        strncpy(head, head_start, i);
                        head[i] = '\0';
                        col += i;
                        if (0 < i && head[i-1] == '\n') {
                            head[i-1] = '\0';
                            i--;
                            if (0 < i && head[i-1] == '\r') {
                                head[i-1] = '\0';
                                i--;
                            }
                        }
                        if (0 < i && head[i-1] == '\n') {
                            head[i-1] = '\0';
                            i--;
                            if (0 < i && head[i-1] == '\r') {
                                head[i-1] = '\0';
                                i--;
                            }
                        }

                        if (head != NULL) {
                            char *p, *p1;
                            char *key = NULL;
                            String tmp_key, tmp_filename;
                            char *head_line;
                            int ii, jj, line_len;

                            ii = 0;
                            while (head[ii] != '\0') {
                                line_len = 0;
                                for (jj = ii; head[jj] != '\0'; jj++) {
                                    if (head[jj] == '\n' && (head[jj+1] == '\0' || !isspace(head[jj+1]))) {
                                        jj++;
                                        line_len = jj;
                                        break;
                                    }
                                }
                                if (line_len == 0) line_len = jj;

                                if (0 < line_len) {
                                    head_line = new char [line_len + 1];
                                    strncpy(head_line, head+ii, line_len);
                                    head_line[line_len] = '\0';

                                    if (!strncasecmp(head_line, "Content-Disposition", 19)) {
                                        if ((p = strstr(head_line, "name=")) != NULL) {
                                            if (p[5] == '"' && (p1 = strchr(p+6, '"')) != NULL) {
                                                key = new char [p1 - (p+5) + 1];
                                                strncpy(key, p+6, p1 - (p+5) - 1);
                                                key[p1 - (p+5) - 1] = '\0';
                                                tmp_key = key;
                                                esc_tmp_key = tmp_key.unescapeQuote();
                                            } else if ((p1 = strchr(p+6, ';')) != NULL || (p1 = strchr(p+6, '\r')) != NULL || (p1 = strchr(p+6, '\n')) != NULL) {
                                                key = new char [p1 - (p+5) + 1];
                                                strncpy(key, p+5, p1 - (p+5));
                                                key[p1 - (p+5)] = '\0';
                                                tmp_key = key;
                                                esc_tmp_key = tmp_key;
                                            }
                                            tmp_key.clear();
                                            if (key != NULL) delete [] key;
                                        }
                                        if ((p = strstr(head_line, "filename=")) != NULL) {
                                            if (p[9] == '"' && (p1 = strchr(p+10, '"')) != NULL) {
                                                key = new char [p1 - (p+9) + 1];
                                                strncpy(key, p+10, p1 - (p+9) - 1);
                                                key[p1 - (p+9) - 1] = '\0';
                                                tmp_filename = key;
                                                esc_tmp_filename = tmp_filename.unescapeQuote();
                                            } else if ((p1 = strchr(p+10, ';')) != NULL || (p1 = strchr(p+10, '\r')) != NULL || (p1 = strchr(p+10, '\n')) != NULL) {
                                                key = new char [p1 - (p+9) + 1];
                                                strncpy(key, p+9, p1 - (p+9));
                                                key[p1 - (p+9)] = '\0';
                                                tmp_filename = key;
                                                esc_tmp_filename = tmp_filename;
                                            }
                                            tmp_filename.clear();
                                            if (key != NULL) delete [] key;
                                        }
                                    } else if (!strncasecmp(head_line, "Content-Type", 12)) {
                                        if (strcasestr(head_line, "text/") == NULL) {
                                            is_text = false;
                                        }
                                    }

                                    if (head_line != NULL)  delete [] head_line;
                                }
                                ii += ((0 < line_len)?line_len:1);
                            }
                        }

                        if (cpstr[col] == '\n') {
                            col++;
                        } else if (cpstr[col] == '\r' && cpstr[col+1] == '\n') {
                            col += 2;
                        }
                        if (cpstr[col] == '\n') {
                            col++;
                        } else if (cpstr[col] == '\r' && cpstr[col+1] == '\n') {
                            col += 2;
                        }

                        k = str.binaryLength();
                        cp = NULL;
                        for (j = col; j < k - boundary_len; j++) {
                            if (cpstr[j] == boundary[0] && !strncmp(cpstr + j, boundary, boundary_len)) {
                                cp = cpstr + j;
                                break;
                            }
                        }
                        if (cp != NULL) {
                            body_len = (long)(cp - (cpstr+col));
                            if (0 < col && cpstr[col+body_len-1] == '\n') {
                                body_len--;
                            }
                            if (0 < col && cpstr[col+body_len-1] == '\r') {
                                body_len--;
                            }
                            body = new char [body_len + 1];
                            memcpy(body, cpstr+col, body_len);
                            body[body_len] = '\0';
                        } else {
                            body_len = strlen(cpstr+col);
                            if (0 < col && cpstr[col+body_len-1] == '\n') {
                                body_len--;
                            }
                            if (0 < col && cpstr[col+body_len-1] == '\r') {
                                body_len--;
                            }
                            body = new char [body_len + 1];
                            memcpy(body, cpstr+col, body_len);
                            body[body_len] = '\0';
                        }

                        if (cp != NULL) {
                            col = cp - cpstr;
                        }
                    }

                    if (is_text) {
                        (*this).requestData[esc_tmp_key] = body;
                    } else if (0 <= body_len) {
                        (*this).requestData[esc_tmp_key].setBinary(body?body:"", body_len);
                    } else {
                        (*this).requestData[esc_tmp_key] = "";
                    }
                    (*this).requestFileName[esc_tmp_key] = esc_tmp_filename;

                    if (head != NULL) delete [] head;
                    if (body != NULL) delete [] body;
                }
            }

            esc_tmp_key.clear();
            esc_tmp_filename.clear();
            delete [] boundary;

        } else if (cpstr != NULL && strcasestr(cpstr, "xml")) {
            // XMLHttpRequest
            str.useAsText();
            (*this).requestData["content"] = str;
        } else if (cpstr != NULL && strcasestr(cpstr, "json")) {
            // XMLHttpRequest (JSON)
            JSONReader reader;
            str.useAsText();
            (*this).requestData["content"] = str;
            reader.parse(str, (*this).requestJSON);
        } else {
            // normal request
            char *p, *prev_key, *prev_value;
            String tmp_key, esc_tmp_key;

            str.useAsText();
            p = new char [len + 1];
            strcpy(p, str.c_str());
            prev_key = p;
            prev_value = NULL;
            for (i = 0; i < len; i++) {
                if (p[i] == '&') {
                    p[i] = '\0';
                    tmp_key = prev_key;
                    esc_tmp_key = tmp_key.unescapeQuote();
                    (*this).requestData[esc_tmp_key] = prev_value;
                    (*this).requestData[esc_tmp_key] = (*this).requestData[esc_tmp_key].decodeURL();
                    prev_key = p + i + 1;
                    prev_value = NULL;
                } else if (p[i] == '=') {
                    p[i] = '\0';
                    prev_value = p + i + 1;
                }
            }
            if (prev_key != NULL && prev_value != NULL) {
                tmp_key = prev_key;
                esc_tmp_key = tmp_key.unescapeQuote();
                (*this).requestData[esc_tmp_key] = prev_value;
                (*this).requestData[esc_tmp_key] = (*this).requestData[esc_tmp_key].decodeURL();
            }
            delete [] p;
            tmp_key.clear();
            esc_tmp_key.clear();
        }
    }

    if ((cpstr = getenv("HTTP_COOKIE")) != NULL) {
        const char *p_base, *p_next, *key, *value;
        long length;
        char *p;
        p_base = cpstr;
        while (p_base[0] != '\0') {
            p_next = strchr(p_base, ';');
            if (p_next) {
                length = p_next - p_base;
            } else {
                length = strlen(p_base);
            }
            if (0 < length) {
                pstr = new char [length + 1];
                strncpy(pstr, p_base, length);
                pstr[length] = '\0';
                p = strchr(pstr, '=');
                if (p != NULL) {
                    p[0] = '\0';
                    key = (const char *)pstr;
                    value = (const char *)(p + 1);
                    if (key[0] != '\0') {
                        String s = value;
                        (*this).cookie[key] = s.unescapeHTML();
                        s.clear();
                    }
                }
                delete [] pstr;
            }
            p_base = (p_next)?(p_next + 1):(p_base + strlen(p_base));
        }
    }

    // HTTP_ACCEPT_LANGUAGE is like "ja,zh-cn;q=0.8,en-us;q=0.3"
    http_accept_language = getenv("HTTP_ACCEPT_LANGUAGE");
    (*this).primary_lang[0] = '\0';
    if (http_accept_language != NULL) {
        strncpy((*this).primary_lang, http_accept_language, sizeof((*this).primary_lang) - 1);
        (*this).primary_lang[sizeof((*this).primary_lang) - 1] = '\0';
        pstr = strchr((*this).primary_lang, ',');
        if (pstr != NULL) *pstr = '\0';
        pstr = strchr((*this).primary_lang, ';');
        if (pstr != NULL) *pstr = '\0';
    }

    if (customWebSocket != NULL &&
        (cpstr = getenv("HTTP_UPGRADE")) != NULL && 
        !strcasecmp(cpstr, "websocket") &&
        getenv("HTTP_SEC_WEBSOCKET_VERSION") != NULL &&
        (cpstr = getenv("HTTP_SEC_WEBSOCKET_KEY")) != NULL) {

        (*this).isWebSocketReq = true;
        if ((*this).webSocket.handshake(cpstr)) {
            (*this).setHeader("Status", "101 Switching Protocols");
            (*this).setHeader("Upgrade", "websocket");
            (*this).setHeader("Connection", "Upgrade");
            (*this).setHeader("Sec-WebSocket-Accept", (*this).webSocket.generateAcceptKey(cpstr));
            (*this).putHeader();
            (*this).putContent("", 0);
        }
    }

    str.clear();
}


/*! Destructor of FCGI.
 *  @param void
 *  @return void
 */
FCGI::~FCGI() {
    (*this).clear();
}


/*! Delete instance of FCGI.
 *  @param void
 *  @retval true   success
 *  @retval false  failure
 */
bool FCGI::clear() {
    (*this).putHead = false;
    return (*this).requestData.clear() && (*this).requestFileName.clear() &&
           (*this).cookie.clear() && (*this).header.clear();
}


/*! Get request value.
 *  @param  key  key of FCGI parameter
 *  @return value of key
 */
String& FCGI::getValue(const String &key) {
    return (*this).requestData[key];
}


/*! Get request value.
 *  @param  key  key of FCGI parameter
 *  @return value of key
 */
String& FCGI::getValue(const char *key) {
    return (*this).requestData[key];
}


/*! Get request JSON.
 *  @param void
 *  @return JSONValue object of request
 */
const JSONValue& FCGI::getRequestJSON() const {
    return (*this).requestJSON;
}


/*! Get environment variable's value
 *  @param  key  key of Environment variable
 *  @return value of key
 */
const char *FCGI::getEnv(const String &key) const {
    return getenv(key.c_str());
}


/*! Get environment variable's value
 *  @param  key  key of Environment variable
 *  @return value of key
 */
const char *FCGI::getEnv(const char *key) const {
    return getenv(key?key:"");
}


/*! Get browser primary language
 *  @param void
 *  return language id, ex. "en", "ja", ..
 */
const char *FCGI::getPrimaryLanguage() const {
    return (const char *)(*this).primary_lang;
}


/*! Delete instance of FCGI.
 *  @param  key  key of FCGI parameter
 *  @return value of key
 */
String& FCGI::getFileName(const String &key, const char * src_charset, const char * dest_charset) {
    return ((String &)((*this).requestFileName[key])).unescapeQuote(src_charset, dest_charset);
}


/*! Delete instance of FCGI.
 *  @param  key  key of FCGI parameter
 *  @return value of key
 */
String& FCGI::getFileName(const char *key, const char * src_charset, const char * dest_charset) {
    return ((String &)((*this).requestFileName[key])).unescapeQuote(src_charset, dest_charset);
}

/*! Get cookie valiables.
 *  @param  key  key of Cookie
 *  @return value of key
 */
String& FCGI::getCookie(const char *key, const char * src_charset, const char * dest_charset) {
    return ((String &)((*this).cookie[key])).strconv(src_charset, dest_charset);
}


/*! Append HTTP header line
 *  @param  key   key of HTTP header
 *  @param  value value of key of HTTP header
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::setHeader(const String &key, const String &value) {
    return (*this).header.add(key + ": " + value);
}


/*! Clear HTTP header lines
 *  @param void
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::clearHeader() {
    return (*this).header.clear();
}


/*! Put HTTP header lines (put "\r\n" footer)
 *  @param void
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::putHeader() {
    long i, max;
    const char *cpstr;

    if ((*this).putHead == true) {
        return true;
    }

    max = header.max();

    for (i = 0; i < max; i++) {
        cpstr = header[i].c_str();
        FCGX_PutStr(cpstr, strlen(cpstr), (*this).request->out);
        FCGX_PutStr("\r\n", 2, (*this).request->out);
    }

    FCGX_PutStr("\r\n", 2, (*this).request->out);
    (*this).putHead = true;

    return true;
}


/*! Put HTTP content (put header if not put)
 *  @param content output content
 *  @param size    size of output content
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::putContent(const String &content, long size) {
    if (size < 0) {
        if (content.isBinary()) {
            size = content.binaryLength();
        } else {
            size = content.len();
        }
    }

    return (*this).putContent(content.c_str(), size);
}


/*! Put HTTP content (put header if not put)
 *  @param content output content
 *  @param size    size of output content
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::putContent(const char *content, long size) {
    if (content == NULL) {
        content = "";
    }
    if (size < 0) {
        size = strlen(content);
    }

    (*this).putHeader();

    FCGX_PutStr(content, (int)size, (*this).request->out);
    FCGX_FFlush((*this).request->out);

    return true;
}


/*! Put message to stderr
 *  @param err_str output string
 *  @param size    size of output error
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::putError(const String &err_str, long size) {
    if (size < 0) {
        size = err_str.len();
    }

    return (*this).putError(err_str.c_str(), size);
}


/*! Put message to stderr
 *  @param err_str output string
 *  @param size    size of output error
 *  @retval true  success
 *  @retval false failure
 */
bool FCGI::putError(const char *err_str, long size) {
    if (err_str == NULL) {
        err_str = "";
    }
    if (size < 0) {
        size = strlen(err_str);
    }

    //fwrite(err_str, (size_t)size, 1, stderr);
    FCGX_PutStr(err_str, (int)size, (*this).request->err);
    FCGX_FFlush((*this).request->err);

    return true;
}

bool FCGI::isWebSocketRequest() {
    return (*this).isWebSocketReq;
}

WebSocket& FCGI::getWebSocket() {
    return (*this).webSocket;
}

} // namespace apolloron
