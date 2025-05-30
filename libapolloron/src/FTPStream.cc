/******************************************************************************/
/*! @file FTPStream.cc
    @brief FTPStream class, based on libOftp
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

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

#include "apolloron.h"
#include "ftp/ftplib.h"

namespace apolloron {

/*! Constructor of FTPStream.
    @param void
    @return void
 */
FTPStream::FTPStream() {
    (*this).host = "";
    (*this).port = "";
    (*this).user = "";
    (*this).passwd = "";
    (*this).loggedIn = false;
    ::memset(&((*this).ftp_obj), 0, sizeof(LIBOFTP));
    ftp_initialize(&((*this).ftp_obj));
    (*this).ftp_obj.socket = -1;
    (*this).ftp_obj.timeout_sec = 10; /* default timeout */
    ftp_passive(&((*this).ftp_obj), 1);
    (*this).pasvMode = true;
    (*this).timeout = 60;
    (*this).tmpString = "";
}


/*! Destructor of FTPStream
    @param void
    @return void
 */
FTPStream::~FTPStream() {
    (*this).logout();
}


/*! Set timeout
   @param sec   timeout seconds
   @retval true  success
   @retval false failure
 */
bool FTPStream::setTimeout(int sec) {
    if (sec < 0) return false;
    (*this).timeout = sec;
    return true;
}


/*! Login
   @param uesr    user
   @param passwd  password
   @param host    host
   @param port    port
   @retval true  success
   @retval false failure
 */
bool FTPStream::login(const String &user, const String &passwd,
                      const String &host, const String &port) {
    int ret;

    if ((*this).loggedIn == true) return false;

    (*this).user = user;
    (*this).passwd = passwd;
    (*this).host = host;
    (*this).port = port;

    ftp_timeout(&((*this).ftp_obj), (*this).timeout);
    ftp_passive(&((*this).ftp_obj), ((*this).pasvMode==true)?1:0);
    ret = ftp_open(&((*this).ftp_obj), (*this).host.c_str(), atoi((*this).port.c_str()));
    if (ret != FTP_NOERROR) {
        return false;
    }
    ret = ftp_user(&((*this).ftp_obj), (*this).user.c_str(), (*this).passwd.c_str());
    if (ret != FTP_NOERROR) {
        return false;
    }
    ftp_type(&((*this).ftp_obj), "binary");

    (*this).loggedIn = true;

    return (*this).loggedIn;
}


/*! Logout
   @param void
   @retval true  success
   @retval false failure
 */
bool FTPStream::logout() {
    if ((*this).loggedIn == true) {
        ftp_quit(&((*this).ftp_obj));
    }
    (*this).loggedIn = false;
    return true;
}


/*! Check login status
   @param void
   @retval true  user logged in
   @retval false user logged out
 */
bool FTPStream::isLoggedIn() {
    return (*this).loggedIn;
}


/*! Change to pasive mode or non passive mode
   @param pasv_mode  passive: true, non-passive: false
   @retval true  success
   @retval false failure
 */
bool FTPStream::passive(bool pasv_mode) {
    if ((*this).loggedIn == false) return false;
    ftp_passive(&((*this).ftp_obj), ((*this).pasvMode == true)?1:0);
    (*this).pasvMode = pasv_mode;
    return true;
}


/*! check pasive mode or non passive mode
   @param void
   @retval true  passive mode
   @retval false non-passive mode
 */
bool FTPStream::isPassive() {
    return (*this).pasvMode;
}


/*! Reset
   @param void
   @retval true  success
   @retval false failure
 */
bool FTPStream::reset() {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_reset(&((*this).ftp_obj));
    return (ret == FTP_NOERROR)?true:false;
}


/*! Noop
   @param void
   @retval true  success
   @retval false failure
 */
bool FTPStream::noop() {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_noop(&((*this).ftp_obj));
    return (ret == FTP_NOERROR)?true:false;
}


/*! Site
   @param cmdline  command line
   @retval true  success
   @retval false failure
 */
bool FTPStream::site(const String &cmdline) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_site(&((*this).ftp_obj), cmdline.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Delete
   @param filename  filename to delete
   @retval true  success
   @retval false failure
 */
bool FTPStream::dele(const String &filename) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_delete(&((*this).ftp_obj), filename.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Rename
   @param from  original filename
   @param to    filename to rename
   @retval true  success
   @retval false failure
 */
bool FTPStream::rename(const String &from, const String &to) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_rename(&((*this).ftp_obj), from.c_str(), to.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Receive
   @param filename  filename to get data
   @return received data
 */
String &FTPStream::receiveBuffer(const String &filename) {
    int ret;
    char *dup_buf;
    long dup_buf_size;
    (*this).tmpString = "";
    if ((*this).loggedIn == false) return (*this).tmpString;
    ret = ftp_get_buffer_dup(&((*this).ftp_obj), filename.c_str(), &dup_buf, &dup_buf_size);
    if (dup_buf) {
        (*this).tmpString.setBinary(dup_buf, dup_buf_size);
        free(dup_buf);
    }
    if (ret != FTP_NOERROR) {
        (*this).tmpString = "";
    }
    return (*this).tmpString;
}


/*! Send file
   @param buf       data to send
   @param filename  filename to send
   @retval true  success
   @retval false failure
 */
bool FTPStream::sendBuffer(const String &buf, const String &filename) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_put_buffer(&((*this).ftp_obj), buf, buf.len(), filename.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Append data to file
   @param buf       data to append
   @param filename  filename to append
   @retval true  success
   @retval false failure
 */
bool FTPStream::appendBuffer(const String &buf, const String &filename) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_append_buffer(&((*this).ftp_obj), buf, buf.len(), filename.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Receive as a local file
   @param filename       filename to receive
   @param local_filename local filename to save
   @retval true  success
   @retval false failure
 */
bool FTPStream::receiveFile(const String &filename, const String &local_filename) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_get_file(&((*this).ftp_obj), filename.c_str(), local_filename.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Send local file
   @param local_filename local filename to send
   @param filename       filename to send
   @retval true  success
   @retval false failure
 */
bool FTPStream::sendFile(const String &local_filename, const String &filename) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_put_file(&((*this).ftp_obj), local_filename.c_str(), filename.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Append local file
   @param local_filename local filename to append
   @param filename       filename to append
   @retval true  success
   @retval false failure
 */
bool FTPStream::appendFile(const String &local_filename, const String &filename) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_append_file(&((*this).ftp_obj), local_filename.c_str(), filename.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Get file list
   @param void
   @retval true  success
   @retval false failure
 */
Sheet &FTPStream::list() {
    int ret;
    char *list_data, *saveptr, *line;
    int line_len, row, i;
    long dup_buf_size;

    (*this).tmpSheet.clear();

    if ((*this).loggedIn == false) return (*this).tmpSheet;

    ret = ftp_list_dup(&((*this).ftp_obj), NULL, &list_data, &dup_buf_size);

    if (ret != FTP_NOERROR || !list_data ||
            (strncmp(list_data, "total ", 6)) || !strchr(list_data, '\n')) {
        if (list_data) free(list_data);
        if (ret != FTP_NOERROR) {
            if ((*this).ftp_obj.reply_code == 450) {
                /* no files found on this directory */
                (*this).tmpSheet.addCol("FILENAME");
                (*this).tmpSheet.addCol("SIZE");
                (*this).tmpSheet.addCol("DATE");
                (*this).tmpSheet.addCol("OWNER_UID");
                (*this).tmpSheet.addCol("OWNER_GID");
                (*this).tmpSheet.addCol("PERMISSION");
                (*this).tmpSheet.addCol("IS_DIRECTORY");
                return (*this).tmpSheet;
            }
        }
        return (*this).tmpSheet;
    }

    (*this).tmpSheet.addCol("FILENAME");
    (*this).tmpSheet.addCol("SIZE");
    (*this).tmpSheet.addCol("DATE");
    (*this).tmpSheet.addCol("OWNER_UID");
    (*this).tmpSheet.addCol("OWNER_GID");
    (*this).tmpSheet.addCol("PERMISSION");
    (*this).tmpSheet.addCol("IS_DIRECTORY");

    /* skip "total xx" line */
    if (!strncmp(list_data, "total ", 6)) {
        line = strchr(list_data, '\n') + 1;
    } else {
        line = list_data;
    }

    /* parse list_data
       ex.
         "-rw-------  1 501  1000   29486 Nov 30 20:15 350px-PopupBlocked.png\n"
         "drwxr-xr-x  2 48   48      4096 Oct 18 16:12 session\n"
         "-rw-------  1 501  1000  19959808 Jan  5  1999 abc.mpg\n"
    */
    row = 0;
    saveptr = NULL;
    line = strtok_r(line, "\n", &saveptr);
    while (line != NULL) {
        line_len = strlen(line);
        if (0 < line_len && line[line_len-1] == '\r') {
            line[line_len-1] = '\0';
            line_len--;
        }
        if (45 < line_len) {
            char permission[20];
            int type;     /* 1:file  2:directory */
            char uid[20];
            char gid[20];
            char *size;
            char date[50];
            String str_dt;
            DateTime dt;
            char *name;
            char *p;

            strncpy(permission, line, 10);
            permission[10] = '\0';
            p = line+10;
            while (*p == ' ') p++;
            type = atoi(p);
            while (*p != '\0' && !isspace(*p)) p++;
            while (*p == ' ') p++;
            i = 0;
            while (*p != '\0' && !isspace(*p)) {
                uid[i++] = *p;
                p++;
            }
            uid[i] = '\0';
            while (*p == ' ') p++;
            i = 0;
            while (*p != '\0' && !isspace(*p)) {
                gid[i++] = *p;
                p++;
            }
            gid[i] = '\0';
            while (*p == ' ') p++;
            size = p;
            while (*p != '\0' && !isspace(*p)) p++;
            while (*p == ' ') p++;
            strncpy(date, p, 12);
            date[12] = '\0';
            p += 12;
            i = 12;
            while (*p != '\0' && !isspace(*p)) {
                date[i++] = *p;
                p++;
            }
            date[i] = '\0';
            if (*p == ' ') p++;
            name = p;

            (*this).tmpSheet.addRow();
            (*this).tmpSheet.setValue("FILENAME", row, name);
            (*this).tmpSheet.setValue("SIZE", row, size);
            str_dt = date;
            dt.set(str_dt);
            (*this).tmpSheet.setValue("DATE", row, dt.toString(DATEFORMAT_ISO8601));
            dt.clear();
            (*this).tmpSheet.setValue("OWNER_UID", row, uid);
            (*this).tmpSheet.setValue("OWNER_GID", row, gid);
            (*this).tmpSheet.setValue("PERMISSION", row, permission);
            (*this).tmpSheet.setValue("IS_DIRECTORY", row, (type==2)?"1":"0");
            row++;
        }
        line = strtok_r(NULL, "\n", &saveptr);
    }

    if (list_data) free(list_data);

    return (*this).tmpSheet;
}


/*! Get file list by nlst
   @param void
   @retval true  success
   @retval false failure
 */
Sheet &FTPStream::nlist() {
    int ret;
    char *list_data, *saveptr, *line;
    int line_len, row, i;
    long dup_buf_size;

    (*this).tmpSheet.clear();

    if ((*this).loggedIn == false) return (*this).tmpSheet;

    ret = ftp_nlist_dup(&((*this).ftp_obj), NULL, &list_data, &dup_buf_size);

    if (ret != FTP_NOERROR || !list_data ||
            (strncmp(list_data, "total ", 6)) || !strchr(list_data, '\n')) {
        if (list_data) free(list_data);
        if (ret != FTP_NOERROR) {
            if ((*this).ftp_obj.reply_code == 450) {
                /* no files found on this directory */
                (*this).tmpSheet.addCol("FILENAME");
                return (*this).tmpSheet;
            }
        }
        return (*this).tmpSheet;
    }

    (*this).tmpSheet.addCol("FILENAME");

    /* skip "total xx" line */
    if (!strncmp(list_data, "total ", 6)) {
        line = strchr(list_data, '\n') + 1;
    } else {
        line = list_data;
    }

    /* parse list_data
       ex.
         "-rw-------  1 501  1000   29486 Nov 30 20:15 350px-PopupBlocked.png\n"
         "drwxr-xr-x  2 48   48      4096 Oct 18 16:12 session\n"
         "-rw-------  1 501  1000  19959808 Jan  5  1999 abc.mpg\n"
    */
    row = 0;
    saveptr = NULL;
    line = strtok_r(line, "\n", &saveptr);
    while (line != NULL) {
        line_len = strlen(line);
        if (0 < line_len && line[line_len-1] == '\r') {
            line[line_len-1] = '\0';
            line_len--;
        }
        if (45 < line_len) {
            char permission[20];
            int type;     /* 1:file  2:directory */
            char uid[20];
            char gid[20];
            char *size;
            char date[50];
            String str_dt;
            DateTime dt;
            char *name;
            char *p;

            strncpy(permission, line, 10);
            permission[10] = '\0';
            p = line+10;
            while (*p == ' ') p++;
            type = atoi(p);
            while (*p != '\0' && !isspace(*p)) p++;
            while (*p == ' ') p++;
            i = 0;
            while (*p != '\0' && !isspace(*p)) {
                uid[i++] = *p;
                p++;
            }
            uid[i] = '\0';
            while (*p == ' ') p++;
            i = 0;
            while (*p != '\0' && !isspace(*p)) {
                gid[i++] = *p;
                p++;
            }
            gid[i] = '\0';
            while (*p == ' ') p++;
            size = p;
            while (*p != '\0' && !isspace(*p)) p++;
            while (*p == ' ') p++;
            strncpy(date, p, 12);
            date[12] = '\0';
            p += 12;
            i = 12;
            while (*p != '\0' && !isspace(*p)) {
                date[i++] = *p;
                p++;
            }
            date[i] = '\0';
            if (*p == ' ') p++;
            name = p;

            (*this).tmpSheet.addRow();
            (*this).tmpSheet.setValue("FILENAME", row, name);
            (*this).tmpSheet.setValue("SIZE", row, size);
            str_dt = date;
            dt.set(str_dt);
            (*this).tmpSheet.setValue("DATE", row, dt.toString(DATEFORMAT_ISO8601));
            dt.clear();
            (*this).tmpSheet.setValue("OWNER_UID", row, uid);
            (*this).tmpSheet.setValue("OWNER_GID", row, gid);
            (*this).tmpSheet.setValue("PERMISSION", row, permission);
            (*this).tmpSheet.setValue("IS_DIRECTORY", row, (type==2)?"1":"0");
            row++;
        }
        line = strtok_r(NULL, "\n", &saveptr);
    }

    if (list_data) free(list_data);

    return (*this).tmpSheet;
}


/*! Create a directory
   @param dirname  directory name to create
   @retval true  success
   @retval false failure
 */
bool FTPStream::mkdir(const String &dirname) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_mkdir(&((*this).ftp_obj), dirname.c_str());
    return (ret == FTP_NOERROR || (*this).ftp_obj.reply_code == 550)?true:false;
}


/*! Create a directory (recursive)
   @param dirname  directory name to create
   @retval true  success
   @retval false failure
 */
bool FTPStream::mkdirp(const String &dirname) {
    char *pathlist[1000], *p;
    int i, j, len, err_status, ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_mkdir(&((*this).ftp_obj), dirname.c_str());
    if (ret != FTP_NOERROR && (*this).ftp_obj.reply_code != 550) {
        ret = FTP_NOERROR;
    }
    if (ret != FTP_NOERROR) {
        err_status = 0;
        len = strlen(dirname);
        pathlist[0] = new char[len + 1];
        strcpy(pathlist[0], dirname);
        for (i = 1; i < 999; i++) {
            p = strrchr(pathlist[i-1], '/');
            if (p == NULL) {
                i--;
                while (0 <= i) {
                    if (pathlist[i][0] && pathlist[i][1]) {
                        if (err_status == 0) {
                            ret = ftp_mkdir(&((*this).ftp_obj), pathlist[i]);
                            if (ret != FTP_NOERROR && (*this).ftp_obj.reply_code != 550) {
                                err_status = ret;
                            }
                        }
                    }
                    delete [] pathlist[i];
                    i--;
                }
                break;
            }
            j = (int)(p - pathlist[i-1]);
            pathlist[i] = new char[len + 1];
            strcpy(pathlist[i], dirname);
            pathlist[i][j] = '\0';
        }
        if (999 <= i) {
            for (j = 0; j < i; j++) {
                delete [] pathlist[j];
            }
            return false;
        }
    }
    return (ret == FTP_NOERROR || (*this).ftp_obj.reply_code == 550)?true:false;
}


/*! Remove a directory
   @param dirname  directory name to remove
   @retval true  success
   @retval false failure
 */
bool FTPStream::rmdir(const String &dirname) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_rmdir(&((*this).ftp_obj), dirname.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


/*! Get current directory path
   @param void
   @retval true  success
   @retval false failure
 */
String &FTPStream::pwd() {
    int ret;
    char buf[4096];
    (*this).tmpString = "";
    if ((*this).loggedIn == false) return (*this).tmpString;
    ret = ftp_pwd(&((*this).ftp_obj), buf, 4095);
    buf[4095] = '\0';
    if (ret != FTP_NOERROR) {
        (*this).tmpString = "";
    } else {
        (*this).tmpString = buf;
    }
    return (*this).tmpString;
}


/*! Change current directory
   @param dirname  directory name
   @retval true  success
   @retval false failure
 */
bool FTPStream::cd(const String &dirname) {
    int ret;
    if ((*this).loggedIn == false) return false;
    ret = ftp_cd(&((*this).ftp_obj), dirname.c_str());
    return (ret == FTP_NOERROR)?true:false;
}


} // namespace apolloron
