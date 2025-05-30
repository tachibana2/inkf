/*
  liboftp: this is an FTP library to simplify the work to a Developer
  who want to work with FTP servers (RFC 959).

  Copyright (c) 2009 hirohito higashi. All rights reserved.
  This file is distributed under BSD license.
*/


#ifndef __FTPLIB_H__
#define __FTPLIB_H__

/* LIBOFTP ERROR CODE */
#define FTP_NOERROR         0
#define FTP_ERROR_OS       -1
#define FTP_ERROR_PROTOCOL -2
#define FTP_ERROR          -3
#define FTP_ERROR_BUFFER   -4
#define FTP_ERROR_TIMEOUT  -5


#include <sys/types.h>
#include <netinet/in.h>

namespace apolloron {

#ifndef _APOLLORON_H_
typedef enum {UNIX = 1, Windows_NT = 2} TFTPSystemType;
typedef enum {ASCII = 1, IMAGE = 3} TFTPDataType;

typedef struct {
    int socket;
    struct sockaddr_in saddr;
    TFTPSystemType system_type;
    TFTPDataType data_type;
    int flag_passive;
    int timeout_sec;

    int reply_code;
    char error_message[256];
} LIBOFTP;
#endif

int ftp_initialize(LIBOFTP *ftp);
int ftp_open(LIBOFTP *ftp, const char *host, int port);
int ftp_user(LIBOFTP *ftp, const char *user, const char *pass);
int ftp_passive(LIBOFTP *ftp, int flag);
int ftp_timeout(LIBOFTP *ftp, int sec);
int ftp_type(LIBOFTP *ftp, const char *type);
int ftp_quit(LIBOFTP *ftp);
int ftp_reset(LIBOFTP *ftp);
int ftp_noop(LIBOFTP *ftp);
int ftp_site(LIBOFTP *ftp, const char *cmdline);
int ftp_delete(LIBOFTP *ftp, const char *fname);
int ftp_rename(LIBOFTP *ftp, const char *from, const char *to);

int ftp_get_buffer(LIBOFTP *ftp, const char *fname, char *buf, long bufsiz);
int ftp_get_buffer_dup(LIBOFTP *ftp, const char *fname, char **dup_buf, long *dup_buf_size);
int ftp_put_buffer(LIBOFTP *ftp, const char *buf, long bufsiz, const char *fname);
int ftp_append_buffer(LIBOFTP *ftp, const char *buf, long bufsiz, const char *fname);
int ftp_get_file(LIBOFTP *ftp, const char *fname, const char *local_fname);
int ftp_put_file(LIBOFTP *ftp, const char *local_fname, const char *fname);
int ftp_append_file(LIBOFTP *ftp, const char *local_fname, const char *fname);
int ftp_get_descriptor(LIBOFTP *ftp, const char *fname);
int ftp_get_descriptor_close(LIBOFTP *ftp, int desc);
int ftp_put_descriptor(LIBOFTP *ftp, const char *fname);
int ftp_put_descriptor_close(LIBOFTP *ftp, int desc);
int ftp_append_descriptor(LIBOFTP *ftp, const char *fname);
int ftp_append_descriptor_close(LIBOFTP *ftp, int desc);

int ftp_list(LIBOFTP *ftp, const char *fglob, char *buf, long bufsiz);
int ftp_list_dup(LIBOFTP *ftp, const char *fglob, char **dup_buf, long *dup_buf_size);
int ftp_nlist(LIBOFTP *ftp, const char *fglob, char *buf, long bufsiz);
int ftp_nlist_dup(LIBOFTP *ftp, const char *fglob, char **dup_buf, long *dup_buf_size);
int ftp_mkdir(LIBOFTP *ftp, const char *dirname);
int ftp_rmdir(LIBOFTP *ftp, const char *dirname);
int ftp_pwd(LIBOFTP *ftp, char *buf, long bufsiz);
int ftp_cd(LIBOFTP *ftp, const char *dirname);

} // namespace apolloron

#endif /* __FTPLIB_H__ */
