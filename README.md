# inkf version 2.1.8 - yet another nkf with international language support
Copyright (C) 2002-2025 Masashi Astro Tachibana <astro@apolloron.org>

## DESCRIPTION
  ```
  inkf is "Yet Another nkf", supported character-set of multi-language unlike
  original nkf. And inkf is able to get http/https contentas input source from
  Internet.
  Unlike other character-set conversion programs, inkf doesn't so strictly dist
  inguish the range of the character-code. For example, SHIFT_JIS and CP932 are
  treated as the same character set.
  ```

## PLATFORM
  - Linux, Solaris, MacOSX and other UNIX compatible systems.
  - GCC (g++) 2.95.3 or greater

## HOW TO MAKE
  - `make`

## HOW TO INSTALL
  - `make install`

## LICENSE
  - LGPLv2

## USAGE
  ```
  inkf -[flags] [--] [in file/in URL] .. [out file for -O flag]
   j/s/e/w/k/c/t  Specify output encoding ISO-2022-JP, Shift_JIS, EUC-JP, UTF-8,
                  EUC-KR, GB2312, BIG5
   J/S/E/W/K/C/T  Specify input encoding ISO-2022-JP, Shift_JIS, EUC-JP, UTF-8,
                  EUC-KR, GB2312, BIG5
   m[BQN0]        MIME decode [B:base64,Q:quoted,N:non-strict,0:no decode]
   M[BQbq]        MIME encode [B:base64 Q:quoted b:base64 header q:quoted
                  header]
   Z              Convert JISX0208 Alphabet to ASCII
   O              Output to File (DEFAULT 'inkf.out')
   L[uwm]         Line mode u:LF w:CRLF m:CR (DEFAULT noconversion)
   N              Do not remove return-code when decoding/encoding MIME
   n              Do not append extra return-code
   --ic=<encoding>        Specify the input encoding
   --oc=<encoding>        Specify the output encoding
    *supported encoding is as follows:
     ISO-2022-JP
     Shift_JIS (CP932)
     EUC-JP (EUCJP-WIN,CP51932)
     EUCJP-MS
     ISO-8859-[1,2,3,4,5,6,7,8,9,10,11,13,14,15,16]
     KOI8-[R,U]
     CP1251
     CP1252
     CP1258
     BIG5 (CP950)
     GB2312 (GBK,GB18030)
     EUC-KR (CP949)
     UTF-8
     UTF-7
     UTF-7-IMAP
     UTF-16, UTF-16BE, UTF-16LE
     UTF-32, UTF-32BE, UTF-32LE
     AUTODETECT
     AUTODETECT_JP
   --hiragana --katakana  Hiragana/Katakana Conversion
   --zenkaku-ascii        Convert ASCII to JISX0208 Alphabet
   --hankaku-ascii        Convert JISX0208 Alphabet to ASCII
   --zenkaku-katakana     Convert Hankaku Kana to JISX0208 Katakana
   --hankaku-katakana     Convert JISX0208 Katakana to Hankaku Kana
   --html-to-plain        Convert HTML to Plain Text
   -g --guess     Guess the input code
   --midi         Create MIDI object from MML like music sequencial text
   --md5          Calc MD5 sum
   --sha1         Calc SHA-1 sum
   --sort-csv=<column>    Sort CSV
   --sort-csv-r=<column>  Sort CSV (reverse)
   --format-json  Reformat JSON
   --minify-json  Minify JSON
   --overwrite    Overwrite original listed files by filtered result
   -v --version   Print the version
   --help/-V      Print this help / configuration
  ```

### HISTORY
  ```
  2.1.8: changed po mailaddress from moomin@users.sourceforge.jp to astro@apolloron.org
  2.1.7: fixed input-charset bugs of --html-to-plain option
  2.1.6: updateed for Clang 11, fixed small bugs in strmidi
  2.1.5: update usage Hankaku-kana in JIS (from JIS8 to JIS7)
  2.1.4: updated CP1252 auto detection
  2.1.3: added BIG5 (CP950) auto detection support
  2.1.2: added libinkf
  2.1.1: added CP1258, ISO-8859-11(CP874) support
  2.1.0: added EUCJP-WIN (CP51932) support
  2.0.7: fixed --html-to-plain and https handshake failure
  2.0.6: optimized covertion of japanese hiragana/katakana from/to UTF-8
  2.0.5: added "ftp://" support
  2.0.4: do not put extra return-code when encoding MIME if -N is not set
  2.0.3: added -n, -M[bq], --re-match option, update options --sort-csv, --sort-csv-r
  2.0.2: added options --sort-csv, --sort-csv-r, --format-json, --minify-json, --md5 and --sha1
  2.0.1: fixed auto detection of UTF-32 big endian
  2.0.0: fixed many problems, UTF-16 and UTF-32 full support
  1.2.4: added UTF-16, UTF-32 as convertable charset
  1.2.3: fixed charset autoditection
  1.2.2: fixed ISO-8859-* convertion
  1.2.1: performance tuning
  1.2.0: GB18030 optimisation, fixed GB18030 UCS2 range and around UTF-7
  1.1.9: GB18030 optimisation, fixed around CP932
  1.1.8: added 4-byte GB18030 <--> UTF-8 convertion support
  1.1.7: fixed memory allocation problem of String
  1.1.6: fixed compiling error on FreeBSD 11
  1.1.5: fixed compiling error on Fedora 28
  1.1.4: fixed convertion from ISO-8859 to UTF-8
  1.1.3: library update
  1.1.2: library update, fixed compiler warngings on CentOS7
  1.1.1: fixed quoted-string decoding
  1.1.0: library update
  1.0.9: fixed MIME decoding
  1.0.8: fixed convertion with "-Sj" option
  1.0.7: modified internal
  1.0.6: modified internal
  1.0.5: fixed compile error
  1.0.4: fixed timeout of Socket
  1.0.3: added HTMLClient class
  1.0.2: fixed a bug of converting charset from UTF-8 to another
  1.0.1: fixed a bug of converting text from HTML to plain
  1.0.0: fixed a bug of charset convertion to UTF-8
  0.9.9: fixed a bug of connect method of Socket class
  0.9.8: fixed for warnings of C++ compiler
  0.9.7: fixed bugs of Socket
  0.9.6: fixed http connection bug
  0.9.5: added --html-to-plain option, hankaku/zenkaku options
  0.9.4: added CP1252, -Z option, fixed charset ditection of http(s)
  0.9.3: fixed bugs of reading charset options
  0.9.2: fixed ISO-8859-*, added KOI8-R, KOI8-U, CP1251
  0.9.1: fixed bugs of "--guess" option and converting UTF-7 / UTF-7-IMAP
  0.9.0: first release
  ```
