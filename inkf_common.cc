/******************************************************************************/
/*! @file inkf_common.cc
    @brief yet another nkf with international language support
    @author Masashi Astro Tachibana
    @date 2002-2025
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "apolloron.h"
#include "inkf_common.h"

using namespace apolloron;


const char *INKF_PROGNAME = "inkf";
const char *INKF_BRIEF    = "Yet Another Network Kanji Filter";
const char *INKF_VERSION  = "2.1.8";
const char *INKF_COPY     = "Copyright (C) 2002-2025 Masashi Astro Tachibana";
const char *INKF_DEF_OUT  = "inkf.out";

const char *RETURN_STR[] = {
    "\n",   // default retuen code
    "\n",   // LF
    "\r\n", // CRLF
    "\r"    // CR
};

static void guess(const String &str, const char *input_charset, char *buf);
static void convert(String &str, const TOption *option);
static void set_input_charset_by_env(char *input_charset);
static void set_output_charset(char *output_charset, const char *input_charset, String &str);
static void get_help(char *buf);
static void get_version(char *buf);
static void get_copy(char *buf);


extern "C" int inkf_command_exec(const TOption *option) {
    int retval = 0;
    FILE *fpin = stdin, *fpout = stdout;
    String tmp_str;
    char buf[4096 + 1], *p;
    int i, j, l;

    // check option
    if (option == (TOption *)NULL) {
        fprintf(stderr, "Invalid parameter(s).\n");
        retval = -1;
        goto exec_command_exit;
    }

    // show help
    if (option->flag_show_help) {
        get_help(buf);
        fwrite(buf, 1, strlen(buf), stderr);
        get_version(buf);
        fwrite(buf, 1, strlen(buf), stderr);
        get_copy(buf);
        fwrite(buf, 1, strlen(buf), stderr);
        goto exec_command_exit;
    }

    // show version
    if (option->flag_show_version) {
        get_version(buf);
        fwrite(buf, 1, strlen(buf), stderr);
        get_copy(buf);
        fwrite(buf, 1, strlen(buf), stderr);
        goto exec_command_exit;
    }

    // open output stream
    if (option->output_filename != NULL && option->output_filename[0] != '\0') {
        fpout = fopen(option->output_filename, "wb");
        if (fpout == (FILE *)NULL) {
            fprintf(stderr, "Cannot open the output file '%s'.\n", option->output_filename);
        }
    }

    // conversion
    if (option->input_filenames == (char **)NULL) {

        // read from stdin
        if (option->mime_decode == MIME_NONE && option->mime_encode == MIME_NONE &&
                !option->flag_guess && !option->flag_html_to_plain && !option->flag_midi &&
                !option->flag_md5 && !option->flag_sha1 &&
                !option->flag_sort_csv && !option->flag_sort_csv_r &&
                !option->flag_format_json && !option->flag_minify_json &&
                !option->flag_re_match &&
                strncasecmp(option->input_charset, "AUTODETECT", 10) != 0 &&
                strncasecmp(option->input_charset, "UTF-16", 6) != 0 &&
                strncasecmp(option->input_charset, "UTF16", 5) != 0 &&
                strncasecmp(option->input_charset, "UTF-32", 6) != 0 &&
                strncasecmp(option->input_charset, "UTF32", 5) != 0 &&
                strcasecmp(option->output_charset, "UTF-8_BOM") != 0 &&
                !(!strncasecmp(option->output_charset, "UTF-16", 6) ||
                !strncasecmp(option->output_charset, "UTF16", 5) ||
                !strncasecmp(option->output_charset, "UTF-32", 6) ||
                !strncasecmp(option->output_charset, "UTF32", 5))) {
            while (!feof(fpin)) {
                tmp_str = "";
                while (!feof(fpin)) {
                    l = fread(buf, 1, 4096, fpin);
                    buf[l] = '\0';
                    p = strrchr(buf, '\n');
                    if (p != NULL) {
                        j = p - buf + 1;
                        tmp_str.add(buf, j);
                        convert(tmp_str, option);
                        if (tmp_str.isBinary()) {
                            fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                        } else {
                            fwrite(tmp_str.c_str(), 1, tmp_str.len(), fpout);
                        }
                        tmp_str = buf + j;
                    } else {
                        tmp_str.add(buf);
                    }
                }
                if (0 < tmp_str.len()) {
                    convert(tmp_str, option);
                    if (tmp_str.isBinary()) {
                        fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                    } else {
                        fwrite(tmp_str.c_str(), 1, tmp_str.len(), fpout);
                    }
                }
            }
        } else {
            tmp_str = "";
            while (!feof(fpin)) {
                l = fread(buf, 1, 4096, fpin);
                if (l <= 0) {
                    continue;
                }
                buf[l] = '\0';
                tmp_str.addBinary(buf, l);
            }
            if (option->flag_guess) {
                guess(tmp_str, option->input_charset, buf);
                fwrite(buf, 1, strlen(buf), fpout);
            } else if (option->flag_midi) {
                tmp_str = tmp_str.toMIDI();
                fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
            } else if (0 < tmp_str.len() || 0 < tmp_str.binaryLength()) {
                convert(tmp_str, option);
                if (tmp_str.isBinary()) {
                    fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                } else {
                    fwrite(tmp_str.c_str(), 1, tmp_str.len(), fpout);
                }
            }
        }
    } else {
        bool charset_autodetect_pre;
        charset_autodetect_pre = false;

        if (!strcasecmp(option->input_charset, "AUTODETECT")) {
            charset_autodetect_pre = true;
        }

        tmp_str.useAsBinary(0);

        // read from file(s) or specified URL
        for (i = 0; option->input_filenames[i] != NULL; i++) {
            char input_charset[32];
            strncpy(input_charset, option->input_charset, 31);
            input_charset[31] = '\0';
            if (charset_autodetect_pre) {
                strcpy(input_charset, "AUTODETECT");
            }
#if __OPENSSL == 1
            if (!strncasecmp(option->input_filenames[i], "http://", 7) ||
                    !strncasecmp(option->input_filenames[i], "https://", 8)) {
#else
            if (!strncasecmp(option->input_filenames[i], "http://", 7)) {
#endif
                HTTPClient hc;
                tmp_str = hc.getURL(option->input_filenames[i]);
                strncpy(input_charset, hc.getOrigCharset().c_str(), 31);
                input_charset[31] = '\0';
            } else if (!strncasecmp(option->input_filenames[i], "ftp://", 6)) {
                FTPStream fs;
                String host;
                String port = 21;
                String path;
                long x;
                host = option->input_filenames[i]+6;
                x = host.search(":");
                if (0 <= x) {
                    host = host.mid(0, x);
                }
                x = host.search("/");
                if (0 <= x) {
                    path = host.mid(x);
                    host = host.mid(0, x);
                }
                if (fs.login("anonymous", "user@example.com", host, port)) {
                    fs.passive();
                    tmp_str = fs.receiveBuffer(path);
                    fs.logout();
                }
            } else {
                tmp_str.loadFile(option->input_filenames[i]);
            }
            if ((option->output_filename == NULL || option->output_filename[0] == '\0') &&
                    option->flag_overwrite) {
                fpout = fopen(option->input_filenames[i], "wb");
                if (fpout == (FILE *)NULL) {
                    fprintf(stderr, "Cannot open the output file '%s'.\n",
                            option->input_filenames[i]);
                    continue;
                }
            }
            if (option->flag_guess) {
                guess(tmp_str, input_charset, buf);
                fwrite(buf, 1, strlen(buf), fpout);
            } else if (option->flag_midi) {
                tmp_str = tmp_str.toMIDI();
                fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                if (!((option->output_filename == NULL || option->output_filename[0] == '\0') &&
                    option->flag_overwrite)) {
                    break;
                }
            } else if (0 < tmp_str.len()) {
                if (charset_autodetect_pre) {
                    TOption tmp_option;
                    memcpy(&tmp_option, option, sizeof(TOption));
                    strcpy(tmp_option.input_charset, input_charset);
                    convert(tmp_str, &tmp_option);
                } else {
                    convert(tmp_str, option);
                }
                if (tmp_str.isBinary()) {
                    fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                } else {
                    fwrite(tmp_str.c_str(), 1, tmp_str.len(), fpout);
                }
            }
            if ((option->output_filename == NULL || option->output_filename[0] == '\0') &&
                    option->flag_overwrite) {
                fclose(fpout);
                fpout = (FILE *)NULL;
            }
        }
    }

    // close output stream
    if (option->output_filename != NULL && option->output_filename[0] != '\0' &&
            fpout != (FILE *)NULL) {
        fclose(fpout);
    }

exec_command_exit:
    tmp_str.clear();

    return retval;
}


extern "C" int inkf_lib_exec(const char *input_str, long input_str_len,
                             char **output_str, long *output_str_len,
                             const TOption *option) {
    int retval = 0;
    FILE *fpout = stdout;
    String tmp_str, ret_str;
    char buf[4096 + 1];
    long length;
    int i;

    if (input_str_len == 0) {
        input_str = "";
    }

    // check option
    if (option == (TOption *)NULL) {
        /* Invalid parameter(s). */
        retval = -1;
        goto exec_lib_exit;
    }

    // show help
    if (option->flag_show_help) {
        get_help(buf);
        ret_str += buf;
        get_version(buf);
        ret_str += buf;
        get_copy(buf);
        ret_str += buf;
        goto exec_lib_exit;
    }

    // show version
    if (option->flag_show_version) {
        get_version(buf);
        ret_str += buf;
        get_copy(buf);
        ret_str += buf;
        goto exec_lib_exit;
    }

    // open output stream
    if (option->output_filename != NULL && option->output_filename[0] != '\0') {
        fpout = fopen(option->output_filename, "wb");
        if (fpout == (FILE *)NULL) {
            /* Cannot open the output file (option->output_filename) */
            retval = -1;
            goto exec_lib_exit;
        }
    }

    // conversion
    if (option->input_filenames == (char **)NULL) {

        // read from input_str
        tmp_str.setBinary(input_str, input_str_len);
        if (option->flag_guess) {
            guess(tmp_str, option->input_charset, buf);
            if (fpout != stdout) {
                fwrite(buf, 1, strlen(buf), fpout);
            } else {
                ret_str = buf;
            }
        } else if (option->flag_midi) {
            tmp_str = tmp_str.toMIDI();
            if (fpout != stdout) {
                fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
            } else {
                ret_str = tmp_str;
            }
        } else if (0 < tmp_str.len() || 0 < tmp_str.binaryLength()) {
            convert(tmp_str, option);
            if (fpout != stdout) {
                if (tmp_str.isBinary()) {
                    fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                } else {
                    fwrite(tmp_str.c_str(), 1, tmp_str.len(), fpout);
                }
            } else {
                ret_str = tmp_str;
            }
        }
    } else {
        bool charset_autodetect_pre;
        charset_autodetect_pre = false;

        if (!strcasecmp(option->input_charset, "AUTODETECT")) {
            charset_autodetect_pre = true;
        }

        tmp_str.useAsBinary(0);

        // read from file(s) or specified URL
        for (i = 0; option->input_filenames[i] != NULL; i++) {
            char input_charset[32];
            strncpy(input_charset, option->input_charset, 31);
            input_charset[31] = '\0';
            if (charset_autodetect_pre) {
                strcpy(input_charset, "AUTODETECT");
            }
#if __OPENSSL == 1
            if (!strncasecmp(option->input_filenames[i], "http://", 7) ||
                    !strncasecmp(option->input_filenames[i], "https://", 8)) {
#else
            if (!strncasecmp(option->input_filenames[i], "http://", 7)) {
#endif
                HTTPClient hc;
                tmp_str = hc.getURL(option->input_filenames[i]);
                strncpy(input_charset, hc.getOrigCharset().c_str(), 31);
                input_charset[31] = '\0';
            } else if (!strncasecmp(option->input_filenames[i], "ftp://", 6)) {
                FTPStream fs;
                String host;
                String port = 21;
                String path;
                long x;
                host = option->input_filenames[i]+6;
                x = host.search(":");
                if (0 <= x) {
                    host = host.mid(0, x);
                }
                x = host.search("/");
                if (0 <= x) {
                    path = host.mid(x);
                    host = host.mid(0, x);
                }
                if (fs.login("anonymous", "user@example.com", host, port)) {
                    fs.passive();
                    tmp_str = fs.receiveBuffer(path);
                    fs.logout();
                }
            } else {
                tmp_str.loadFile(option->input_filenames[i]);
            }
            if ((option->output_filename == NULL || option->output_filename[0] == '\0') &&
                    option->flag_overwrite) {
                fpout = fopen(option->input_filenames[i], "wb");
                if (fpout == (FILE *)NULL) {
                    fprintf(stderr, "Cannot open the output file '%s'.\n",
                            option->input_filenames[i]);
                    retval = -1;
                    goto exec_lib_exit;
                }
            }
            if (option->flag_guess) {
                guess(tmp_str, input_charset, buf);
                if (fpout != stdout) {
                    fwrite(buf, 1, strlen(buf), fpout);
                } else {
                    ret_str += buf;
                }
            } else if (option->flag_midi) {
                tmp_str = tmp_str.toMIDI();
                if (fpout != stdout) {
                    fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                } else {
                    ret_str = tmp_str;
                }
                if (!((option->output_filename == NULL || option->output_filename[0] == '\0') &&
                    option->flag_overwrite)) {
                    break;
                }
            } else if (0 < tmp_str.len()) {
                if (charset_autodetect_pre) {
                    TOption tmp_option;
                    memcpy(&tmp_option, option, sizeof(TOption));
                    strcpy(tmp_option.input_charset, input_charset);
                    convert(tmp_str, &tmp_option);
                } else {
                    convert(tmp_str, option);
                }
                if (fpout != stdout) {
                    if (tmp_str.isBinary()) {
                        fwrite(tmp_str.c_str(), 1, tmp_str.binaryLength(), fpout);
                    } else {
                        fwrite(tmp_str.c_str(), 1, tmp_str.len(), fpout);
                    }
                } else {
                    ret_str = tmp_str;
                }
            }
            if ((option->output_filename == NULL || option->output_filename[0] == '\0') &&
                    option->flag_overwrite) {
                fclose(fpout);
                fpout = (FILE *)NULL;
            }
        }
    }

    // close output stream
    if (option->output_filename != NULL && option->output_filename[0] != '\0' &&
            fpout != (FILE *)NULL) {
        fclose(fpout);
    }

exec_lib_exit:
    tmp_str.clear();
    if (ret_str.isBinary()) {
        length = ret_str.binaryLength();
        if (output_str_len) *output_str_len = length;
        if (output_str) {
            *output_str = new char [length+1];
            memcpy(*output_str, ret_str.c_str(), length);
            (*output_str)[length] = '\0';
        }
    } else {
        length = ret_str.len();
        if (output_str_len) *output_str_len = length;
        if (output_str) {
            *output_str = new char [length+1];
            strncpy(*output_str, ret_str.c_str(), length);
            (*output_str)[length] = '\0';
        }
    }
    ret_str.clear();

    return retval;
}


extern "C" void inkf_free_output_str(char *output_str) {
    if (output_str != (char *)NULL) {
        delete [] output_str;
        output_str = (char *)NULL;
    }
}


extern "C" int inkf_set_option(TOption *option, int argc, const char **argv) {
    bool o_param_found = false;
    int input_filenames_max = 0;
    int i, j;

    // set default values
    option->flag_show_help = 0;
    option->flag_show_version = 0;
    option->flag_guess = 0;
    option->flag_overwrite = 0;
    option->flag_html_to_plain = 0;
    option->flag_hankaku_ascii = 0;
    option->flag_zenkaku_ascii = 0;
    option->flag_hankaku_katakana = 0;
    option->flag_zenkaku_katakana = 0;
    option->flag_hiragana = 0;
    option->flag_katakana = 0;
    option->flag_midi = 0;
    option->flag_md5 = 0;
    option->flag_sha1 = 0;
    option->flag_sort_csv = 0;
    option->flag_sort_csv_r = 0;
    option->csv_column = NULL;
    option->flag_format_json = 0;
    option->flag_minify_json = 0;
    option->flag_re_match = 0;
    option->re_match_pattern = NULL;
    option->flag_no_return_param = 0;
    option->flag_no_return = 0;
    strcpy(option->input_charset, "AUTODETECT");
    option->output_charset[0] = '\0';
    option->mime_decode = MIME_NONE;
    option->mime_encode = MIME_NONE;
    option->line_mode = LINE_MODE_NOCONVERSION;
    option->input_filenames = (char **)NULL; // default is stdin
    option->output_filename = NULL; // default is stdout

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (!strcasecmp(argv[i], "--help")) {
                option->flag_show_help = 1;
                break;
            }
            if (!strcasecmp(argv[i], "--version")) {
                option->flag_show_version = 1;
                break;
            }
            if (!strcasecmp(argv[i], "--guess")) {
                option->flag_guess = 1;
            } else if (!strncasecmp(argv[i], "--ic=", 5)) {
                strncpy(option->input_charset, argv[i]+5, 31);
                option->input_charset[31] = '\0';
            } else if (!strncasecmp(argv[i], "--oc=", 5)) {
                strncpy(option->output_charset, argv[i]+5, 31);
                option->output_charset[31] = '\0';
            } else if (!strcasecmp(argv[i], "--overwrite")) {
                option->flag_overwrite = 1;
            } else if (!strcasecmp(argv[i], "--html-to-plain")) {
                option->flag_html_to_plain = 1;
            } else if (!strcasecmp(argv[i], "--hankaku-ascii")) {
                option->flag_hankaku_ascii = 1;
            } else if (!strcasecmp(argv[i], "--zenkaku-ascii")) {
                option->flag_zenkaku_ascii = 1;
            } else if (!strcasecmp(argv[i], "--hankaku-katakana")) {
                option->flag_hankaku_katakana = 1;
            } else if (!strcasecmp(argv[i], "--zenkaku-katakana")) {
                option->flag_zenkaku_katakana = 1;
            } else if (!strcasecmp(argv[i], "--hiragana")) {
                option->flag_hiragana = 1;
            } else if (!strcasecmp(argv[i], "--katakana")) {
                option->flag_katakana = 1;
            } else if (!strcasecmp(argv[i], "--midi")) {
                option->flag_midi = 1;
            } else if (!strcasecmp(argv[i], "--md5")) {
                option->flag_md5 = 1;
            } else if (!strcasecmp(argv[i], "--sha1")) {
                option->flag_sha1 = 1;
            } else if (!strncasecmp(argv[i], "--sort-csv=", 11)) {
                option->flag_sort_csv = 1;
                option->csv_column = &(argv[i][11]);
                if (option->csv_column[0] == '\0') {
                    return -1; // invalid parameter
                }
            } else if (!strncasecmp(argv[i], "--sort-csv-r=", 13)) {
                option->flag_sort_csv_r = 1;
                option->csv_column = &(argv[i][13]);
                if (option->csv_column[0] == '\0') {
                    return -2; // invalid parameter
                }
            } else if (!strcasecmp(argv[i], "--format-json")) {
                option->flag_format_json = 1;
            } else if (!strcasecmp(argv[i], "--minify-json")) {
                option->flag_minify_json = 1;
            } else if (!strncasecmp(argv[i], "--re-match=", 11)) {
                option->flag_re_match = 1;
                option->re_match_pattern = &(argv[i][11]);
                if (option->re_match_pattern[0] == '\0') {
                    return -3; // invalid parameter
                }
            } else {
                return -4; // invalid parameter
            }
        } else if (argv[i][0] == '-') {
            for (j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'v':
                        option->flag_show_version = 1;
                        return 0;
                        break;
                    case 'V':
                        option->flag_show_help = 1;
                        return 0;
                        break;
                    case 'g':
                        option->flag_guess = 1;
                        break;
                    case 'f':
                        if (argv[i][j+1] == '\0' && i + 1 < argc) {
                            strncpy(option->input_charset, argv[i+1], 31);
                            option->input_charset[31] = '\0';
                            i++;
                            j = strlen(argv[i]) - 1;
                        } else {
                            return -5; // invalid parameter
                        }
                        break;
                    case 't':
                        if (argv[i][j+1] == '\0' && i + 1 < argc) {
                            strncpy(option->output_charset, argv[i+1], 31);
                            option->output_charset[31] = '\0';
                            i++;
                            j = strlen(argv[i]) - 1;
                        } else {
                            return -6; // invalid parameter
                        }
                        break;
                    case 'j':
                        strcpy(option->output_charset, "ISO-2022-JP");
                        break;
                    case 's':
                        strcpy(option->output_charset, "SHIFT_JIS");
                        break;
                    case 'e':
                        strcpy(option->output_charset, "EUC-JP");
                        break;
                    case 'l':
                        strcpy(option->output_charset, "LATIN1");
                        break;
                    case 'w':
                        if (argv[i][j + 1] == '1' && argv[i][j + 2] == '6') {
                            if (argv[i][j + 3] == 'L' && argv[i][j + 4] == '0') {
                                strcpy(option->output_charset, "UTF-16LE");
                                j += 4;
                            } else if (argv[i][j + 3] == 'L') {
                                strcpy(option->output_charset, "UTF-16LE_BOM");
                                j += 3;
                            } else if (argv[i][j + 3] == 'B' && argv[i][j + 4] == '0') {
                                strcpy(option->output_charset, "UTF-16BE");
                                j += 4;
                            } else if (argv[i][j + 3] == 'B') {
                                strcpy(option->output_charset, "UTF-16BE_BOM");
                                j += 3;
                            } else {
                                strcpy(option->output_charset, "UTF-16BE");
                                j += 2;
                            }
                        } else if (argv[i][j + 1] == '3' && argv[i][j + 2] == '2') {
                            if (argv[i][j + 3] == 'L' && argv[i][j + 4] == '0') {
                                strcpy(option->output_charset, "UTF-32LE");
                                j += 4;
                            } else if (argv[i][j + 3] == 'L') {
                                strcpy(option->output_charset, "UTF-32LE_BOM");
                                j += 3;
                            } else if (argv[i][j + 3] == 'B' && argv[i][j + 4] == '0') {
                                strcpy(option->output_charset, "UTF-32BE");
                                j += 4;
                            } else if (argv[i][j + 3] == 'B') {
                                strcpy(option->output_charset, "UTF-32BE_BOM");
                                j += 3;
                            } else {
                                strcpy(option->output_charset, "UTF-32BE");
                                j += 2;
                            }
                        } else if (argv[i][j + 1] == '8' && argv[i][j + 2] == '0') {
                            strcpy(option->output_charset, "UTF-8");
                            j += 2;
                        } else if (argv[i][j + 1] == '8') {
                            strcpy(option->output_charset, "UTF-8_BOM");
                            j++;
                        } else {
                            strcpy(option->output_charset, "UTF-8");
                        }
                        break;
                    case 'k':
                        strcpy(option->output_charset, "EUC-KR");
                        break;
                    case 'c':
                        strcpy(option->output_charset, "GB2312");
                        break;
                    case 'b':
                        strcpy(option->output_charset, "BIG5");
                        break;
                    case 'J':
                        strcpy(option->input_charset, "ISO-2022-JP");
                        break;
                    case 'S':
                        strcpy(option->input_charset, "SHIFT_JIS");
                        break;
                    case 'E':
                        strcpy(option->input_charset, "EUC-JP");
                        break;
                    case 'W':
                        if (argv[i][j + 1] == '1' && argv[i][j + 2] == '6') {
                            if (argv[i][j + 3] == 'L') {
                                strcpy(option->input_charset, "UTF-16LE");
                                j += 3;
                            } else if (argv[i][j + 3] == 'B') {
                                strcpy(option->input_charset, "UTF-16BE");
                                j += 3;
                            } else {
                                strcpy(option->input_charset, "UTF-16LE");
                                j += 2;
                            }
                        } else if (argv[i][j + 1] == '3' && argv[i][j + 2] == '2') {
                            if (argv[i][j + 3] == 'L') {
                                strcpy(option->input_charset, "UTF-32LE");
                                j += 3;
                            } else if (argv[i][j + 3] == 'B') {
                                strcpy(option->input_charset, "UTF-32BE");
                                j += 3;
                            } else {
                                strcpy(option->input_charset, "UTF-32LE");
                                j += 2;
                            }
                        } else if (argv[i][j + 1] == '8') {
                            strcpy(option->input_charset, "UTF-8");
                            j++;
                        } else {
                            strcpy(option->input_charset, "UTF-8");
                        }
                        break;
                    case 'K':
                        strcpy(option->input_charset, "EUC-KR");
                        break;
                    case 'C':
                        strcpy(option->input_charset, "GB2312");
                        break;
                    case 'B':
                        strcpy(option->input_charset, "BIG5");
                        break;
                    case 'm':
                        option->mime_decode = MIME_AUTODETECT;
                        if (argv[i][j + 1] == 'B') {
                            option->mime_decode = MIME_BASE64;
                            j++;
                        } else if (argv[i][j + 1] == 'Q') {
                            option->mime_decode = MIME_QUOTED_PRINTABLE;
                            j++;
                        } else if (argv[i][j + 1] == 'N') {
                            option->mime_decode = MIME_AUTODETECT;
                            j++;
                        } else if (argv[i][j + 1] == '0') {
                            option->mime_decode = MIME_NONE;
                            j++;
                        }
                        break;
                    case 'M':
                        option->mime_encode = MIME_BASE64_FULL;
                        if (argv[i][j + 1] == 'B') {
                            option->mime_encode = MIME_BASE64;
                            j++;
                        } else if (argv[i][j + 1] == 'Q') {
                            option->mime_encode = MIME_QUOTED_PRINTABLE;
                            j++;
                        } else if (argv[i][j + 1] == 'b') {
                            option->mime_encode = MIME_BASE64_FULL;
                            j++;
                        } else if (argv[i][j + 1] == 'q') {
                            option->mime_encode = MIME_QUOTED_PRINTABLE_FULL;
                            j++;
                        }
                        break;
                    case 'Z':
                        option->flag_hankaku_ascii = 1;
                        break;
                    case 'O':
                        o_param_found = true;
                        break;
                    case 'L':
                        option->line_mode = LINE_MODE_NOCONVERSION;
                        if (argv[i][j + 1] == 'u') {
                            option->line_mode = LINE_MODE_LF;
                            j++;
                        } else if (argv[i][j + 1] == 'w') {
                            option->line_mode = LINE_MODE_CRLF;
                            j++;
                        } else if (argv[i][j + 1] == 'm') {
                            option->line_mode = LINE_MODE_CR;
                            j++;
                        }
                        break;
                    case 'N':
                        option->flag_no_return_param = 1;
                        break;
                    case 'n':
                        option->flag_no_return = 1;
                        break;
                    default:
                        return -7; // invalid parameter
                        break;
                }
            }
        } else {
            j = argc - i - (o_param_found?1:0);
            if (0 < j) {
                option->input_filenames = new char * [j + 1];
            }
            for (j = i; j < argc; j++) {
                if (o_param_found && j == argc - 1) {
                    option->output_filename = argv[j];
                } else if (argv[j][0] != '\0') {
                    option->input_filenames[input_filenames_max] = (char *)argv[j];
                    option->input_filenames[input_filenames_max + 1] = NULL;
                    input_filenames_max++;
                }
            }
            i = argc;
            break;
        }
    }

    if (input_filenames_max == 0 && option->input_filenames != (char **)NULL) {
        delete [] option->input_filenames;
        option->input_filenames = (char **)NULL;
    }

    if (option->output_filename != NULL && option->output_filename[0] == '\0') {
        option->output_filename = NULL;
    }

    if (o_param_found && option->output_filename == NULL) {
        option->output_filename = INKF_DEF_OUT;
    }

    return 0;
}


extern "C" void inkf_free_option(TOption *option) {
    if (option->input_filenames != (char **)NULL) {
        delete [] option->input_filenames;
    }
    memset(option, 0, sizeof(TOption));
}


static void guess(const String &str, const char *input_charset, char *buf) {
    const char *charcode;
    const char *retcode;
    long i, length;
    char endian = 'L';

    if (input_charset != NULL && !strncasecmp(input_charset, "AUTODETECT", 10)) {
        if (strcasestr(input_charset, "JP")) {
            charcode = str.detectCharSetJP();
        } else {
            charcode = str.detectCharSet();
        }

        // for compatibility with nkf 2.x
        if (!strcmp(charcode, "CP932")) {
            charcode = "Shift_JIS";
        } else if (!strcmp(charcode, "CP949")) {
            charcode = "EUC-KR";
        } else if (!strcmp(charcode, "CP950")) {
            charcode = "BIG5";
        } else if (!strcmp(charcode, "GB18030")) {
            charcode = "GB2312";
        } else if (!strcmp(charcode, "CP1252")) {
            charcode = "Windows-1252";
        }
    } else {
        charcode = input_charset;
    }

    if ((!strncasecmp(charcode, "UTF-16", 6) || !strncasecmp(charcode, "UTF16", 5) ||
            !strncasecmp(charcode, "UTF-32", 6) || !strncasecmp(charcode, "UTF32", 5)) &&
            strcasestr(charcode, "BE") != NULL) {
        endian = 'B';
    } else {
        endian = 'L';
    }

    retcode = "LF";
    if (!strncmp(charcode, "UTF-16", 6)) {
        length = str.binaryLength();
        for (i = 0; i < length; i += 2) {
            if (endian == 'L' && i + 3 < length &&
                    str[i] == '\x0D' && str[i+1] == '\x00' &&
                    str[i+2] == '\x0A' && str[i+3] == '\x00') {
                retcode = "CRLF";
                break;
            } else if (endian == 'B' && i + 3 < length &&
                    str[i] == '\x00' && str[i+1] == '\x0D' &&
                    str[i+2] == '\x00' && str[i+3] == '\x0A') {
                retcode = "CRLF";
                break;
            }
        }
        if (!strcmp(retcode, "LF")) {
            for (i = 0; i < length; i += 2) {
                if (endian == 'L' && str[i] == '\x0D' && str[i+1] == '\x00') {
                    retcode = "CR";
                    break;
                } else if (endian == 'B' && str[i] == '\x00' && str[i+1] == '\x0D') {
                    retcode = "CR";
                    break;
                }
            }
        }
    } else if (!strncmp(charcode, "UTF-32", 6)) {
        length = str.binaryLength();
        for (i = 0; i < length; i += 4) {
            if (endian == 'L' && i + 7 < length &&
                    str[i] == '\x0D' && str[i+1] == '\x00' &&
                    str[i+2] == '\x00' && str[i+3] == '\x00' &&
                    str[i+4] == '\x0A' && str[i+5] == '\x00' &&
                    str[i+6] == '\x00' && str[i+7] == '\x00') {
                retcode = "CRLF";
                break;
            } else if (endian == 'B' && i + 7 < length &&
                    str[i] == '\x00' && str[i+1] == '\x00' &&
                    str[i+2] == '\x00' && str[i+3] == '\x0D' &&
                    str[i+4] == '\x00' && str[i+5] == '\x00' &&
                    str[i+6] == '\x00' && str[i+7] == '\x0A') {
                retcode = "CRLF";
                break;
            }
        }
        if (!strcmp(retcode, "LF")) {
            for (i = 0; i < length; i += 4) {
                if (endian == 'L' && i + 3 < length &&
                        str[i] == '\x0D' && str[i+1] == '\x00' &&
                        str[i+2] == '\x00' && str[i+3] == '\x00') {
                    retcode = "CR";
                    break;
                } else if (endian == 'B' && i + 3 < length &&
                        str[i] == '\x00' && str[i+1] == '\x00' &&
                        str[i+2] == '\x00' && str[i+3] == '\x0D') {
                    retcode = "CR";
                    break;
                }
            }
        }
    } else {
        if (0 <= str.search(RETURN_STR[LINE_MODE_CRLF])) {
            retcode = "CRLF";
        } else if (0 <= str.search(RETURN_STR[LINE_MODE_CR])) {
            retcode = "CR";
        }
    }
    sprintf(buf, "%s (%s)\n", charcode, retcode);
}


static void convert(String &str, const TOption *option) {
    String u8str, csv_column, re_pattern;
    Sheet csv_sheet;
    char input_charset[32], tmp_input_charset[32];
    char tmp_output_charset[32];
    bool append_return = false;
    char r[4];

    strncpy(input_charset, option->input_charset, 31);
    input_charset[31] = '\0';

    if (option->mime_decode == MIME_BASE64) {
        str = str.decodeBASE64();
    } else if (option->mime_decode == MIME_QUOTED_PRINTABLE) {
        str = str.decodeQuotedPrintable();
    } else if (option->mime_decode == MIME_AUTODETECT) {
        if (0 <= str.search("=?")) {
            set_input_charset_by_env(input_charset);
            if (!strncasecmp(input_charset, "AUTODETECT", 10)) {
                str = str.decodeMIME(input_charset, "UTF-8");
            } else {
                str = str.decodeMIME(input_charset, "UTF-8");
            }
            strcpy(input_charset, "UTF-8");
            append_return = true;
        } else if (str[0] != '=') {
            str = str.decodeBASE64();
        } else {
            str = str.decodeQuotedPrintable();
        }
    }

    if (!(!strcasecmp(input_charset, "AUTODETECT") && option->output_charset[0] == '\0')) {
        str = str.strconv(input_charset, option->output_charset);
    }
    if (option->flag_html_to_plain) {
        str = str.convertHTMLToPlain(option->output_charset, option->output_charset);
    }
    if (option->flag_hankaku_ascii) {
        str = str.changeWidth("a", option->output_charset, option->output_charset);
    } else if (option->flag_zenkaku_ascii) {
        str = str.changeWidth("A", option->output_charset, option->output_charset);
    }
    if (option->flag_hiragana) {
        str = str.changeWidth("HI", option->output_charset, option->output_charset);
    } else if (option->flag_katakana) {
        str = str.changeWidth("J", option->output_charset, option->output_charset);
    }
    if (option->flag_hankaku_katakana) {
        str = str.changeWidth("k", option->output_charset, option->output_charset);
    } else if (option->flag_zenkaku_katakana) {
        str = str.changeWidth("K", option->output_charset, option->output_charset);
    }
    if (option->line_mode != LINE_MODE_NOCONVERSION) {
        u8str = str.strconv(option->output_charset, "UTF-8");
        u8str = u8str.changeReturnCode(RETURN_STR[option->line_mode]);
        str = u8str.strconv("UTF-8", option->output_charset);
        u8str.clear();
    }
    str.gc();
    if (option->mime_encode == MIME_BASE64) {
        u8str = str.strconv(option->output_charset, "UTF-8");
        if (0 < u8str.len() && option->flag_no_return_param == false && u8str.search("\n") == u8str.searchR("\n") && u8str[u8str.len()-1] == '\n'){
            str = u8str.changeReturnCode("").strconv("UTF-8", option->output_charset);
        }
        str = str.encodeBASE64();
    } else if (option->mime_encode == MIME_QUOTED_PRINTABLE) {
        u8str = str.strconv(option->output_charset, "UTF-8");
        if (0 < u8str.len() && option->flag_no_return_param == false && u8str.search("\n") == u8str.searchR("\n") && u8str[u8str.len()-1] == '\n'){
            str = u8str.changeReturnCode("").strconv("UTF-8", option->output_charset);
        }
        str = str.encodeQuotedPrintable();
    } else if (option->mime_encode == MIME_BASE64_FULL) {
        u8str = str.strconv(option->output_charset, "UTF-8");
        if (0 < u8str.len() && option->flag_no_return_param == false && u8str.search("\n") == u8str.searchR("\n") && u8str[u8str.len()-1] == '\n'){
            str = u8str.changeReturnCode("").strconv("UTF-8", option->output_charset);
        }
        u8str.clear();
        sprintf(r, "%s\t", RETURN_STR[option->line_mode]);
        str = str.encodeMIME(option->output_charset,
                option->output_charset, 72, r, 'B');
        append_return = true;
    } else if (option->mime_encode == MIME_QUOTED_PRINTABLE_FULL) {
        u8str = str.strconv(option->output_charset, "UTF-8");
        if (0 < u8str.len() && option->flag_no_return_param ==false && u8str.search("\n") == u8str.searchR("\n") && u8str[u8str.len()-1] == '\n'){
            str = u8str.changeReturnCode("").strconv("UTF-8", option->output_charset);
        }
        u8str.clear();
        sprintf(r, "%s\t", RETURN_STR[option->line_mode]);
        str = str.encodeMIME(option->output_charset,
                option->output_charset, 72, r, 'Q');
        append_return = true;
    } else if (option->flag_sort_csv) {
        csv_sheet.setCSV(str, option->output_charset, "UTF-8");
        set_input_charset_by_env(tmp_input_charset);
        if (2 <= strlen(option->csv_column) && option->csv_column[0] == '"' &&
                option->csv_column[strlen(option->csv_column)-1] == '"') {
            csv_column = option->csv_column;
            csv_column = csv_column.mid(1, csv_column.len()-2).unescapeQuote(tmp_input_charset, "UTF-8");
        } else {
            csv_column = option->csv_column;
            csv_column = csv_column.strconv(tmp_input_charset, "UTF-8");
        }
        csv_sheet.sort(csv_column.c_str());
        strcpy(tmp_output_charset, option->output_charset);
        set_output_charset(tmp_output_charset, input_charset, str);
        str = csv_sheet.getCSV("UTF-8", tmp_output_charset,
                RETURN_STR[option->line_mode]);
    } else if (option->flag_sort_csv_r) {
        csv_sheet.setCSV(str, option->output_charset, "UTF-8");
        set_input_charset_by_env(tmp_input_charset);
        if (2 <= strlen(option->csv_column) && option->csv_column[0] == '"' &&
                option->csv_column[strlen(option->csv_column)-1] == '"') {
            csv_column = option->csv_column;
            csv_column = csv_column.mid(1, csv_column.len()-2).unescapeQuote(tmp_input_charset, "UTF-8");
        } else {
            csv_column = option->csv_column;
            csv_column = csv_column.strconv(tmp_input_charset, "UTF-8");
        }
        csv_sheet.sortr(csv_column.c_str());
        strcpy(tmp_output_charset, option->output_charset);
        set_output_charset(tmp_output_charset, input_charset, str);
        str = csv_sheet.getCSV("UTF-8", tmp_output_charset,
                RETURN_STR[option->line_mode]);
    } else if (option->flag_format_json) {
        JSONValue json_root;
        JSONReader reader;
        JSONStyledWriter writer;
        u8str = str.strconv(option->output_charset, "UTF-8");
        reader.parse(u8str, json_root);
        u8str.clear();
        strcpy(tmp_output_charset, option->output_charset);
        set_output_charset(tmp_output_charset, input_charset, str);
        str = writer.write(json_root).strconv("UTF-8", tmp_output_charset);
    } else if (option->flag_minify_json) {
        JSONValue json_root;
        JSONReader reader;
        JSONFastWriter writer;
        u8str = str.strconv(option->output_charset, "UTF-8");
        reader.parse(u8str, json_root);
        u8str.clear();
        strcpy(tmp_output_charset, option->output_charset);
        set_output_charset(tmp_output_charset, input_charset, str);
        str = writer.write(json_root).strconv("UTF-8", tmp_output_charset);
    } else if (option->flag_re_match) {
        strcpy(tmp_output_charset, option->output_charset);
        set_output_charset(tmp_output_charset, input_charset, str);
        str = str.strconv(option->output_charset, "UTF-8");
        set_input_charset_by_env(tmp_input_charset);
        if (2 <= strlen(option->re_match_pattern) && option->re_match_pattern[0] == '"' &&
                option->re_match_pattern[strlen(option->re_match_pattern)-1] == '"') {
            re_pattern = option->re_match_pattern;
            re_pattern = re_pattern.mid(1, re_pattern.len()-2).unescapeQuote(tmp_input_charset, "UTF-8");
        } else {
            re_pattern = option->re_match_pattern;
            re_pattern = re_pattern.strconv(tmp_input_charset, "UTF-8");
        }
        str = str.reMatch(re_pattern.c_str());
        str = str.strconv("UTF-8", tmp_output_charset);
    }
    if (option->flag_md5) {
        str = str.md5();
        append_return = true;
    } else if (option->flag_sha1) {
        str = str.sha1();
        append_return = true;
    }

    if (append_return && option->flag_no_return == false) {
        if (str.isBinary()) {
            u8str = str.strconv(option->output_charset, "UTF-8");
            u8str += '\n';
            str = u8str.strconv("UTF-8", option->output_charset);
            u8str.clear();
        } else {
            str.add('\n');
        }
    }
    str.gc();
}


static void set_input_charset_by_env(char *input_charset) {
    const char *env_lang;
    env_lang = getenv("LANG");
    if (env_lang != NULL) {
        if (strcasestr(env_lang, "UTF-8") != NULL) {
            strcpy(input_charset, "UTF-8");
        } else if (strcasestr(env_lang, "eucJP") != NULL) {
            strcpy(input_charset, "EUC-JP");
        } else if (strcasestr(env_lang, "SJIS") != NULL ||
                !strcasecmp(env_lang, "ja_JP.PCK")) {
            strcpy(input_charset, "SHIFT_JIS");
        } else if (strcasestr(env_lang, "ja") != NULL) {
            strcpy(input_charset, "AUTODETECT_JP");
        } else {
            strcpy(input_charset, "AUTODETECT");
        }
    } else {
        strcpy(input_charset, "AUTODETECT");
    }
}


static void set_output_charset(char *output_charset, const char *input_charset, String &str) {
    if (output_charset[0] == '\0') {
        strcpy(output_charset, input_charset);
        if (!strcasecmp(output_charset, "AUTODETECT_JP")) {
            strcpy(output_charset, str.detectCharSetJP());
        } else if (!strncasecmp(output_charset, "AUTODETECT", 10)) {
            strcpy(output_charset, str.detectCharSet());
        }
    }
}


static void get_help(char *buf) {
    sprintf(buf, 
      "Usage:  %s -[flags] [--] [in file/in URL] .. [out file for -O flag]\n"
      " j/s/e/w/k/c/b  Specify output encoding ISO-2022-JP, Shift_JIS, EUC-JP, UTF-8,\n"
      "                EUC-KR, GB2312, BIG5\n"
      " l              LATIN1\n"
      " w8[0]          UTF-8\n"
      " w16/w32[BL][0] UTF-16, UTF-32\n"
      " J/S/E/W/K/C/B  Specify input encoding ISO-2022-JP, Shift_JIS, EUC-JP, UTF-8,\n"
      "                EUC-KR, GB2312, BIG5\n"
      " W8             UTF-8\n"
      " W16/W32[BL]    UTF-16, UTF-32\n"
      " m[BQN0]        MIME decode [B:base64,Q:quoted,N:non-strict,0:no decode]\n"
      " M[BQbq]        MIME encode [B:base64 Q:quoted b:base64 header q:quoted header]\n"
      " Z              Convert JISX0208 Alphabet to ASCII\n"
      " O              Output to File (DEFAULT '%s')\n"
      " L[uwm]         Line mode u:LF w:CRLF m:CR (DEFAULT noconversion)\n"
      " N              Do not remove return-code when decoding/encoding MIME\n"
      " n              Do not append extra return-code\n"
      " t <encoding>   Specify output encoding (iconv compatible)\n"
      " f <encoding>   Specify input encoding (iconv compatible)\n"
      " --oc=<encoding>    Specify the output encoding\n"
      " --ic=<encoding>    Specify the input encoding\n"
      "  *supported encoding is as follows:\n"
      "   ISO-2022-JP\n"
      "   Shift_JIS (CP932)\n"
      "   EUC-JP (EUCJP-WIN,CP51932)\n"
      "   EUCJP-MS\n"
      "   ISO-8859-[1,2,3,4,5,6,7,8,9,10,11,13,14,15,16]\n"
      "   KOI8-[R,U]\n"
      "   CP1251\n"
      "   CP1252\n"
      "   CP1258\n"
      "   BIG5 (CP950)\n"
      "   GB2312 (GBK,GB18030)\n"
      "   EUC-KR (CP949)\n"
      "   UTF-8\n"
      "   UTF-7\n"
      "   UTF-7-IMAP\n"
      "   UTF-16, UTF-16BE, UTF-16LE\n"
      "   UTF-32, UTF-32BE, UTF-32LE\n"
      "   AUTODETECT\n"
      "   AUTODETECT_JP\n"
      " --hiragana --katakana  Hiragana/Katakana Conversion\n"
      " --zenkaku-ascii        Convert ASCII to JISX0208 Alphabet\n"
      " --hankaku-ascii        Convert JISX0208 Alphabet to ASCII\n"
      " --zenkaku-katakana     Convert Hankaku Kana to JISX0208 Katakana\n"
      " --hankaku-katakana     Convert JISX0208 Katakana to Hankaku Kana\n"
      " --html-to-plain        Convert HTML to Plain Text\n"
      " -g --guess     Guess the input code\n"
      " --midi         Create MIDI object from MML like music sequencial text\n"
      " --md5          Calc MD5 sum\n"
      " --sha1         Calc SHA-1 sum\n"
      " --sort-csv=<column>    Sort CSV\n"
      " --sort-csv-r=<column>  Sort CSV (reverse)\n"
      " --format-json  Reformat JSON\n"
      " --minify-json  Minify JSON\n"
      " --re-match=<pattern>   Regular Expression match\n"
      " --overwrite    Overwrite original listed files by filtered result\n"
      " -v --version   Print the version\n"
      " --help/-V      Print this help / configuration\n"
      , INKF_PROGNAME, INKF_DEF_OUT);
}


static void get_version(char *buf) {
    sprintf(buf, "%s - %s Version %s\n", INKF_PROGNAME, INKF_BRIEF, INKF_VERSION);
}


static void get_copy(char *buf) {
    sprintf(buf, "%s\n", INKF_COPY);
}
