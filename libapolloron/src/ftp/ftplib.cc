/*
  Some global functions for libOftp.

  Copyright (c) 2009-2009 hirohito higashi. All rights reserved.
  This file is distributed under BSD license.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>

#include "ftplib.h"

namespace {
using namespace apolloron;

#define TRANSFER_SEGMENT_SIZE  65536  /* block size(byte) of recv() */

#if 0
#define FTP_DEBUG
#endif

#ifdef FTP_DEBUG
#define DEBUGPRINT1(f,a1) fprintf(stderr, "(" __FILE__ ") " f,(a1))
#else
#define DEBUGPRINT1(f,a1)
#endif

#define copy_strerror() \
    strncpy(ftp->error_message, strerror(errno), sizeof(ftp->error_message)-1);

int sendn(int sd, const char *buf, int len, int flags);
int ftp_send_command(LIBOFTP *ftp, const char *cmd);
int ftp_receive_response(LIBOFTP *ftp, char *res_buf, long bufsiz);
int ftp_getready_active(LIBOFTP *ftp, const char *cmd, const char *fname);
int ftp_getready_pasv(LIBOFTP *ftp, const char *cmd, const char *fname);

#if defined( __QNX__ ) && !defined( __QNXNTO__ )
int snprintf( char *, int, char *, ... );
#endif


/*! read a line
 *@param  sd  socket descriptor
 *@param  buf  pointer of receive buffer
 *@param  bufsiz  buffer size
 *@param  flags  flags of recv()
 *@retval  int  number of bytes received, or -1 if an error.
 */
static int recv_line(int sd, char *buf, long bufsiz, int flags) {
    int n;
    char c;
    char *p = buf;
    int total_len = 0;

    bufsiz--;

    while( 1 ) {
        n = recv( sd, &c, 1, flags );
        if( n < 0 ) {
            if( errno == EINTR ) continue;
            *p = 0;
            DEBUGPRINT1("recv_line: get minus. %s\n", strerror(errno) );
            return n;
        }
        if( n == 0 ) {
            DEBUGPRINT1("recv_line: get zero. %s\n", strerror(errno) );
            break;
        }

        *p++ = c;
        total_len++;

        if( c == '\n' ) break;
        if( bufsiz < total_len ) break;
    }
    *p = 0;

    return total_len;
}


/*! send n bytes.
 *@param  sd  socket descriptor
 *@param  buf  pointer of send buffer
 *@param  len  message length
 *@param  flags  flags of send()
 *@retval  int  number of bytes sent, or -1 if an error.
 */
int sendn(int sd, const char *buf, int len, int flags) {
    int left = len;
    int n;

    while( left > 0 ) {
        n = send( sd, buf, left, flags );
        if( n < 0 ) {
            if( errno == EINTR ) continue;
            DEBUGPRINT1("sendn: get minus. %s\n", strerror(errno) );
            return n;
        }
        buf += n;
        left -= n;
    }

    return len;
}


/*! send ftp command
 *@param  ftp  pointer of LIBOFTP.
 *@param  cmd  ftp command.
 *@retval  int  0 is no error, or -1 if an error.
 */
int ftp_send_command(LIBOFTP *ftp, const char *cmd) {
    int ret;
    DEBUGPRINT1("SENDC: %s", cmd);
    ret = sendn( ftp->socket, cmd, strlen( cmd ), 0 );
    if( ret < 0 ) {
        copy_strerror();
        return ret;
    }

    return 0;
}


/*! receive response
 *
 *@param  ftp  pointer of LIBOFTP.
 *@param  res_buf  response buffer ( null ok )
 *@param  bufsiz  response buffer size.
 *@retval  int  reply code, or error code (minus value)
 *@note
 * set reply code to ftp attribute reply_code;
 */
int ftp_receive_response(LIBOFTP *ftp, char *res_buf, long bufsiz) {
    char str1[256];
    int n;

    ftp->reply_code = 0;
    if (res_buf) {
        *res_buf = 0;
    }

    /* receive 1st line. */
    if((n = recv_line(ftp->socket, str1, sizeof(str1), 0)) < 0) {
        if( errno == EAGAIN ) {
            return FTP_ERROR_TIMEOUT;
        }
        copy_strerror();
        return FTP_ERROR_OS;
    }
    DEBUGPRINT1("RESP1: %s\n", str1);
    if(n < 3) {
        return FTP_ERROR_PROTOCOL;
    }
    ftp->reply_code = atoi(str1);

    /* if exist next line, receive all response. */
    if (str1[3] == '-') {
        while(1) {
            if((n = recv_line( ftp->socket, str1, sizeof(str1), 0 )) < 0) {
                if( errno == EAGAIN ) {
                    return FTP_ERROR_TIMEOUT;
                }
                copy_strerror();
                return FTP_ERROR_OS;
            }
            DEBUGPRINT1("RESP2: %s\n", str1);
            if( n < 3 ) {
                return FTP_ERROR_PROTOCOL;
            }

            if (atoi(str1) == ftp->reply_code && str1[3] == ' ') {
                break;
            }
        }
    }

    /* copy a response strings to user buffer, if need */
    if (res_buf) {
        strncpy( res_buf, str1, bufsiz );
        res_buf[bufsiz-1] = 0;
    }

    return ftp->reply_code;
}


/*! Prepare send/receive: active mode
 *@param  ftp  pointor of LIBOFTP
 *@param  cmd  FTP command (ex: RETR, STOR, APPE)
 *@param  fname  filename on server
 *@retval  int  socket (error code if < 0)
 *@note
 * This support 3 status below in active mode:
 * ex) receiving
 *    (port open) --> PORT --> RETR
 */
int ftp_getready_active(LIBOFTP *ftp, const char *cmd, const char *fname) {
    int sock_listen, sock_accept = -1;
    struct sockaddr_in saddr;
    socklen_t saddr_len;
    unsigned char *ip, *pt;
    char str1[256];
    int res;
    struct timeval timeout;
    fd_set rfds, wfds;
    int flag_status_ok = 0;

    /*
     * open data port.
     * (note)
     *  same command port ip and automatic random port.
     */
    sock_listen = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if (sock_listen < 0) {
        DEBUGPRINT1("getready_active: can't open socket. %s\n", strerror(errno));
        goto ERROR_OS;
    }

    memset( &saddr, 0, sizeof( saddr ) );
    saddr_len = sizeof( saddr );
    if (getsockname(ftp->socket, (struct sockaddr *)&saddr, &saddr_len ) < 0) {
        DEBUGPRINT1("getready_active: can't get control socket name. %s\n", strerror(errno));
        goto ERROR_OS;
    }
    saddr.sin_port = htons( 0 );

    if (bind(sock_listen, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        DEBUGPRINT1("getready_active: can't bind socket. %s\n", strerror(errno));
        goto ERROR_OS;
    }

    if (listen( sock_listen, 1 ) < 0) {
        DEBUGPRINT1( "getready_active: can't listen socket. %s\n", strerror(errno) );
        goto ERROR_OS;
    }

    /* make PORT command. */
    memset( &saddr, 0, sizeof( saddr ) );
    saddr_len = sizeof( saddr );
    if (getsockname( sock_listen, (struct sockaddr *)&saddr, &saddr_len ) < 0) {
        DEBUGPRINT1("getready_active: can't get data socket name. %s\n", strerror(errno));
        goto ERROR_OS;
    }
    ip = (unsigned char*)&saddr.sin_addr.s_addr;
    pt = (unsigned char*)&saddr.sin_port;
    snprintf( str1, sizeof(str1), "PORT %d,%d,%d,%d,%d,%d\r\n", ip[0], ip[1], ip[2], ip[3], pt[0], pt[1] );

    /* send PORT command and get status. */
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("getready_active: %s command sending error.\n", "PORT");
        close( sock_listen );
        return FTP_ERROR_OS;
    }
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 200) { /* 200: Command okay. */
        DEBUGPRINT1("getready_active: get PORT response code %d\n", res);
        close(sock_listen);
        return FTP_ERROR_PROTOCOL;
    }

    /* send ftp command. */
    if (fname) {
        snprintf( str1, sizeof(str1), "%s %s\r\n", cmd, fname );
    } else {
        snprintf( str1, sizeof(str1), "%s\r\n", cmd );
    }
    if(ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("getready_active: command sending error. %s", str1);
        close(sock_listen);
        return FTP_ERROR_OS;
    }

    /* accept data connection with timeout, or get status by control connection. */
    timeout.tv_sec = ftp->timeout_sec;
    timeout.tv_usec = 0;
    do {
        int nfds = (sock_listen > ftp->socket)? sock_listen: ftp->socket;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        if (sock_listen >= 0) {
            FD_SET(sock_listen, &rfds);
            FD_SET(sock_listen, &wfds);
        }
        FD_SET(ftp->socket, &rfds);

        res = select( nfds+1, &rfds, &wfds, 0, &timeout );
        /* ckeck timeout */
        if(res == 0) {
            DEBUGPRINT1("getready_active: waiting connection timeout.%s\n", "");
            close(sock_listen );
            strncpy(ftp->error_message, "connection timeout.", sizeof(ftp->error_message ) - 1);
            return FTP_ERROR_TIMEOUT;
        }

        /* check some OS errors. */
        if( res < 0 ) {
            if( errno == EINTR ) continue;
            DEBUGPRINT1("getready_active: select error. %s\n", strerror(errno));
            goto ERROR_OS;
        }

        /* accept data connection. */
        if( FD_ISSET( sock_listen, &rfds ) || FD_ISSET( sock_listen, &wfds ) ) {
            sock_accept = accept( sock_listen, (struct sockaddr *)&saddr, &saddr_len );
            close( sock_listen );
            sock_listen = -1;
            if( sock_accept < 0 ) {
                DEBUGPRINT1("getready_active: socket accept error. %s\n", strerror(errno));
                goto ERROR_OS;
            }
            DEBUGPRINT1("getready_active: socket accept. fd=%d\n", sock_accept);
        }

        /* get status. */
        if( FD_ISSET( ftp->socket, &rfds ) ) {
            res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
            DEBUGPRINT1("getready_active: get STOR/RETR response code %d\n", res);
            if( res != 150 ) { /* 150: File status okay; about to open data connection. */
                if( sock_listen >= 0 ) close( sock_listen );
                if( sock_accept >= 0 ) close( sock_accept );
                return FTP_ERROR_PROTOCOL;
            }
            flag_status_ok = 1;
        }

    } while(!(sock_accept >= 0 && flag_status_ok));

    return sock_accept;

ERROR_OS:
    copy_strerror();
    if (sock_listen >= 0) {
        close(sock_listen);
    }
    return FTP_ERROR_OS;
}


/*! Prepare send/receive: passive mode
 *@param  ftp  pointor of LIBOFTP
 *@param  cmd  FTP command (ex: RETR, STOR, APPE)
 *@param  fname  filename on server
 *@retval  int  socket (error code if < 0)
 *@note
 * This support 3 status below in passive mode:
 * ex) receiving
 *    PASV --> RETR --> (port open)
 */
int ftp_getready_pasv(LIBOFTP *ftp, const char *cmd, const char *fname) {
    char str1[256];
    int res;
    char *p;
    int h1, h2, h3, h4, p1, p2;
    struct sockaddr_in saddr;
    int sock;
    struct timeval tval;

    /* send "PASV" command and get status. */
    str1[255] = 0;
    snprintf(str1, sizeof(str1)-1, "PASV\r\n");
    if( ftp_send_command( ftp, str1 ) < 0 ) {
        return FTP_ERROR_OS;
    }
    res = ftp_receive_response(ftp, str1, sizeof(str1) - 1);
    if( res != 227 ) { /* 227: Entering Passive Mode */
        DEBUGPRINT1("getready_pasv: get ftp response code %d\n", res);
        strncpy(ftp->error_message, str1, sizeof( ftp->error_message ) - 1);
        return FTP_ERROR_PROTOCOL;
    }

    /* parse (h1,h2,h3,h4,p1,p2) */
    p = strchr( str1, '(' );
    if (p == NULL) {
        DEBUGPRINT1("getready_pasv: response parse error. %d", 1);
        strncpy(ftp->error_message, "Illegal pasv response", sizeof(ftp->error_message) - 1);
        return FTP_ERROR_PROTOCOL;
    }
    if (sscanf(p+1, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
        DEBUGPRINT1("getready_pasv: response parse error. %d", 2);
        strncpy(ftp->error_message, "Illegal pasv response", sizeof(ftp->error_message) - 1);
        return FTP_ERROR_PROTOCOL;
    }

    /* send ftp command. */
    if (fname) {
        snprintf(str1, sizeof(str1), "%s %s\r\n", cmd, fname);
    } else {
        snprintf(str1, sizeof(str1), "%s\r\n", cmd);
    }
    if(ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("getready_pasv: command sending error. %s", str1);
        return FTP_ERROR_OS;
    }

    /* open data port. */
    sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( sock < 0 ) {
        DEBUGPRINT1("getready_pasv: can't open socket. %s\n", strerror(errno));
        copy_strerror();
        return FTP_ERROR_OS;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl((((unsigned long)h1) << 24) | (h2 << 16) | (h3 << 8) | h4);
    saddr.sin_port = htons((p1 << 8) | p2);

    if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        DEBUGPRINT1("getready_pasv: can't connect socket. %s\n", strerror(errno));
        copy_strerror();
        close(sock);
        return FTP_ERROR_OS;
    }

    tval.tv_sec = ftp->timeout_sec;
    tval.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(struct timeval)) < 0) {
        DEBUGPRINT1("setsockopt(SO_RCVTIMEO) failed. %s\n", strerror(errno));
        copy_strerror();
        close(sock);
        return FTP_ERROR_OS;
    }

    /* check response. */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res == 150 || res == 125) {
        return sock;
    } else {
        close(sock);
        return FTP_ERROR_PROTOCOL;
    }
}

} // namespace


namespace apolloron {

/*! sending file main
 *
 *@param    ftp         pointer to LIBOFTP
 *@param    local_fname local filename
 *@param    fname       server filename
 *@param    cmd         FTP command to send
 *@retval   int         error code
 */
static int ftp_put_file_main(LIBOFTP *ftp, const char *local_fname, const char *fname, const char *cmd ) {
    int data_socket;
    int fd;
    char buf[TRANSFER_SEGMENT_SIZE];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* open local file */
    fd = open(local_fname, O_RDONLY);
    if (fd < 0) {
        DEBUGPRINT1("local file open error. %s\n", strerror(errno));
        return FTP_ERROR_OS;
    }

    /* prepair for sending */
    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, cmd, fname);
    } else {
        data_socket = ftp_getready_active(ftp, cmd, fname);
    }
    if (data_socket < 0) {
        close(fd);
        return data_socket;
    }

    /* block send for care for timeout */
    while(1) {
        int n_rd, n_wr;
        n_rd = read(fd, buf, TRANSFER_SEGMENT_SIZE);
        if (n_rd == 0) break;

        if (n_rd < 0) {
            if (errno == EINTR) continue;

            DEBUGPRINT1("local file read error. %s\n", strerror(errno));
            copy_strerror();
            close(data_socket);
            return FTP_ERROR_OS;
        }

        n_wr = sendn(data_socket, buf, n_rd, 0);
        DEBUGPRINT1("SEND: n=%d\n", n_wr);
        if (n_wr < 0) {
            copy_strerror();
            close(data_socket);
            return FTP_ERROR_OS;
        }
    }
    close(fd);
    close(data_socket);

    /* receive response. */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 226) { /* 226: Closing data connection. */
        DEBUGPRINT1("got illegal response %d\n", res);
        return res<0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! Get list into buffer (procedure)
 *@param    ftp    pointor to LIBOFTP
 *@param    cmd    command (RETR|LIST|NLST)
 *@param    fname  filename or blob
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@retval   int    receive size (byte), error code if minus
 */
static int ftp_get_buffer_main(LIBOFTP *ftp, const char *cmd, const char *fname, char *buf, long bufsiz) {
    int data_socket;
    char *p = buf;
    int res;

    if (ftp->socket < 0 ) return FTP_ERROR;

    /* prepare to receive */
    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, cmd, fname);
    } else {
        data_socket = ftp_getready_active(ftp, cmd, fname);
    }
    if (data_socket < 0) {
        return data_socket;
    }

    /* block recv to care for timeout */
    while (1) {
        int n;
        int len = bufsiz;
        if (len > TRANSFER_SEGMENT_SIZE) {
            len = TRANSFER_SEGMENT_SIZE;
        }
        n = recv( data_socket, p, len, 0);
        DEBUGPRINT1("RECV: n=%d\n", n);
        if (n < 0) {
            int ret;
            DEBUGPRINT1("recv error. %s\n", strerror(errno));
            if (errno == EAGAIN) {
                ret = FTP_ERROR_TIMEOUT;
            } else {
                ret = FTP_ERROR_OS;
                copy_strerror();
            }
            close( data_socket);
            return ret;
        }
        if (n == 0) {
            break;
        }
        p += n;
        bufsiz -= n;
        if (bufsiz <= 0) {
            break; /* buffer too small */
        }
    }

    if (0 < bufsiz) { /* buffer is small? */
        /* enough */
        close( data_socket);

        /* receive response. */
        res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
        if (res != 226) { /* 226: Closing data connection. */
            DEBUGPRINT1("got illegal response %d\n", res);
            return res < 0? res: FTP_ERROR_PROTOCOL;
        }

        return p - buf; /* return with transfered bytes */

    } else {
        /* not enough buffer */
        DEBUGPRINT1("buffer too small. %s\n", "");
        strncpy(ftp->error_message, "buffer too small", sizeof( ftp->error_message ) - 1);

        if (ftp_send_command( ftp, "ABOR\r\n" ) < 0) {
            DEBUGPRINT1("command sending error. %s\n", "ABOR");
            close(data_socket);
            return FTP_ERROR_BUFFER;
        }

        res = ftp_receive_response(ftp, 0, 0);
        if (res == 426) { /* 426: Connection closed; transfer aborted. */
            res = ftp_receive_response(ftp, 0, 0);
        }
        close(data_socket);
        if (res != 226) { /* 226: Closing data connection. */
            DEBUGPRINT1("got illegal response %d\n", res);
            return (res < 0)? res: FTP_ERROR_PROTOCOL;
        }

        return FTP_ERROR_BUFFER;
    }
}


/*! Get list into new buffer (procedure)
 *@param    ftp    pointor to LIBOFTP
 *@param    cmd    command (RETR|LIST|NLST)
 *@param    fname  filename or blob
 *@param    dup_buf[out]  pointor to buffer (need free if not NULL)
 *@retval   int    receive size (byte), error code if minus
 */
static int ftp_get_buffer_dup_main(LIBOFTP *ftp, const char *cmd, const char *fname, char **dup_buf, long *dup_buf_size) {
    int data_socket;
    char tmp_buf[TRANSFER_SEGMENT_SIZE+1];
    long size = 0;
    int res;

    if (dup_buf == NULL ) return FTP_ERROR;
    *dup_buf = NULL;
    if (ftp->socket < 0 ) return FTP_ERROR;

    /* prepare to receive */
    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, cmd, fname);
    } else {
        data_socket = ftp_getready_active(ftp, cmd, fname);
    }
    if (data_socket < 0) {
        return data_socket;
    }

    /* block recv to care for timeout */
    while (1) {
        char *p;
        int n;
        int ret = FTP_NOERROR;
        n = recv( data_socket, tmp_buf, TRANSFER_SEGMENT_SIZE, 0);
        DEBUGPRINT1("RECV: n=%d\n", n);
        if (n < 0) {
            DEBUGPRINT1("recv error. %s\n", strerror(errno));
            if (errno == EAGAIN) {
                ret = FTP_ERROR_TIMEOUT;
            } else {
                ret = FTP_ERROR_OS;
                copy_strerror();
            }
            close( data_socket);
            return ret;
        }
        if (n == 0) {
            break;
        }
        p = new char [size + n + 1];
        if (p == NULL) {
            ret = FTP_ERROR_OS;
            copy_strerror();
            close( data_socket);
            if (*dup_buf) {
                delete [] (*dup_buf);
                *dup_buf = NULL;
            }
            return ret;
        }
        memcpy(p, (*dup_buf), size);
        delete [] (*dup_buf);
        *dup_buf = p;
        memcpy((*dup_buf) + size, tmp_buf, n);
        size += n;
        (*dup_buf)[size] = '\0';
    }

    /* alright */
    close( data_socket);

    /* receive response. */
    res = ftp_receive_response( ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 226) { /* 226: Closing data connection. */
        DEBUGPRINT1("got illegal response %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    if (dup_buf_size != NULL) *dup_buf_size = size;

    return FTP_NOERROR;
}


static const char * my_hstrerror(int no) {
    const char *unknown_e = "Unknown error";
    switch(no) {
        case HOST_NOT_FOUND:
            return "Unknown host";
        case TRY_AGAIN:
            return "Host name lookup failure";
        case NO_RECOVERY:
            return "Unknown server error";
        case NO_DATA:
            return "No address associated with name";
        default:
            return unknown_e;
    }
    return unknown_e;
}


/*! Init ftp structure
 *@param   ftp   pointor to LIBOFTP
 *@retval  int   error code
 *@note
 * ftp structure must be allocated by caller
 */
int ftp_initialize(LIBOFTP *ftp) {
    memset(ftp, 0, sizeof(LIBOFTP));
    ftp->socket = -1;
    ftp->timeout_sec = 10; /* default timeout */

    return FTP_NOERROR;
}


/*! Connect ftp server
 *@param   ftp   pointor to LIBOFTP
 *@param   host  host name or IP address
 *@param   port  port number (use default port if 0)
 *@retval  int   error code
 *@note
 * ftp structure must be allocated by caller
 */
int ftp_open(LIBOFTP *ftp, const char *host, int port) {
    struct hostent *p_hostent;
    int res;

    if (ftp->socket >= 0) return FTP_ERROR;
    if (port == 0) port = 21;

    /*  search host entry */
    p_hostent = gethostbyname( host);
    if (p_hostent == NULL) {
        DEBUGPRINT1("gethostbyname() function failed. %s\n", my_hstrerror(h_errno));
        strncpy(ftp->error_message, my_hstrerror(h_errno), sizeof(ftp->error_message) - 1);
        return FTP_ERROR_OS;
    }
    ftp->saddr.sin_family = AF_INET;
    memcpy(&ftp->saddr.sin_addr, p_hostent->h_addr, p_hostent->h_length);
    ftp->saddr.sin_port = htons( port);

    /* make control connection. */
    ftp->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ftp->socket < 0) {
        DEBUGPRINT1("can't create socket. %s\n", strerror(errno));
        copy_strerror();
        ftp->socket = -1;
        return FTP_ERROR_OS;
    }

    if (connect(ftp->socket, (struct sockaddr *)&(ftp->saddr), sizeof(struct sockaddr)) < 0) {
        DEBUGPRINT1("can't connect socket. %s\n", strerror(errno));
        copy_strerror();
        close(ftp->socket);
        ftp->socket = -1;
        return FTP_ERROR_OS;
    }

    res = ftp_timeout(ftp, ftp->timeout_sec);
    if (res < 0) {
        close(ftp->socket);
        ftp->socket = -1;
        return res;
    }

    /* receive banner */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message));
    if (res != 220) { /* 220: Service ready for new user. */
        DEBUGPRINT1("Illegal response. %d\n", res);
        close(ftp->socket);
        ftp->socket = -1;
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! User Authentication
 *@param  ftp   pointor of LIBOFTP
 *@param  user  user name
 *@param  pass  password
 *@retval int   error code
 *@note
 * send general commands after auth
 */
int ftp_user(LIBOFTP *ftp, const char *user, const char *pass) {
    char str1[256];
    int res;

    if (ftp->socket < 0 ) return FTP_ERROR;

    /* send user name. */
    snprintf(str1, sizeof(str1)-1, "USER %s\r\n", user);
    if (ftp_send_command( ftp, str1 ) < 0 ) {
        DEBUGPRINT1("user command sending error.%s\n", "");
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response( ftp, ftp->error_message, sizeof(ftp->error_message));
    if (res == 230) { /* 230: User logged in, proceed. */
        goto PROCEED;
    }
    if (res != 331) { /* 331: User name okay, need password. */
        DEBUGPRINT1("USER command response error. %d\n", res);
        return (res < 0)? res: FTP_ERROR_PROTOCOL;
    }

    /* send password. */
    snprintf(str1, sizeof(str1)-1, "PASS %s\r\n", pass);
    if (ftp_send_command( ftp, str1 ) < 0) {
        DEBUGPRINT1("pass command sending error.%s\n", "");
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message));
    if (res != 230) { /* 230: User logged in, proceed. */
        DEBUGPRINT1("user authentication error.%s\n", "");
        return (res < 0)? res: FTP_ERROR_PROTOCOL;
    }

PROCEED:
    /* system type. */
    snprintf( str1, sizeof(str1)-1, "SYST\r\n");
    if (ftp_send_command( ftp, str1 ) < 0) {
        DEBUGPRINT1("SYST command sending error.%s\n", "");
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, str1, sizeof(str1));
    if (res == 215 ) {
        if (strstr(str1, "UNIX") != 0 ) {
            ftp->system_type = UNIX;
        } else if (strstr(str1, "Windows_NT") != 0) {
            ftp->system_type = Windows_NT;
        }
        DEBUGPRINT1("system type is %d\n", ftp->system_type);
    }

    return FTP_NOERROR;
}


/*! Set/Unset PASSIVE MODE
 *@param   ftp   pointor to LIBOFTP
 *@param   flag  ON(PASSIVE)/OFF
 *@retval  int   0
 */
int ftp_passive(LIBOFTP *ftp, int flag) {
    ftp->flag_passive = flag? 1: 0;

    return 0;
}




/*! set timeout
 *@param   ftp  pointor to LIBOFTP
 *@param   sec  timeout (sec)
 *@retval  int  error code
 *@note
 */
int ftp_timeout(LIBOFTP *ftp, int sec) {
    struct timeval tval;

    ftp->timeout_sec = sec;
    if (ftp->socket < 0) {
        return FTP_NOERROR;
    }

    tval.tv_sec = ftp->timeout_sec;
    tval.tv_usec = 0;
    if (setsockopt(ftp->socket, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(struct timeval)) < 0) {
        DEBUGPRINT1("setsockopt(SO_SNDTIMEO) failed. %s\n", strerror(errno) );
        copy_strerror();
        return FTP_ERROR_OS;
    }
    if (setsockopt(ftp->socket, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(struct timeval)) < 0) {
        DEBUGPRINT1("setsockopt(SO_RCVTIMEO) failed. %s\n", strerror(errno));
        copy_strerror();
        return FTP_ERROR_OS;
    }

    return FTP_NOERROR;
}


/*! TYPE command
 *@param   ftp   pointor to LIBOFTP
 *@param   type  {ascii|binary|image}
 *@retval  int   error code
 *@note
 * send "TYPE" command only, data transfer always do without encoding.
 * if you need CR => CRLF, do it yourself
 */
int ftp_type(LIBOFTP *ftp, const char *type) {
    char str1[256];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    if (strcmp(type, "ascii" ) == 0) {
        snprintf(str1, sizeof(str1)-1, "TYPE A\r\n");
        ftp->data_type = ASCII;
    } else if (strcmp(type, "binary") == 0) {
        snprintf( str1, sizeof(str1)-1, "TYPE I\r\n");
        ftp->data_type = IMAGE;
    } else if (strcmp( type, "image" ) == 0) {
        snprintf( str1, sizeof(str1)-1, "TYPE I\r\n");
        ftp->data_type = IMAGE;
    } else {
        DEBUGPRINT1("argument error. %s\n", type);
        strncpy(ftp->error_message, "argument error.", sizeof(ftp->error_message));
        return FTP_ERROR;
    }

    /* send TYPE command */
    if (ftp_send_command( ftp, str1 ) < 0 ) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 200) { /* 200: Command okay. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! QUIT command
 *@param    ftp  pointor to LIBOFTP
 *@retval   int  error code
 */
int ftp_quit(LIBOFTP *ftp) {
    char str1[] = "QUIT\r\n";
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* send QUIT command */
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        res = FTP_ERROR_OS;
        goto CLOSE;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res == 221) { /* 221: Service closing control connection. */
        res = FTP_NOERROR;
    } else {
        DEBUGPRINT1("command response error. %d\n", res);
        if (res >= 0 ) res = FTP_ERROR_PROTOCOL;
    }

    /* close socket. */
CLOSE:
    close(ftp->socket);
    ftp->socket = -1;

    return res;
}


/*! recover error
 *@param   ftp  pointor to LIBOFTP
 *@retval  int  error code
 *@note
 * when error orrured and ftp command sequense broke, for trying resync
 * clear reply queue.
 */
int ftp_reset(LIBOFTP *ftp) {
    int now_timeout_sec = ftp->timeout_sec;
    int ret;
    int i;

    if (ftp->socket < 0) return FTP_ERROR;

    /* send "CRLF" only */
    if (ftp_send_command(ftp, "\r\n") < 0) {
        DEBUGPRINT1("command sending error. %s\n", "CRLF");
        return FTP_ERROR_OS;
    }

    /* receive response until timeout. 100 cycle maxmum. */
    ftp_timeout(ftp, 1); /* set 1 sec */

    for(i = 0; i < 100; i++) {
        ret = ftp_receive_response(ftp, 0, 0);
        if (ret == FTP_ERROR_TIMEOUT) {
            ret = FTP_NOERROR;
            break;
        }

        if (ret == FTP_ERROR_OS) {
            DEBUGPRINT1("os level error occurred.\n%s", "");
            break;
        }
    }

    ftp_timeout( ftp, now_timeout_sec );
    return ret;
}


/*! NOOP command
 *@param   ftp      pointor to LIBOFTP
 *@retval  int      error code
 *@note
 */
int ftp_noop(LIBOFTP *ftp) {
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* send NOOP command */
    if (ftp_send_command( ftp, "NOOP\r\n") < 0) {
        DEBUGPRINT1("noop: command sending error%s\n", "");
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 200) { /* 200: Command okay. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! SITE command
 *@param   ftp      pointor to LIBOFTP
 *@param   cmdline  command line
 *@retval  int      error code
 *@note
 */
int ftp_site(LIBOFTP *ftp, const char *cmdline) {
    char str1[512];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* send SITE command */
    snprintf(str1, sizeof(str1)-1, "SITE %s\r\n", cmdline);
    if (ftp_send_command( ftp, str1 ) < 0) {
        DEBUGPRINT1("site: command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 200) { /* 200: Command okay. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! DELE command
 *@param    ftp    pointor to LIBOFTP
 *@param    fname  target filename
 *@retval   int    error code
 */
int ftp_delete(LIBOFTP *ftp, const char *fname) {
    char str1[512];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* send DELE command */
    snprintf(str1, sizeof(str1)-1, "DELE %s\r\n", fname);
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 250) { /* 250: Requested file action okay, completed. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! rename file
 *@param    ftp    pointor to LIBOFTP
 *@param    from   target filename
 *@param    to     filename to change
 *@retval   int    error code
 *@note
 */
int ftp_rename(LIBOFTP *ftp, const char *from, const char *to) {
    char str1[512];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* send RNFR command */
    snprintf(str1, sizeof(str1)-1, "RNFR %s\r\n", from);
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("RNFR command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 350 ) { /* 350: Requested file action pending further information */
        DEBUGPRINT1("RNFR command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    /* send RNTO command */
    snprintf(str1, sizeof(str1)-1, "RNTO %s\r\n", to);
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("rename: RNTO command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 250) { /* 250: Requested file action okay, completed. */
        DEBUGPRINT1("RNTO command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! Get file data into buffer
 *@param    ftp    pointor to LIBOFTP
 *@param    fname  filename on server
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@retval   int    size(byte) or error code if minus
 */
int ftp_get_buffer(LIBOFTP *ftp, const char *fname, char *buf, long bufsiz) {
    return ftp_get_buffer_main(ftp, "RETR", fname, buf, bufsiz);
}


/*! Get file data into new buffer
 *@param    ftp    pointor to LIBOFTP
 *@param    fname  filename on server
 *@param    dup_buf[out]  pointor to buffer (need free if not NULL)
 *@retval   int    size(byte) or error code if minus
 */
int ftp_get_buffer_dup(LIBOFTP *ftp, const char *fname, char **dup_buf, long *dup_buf_size) {
    return ftp_get_buffer_dup_main(ftp, "RETR", fname, dup_buf, dup_buf_size);
}


/*! Send file from buffer main
 *@param    ftp    pointor to LIBOFTP
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@param    fname  filename on server
 *@param    cmd    FTP command to send
 *@retval   int    error code
 */
static int ftp_put_buffer_main(LIBOFTP *ftp, const char *buf, long bufsiz, const char *fname, const char *cmd) {
    int data_socket;
    const char *p = buf;
    int n, res;

    if (ftp->socket < 0) return FTP_ERROR;

    /* prepare to send */
    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, cmd, fname);
    } else {
        data_socket = ftp_getready_active(ftp, cmd, fname);
    }
    if (data_socket < 0) {
        return data_socket;
    }

    /* block sending to care for timeout */
    while(1) {
        int len = bufsiz;
        if (len > TRANSFER_SEGMENT_SIZE) {
            len = TRANSFER_SEGMENT_SIZE;
        }
        n = sendn( data_socket, p, len, 0);
        DEBUGPRINT1("SEND: n=%d\n", n);
        if (n < 0) {
            DEBUGPRINT1("recv error. %s\n", strerror(errno));
            copy_strerror();
            close(data_socket);
            return FTP_ERROR_OS;
        }

        p += n;
        bufsiz -= n;
        if (bufsiz <= 0) {
            break;
        }
    }
    close( data_socket);

    /*
     * receive response.
     */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 226) { /* 226: Closing data connection. */
        DEBUGPRINT1("got illegal response %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! Send file from buffer
 *@param    ftp    pointor to LIBOFTP
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@param    fname  filename on server
 *@retval   int    error code
 */
int ftp_put_buffer( LIBOFTP *ftp, const char *buf, long bufsiz, const char *fname) {
    return ftp_put_buffer_main( ftp, buf, bufsiz, fname, "STOR");
}


/*! Send file from buffer: append mode
 *@param    ftp    pointor to LIBOFTP
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@param    fname  filename on server
 *@retval   int    error code
 */
int ftp_append_buffer(LIBOFTP *ftp, const char *buf, long bufsiz, const char *fname) {
    return ftp_put_buffer_main(ftp, buf, bufsiz, fname, "APPE");
}


/*! Get file and store to local
 *@param   ftp          pointor to LIBOFTP
 *@param   fname        filename on server
 *@param   local_fname  local filename
 *@retval  int          error code
 */
int ftp_get_file(LIBOFTP *ftp, const char *fname, const char *local_fname) {
    int data_socket;
    int n, res;
    int fd;
    char buf[TRANSFER_SEGMENT_SIZE];

    if (ftp->socket < 0) return FTP_ERROR;

    /* open local file */
    fd = open(local_fname, O_WRONLY|O_CREAT, 0644);
    if (fd < 0) {
        DEBUGPRINT1("local file open error. %s\n", strerror(errno));
        copy_strerror();
        return FTP_ERROR_OS;
    }

    /* prepare to receive */
    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, "RETR", fname);
    } else {
        data_socket = ftp_getready_active(ftp, "RETR", fname);
    }
    if (data_socket < 0) {
        close(fd);
        return data_socket;
    }

    /* block recv to care timeout */
    while(1) {
        n = recv(data_socket, buf, TRANSFER_SEGMENT_SIZE, 0);
        DEBUGPRINT1("RECV: n=%d\n", n);
        if (n < 0) {
            int ret;
            DEBUGPRINT1("recv error. %s\n", strerror(errno));
            if (errno == EAGAIN) {
                ret = FTP_ERROR_TIMEOUT;
            } else {
                ret = FTP_ERROR_OS;
                copy_strerror();
            }
            close( fd);
            close( data_socket);
            return ret;
        }
        if (n == 0) break;

        if (write(fd, buf, n) != n) {
            DEBUGPRINT1("write error. %s\n", strerror(errno));
            copy_strerror();
            close(fd);
            close(data_socket);
            return FTP_ERROR_OS;
        }
    }
    close(fd);
    close(data_socket);

    /* receive response. */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 226) { /* 226: Closing data connection. */
        DEBUGPRINT1("got illegal response %d\n", res);
        return (res < 0)? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! Send file
 *@param    ftp          pointor to LIBOFTP
 *@param    local_fname  local filename
 *@param    fname        server filename
 *@retval   int          error code
 */
int ftp_put_file(LIBOFTP *ftp, const char *local_fname, const char *fname) {
    return ftp_put_file_main(ftp, local_fname, fname, "STOR");
}


/*! Send file: append mode
 *@param    ftp          pointor to LIBOFTP
 *@param    local_fname  local filename
 *@param    fname        server filename
 *@retval   int          error code
 */
int ftp_append_file(LIBOFTP *ftp, const char *local_fname, const char *fname) {
    return ftp_put_file_main(ftp, local_fname, fname, "APPE");
}


/*! Create descriptor to get file
 *@param   ftp    pointor to LIBOFTP
 *@param   fname  filename on server
 *@retval  int    descriptor, error code if minus
 */
int ftp_get_descriptor(LIBOFTP *ftp, const char *fname) {
    int data_socket;

    if (ftp->socket < 0) return FTP_ERROR;

    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, "RETR", fname);
    } else {
        data_socket = ftp_getready_active(ftp, "RETR", fname);
    }

    return data_socket;
}


/*! Close descriptor to get file
 *@param   ftp    pointor to LIBOFTP
 *@param   desc   descriptor
 *@retval  int    error code
 */
int ftp_get_descriptor_close(LIBOFTP *ftp, int desc) {
    int res;

    /* close socket. */
    close(desc);

    if (ftp->socket < 0 ) return FTP_ERROR;

    /* receive response. */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 226) { /* 226: Closing data connection. */
        DEBUGPRINT1("got illegal response %d\n", res);
        return res<0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! Create file descriptor to send
 *@param    ftp    pointor to LIBOFTP
 *@param    fname  filename on server
 *@retval   int    descriptor, minus if error
 */
int ftp_put_descriptor( LIBOFTP *ftp, const char *fname) {
    int data_socket;

    if (ftp->socket < 0) return FTP_ERROR;

    if (ftp->flag_passive) {
        data_socket = ftp_getready_pasv(ftp, "STOR", fname);
    } else {
        data_socket = ftp_getready_active(ftp, "STOR", fname);
    }
    return data_socket;
}


/*! Close file discriptor to send
 *@param    ftp    pointor to LIBOFTP
 *@param    desc   descriptor
 *@retval   int    error code
 */
int ftp_put_descriptor_close(LIBOFTP *ftp, int desc) {
    int res;

    /*
     * close socket.
     */
    close( desc);

    if (ftp->socket < 0 ) return FTP_ERROR;

    /* receive response. */
    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 226) { /* 226: Closing data connection. */
        DEBUGPRINT1("got illegal response %d\n", res);
        return (res < 0)? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! Create file descriptor to send: append mode
 *@param    ftp    pointor to LIBOFTP
 *@param    fname  filename on server
 *@retval   int    descriptor, minus if error
 */
int ftp_append_descriptor(LIBOFTP *ftp, const char *fname) {
    int data_socket;

    if (ftp->socket < 0 ) return FTP_ERROR;

    if (ftp->flag_passive ) {
        data_socket = ftp_getready_pasv(ftp, "APPE", fname);
    } else {
        data_socket = ftp_getready_active(ftp, "APPE", fname);
    }
    return data_socket;
}


/*! Close file discriptor to send: append mode
 *@param    ftp    pointor to LIBOFTP
 *@param    desc   descriptor
 *@retval   int    error code
 */
int ftp_append_descriptor_close(LIBOFTP *ftp, int desc) {
    return ftp_put_descriptor_close(ftp, desc);
}


/*! Get directory list(LIST)
 *@param    ftp    pointor to LIBOFTP
 *@param    fglob  file list glob (ex: *.txt) or NULL
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@retval   int    error code
 */
int ftp_list(LIBOFTP *ftp, const char *fglob, char *buf, long bufsiz) {
    int ret;

    ret = ftp_get_buffer_main(ftp, "LIST", fglob, buf, bufsiz);
    if (0 < ret) {
        buf[ret] = 0;
        return FTP_NOERROR;
    }

    return ret;
}


/*! Get directory list into new buffer(LIST)
 *@param    ftp    pointor to LIBOFTP
 *@param    fglob  file list glob (ex: *.txt) or NULL
 *@param    dup_buf[out]  pointor to buffer (need free if not NULL)
 *@retval   int    error code
 */
int ftp_list_dup(LIBOFTP *ftp, const char *fglob, char **dup_buf, long *dup_buf_size) {
    int ret;

    ret = ftp_get_buffer_dup_main(ftp, "LIST", fglob, dup_buf, dup_buf_size);

    return ret;
}


/*! Get directory list(NLST)
 *@param    ftp    pointor to LIBOFTP
 *@param    fglob  file list glob (ex: *.txt) or NULL
 *@param    buf    pointor to buffer
 *@param    bufsiz size of buffer
 *@retval   int    error code
 */
int ftp_nlist(LIBOFTP *ftp, const char *fglob, char *buf, long bufsiz ) {
    int ret;

    ret = ftp_get_buffer_main(ftp, "NLST", fglob, buf, bufsiz);
    if (0 < ret) {
        buf[ret] = 0;
        return FTP_NOERROR;
    }

    return ret;
}


/*! Get directory list(NLST)
 *@param    ftp    pointor to LIBOFTP
 *@param    fglob  file list glob (ex: *.txt) or NULL
 *@param    dup_buf[out]  pointor to buffer (need free if not NULL)
 *@retval   int    error code
 */
int ftp_nlist_dup(LIBOFTP *ftp, const char *fglob, char **dup_buf, long *dup_buf_size) {
    int ret;

    ret = ftp_get_buffer_dup_main(ftp, "NLST", fglob, dup_buf, dup_buf_size);

    return ret;
}




/*! MKD command
 *@param   ftp      pointor to LIBOFTP
 *@param   dirname  target directory name
 *@retval  int      error code
 */
int ftp_mkdir(LIBOFTP *ftp, const char *dirname) {
    char str1[512];
    int res;

    if(ftp->socket < 0) return FTP_ERROR;

    /* send MKD command */
    snprintf( str1, sizeof(str1)-1, "MKD %s\r\n", dirname);
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 257) { /* 257: "PATHNAME" created. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! RMD command
 *@param   ftp      pointor to LIBOFTP
 *@param   dirname  target directory name
 *@retval  int      error code
 */
int ftp_rmdir(LIBOFTP *ftp, const char *dirname) {
    char str1[512];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /*
     * send RMD command
     */
    snprintf(str1, sizeof(str1)-1, "RMD %s\r\n", dirname);
    if (ftp_send_command(ftp, str1) < 0) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if (res != 250 ) { /* 250: Requested file action okay, completed. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}


/*! PWD command
 *@param    ftp     pointor to LIBOFTP
 *@param    [out]buf     buffer for result
 *@param    bufsiz  buffer size
 *@retval   int     error code
 */
int ftp_pwd(LIBOFTP *ftp, char *buf, long bufsiz) {
    char str1[512] = "PWD\r\n";
    int res;
    char *p1, *p2;

    if (ftp->socket < 0 ) return FTP_ERROR;

    /* send PWD command */
    if (ftp_send_command( ftp, str1 ) < 0 ) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    if ((res = ftp_receive_response(ftp, str1, sizeof(str1))) != 257) { /* 257: "PATHNAME" created. */
        DEBUGPRINT1("command response error. %d\n", res);
        strncpy(ftp->error_message, str1, sizeof(ftp->error_message) - 1);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    /* parse response */
    p1 = strchr(str1, '"');
    if (p1 == NULL) {
        DEBUGPRINT1("reply string parse error. %s\n", str1);
        strncpy(ftp->error_message, "reply string parse error.", sizeof(ftp->error_message));
        return FTP_ERROR_PROTOCOL;
    }

    p2 = strchr(p1+1, '"');
    if (p2 == NULL) {
        DEBUGPRINT1("reply string parse error. %s\n", str1);
        strncpy(ftp->error_message, "reply string parse error.", sizeof(ftp->error_message));
        return FTP_ERROR_PROTOCOL;
    }
    if (bufsiz < (p2-p1)) {
        DEBUGPRINT1("buffer too small.%s", "");
        strncpy(ftp->error_message, "buffer too small", sizeof(ftp->error_message));
        return FTP_ERROR_BUFFER;
    }

    memcpy(buf, p1+1, p2 - p1 - 1);
    buf[p2 - p1 - 1] = 0;

    return FTP_NOERROR;
}


/*! CWD command
 *@param  ftp  pointor to LIBOFTP
 *@param  dirname  target directory name
 *@retval   int  error code
 */
int ftp_cd( LIBOFTP *ftp, const char *dirname) {
    char str1[512];
    int res;

    if (ftp->socket < 0) return FTP_ERROR;

    /*
     * send CWD command
     */
    snprintf(str1, sizeof(str1)-1, "CWD %s\r\n", dirname);
    if( ftp_send_command( ftp, str1 ) < 0 ) {
        DEBUGPRINT1("command sending error. %s\n", str1);
        return FTP_ERROR_OS;
    }

    res = ftp_receive_response(ftp, ftp->error_message, sizeof(ftp->error_message) - 1);
    if( res != 250 ) { /* 250: Requested file action okay, completed. */
        DEBUGPRINT1("command response error. %d\n", res);
        return res < 0? res: FTP_ERROR_PROTOCOL;
    }

    return FTP_NOERROR;
}

} // namespace apolloron
