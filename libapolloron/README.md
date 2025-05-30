# libapolloron version 1.1.9 - Multi-purpose C++ class library
Copyright (C) 2002-2025 Masashi Astro Tachibana <astro@apolloron.org>

# HOW TO MAKE
  [normal]  
  `make`

  [supporting libiconv (not recommended)]  
  `./configure --with-iconv; make`

  [supporting openssl (this is default if possible to link)]  
  `./configure --with-openssl; make`

  [not supporting openssl]  
  `./configure --without-openssl; make`


# HOW TO USE
  1. See "test/test.cc" as an example program.
  2. See the library header file "include/apolloron.h".
  3. See Source files.
  4. Ganbaru (never mind).


## LICENSE
  LGPLv2  
  - Oniguruma is BSD style license.
  - MD5 library is zlib/libpng license.
  - SHA1 library is BSD style license.
  - FCGI is BSD like license; See LICENSE.FCGI.
  - qsort is BSD style license.
  - jsoncpp is Public Domain.
  - libOftp is BSD style license.

## HISTORY
  ```
  1.1.9 : added escape method in String class for SQLite3 and MySQL
  1.1.8 : update around IMAP methods (alpha1)
  1.1.7 : fixed JOSN problems, update Keys class and fixed small bugs
  1.1.6 : added WebSocket functions, fixed build error
  1.1.5 : added addKey(const String &key, const String &value)
  1.1.4 : changed JIS encoding in from JIS8 into JIS7
  1.1.3 : updated CP1252 auto detection
  1.1.2 : added CP950 auto detection support
  1.1.1 : added CP1258, ISO-8859-11(CP874) support
  1.1.0 : added EUCJP-WIN support
  1.0.9 : fixed SSL connection for handshake failure
  1.0.8 : added String.convertPlainToHTML, fixed String.convertHTMLToPlain
  1.0.7 : experimentally fixed auto_detect for CP1252
  1.0.6 : optimized covertion of japanese hiragana/katakana from/to UTF-8
  1.0.5 : fixed bugs around FTPStream
  1.0.4 : added %S in format string of String.sprintf()
  1.0.3 : optimized GB18030 convertion
  1.0.2 : fixed charset autodetection in Sheet.setCSV
  1.0.1 : fixed auto detection of UTF-32 big endian
  1.0.0 : 1st release, fixed many problems
  0.47.8: added UTF-16, UTF-32 support
  0.47.7: fixed charset auto detection
  0.47.6: fixed ISO-8859-* convertion
  0.47.5: optimized 2 byte GB18030 convertion
  0.47.4: fixed 2 byte GB18030 convertion, and also UTF-7 and UTF-7-IMAP
  0.47.3: fixed utf8_len macro, convertion around cp932 and GB18030
  0.47.2: added 4-byte GB18030 <--> UTF-8 convertion support
  0.47.1: added fflush of stderr stream in CGI.cc
  0.47.0: fixed unit test error on OpenBSD
  0.46.9: fixed JSON library
  0.46.8: fixed to enable OpenSSL on MacOS
  0.46.7: fixed for clang++ warnings
  0.46.6: fixed Keys class and fixed a bug of converting ISO-8859 to UTF-8
  0.46.5: fixed FastCGI support (experimental)
  0.46.4: fixed MIME encoding bugs in String class
  0.46.3: fixed MIME 'Q' encoding bugs in String class
  0.46.2: fixed reading content by xmlHttpRequest in FCGI and CGI class
  0.46.1: added putError method of FCGI and CGI class
  0.46.0: added FTPStream class
  0.45.0: fixed decodeMIME method and escape methods in String class
  0.44.9: fixed decodeMIME method in String class
  0.44.8: fixed Shift_JIS to JIS convertion problem
  0.44.7: added mail classes
  0.44.6: added systeminfo
  0.44.5: added MIMEHeader class
  0.44.4: added Sheet class
  0.44.3: fixed bugs
  0.44.2: fixed bugs
  0.44.1: fixed bugs
  0.44.0: added putHeader, putContent in CGI/FCGI class
  0.43.9: fixed bugs in CGI class
  0.43.8: added getCookie method in CGI class
  0.43.7: added replace method in String class
  0.43.6: fixed timeout problem in Socket
  0.43.5: added crypt and decrypt medhod in String class
  0.43.4: added loadFile and saveFile method in Keys class
  0.43.3: fixed DateTime.set for IMAP INTERNALDATE of structure
  0.43.2: added String.cramMd5
  0.43.1: fixed a bug of converting charset from UTF-8 to another
  0.43.0: fixed a bug of converting text from HTML to plain
  0.42.9: fixed a bug of charset convertion to UTF-8
  0.42.8: fixed a bug of connect method of Socket class
  0.42.7: fixed for warnings of C++ compiler
  0.42.6: fixed bugs of Socket
  0.42.5: fixed a bug of connecting host name in Socket class
  0.42.4: added String.convertHTMLToPlain
  0.42.3: added CP1252
  0.42.2: fixed ISO-8859-*, added KOI8-R, KOI8-U and CP1251
  0.42.1: fixed bug of converting UTF-7 / UTF-7-IMAP (return code)
  0.42.0: added configure options
  0.41.9: added isEmailAddress
  0.41.8: added startTls
  0.41.7: added OpenSSL support in Socket class
  0.41.6: modified configure for supporting libiconv
  0.41.5: supported IPv6
  0.41.4: added changeReturnCode
  0.41.3: renamed CGI class
  0.41.2: fixed unescapeHTML for compatible with IE
  0.41.1: improved performance
  0.41.0: added Wild Card match method in String
  0.40.9: defined DATEFORMAT_RFC822 and DATEFORMAT_ISO8601
  0.40.8: added utils.cc
  0.40.7: added flock
  0.40.6: added DateTime class
  0.40.5: added UTF-8 clean up function for internal use
  0.40.4: added loadConfigFile method in Keys class
  0.40.3: fixed hankaku-kana problem in jis_to_sjis function
  0.40.2: added multipart/formdata receiving in Cgi class
  0.40.1: added <#if> in evalText function
  0.40.0: fixed bugs in evalText function
  0.39.9: added evalText function
  0.39.8: fixed some problems for CGI
  0.39.7: to be simple, and add UTF-7 character-set
  0.39.6: fixed charset converting bug
  0.39.5: -
  0.39.4: add japanese width converting function
  0.39.3: changed Object class
  0.39.2: added web tools (not completed)
  0.39.1: changed class name: List and Keys
  0.39.0: added internal function _ltoa(), _dtoa()
  0.38.9: changed source directory of regex, md5
  0.38.8: added namespace apolloron
  0.38.7: added HMAC-MD5 function
  0.38.6: continuing implementation of apolloron database engine.
  0.38.5: change several filenames of source.
  0.38.4: started implementation of apolloron database engine.
  0.38.3: String::version() --> const char* LIBAPOLLORON_VERSION
  0.38.2: charnge linux linker-option
  0.38.1: added socketAccept() (experimental)
  0.38.0: changed library name from liblongstr to libapolloron
  ```
