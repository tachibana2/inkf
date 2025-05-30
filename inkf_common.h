/******************************************************************************/
/*! @file inkf_common.h
    @brief yet another nkf with international language support
    @author Masashi Astro Tachibana
    @date 2002-2025
 ******************************************************************************/

#ifndef _INKF_COMMON_H_
#define _INKF_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MIME_NONE,
    MIME_BASE64,
    MIME_QUOTED_PRINTABLE,
    MIME_BASE64_FULL,
    MIME_QUOTED_PRINTABLE_FULL,
    MIME_AUTODETECT
} TMIME;

typedef enum {
    LINE_MODE_NOCONVERSION = 0,
    LINE_MODE_LF = 1,
    LINE_MODE_CRLF = 2,
    LINE_MODE_CR = 3
} TLineMode;

typedef struct {
    int flag_show_help;
    int flag_show_version;
    int flag_guess;
    int flag_overwrite;
    int flag_html_to_plain;
    int flag_hankaku_ascii;
    int flag_zenkaku_ascii;
    int flag_hankaku_katakana;
    int flag_zenkaku_katakana;
    int flag_hiragana;
    int flag_katakana;
    int flag_midi;
    int flag_md5;
    int flag_sha1;
    int flag_sort_csv;
    int flag_sort_csv_r;
    const char *csv_column;
    int flag_format_json;
    int flag_minify_json;
    int flag_re_match;
    const char *re_match_pattern;
    int flag_no_return_param;
    int flag_no_return;
    char input_charset[32];
    char output_charset[32];
    TMIME mime_decode;
    TMIME mime_encode;
    TLineMode line_mode;
    char **input_filenames;
    const char *output_filename;
} TOption;


int inkf_command_exec(const TOption *option);
int inkf_lib_exec(const char *input_str, long input_str_len,
                  char **output_str, long *output_str_len,
                  const TOption *option);
void inkf_free_output_str(char *output_str);
int inkf_set_option(TOption *option, int argc, const char **argv);
void inkf_free_option(TOption *option);

#ifdef __cplusplus
}
#endif

#endif
