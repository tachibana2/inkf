/******************************************************************************/
/*! @file apolloron.h
    @brief Header file of libapolloron.
    @author Masashi Astro Tachibana, Apolloron Project.
    @date 2025-05-31
 ******************************************************************************/

#ifndef _APOLLORON_H_
#define _APOLLORON_H_

#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <string>

/*! @brief Version of libapolloron.
 */
#define LIBAPOLLORON_VERSION "1.1.9"


/// If defined, indicates that json library is embedded in CppTL library.
//# define JSON_IN_CPPTL 1

/// If defined, indicates that json may leverage CppTL library
//#  define JSON_USE_CPPTL 1
/// If defined, indicates that cpptl vector based map should be used instead of std::map
/// as Value container.
//#  define JSON_USE_CPPTL_SMALLMAP 1
/// If defined, indicates that Json specific container should be used
/// (hash table & simple deque container with customizable allocator).
/// THIS FEATURE IS STILL EXPERIMENTAL!
//#  define JSON_VALUE_USE_INTERNAL_MAP 1
/// Force usage of standard new/malloc based allocator instead of memory pool based allocator.
/// The memory pools allocator used optimization (initializing Value and ValueInternalLink
/// as if it was a POD) that may cause some validation tool to report errors.
/// Only has effects if JSON_VALUE_USE_INTERNAL_MAP is defined.
//#  define JSON_USE_SIMPLE_INTERNAL_ALLOCATOR 1

/// If defined, indicates that Json use exception to report invalid type manipulation
/// instead of C assert macro.
# define JSON_USE_EXCEPTION 0

# ifdef JSON_IN_CPPTL
#  include <cpptl/config.h>
#  ifndef JSON_USE_CPPTL
#   define JSON_USE_CPPTL 1
#  endif
# endif

# ifdef JSON_IN_CPPTL
#  define JSON_API CPPTL_API
# elif defined(JSON_DLL_BUILD)
#  define JSON_API __declspec(dllexport)
# elif defined(JSON_DLL)
#  define JSON_API __declspec(dllimport)
# else
#  define JSON_API
# endif

# include <string>
# include <vector>
# include <deque>
# include <stack>
# include <string>
# include <iostream>

# ifndef JSON_USE_CPPTL_SMALLMAP
#  include <map>
# else
#  include <cpptl/smallmap.h>
# endif
# ifdef JSON_USE_CPPTL
#  include <cpptl/forwards.h>
# endif


namespace apolloron {

extern const char *BUILD_DATE;
extern const char *BUILD_HOST;
extern const char *BUILD_OS;
extern const char *BUILD_OS_DISTRO;
extern const char *BUILD_OS_KERNEL;

class List;
class Keys;


/*----------------------------------------------------------------------------*/
/* String class                                                               */
/*----------------------------------------------------------------------------*/
/*! @brief Class of variable length character sequence.
 */
class String {
protected:
    // nBinaryLength <= nFixedLength <= nCapacity
    char *pText; // String
    long nLength; // Size of string
    long nCapacity; // Size of all allocated memory
    long nFixedLength; // Memory size which carries out minimum allocation
    long nBinaryLength; // Size in the case of treating as binary data
    bool nLengthRenewRecommended; // this is true if the length of string is uncertain
    String ***pTmp; // Temporary String
    long *pTmpLen; // Number of array of temporary String
    virtual String *tmpStr() const; // Reference of temporary String
    virtual bool clearTmpStr() const; // Deletion of all temporary String
    virtual bool pTextReplace(char *str, long text_len, long binary_len, long capacity); // Replacement pText
    virtual bool resize(long); // Change the size of allocated memory
    virtual bool nLengthRenew(); // renew nLength
    virtual bool nLengthNeedRenew(); // It calls, when length may become unfixed
    virtual bool mresize(long = 0); // Change size of minimum memory allocation
public:
    String();
    String(const String &value);
    String(const char *value);
    String(long value);
    String(int value);
    String(char value);
    String(double value);
    virtual ~String();

    // Deletion of String instance
    virtual bool clear();

    // Check if String is empty
    virtual bool empty() const;

    // Deletion of all temporary String
    virtual bool gc() const;

    // Reference of length of string
    virtual long len() const;

    // Reference of width of string
    virtual long width(const char * src_charset="UTF-8") const;

    // Get capacity of variable
    virtual long capacity() const;

    // Memory size which carries out minimum allocation
    virtual long fixedLength() const;
    virtual bool setFixedLength(long size);

    // Reference of size as binary String
    virtual long binaryLength() const;

    // Change to binary mode (or change binary size)
    virtual bool useAsBinary(long size);

    // Change to text mode
    virtual bool useAsText();

    // Reference of current mode
    virtual bool isText() const;
    virtual bool isBinary() const;

    // Cast
    virtual const char* c_str() const;
    virtual long toLong() const;
    virtual int toInt() const;
    virtual char toChar() const;
    virtual double toDouble() const;

    virtual operator const char*() const;
    virtual operator long() const;
    virtual operator int() const;
    virtual operator const char() const;
    virtual operator double() const;

    // Reference of array
    virtual char& operator [](long index);
    virtual char& operator [](int index);

    // Setup
    virtual bool set(const String &value);
    virtual bool set(const char *value, long length=-1L);
    virtual bool set(long value);
    virtual bool set(int value);
    virtual bool set(char value);
    virtual bool set(double value);
    virtual bool setBinary(char value);
    virtual bool setBinary(const char *value, long length);

    virtual const String& operator = (const String &value);
    virtual const String& operator = (const char *value);
    virtual const String& operator = (long value);
    virtual const String& operator = (int value);
    virtual const String& operator = (char value);
    virtual const String& operator = (double value);

    // Comparison
    virtual int compare(const String &value) const;
    virtual int compare(const char *value) const;
    virtual int compare(long value) const;
    virtual int compare(int value) const;
    virtual int compare(char value) const;
    virtual int compare(double value) const;

    virtual int operator == (const String &value) const;
    virtual int operator == (const char *value) const;
    virtual int operator == (long value) const;
    virtual int operator == (int value) const;
    virtual int operator == (char value) const;
    virtual int operator == (double value) const;

    virtual int operator != (const String &value) const;
    virtual int operator != (const char *value) const;
    virtual int operator != (long value) const;
    virtual int operator != (int value) const;
    virtual int operator != (char value) const;
    virtual int operator != (double value) const;

    virtual int operator <= (const String &value) const;
    virtual int operator <= (const char *value) const;
    virtual int operator <= (long value) const;
    virtual int operator <= (int value) const;
    virtual int operator <= (char value) const;
    virtual int operator <= (double value) const;

    virtual int operator >= (const String &value) const;
    virtual int operator >= (const char *value) const;
    virtual int operator >= (long value) const;
    virtual int operator >= (int value) const;
    virtual int operator >= (char value) const;
    virtual int operator >= (double value) const;

    virtual int operator < (const String &value) const;
    virtual int operator < (const char *value) const;
    virtual int operator < (long value) const;
    virtual int operator < (int value) const;
    virtual int operator < (char value) const;
    virtual int operator < (double value) const;

    virtual int operator > (const String & value) const;
    virtual int operator > (const char *value) const;
    virtual int operator > (long value) const;
    virtual int operator > (int value) const;
    virtual int operator > (char value) const;
    virtual int operator > (double value) const;

    // Addition
    virtual bool add(const String &value);
    virtual bool add(const char *value);
    virtual bool add(const String &value, long length);
    virtual bool add(const char *value, long length);
    virtual bool add(long value);
    virtual bool add(int value);
    virtual bool add(char value);
    virtual bool add(double value);
    virtual bool addBinary(char value);
    virtual bool addBinary(const char *value, long length);

    virtual const String& operator += (const String &value);
    virtual const String& operator += (const char *value);
    virtual const String& operator += (long value);
    virtual const String& operator += (int value);
    virtual const String& operator += (char value);
    virtual const String& operator += (double value);

    virtual String& operator + (const String &value) const;
    virtual String& operator + (const char *value) const;
    virtual String& operator + (long value) const;
    virtual String& operator + (int value) const;
    virtual String& operator + (char value) const;
    virtual String& operator + (double value) const;

    // Insertion
    virtual String& insert(long pos, const String &value) const;
    virtual String& insert(long pos, const char *value) const;
    virtual String& insert(long pos, long value) const;
    virtual String& insert(long pos, int value) const;
    virtual String& insert(long pos, char value) const;
    virtual String& insert(long pos, double value) const;

    // Trim
    virtual String& left(long length) const;
    virtual String& right(long length) const;
    virtual String& mid(long pos, long length=-1L) const;
    virtual String& trimR() const;
    virtual String& trimL() const;
    virtual String& trim() const;

    // Upper / lower
    virtual String& upper() const;
    virtual String& lower() const;

    // Search
    virtual long search(const String &key, long pos=0L) const;
    virtual long search(const char *key, long pos=0L) const;
    virtual long searchCase(const String &key, long pos=0L) const;
    virtual long searchCase(const char *key, long pos=0L) const;
    virtual long searchChar(const char c, long pos=0L) const;
    virtual long searchR(const String &key, long pos=-1L) const;
    virtual long searchR(const char *key, long pos=-1L) const;
    virtual long searchCaseR(const String &key, long pos=-1L) const;
    virtual long searchCaseR(const char *key, long pos=-1L) const;
    virtual long searchCharR(const char c, long pos=-1L) const;
    virtual String& replace(const String &key, const String &replace_str, long pos=0L) const;
    virtual String& replaceCase(const String &key, const String &replace_str, long pos=0L) const;

    // Format
    virtual int sprintf(const char *format, ...);

    // URL encoding
    virtual String& encodeURL() const;
    virtual String& decodeURL(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape HTML tag
    virtual String& escapeHTML(const char * src_charset="UTF-8", const char * dest_charset="UTF-8", const char *options="") const;
    virtual String& unescapeHTML(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;
    virtual String& escapeHTMLBackQuote(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape XML tag
    virtual String& escapeXML(const char * src_charset="UTF-8", const char * dest_charset="UTF-8", const char *options="") const;
    virtual String& unescapeXML(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape for Databases
    virtual String& escapeSQLite3(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;
    virtual String& escapeMySQL(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape for JSON
    virtual String& escapeJSON(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;
    virtual String& unescapeJSON(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape command line params
    virtual String& escapeCommandParam(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape Back-slash, Double-quote, Single-quote
    virtual String& escapeQuote(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;
    virtual String& unescapeQuote(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // Escape for CSV (" -> "")
    virtual String& escapeCSV(const char * src_charset="UTF-8", const char * dest_charset="UTF-8", const char * return_str="\n") const;
    virtual String& unescapeCSV(const char * src_charset="UTF-8", const char * dest_charset="UTF-8", const char * return_str="\n") const;

    // Escape filename
    virtual String& escapeFileName(const char * src_charset="UTF-8", const char * dest_charset="UTF-8") const;

    // HEX encoding
    virtual String& encodeHEX() const;
    virtual String& decodeHEX() const;

    // UUencoding
    virtual String& decodeUU(String * filename=(String *)0) const;

    // MIME encoding
    virtual String& encodeMIME(const char * src_charset="UTF-8", const char * dest_charset="UTF-8", long max_width=72, const char * return_str="\n", const char enctype='B') const;
    virtual String& decodeMIME(const char * src_charset="AUTODETECT", const char * dest_charset="UTF-8") const;
    virtual String& encodeBASE64(long max_width=72, const char * return_str="\n", const char * prefix_str=(const char *)0, const char * suffix_str=(const char *)0) const;
    virtual String& decodeBASE64() const;
    virtual String& encodeQuotedPrintable(long max_width=72, const char * return_str="\n", const char * prefix_str=(const char *)0, const char * suffix_str=(const char *)0) const;
    virtual String& decodeQuotedPrintable() const;
    virtual bool parseEmail(String& name, String& user, String& domain, const char * dest_charset="") const;

    // Crypt / Decrypt
    virtual String& crypt() const;
    virtual String& decrypt() const;

    // MD5 digest key
    virtual String& md5() const;
    virtual String& hmacMd5(const String & key) const;
    virtual String& cramMd5(const String & user, const String & challenge) const;

    // SHA1 hash
    virtual String& sha1() const;
    virtual String& hmacSha1(const String & key) const;

    // Plain text or HTML template convertion
    virtual String& evalText(const Keys replace_keys) const;

    // HTML to plain text
    virtual String& convertHTMLToPlain(const char *src_charset="UTF-8", const char *dest_charset="UTF-8") const;

    // Plain text to HTML
    virtual String& convertPlainToHTML(const char *src_charset="UTF-8", const char *dest_charset="UTF-8") const;

    // Character code conversion
    virtual String& strconv(const char *src_charset, const char *dest_charset) const;

    // Character code detection
    virtual const char* detectCharSet() const;
    virtual const char* detectCharSetJP() const;

    // return code (CR/LF/CRLF)
    virtual String& changeReturnCode(const char * return_str="\n") const;

    // Character width converting
    virtual String& changeWidth(const char *option, const char *src_charset="UTF-8", const char *dest_charset="UTF-8") const;

    // Regular expression
    virtual String& reMatch(const char *regex) const;
    virtual String& reSubst(const char *regex1, const char *regex2) const;
    virtual String& reTrans(const char *regex1, const char *regex2) const;

    // Wild Card
    virtual String& wcMatch(const char *wildcard, const char **sub_str=NULL) const;
    virtual bool isWcMatchAll(const char *wildcard) const;

    // Multi-line string processing
    virtual String& uniq() const;

    // Files
    virtual long loadFile(const String& filename);
    virtual long loadFile(const char *filename);
    virtual long loadFilePos(const String& filename, long start_pos, long size);
    virtual long loadFilePos(const char *filename, long start_pos, long size);
    virtual bool saveFile(const String& filename) const;
    virtual bool saveFile(const char *filename) const;
    virtual bool saveFilePos(const String& filename, long start_pos, long size) const;
    virtual bool saveFilePos(const char *filename, long start_pos, long size) const;

    // MIDI data conversion
    virtual String& toMIDI() const;

    // Stream operators (an emulation of std::ostream)
    String& operator << (const String& in) {
        *this += in;
        return *this;
    }

    String& operator << (const char* in) {
        *this += in;
        return *this;
    }
};

String operator + (const char* value1, const String& value2);


/*----------------------------------------------------------------------------*/
/* List class                                                                 */
/*----------------------------------------------------------------------------*/
/*! @brief Class of array.
 */
class List {
protected:
    String **pList; // content of array
    long pListSize; // Size of array
    long pListCapacity; // Capacity of array
    virtual bool addString(const String &); // Addition of string
    virtual bool delString(long index); // Deletion of string
    virtual bool insertString(long index, const String & value); // Insertion of string
public:
    List();
    List(const List & list);
    virtual ~List();

    // Deletion of object instance
    virtual bool clear();

    // Reference of element of array
    virtual String& operator [](long index);
    virtual String& operator [](int index);
    virtual const char* read(long index) const; // for multi-threaded use
    virtual const char* read(int index) const; // for multi-threaded use

    // Number of elements of array
    virtual long max() const;

    // Setup of array
    virtual bool set(const List & list);
    virtual const List& operator = (const List &list);
    virtual bool setDirFileList(const String & dirname, const char *wildcard="*", bool fullpath=false);
    virtual bool setFileList(const String & dirname, const char *wildcard="*", bool fullpath=false);
    virtual bool setDirList(const String & dirname, const char *wildcard="*", bool fullpath=false);
    virtual bool setEmails(const String & emails_str); /* comma separated */

    // Addition of element of array
    virtual const List& operator += (const String &value);
    virtual bool add(const String &value);

    // Insertion of element of array
    virtual bool insert(long index, const String &value);

    // Deletion of array
    virtual bool remove(long index);
};


// Binary tree element structure
typedef struct {
    char *key;
    String *value;
} Element;

/*----------------------------------------------------------------------------*/
/* Keys class                                                                 */
/*----------------------------------------------------------------------------*/
/*! @brief Class of associative array.
 */
class Keys {
protected:
    long nElementTotal[64];
    long nElementCapacity[64];
    Element ***pElement; // (pointers of array) x 64
    String *tmpString; // temporary string
    String *emptyString; // empty string to retuen if not found
public:
    Keys();
    Keys(const Keys & keys);
    virtual ~Keys();

    // Deletion of object instance
    virtual bool clear();

    // Reference of value of element of key
    virtual String& operator [](const String &key);
    virtual const String& operator [](const String &key) const;
    virtual String& operator [](const char *key);
    virtual const String& operator [](const char *key) const;

    virtual String& getValue(const String &key) const;
    virtual String& getValue(const char *key) const;
    virtual const char* read(const String &key) const; // for multi-threaded use
    virtual const char* read(const char *key) const; // for multi-threaded use

    // Key's existence check
    virtual bool isKeyExist(const String &key) const;
    virtual bool isKeyExist(const char *key) const;

    // Get number of key
    virtual long max() const;

    // Setup of keys
    virtual bool set(const Keys & keys);
    virtual const Keys& operator = (const Keys &keys);

    // Looking up key
    virtual const char *key(long index) const;
    virtual String& value(long index) const;
    virtual Element* array(long index) const;

    // Addition of key
    virtual bool addKey(const String &key, const String &value);
    virtual bool addKey(const String &key, const char *value);
    virtual bool addKey(const char *key, const String &value);
    virtual bool addKey(const char *key, const char *value);
    virtual bool addKey(const char *key, const char *value, long keyLen);

    // Deletion of key
    virtual bool delKey(const String &key);
    virtual bool delKey(const char *key);

    // Serialize
    virtual String& toString();
    virtual bool setString(const String &str);

    // Files
    virtual bool loadFile(const String &filename);
    virtual bool loadFile(const char *filename);
    virtual bool saveFile(const String &filename) const;
    virtual bool saveFile(const char *filename) const;
};


/*----------------------------------------------------------------------------*/
/* Sheet class                                                                */
/*----------------------------------------------------------------------------*/
/*! @brief Class of sheet.
 */
class Sheet {
protected:
    List columns; // Column names (with order)
    Keys **pRows; // Rows
    long rowsMax;
    long rowsCapacity;
    String *tmpString; // temporary string
    String *tmpCSVString; // temporary string for getCSV()
public:
    Sheet();
    Sheet(Sheet & sheet);
    virtual ~Sheet();

    // Deletion of object instance
    virtual bool clear();

    // Get number of cells
    virtual long maxCol() const;
    virtual long maxRow() const;

    // Setup of Sheet
    virtual bool set(Sheet &sheet);
    virtual const Sheet& operator = (Sheet &sheet);

    // compare of sheet
    virtual int operator == (Sheet &sheet);
    virtual int operator != (Sheet &sheet);

    // Columns
    virtual bool setCols(const List &list);
    virtual String& col(long col);
    virtual long colIndex(const String &colname);
    virtual bool addCol(const String &colname);
    virtual bool delCol(const String &colname);
    virtual bool renameCol(const String &old_colname, const String &new_colname);

    // Rows
    virtual bool addRow();
    virtual bool insertRow(long row);
    virtual bool delRow(long row);

    // Get or set value
    virtual String& getValue(const String &colname, long row);
    virtual String& getValue(long col, long row);
    virtual bool setValue(const String &colname, long row, const String &value);
    virtual bool setValue(long col, long row, const String &value);

    // Search
    virtual long searchRow(const String &colname, const String &value);

    // Sort
    virtual bool sort(const String &colname);
    virtual bool sortr(const String &colname);
    virtual bool sortnum(const String &colname);
    virtual bool sortnumr(const String &colname);

    // CSV
    virtual String& getCSV(const char * src_charset="UTF-8", const char * dest_charset="UTF-8", const char * return_str="\n");
    virtual bool setCSV(const String &csv, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");
    virtual bool loadCSV(const String &filename, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");
    virtual bool saveCSV(const String &filename, const char * src_charset="UTF-8", const char * dest_charset="UTF-8", const char * return_str="\n");
};


typedef struct {
    int mday;    // Day.     [1-31]
    int mon;     // Month.   [1-12]
    int year;    // Year.    [1970-2037]
    int wday;    // Day of week. [0-6]
    int yday;    // Days in year.[0-365]
} TDate;

typedef struct {
    int sec;     // Seconds. [0-61] (2 leap second)
    int min;     // Minutes. [0-59]
    int hour;    // Hours.   [0-23]
} TTime;

typedef struct {
    TDate date;  // Date
    TTime time;  // Time
    long gmtoff; // Registrant timezone (GMT offset)
} TDateTime;

/*----------------------------------------------------------------------------*/
/* DateTime class                                                             */
/*----------------------------------------------------------------------------*/
/*! @brief Class of date and time.
 */
class DateTime {
protected:
    TDateTime tmpDateTime; // Date and time
    String ***pTmp; // Temporary String
    long *pTmpLen; // Number of array of temporary String
    virtual String *tmpStr() const; // Reference of temporary String
    virtual bool clearTmpStr() const; // Deletion of all temporary String
public:
    DateTime();
    DateTime(DateTime &datetime);
    DateTime(TDateTime &datetime);
    DateTime(time_t time_t_time, long gmtoff=0L);
    DateTime(String &datetimestr);
    virtual ~DateTime();

    // Deletion of object instance
    virtual bool clear();

    // Setup of date and time
    virtual DateTime &operator = (const DateTime &datetime);
    virtual bool setNow();
    virtual bool set(const DateTime &datetime);
    virtual bool set(const String &datetimestr);
    virtual bool set(const TDateTime &datetime);
    virtual bool set(const time_t time_t_time, long gmtoff=0L);
    virtual bool setDate(const TDate &date);
    virtual bool setTime(const TTime &time);
    virtual bool setGMTOffset(long gmtoff);

    // Getting date and time
    virtual String &toString(const String &format) const;
#define DATEFORMAT_RFC822  "%a, %02d %b %04Y %02H:%02M:%02S %z"
#define DATEFORMAT_INTERNALDATE "%02d-%b-%04Y %02H:%02M:%02S %z"
#define DATEFORMAT_ISO8601 "%04Y-%02m-%02dT%02H:%02M:%02S%Z"
    virtual String &getMessage(const char * msgid, const char * lang="en", const char * charset="UTF-8") const;
    virtual const TDateTime &getDateTime() const;
    virtual const TDate &getDate() const;
    virtual const TTime &getTime() const;
    virtual long getGMTOffset() const;

    // Calculating date and time
    virtual bool adjustYear(int year);
    virtual bool adjustMonth(int month);
    virtual bool adjustDate(int day);
    virtual bool adjustHour(int hour);
    virtual bool adjustSeconds(long sec);
    virtual bool changeGMTOffset(long gmtoff);

    // Comparison
    virtual int compare(const DateTime &value) const;

    virtual int operator == (const DateTime &value) const;
    virtual int operator != (const DateTime &value) const;
    virtual int operator <= (const DateTime &value) const;
    virtual int operator >= (const DateTime &value) const;
    virtual int operator < (const DateTime &value) const;
    virtual int operator > (const DateTime &value) const;
};


/*----------------------------------------------------------------------------*/
/* MIMEHeader class                                                           */
/*----------------------------------------------------------------------------*/
/*! @brief Class of MIMEHeader.
 */
class MIMEHeader {
protected:
    List names; // Header names
    List values; // Header values
    String *tmpString; // temporary string
public:
    MIMEHeader();
    MIMEHeader(const String & mime_header);
    MIMEHeader(MIMEHeader & mime_header);
    virtual ~MIMEHeader();

    // Deletion of object instance
    virtual bool clear();

    // Get number of line
    virtual long maxRow() const;

    // Setup of MIMEHeader
    virtual bool set(const String & mime_header);
    virtual bool set(MIMEHeader &mime_header);
    virtual const MIMEHeader& operator = (const String & mime_header);
    virtual const MIMEHeader& operator = (MIMEHeader &mime_header);

    // Access
    virtual String& getName(long row);
    virtual long searchRow(const String &name);
    virtual String& getValue(long row);
    virtual String& getValue(const String &name); // ignore case, first match
    virtual bool addRow(const String &name, const String &value); // overwrite if same name exist
    virtual bool insertRow(long row, const String &name, const String &value);
    virtual bool delRow(long row);
    virtual bool setName(long row, const String &name);
    virtual bool setValue(long row, const String &value);

    // Get header as text
    virtual String& toString();
};


/*----------------------------------------------------------------------------*/
/* Socket class                                                               */
/*----------------------------------------------------------------------------*/
/*! @brief Class of basic networking of TCP socket.
 */
class Socket {
protected:
    char dstHost[1024]; // Destination host name
    int dstSocket; // Destination socket number
    int nTimeout; // Timeout value (second)
    int nErrno; // Error number
    bool bConnected; // false: Disconnected  true: Connected
    String *tmpString; // Cash for receiving data
    bool bSSLStream; // false: non SSL  true: SSL
    void *pSSL; // SSL*
    void *pSSLCTX; // SSL_CTX*
public:
    Socket(int recycle_dstSocket = -1);
    virtual ~Socket();

    // Deletion of object instance
    virtual bool clear();

    // Connection
    virtual bool connect(const char *host, const char *port, bool ssl=false);

    // Disconnection
    virtual bool disconnect();

    // Timeout setup
    virtual bool setTimeout(int sec);

    // Refer to error number
    virtual int error() const;

    // Connection status check
    virtual bool connected() const;

    // STARTTLS
    virtual bool startTls();

    // Data transmission
    virtual bool send(const String &str);
    virtual bool send(const char *str);

    // Data reception
    virtual String &receive(long size);
    virtual String &receiveLine();
    virtual String &receivedData() const;
};


/*----------------------------------------------------------------------------*/
/* JSON library                                                               */
/*----------------------------------------------------------------------------*/
// writer
class JSONFastWriter;
class JSONStyledWriter;

// reader
class JSONReader;

// features
class JSONFeatures;

// value
typedef int Int;
typedef unsigned int UInt;
class StaticString;
class Path;
class PathArgument;
class JSONValue;
class JSONValueIteratorBase;
class JSONValueIterator;
class JSONValueConstIterator;
#ifdef JSON_VALUE_USE_INTERNAL_MAP
class JSONValueAllocator;
class JSONValueMapAllocator;
class JSONValueInternalLink;
class JSONValueInternalArray;
class JSONValueInternalMap;
#endif // #ifdef JSON_VALUE_USE_INTERNAL_MAP


/** \brief JSON (JavaScript Object Notation).
 */

/** \brief Type of the value held by a JSONValue object.
 */
enum JSONValueType {
    nullValue = 0, ///< 'null' value
    intValue,      ///< signed integer value
    uintValue,     ///< unsigned integer value
    realValue,     ///< double value
    stringValue,   ///< UTF-8 string value
    booleanValue,  ///< bool value
    arrayValue,    ///< array value (ordered list)
    objectValue    ///< object value (collection of name/value pairs).
};

enum CommentPlacement {
    commentBefore = 0,        ///< a comment placed on the line before a value
    commentAfterOnSameLine,   ///< a comment just after a value on the same line
    commentAfter,             ///< a comment on the line after a value (only make sense for root value)
    numberOfCommentPlacement
};

//# ifdef JSON_USE_CPPTL
//   typedef CppTL::AnyEnumerator<const char *> EnumMemberNames;
//   typedef CppTL::AnyEnumerator<const JSONValue &> EnumValues;
//# endif

/** \brief Lightweight wrapper to tag static string.
 *
 * JSONValue constructor and objectValue member assignement takes advantage of the
 * StaticString and avoid the cost of string duplication when storing the
 * string or the member name.
 *
 * Example of usage:
 * \code
 * apolloron::JSONValue aValue( StaticString("some text") );
 * apolloron::JSONValue object;
 * static const StaticString code("code");
 * object[code] = 1234;
 * \endcode
 */
class JSON_API StaticString {
public:
    explicit StaticString(const char *czstring)
        : str_(czstring) {
    }

    operator const char *() const {
        return str_;
    }

    const char *c_str() const {
        return str_;
    }

private:
    const char *str_;
};

/** \brief Represents a <a HREF="http://www.json.org">JSON</a> value.
 *
 * This class is a discriminated union wrapper that can represents a:
 * - signed integer [range: JSONValue::minInt - JSONValue::maxInt]
 * - unsigned integer (range: 0 - JSONValue::maxUInt)
 * - double
 * - UTF-8 string
 * - boolean
 * - 'null'
 * - an ordered list of JSONValue
 * - collection of name/value pairs (javascript object)
 *
 * The type of the held value is represented by a #JSONValueType and
 * can be obtained using type().
 *
 * values of an #objectValue or #arrayValue can be accessed using operator[]() methods.
 * Non const methods will automatically create the a #nullValue element
 * if it does not exist.
 * The sequence of an #arrayValue will be automatically resize and initialized
 * with #nullValue. resize() can be used to enlarge or truncate an #arrayValue.
 *
 * The get() methods can be used to obtanis default value in the case the required element
 * does not exist.
 *
 * It is possible to iterate over the list of a #objectValue values using
 * the getMemberNames() method.
 */
class JSON_API JSONValue {
    friend class JSONValueIteratorBase;
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    friend class JSONValueInternalLink;
    friend class JSONValueInternalMap;
# endif
public:
    typedef std::vector<std::string> Members;
    typedef JSONValueIterator iterator;
    typedef JSONValueConstIterator const_iterator;
    typedef apolloron::UInt UInt;
    typedef apolloron::Int Int;
    typedef UInt ArrayIndex;

    static const JSONValue null;
    static const Int minInt;
    static const Int maxInt;
    static const UInt maxUInt;

private:
#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
# ifndef JSON_VALUE_USE_INTERNAL_MAP
    class CZString {
    public:
        enum DuplicationPolicy {
            noDuplication = 0,
            duplicate,
            duplicateOnCopy
        };
        CZString(int index);
        CZString(const char *cstr, DuplicationPolicy allocate);
        CZString(const CZString &other);
        ~CZString();
        CZString &operator =(const CZString &other);
        bool operator<(const CZString &other) const;
        bool operator==(const CZString &other) const;
        int index() const;
        const char *c_str() const;
        bool isStaticString() const;
    private:
        void swap(CZString &other);
        const char *cstr_;
        int index_;
    };

public:
#  ifndef JSON_USE_CPPTL_SMALLMAP
    typedef std::map<CZString, JSONValue> ObjectValues;
#  else
    typedef CppTL::SmallMap<CZString, JSONValue> ObjectValues;
#  endif // ifndef JSON_USE_CPPTL_SMALLMAP
# endif // ifndef JSON_VALUE_USE_INTERNAL_MAP
#endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

public:
    /** \brief Create a default JSONValue of the given type.

      This is a very useful constructor.
      To create an empty array, pass arrayValue.
      To create an empty object, pass objectValue.
      Another JSONValue can then be set to this one by assignment.
    This is useful since clear() and resize() will not alter types.

           Examples:
    \code
    apolloron::JSONValue null_value; // null
    apolloron::JSONValue arr_value(apolloron::arrayValue); // []
    apolloron::JSONValue obj_value(apolloron::objectValue); // {}
    \endcode
         */
    JSONValue(JSONValueType type = nullValue);
    JSONValue(Int value);
    JSONValue(UInt value);
    JSONValue(double value);
    JSONValue(const char *value);
    JSONValue(const char *beginValue, const char *endValue);
    /** \brief Constructs a value from a static string.

     * Like other value string constructor but do not duplicate the string for
     * internal storage. The given string must remain alive after the call to this
     * constructor.
     * Example of usage:
     * \code
     * apolloron::JSONValue aValue( StaticString("some text") );
     * \endcode
     */
    JSONValue(const StaticString &value);
    JSONValue(const std::string &value);
    JSONValue(const String &value);
# ifdef JSON_USE_CPPTL
    JSONValue(const CppTL::ConstString &value);
# endif
    JSONValue(bool value);
    JSONValue(const JSONValue &other);
    ~JSONValue();

    JSONValue &operator=(const JSONValue &other);
    /// Swap values.
    /// \note Currently, comments are intentionally not swapped, for
    /// both logic and efficiency.
    void swap(JSONValue &other);

    JSONValueType type() const;

    bool operator <(const JSONValue &other) const;
    bool operator <=(const JSONValue &other) const;
    bool operator >=(const JSONValue &other) const;
    bool operator >(const JSONValue &other) const;

    bool operator ==(const JSONValue &other) const;
    bool operator !=(const JSONValue &other) const;

    int compare(const JSONValue &other);

    const char *c_str() const;
    String asString() const;
# ifdef JSON_USE_CPPTL
    CppTL::ConstString asConstString() const;
# endif
    Int asInt() const;
    UInt asUInt() const;
    double asDouble() const;
    bool asBool() const;

    bool isNull() const;
    bool isBool() const;
    bool isInt() const;
    bool isUInt() const;
    bool isIntegral() const;
    bool isDouble() const;
    bool isNumeric() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;

    bool isConvertibleTo(JSONValueType other) const;

    /// Number of values in array or object
    UInt size() const;

    /// \brief Return true if empty array, empty object, or null;
    /// otherwise, false.
    bool empty() const;

    /// Return isNull()
    bool operator!() const;

    /// Remove all object members and array elements.
    /// \pre type() is arrayValue, objectValue, or nullValue
    /// \post type() is unchanged
    void clear();

    /// Resize the array to size elements.
    /// New elements are initialized to null.
    /// May only be called on nullValue or arrayValue.
    /// \pre type() is arrayValue or nullValue
    /// \post type() is arrayValue
    void resize(UInt size);

    /// Access an array element (zero based index ).
    /// If the array contains less than index element, then null value are inserted
    /// in the array so that its size is index+1.
    /// (You may need to say 'value[0u]' to get your compiler to distinguish
    ///  this from the operator[] which takes a string.)
    JSONValue &operator[](UInt index);
    /// Access an array element (zero based index )
    /// (You may need to say 'value[0u]' to get your compiler to distinguish
    ///  this from the operator[] which takes a string.)
    const JSONValue &operator[](UInt index) const;
    /// If the array contains at least index+1 elements, returns the element value,
    /// otherwise returns defaultValue.
    JSONValue get(UInt index,
                  const JSONValue &defaultValue) const;
    /// Return true if index < size().
    bool isValidIndex(UInt index) const;
    /// \brief Append value to array at the end.
    ///
    /// Equivalent to jsonvalue[jsonvalue.size()] = value;
    JSONValue &append(const JSONValue &value);

    /// Access an object value by name, create a null member if it does not exist.
    JSONValue &operator[](const String &key);
    JSONValue &operator[](const char *key);
    /// Access an object value by name, returns null if there is no member with that name.
    const JSONValue &operator[](const String &key) const;
    const JSONValue &operator[](const char *key) const;
    /// Access an object value by name, create a null member if it does not exist.
    JSONValue &operator[](const std::string &key);
    /// Access an object value by name, returns null if there is no member with that name.
    const JSONValue &operator[](const std::string &key) const;
    /** \brief Access an object value by name, create a null member if it does not exist.

     * If the object as no entry for that name, then the member name used to store
     * the new entry is not duplicated.
     * Example of use:
     * \code
     * apolloron::JSONValue object;
     * static const StaticString code("code");
     * object[code] = 1234;
     * \endcode
     */
    JSONValue &operator[](const StaticString &key);
# ifdef JSON_USE_CPPTL
    /// Access an object value by name, create a null member if it does not exist.
    JSONValue &operator[](const CppTL::ConstString &key);
    /// Access an object value by name, returns null if there is no member with that name.
    const JSONValue &operator[](const CppTL::ConstString &key) const;
# endif
    /// Return the member named key if it exist, defaultValue otherwise.
    JSONValue get(const String &key,
                  const JSONValue &defaultValue) const;
    /// Return the member named key if it exist, defaultValue otherwise.
    JSONValue get(const char *key,
                  const JSONValue &defaultValue) const;
    /// Return the member named key if it exist, defaultValue otherwise.
    JSONValue get(const std::string &key,
                  const JSONValue &defaultValue) const;
# ifdef JSON_USE_CPPTL
    /// Return the member named key if it exist, defaultValue otherwise.
    JSONValue get(const CppTL::ConstString &key,
                  const JSONValue &defaultValue) const;
# endif
    /// \brief Remove and return the named member.
    ///
    /// Do nothing if it did not exist.
    /// \return the removed JSONValue, or null.
    /// \pre type() is objectValue or nullValue
    /// \post type() is unchanged
    JSONValue removeMember(const char* key);
    /// Same as removeMember(const char*)
    JSONValue removeMember(const std::string &key);
    JSONValue removeMember(const String &key);

    /// Return true if the object has a member named key.
    bool isMember(const char *key) const;
    /// Return true if the object has a member named key.
    bool isMember(const std::string &key) const;
    bool isMember(const String &key) const;
# ifdef JSON_USE_CPPTL
    /// Return true if the object has a member named key.
    bool isMember(const CppTL::ConstString &key) const;
# endif

    /// \brief Return a list of the member names.
    ///
    /// If null, return an empty list.
    /// \pre type() is objectValue or nullValue
    /// \post if type() was nullValue, it remains nullValue
    Members getMemberNames() const;

//# ifdef JSON_USE_CPPTL
//      EnumMemberNames enumMemberNames() const;
//      EnumValues enumValues() const;
//# endif

    /// Comments must be //... or /* ... */
    void setComment(const String &comment,
                    CommentPlacement placement);
    /// Comments must be //... or /* ... */
    void setComment(const char *comment,
                    CommentPlacement placement);
    /// Comments must be //... or /* ... */
    void setComment(const std::string &comment,
                    CommentPlacement placement);
    bool hasComment(CommentPlacement placement) const;
    /// Include delimiters and embedded newlines.
    String getComment(CommentPlacement placement) const;

    String toStyledString() const;

    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

private:
    JSONValue &resolveReference(const char *key,
                                bool isStatic);

# ifdef JSON_VALUE_USE_INTERNAL_MAP
    inline bool isItemAvailable() const {
        return itemIsUsed_ == 0;
    }

    inline void setItemUsed(bool isUsed=true) {
        itemIsUsed_ = isUsed ? 1 : 0;
    }

    inline bool isMemberNameStatic() const {
        return memberNameIsStatic_ == 0;
    }

    inline void setMemberNameIsStatic(bool isStatic) {
        memberNameIsStatic_ = isStatic ? 1 : 0;
    }
# endif // # ifdef JSON_VALUE_USE_INTERNAL_MAP

private:
    struct CommentInfo {
        CommentInfo();
        ~CommentInfo();

        void setComment(const char *text);

        char *comment_;
    };

    //struct MemberNamesTransform
    //{
    //   typedef const char *result_type;
    //   const char *operator()( const CZString &name ) const
    //   {
    //      return name.c_str();
    //   }
    //};

    union JSONValueHolder {
        Int int_;
        UInt uint_;
        double real_;
        bool bool_;
        char *string_;
# ifdef JSON_VALUE_USE_INTERNAL_MAP
        ValueInternalArray *array_;
        ValueInternalMap *map_;
#else
        ObjectValues *map_;
# endif
    } value_;
    JSONValueType type_ : 8;
    int allocated_ : 1;     // Notes: if declared as bool, bitfield is useless.
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    unsigned int itemIsUsed_ : 1;      // used by the ValueInternalMap container.
    int memberNameIsStatic_ : 1;       // used by the ValueInternalMap container.
# endif
    CommentInfo *comments_;
};


/** \brief Experimental and untested: represents an element of the "path" to access a node.
 */
class PathArgument {
public:
    friend class Path;

    PathArgument();
    PathArgument(UInt index);
    PathArgument(const String &key);
    PathArgument(const char *key);
    PathArgument(const std::string &key);

private:
    enum Kind {
        kindNone = 0,
        kindIndex,
        kindKey
    };
    std::string key_;
    UInt index_;
    Kind kind_;
};

/** \brief Experimental and untested: represents a "path" to access a node.
 *
 * Syntax:
 * - "." => root node
 * - ".[n]" => elements at index 'n' of root node (an array value)
 * - ".name" => member named 'name' of root node (an object value)
 * - ".name1.name2.name3"
 * - ".[0][1][2].name1[3]"
 * - ".%" => member name is provided as parameter
 * - ".[%]" => index is provied as parameter
 */
class Path {
public:
    Path(const String &path,
         const PathArgument &a1 = PathArgument(),
         const PathArgument &a2 = PathArgument(),
         const PathArgument &a3 = PathArgument(),
         const PathArgument &a4 = PathArgument(),
         const PathArgument &a5 = PathArgument());

    Path(const std::string &path,
         const PathArgument &a1 = PathArgument(),
         const PathArgument &a2 = PathArgument(),
         const PathArgument &a3 = PathArgument(),
         const PathArgument &a4 = PathArgument(),
         const PathArgument &a5 = PathArgument());

    Path(const char *path,
         const PathArgument &a1 = PathArgument(),
         const PathArgument &a2 = PathArgument(),
         const PathArgument &a3 = PathArgument(),
         const PathArgument &a4 = PathArgument(),
         const PathArgument &a5 = PathArgument());

    const JSONValue &resolve(const JSONValue &root) const;
    JSONValue resolve(const JSONValue &root,
                      const JSONValue &defaultValue) const;
    /// Creates the "path" to access the specified node and returns a reference on the node.
    JSONValue &make(JSONValue &root) const;

private:
    typedef std::vector<const PathArgument *> InArgs;
    typedef std::vector<PathArgument> Args;

    void makePath(const std::string &path,
                  const InArgs &in);
    void addPathInArg(const std::string &path,
                      const InArgs &in,
                      InArgs::const_iterator &itInArg,
                      PathArgument::Kind kind);
    void invalidPath(const std::string &path,
                     int location);

    Args args_;
};

/** \brief Experimental do not use: Allocator to customize member name and string value memory management done by JSONValue.
 *
 * - makeMemberName() and releaseMemberName() are called to respectively duplicate and
 *   free an apolloron::objectValue member name.
 * - duplicateStringValue() and releaseStringValue() are called similarly to
 *   duplicate and free a apolloron::stringValue value.
 */
class JSONValueAllocator {
public:
    enum { unknown=(unsigned) - 1 };

    virtual ~JSONValueAllocator();

    virtual char *makeMemberName(const char *memberName) = 0;
    virtual void releaseMemberName(char *memberName) = 0;
    virtual char *duplicateStringValue(const char *value,
                                       unsigned int length=unknown) = 0;
    virtual void releaseStringValue(char *value) = 0;
};

#ifdef JSON_VALUE_USE_INTERNAL_MAP
/** \brief Allocator to customize JSONValue internal map.
 * Below is an example of a simple implementation (default implementation actually
 * use memory pool for speed).
 * \code
   class DefaultValueMapAllocator : public JSONValueMapAllocator
   {
   public: // overridden from JSONValueMapAllocator
      virtual JSONValueInternalMap *newMap() {
         return new JSONValueInternalMap();
      }

      virtual JSONValueInternalMap *newMapCopy(const JSONValueInternalMap &other) {
         return new JSONValueInternalMap(other);
      }

      virtual void destructMap(JSONValueInternalMap *map) {
         delete map;
      }

      virtual JSONValueInternalLink *allocateMapBuckets(unsigned int size) {
         return new JSONValueInternalLink[size];
      }

      virtual void releaseMapBuckets(JSONValueInternalLink *links) {
         delete [] links;
      }

      virtual JSONValueInternalLink *allocateMapLink() {
         return new JSONValueInternalLink();
      }

      virtual void releaseMapLink(JSONValueInternalLink *link) {
         delete link;
      }
   };
 * \endcode
 */
class JSON_API JSONValueMapAllocator {
public:
    virtual ~JSONValueMapAllocator();
    virtual JSONValueInternalMap *newMap() = 0;
    virtual JSONValueInternalMap *newMapCopy(const JSONValueInternalMap &other) = 0;
    virtual void destructMap(JSONValueInternalMap *map) = 0;
    virtual JSONValueInternalLink *allocateMapBuckets(unsigned int size) = 0;
    virtual void releaseMapBuckets(JSONValueInternalLink *links) = 0;
    virtual JSONValueInternalLink *allocateMapLink() = 0;
    virtual void releaseMapLink(JSONValueInternalLink *link) = 0;
};

/** \brief JSONValueInternalMap hash-map bucket chain link (for internal use only).
 * \internal previous_ & next_ allows for bidirectional traversal.
 */
class JSON_API JSONValueInternalLink {
public:
    enum {itemPerLink=6};  // sizeof(JSONValueInternalLink) = 128 on 32 bits architecture.
    enum InternalFlags {
        flagAvailable = 0,
        flagUsed = 1
    };

    JSONValueInternalLink();

    ~JSONValueInternalLink();

    JSONValue items_[itemPerLink];
    char *keys_[itemPerLink];
    JSONValueInternalLink *previous_;
    JSONValueInternalLink *next_;
};


/** \brief A linked page based hash-table implementation used internally by JSONValue.
 * \internal JSONValueInternalMap is a tradional bucket based hash-table, with a linked
 * list in each bucket to handle collision. There is an addional twist in that
 * each node of the collision linked list is a page containing a fixed amount of
 * value. This provides a better compromise between memory usage and speed.
 *
 * Each bucket is made up of a chained list of JSONValueInternalLink. The last
 * link of a given bucket can be found in the 'previous_' field of the following bucket.
 * The last link of the last bucket is stored in tailLink_ as it has no following bucket.
 * Only the last link of a bucket may contains 'available' item. The last link always
 * contains at least one element unless is it the bucket one very first link.
 */
class JSON_API JSONValueInternalMap {
    friend class JSONValueIteratorBase;
    friend class JSONValue;
public:
    typedef unsigned int HashKey;
    typedef unsigned int BucketIndex;

# ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
    struct IteratorState {
        IteratorState()
            : map_(0)
            , link_(0)
            , itemIndex_(0)
            , bucketIndex_(0) {
        }
        JSONValueInternalMap *map_;
        JSONValueInternalLink *link_;
        BucketIndex itemIndex_;
        BucketIndex bucketIndex_;
    };
# endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

    JSONValueInternalMap();
    JSONValueInternalMap(const JSONValueInternalMap &other);
    JSONValueInternalMap &operator =(const JSONValueInternalMap &other);
    ~JSONValueInternalMap();

    void swap(JSONValueInternalMap &other);

    BucketIndex size() const;

    void clear();

    bool reserveDelta(BucketIndex growth);

    bool reserve(BucketIndex newItemCount);

    const JSONValue *find(const char *key) const;

    JSONValue *find(const char *key);

    JSONValue &resolveReference(const char *key,
                                bool isStatic);

    void remove(const char *key);

    void doActualRemove(JSONValueInternalLink *link,
                        BucketIndex index,
                        BucketIndex bucketIndex);

    JSONValueInternalLink *&getLastLinkInBucket(BucketIndex bucketIndex);

    JSONValue &setNewItem(const char *key,
                          bool isStatic,
                          JSONValueInternalLink *link,
                          BucketIndex index);

    JSONValue &unsafeAdd(const char *key,
                         bool isStatic,
                         HashKey hashedKey);

    HashKey hash(const char *key) const;

    int compare(const JSONValueInternalMap &other) const;

private:
    void makeBeginIterator(IteratorState &it) const;
    void makeEndIterator(IteratorState &it) const;
    static bool equals(const IteratorState &x, const IteratorState &other);
    static void increment(IteratorState &iterator);
    static void incrementBucket(IteratorState &iterator);
    static void decrement(IteratorState &iterator);
    static const char *key(const IteratorState &iterator);
    static const char *key(const IteratorState &iterator, bool &isStatic);
    static JSONValue &value(const IteratorState &iterator);
    static int distance(const IteratorState &x, const IteratorState &y);

private:
    JSONValueInternalLink *buckets_;
    JSONValueInternalLink *tailLink_;
    BucketIndex bucketsSize_;
    BucketIndex itemCount_;
};

/** \brief A simplified deque implementation used internally by JSONValue.
* \internal
* It is based on a list of fixed "page", each page contains a fixed number of items.
* Instead of using a linked-list, a array of pointer is used for fast item look-up.
* Look-up for an element is as follow:
* - compute page index: pageIndex = itemIndex / itemsPerPage
* - look-up item in page: pages_[pageIndex][itemIndex % itemsPerPage]
*
* Insertion is amortized constant time (only the array containing the index of pointers
* need to be reallocated when items are appended).
*/
class JSON_API JSONValueInternalArray {
    friend class JSONValue;
    friend class JSONValueIteratorBase;
public:
    enum {itemsPerPage=8};    // should be a power of 2 for fast divide and modulo.
    typedef JSONValue::ArrayIndex ArrayIndex;
    typedef unsigned int PageIndex;

# ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
    struct IteratorState { // Must be a POD
        IteratorState()
            : array_(0)
            , currentPageIndex_(0)
            , currentItemIndex_(0) {
        }
        JSONValueInternalArray *array_;
        JSONValue **currentPageIndex_;
        unsigned int currentItemIndex_;
    };
# endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

    JSONValueInternalArray();
    JSONValueInternalArray(const JSONValueInternalArray &other);
    JSONValueInternalArray &operator =(const JSONValueInternalArray &other);
    ~JSONValueInternalArray();
    void swap(JSONValueInternalArray &other);

    void clear();
    void resize(ArrayIndex newSize);

    JSONValue &resolveReference(ArrayIndex index);

    JSONValue *find(ArrayIndex index) const;

    ArrayIndex size() const;

    int compare(const JSONValueInternalArray &other) const;

private:
    static bool equals(const IteratorState &x, const IteratorState &other);
    static void increment(IteratorState &iterator);
    static void decrement(IteratorState &iterator);
    static JSONValue &dereference(const IteratorState &iterator);
    static JSONValue &unsafeDereference(const IteratorState &iterator);
    static int distance(const IteratorState &x, const IteratorState &y);
    static ArrayIndex indexOf(const IteratorState &iterator);
    void makeBeginIterator(IteratorState &it) const;
    void makeEndIterator(IteratorState &it) const;
    void makeIterator(IteratorState &it, ArrayIndex index) const;

    void makeIndexValid(ArrayIndex index);

    JSONValue **pages_;
    ArrayIndex size_;
    PageIndex pageCount_;
};

/** \brief Experimental: do not use. Allocator to customize JSONValue internal array.
 * Below is an example of a simple implementation (actual implementation use
 * memory pool).
   \code
class JSONDefaultValueArrayAllocator : public JSONValueArrayAllocator
{
public: // overridden from JSONValueArrayAllocator
virtual ~JSONDefaultValueArrayAllocator() {
}

virtual JSONValueInternalArray *newArray() {
   return new JSONValueInternalArray();
}

virtual JSONValueInternalArray *newArrayCopy(const JSONValueInternalArray &other) {
   return new JSONValueInternalArray( other );
}

virtual void destruct(JSONValueInternalArray *array)
{
   delete array;
}

virtual void reallocateArrayPageIndex(JSONValue **&indexes,
                                      JSONValueInternalArray::PageIndex &indexCount,
                                      JSONValueInternalArray::PageIndex minNewIndexCount)
{
   JSONValueInternalArray::PageIndex newIndexCount = (indexCount*3)/2 + 1;
   if ( minNewIndexCount > newIndexCount )
      newIndexCount = minNewIndexCount;
   void *newIndexes = realloc(indexes, sizeof(JSONValue*) * newIndexCount);
   if ( !newIndexes )
      throw std::bad_alloc();
   indexCount = newIndexCount;
   indexes = static_cast<JSONValue **>( newIndexes );
}
virtual void releaseArrayPageIndex(JSONValue **indexes,
                                   JSONValueInternalArray::PageIndex indexCount) {
   if (indexes)
      free(indexes);
}

virtual JSONValue *allocateArrayPage() {
   return static_cast<JSONValue *>(malloc(sizeof(JSONValue) * JSONValueInternalArray::itemsPerPage));
}

virtual void releaseArrayPage(JSONValue *value)
{
   if (value)
      free(value);
}
};
   \endcode
 */
class JSON_API JSONValueArrayAllocator {
public:
    virtual ~JSONValueArrayAllocator();
    virtual JSONValueInternalArray *newArray() = 0;
    virtual JSONValueInternalArray *newArrayCopy(const JSONValueInternalArray &other) = 0;
    virtual void destructArray(JSONValueInternalArray *array) = 0;
    /** \brief Reallocate array page index.
     * Reallocates an array of pointer on each page.
     * \param indexes [input] pointer on the current index. May be \c NULL.
     *                [output] pointer on the new index of at least
     *                         \a minNewIndexCount pages.
     * \param indexCount [input] current number of pages in the index.
     *                   [output] number of page the reallocated index can handle.
     *                            \b MUST be >= \a minNewIndexCount.
     * \param minNewIndexCount Minimum number of page the new index must be able to
     *                         handle.
     */
    virtual void reallocateArrayPageIndex(JSONValue **&indexes,
                                          JSONValueInternalArray::PageIndex &indexCount,
                                          JSONValueInternalArray::PageIndex minNewIndexCount) = 0;
    virtual void releaseArrayPageIndex(JSONValue **indexes,
                                       JSONValueInternalArray::PageIndex indexCount) = 0;
    virtual JSONValue *allocateArrayPage() = 0;
    virtual void releaseArrayPage(JSONValue *value) = 0;
};
#endif // #ifdef JSON_VALUE_USE_INTERNAL_MAP


/** \brief base class for JSONValue iterators.
 *
 */
class JSONValueIteratorBase {
public:
    typedef unsigned int size_t;
    typedef int difference_type;
    typedef JSONValueIteratorBase SelfType;

    JSONValueIteratorBase();
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    explicit JSONValueIteratorBase(const JSONValue::ObjectValues::iterator &current);
#else
    JSONValueIteratorBase(const JSONValueInternalArray::IteratorState &state);
    JSONValueIteratorBase(const JSONValueInternalMap::IteratorState &state);
#endif

    bool operator ==(const SelfType &other) const {
        return isEqual(other);
    }

    bool operator !=(const SelfType &other) const {
        return !isEqual(other);
    }

    difference_type operator -(const SelfType &other) const {
        return computeDistance(other);
    }

    /// Return either the index or the member name of the referenced value as a JSONValue.
    JSONValue key() const;

    /// Return the index of the referenced JSONValue. -1 if it is not an arrayValue.
    UInt index() const;

    /// Return the member name of the referenced JSONValue. "" if it is not an objectValue.
    const char *memberName() const;

protected:
    JSONValue &deref() const;

    void increment();

    void decrement();

    difference_type computeDistance(const SelfType &other) const;

    bool isEqual(const SelfType &other) const;

    void copy(const SelfType &other);

private:
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    JSONValue::ObjectValues::iterator current_;
    // Indicates that iterator is for a null value.
    bool isNull_;
#else
    union {
        JSONValueInternalArray::IteratorState array_;
        JSONValueInternalMap::IteratorState map_;
    } iterator_;
    bool isArray_;
#endif
};

/** \brief const iterator for object and array value.
 *
 */
class JSONValueConstIterator : public JSONValueIteratorBase {
    friend class JSONValue;
public:
    typedef unsigned int size_t;
    typedef int difference_type;
    typedef const JSONValue &reference;
    typedef const JSONValue *pointer;
    typedef JSONValueConstIterator SelfType;

    JSONValueConstIterator();
private:
    /*! \internal Use by JSONValue to create an iterator.
     */
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    explicit JSONValueConstIterator(const JSONValue::ObjectValues::iterator &current);
#else
    JSONValueConstIterator(const JSONValueInternalArray::IteratorState &state);
    JSONValueConstIterator(const JSONValueInternalMap::IteratorState &state);
#endif
public:
    SelfType &operator =(const JSONValueIteratorBase &other);

    SelfType operator++(int) {
        SelfType temp(*this);
        ++*this;
        return temp;
    }

    SelfType operator--(int) {
        SelfType temp(*this);
        --*this;
        return temp;
    }

    SelfType &operator--() {
        decrement();
        return *this;
    }

    SelfType &operator++() {
        increment();
        return *this;
    }

    reference operator *() const {
        return deref();
    }
};


/** \brief Iterator for object and array value.
 */
class JSONValueIterator : public JSONValueIteratorBase {
    friend class JSONValue;
public:
    typedef unsigned int size_t;
    typedef int difference_type;
    typedef JSONValue &reference;
    typedef JSONValue *pointer;
    typedef JSONValueIterator SelfType;

    JSONValueIterator();
    JSONValueIterator(const JSONValueConstIterator &other);
    JSONValueIterator(const JSONValueIterator &other);
private:
    /*! \internal Use by JSONValue to create an iterator.
     */
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    explicit JSONValueIterator(const JSONValue::ObjectValues::iterator &current);
#else
    JSONValueIterator(const JSONValueInternalArray::IteratorState &state);
    JSONValueIterator(const JSONValueInternalMap::IteratorState &state);
#endif
public:

    SelfType &operator =(const SelfType &other);

    SelfType operator++(int) {
        SelfType temp(*this);
        ++*this;
        return temp;
    }

    SelfType operator--(int) {
        SelfType temp(*this);
        --*this;
        return temp;
    }

    SelfType &operator--() {
        decrement();
        return *this;
    }

    SelfType &operator++() {
        increment();
        return *this;
    }

    reference operator *() const {
        return deref();
    }
};


/** \brief Configuration passed to reader and writer.
 * This configuration object can be used to force the JSONReader or JSONWriter
 * to behave in a standard conforming way.
 */
class JSON_API JSONFeatures {
public:
    /** \brief A configuration that allows all features and assumes all strings are UTF-8.
     * - C & C++ comments are allowed
     * - Root object can be any JSON value
     * - Assumes JSONValue strings are encoded in UTF-8
     */
    static JSONFeatures all();

    /** \brief A configuration that is strictly compatible with the JSON specification.
     * - Comments are forbidden.
     * - Root object must be either an array or an object value.
     * - Assumes JSONValue strings are encoded in UTF-8
     */
    static JSONFeatures strictMode();

    /** \brief Initialize the configuration like JsonConfig::allFeatures;
     */
    JSONFeatures();

    /// \c true if comments are allowed. Default: \c true.
    bool allowComments_;

    /// \c true if root must be either an array or an object value. Default: \c false.
    bool strictRoot_;
};


/** \brief Unserialize a <a HREF="http://www.json.org">JSON</a> document into a JSONValue.
 *
 */
class JSON_API JSONReader {
public:
    typedef char Char;
    typedef const Char *Location;

    /** \brief Constructs a JSONReader allowing all features
     * for parsing.
     */
    JSONReader();

    /** \brief Constructs a JSONReader allowing the specified feature set
     * for parsing.
     */
    JSONReader(const JSONFeatures &features);

    /** \brief Read a JSONValue from a <a HREF="http://www.json.org">JSON</a> document.
     * \param document UTF-8 encoded string containing the document to read.
     * \param root [out] Contains the root value of the document if it was
     *             successfully parsed.
     * \param collectComments \c true to collect comment and allow writing them back during
     *                        serialization, \c false to discard comments.
     *                        This parameter is ignored if JSONFeatures::allowComments_
     *                        is \c false.
     * \return \c true if the document was successfully parsed, \c false if an error occurred.
     */
    bool parse(const char *document,
               JSONValue &root,
               bool collectComments=true);

    /** \brief Read a JSONValue from a <a HREF="http://www.json.org">JSON</a> document.
     * \param document UTF-8 encoded string containing the document to read.
     * \param root [out] Contains the root value of the document if it was
     *             successfully parsed.
     * \param collectComments \c true to collect comment and allow writing them back during
     *                        serialization, \c false to discard comments.
     *                        This parameter is ignored if JSONFeatures::allowComments_
     *                        is \c false.
     * \return \c true if the document was successfully parsed, \c false if an error occurred.
     */
    bool parse(const String &document,
               JSONValue &root,
               bool collectComments=true);

    /** \brief Read a JSONValue from a <a HREF="http://www.json.org">JSON</a> document.
     * \param document UTF-8 encoded string containing the document to read.
     * \param root [out] Contains the root value of the document if it was
     *             successfully parsed.
     * \param collectComments \c true to collect comment and allow writing them back during
     *                        serialization, \c false to discard comments.
     *                        This parameter is ignored if JSONFeatures::allowComments_
     *                        is \c false.
     * \return \c true if the document was successfully parsed, \c false if an error occurred.
     */
    bool parse(const std::string &document,
               JSONValue &root,
               bool collectComments=true);

    /** \brief Read a JSONValue from a <a HREF="http://www.json.org">JSON</a> document.
     * \param document UTF-8 encoded string containing the document to read.
     * \param root [out] Contains the root value of the document if it was
     *             successfully parsed.
     * \param collectComments \c true to collect comment and allow writing them back during
     *                        serialization, \c false to discard comments.
     *                        This parameter is ignored if JSONFeatures::allowComments_
     *                        is \c false.
     * \return \c true if the document was successfully parsed, \c false if an error occurred.
     */
    bool parse(const char *beginDoc, const char *endDoc,
               JSONValue &root,
               bool collectComments=true);

    /// \brief Parse from input stream.
    /// \see apolloron::operator>>(std::istream&, apolloron::JSONValue&).
    bool parse(std::istream &is,
               JSONValue &root,
               bool collectComments=true);

    /** \brief Returns a user friendly string that list errors in the parsed document.
     * \return Formatted error message with the list of errors with their location in
     *         the parsed document. An empty string is returned if no error occurred
     *         during parsing.
     */
    String getFormatedErrorMessages() const;

private:
    enum TokenType {
        tokenEndOfStream = 0,
        tokenObjectBegin,
        tokenObjectEnd,
        tokenArrayBegin,
        tokenArrayEnd,
        tokenString,
        tokenNumber,
        tokenTrue,
        tokenFalse,
        tokenNull,
        tokenArraySeparator,
        tokenMemberSeparator,
        tokenComment,
        tokenError
    };

    class Token {
    public:
        TokenType type_;
        Location start_;
        Location end_;
    };

    class ErrorInfo {
    public:
        Token token_;
        std::string message_;
        Location extra_;
    };

    typedef std::deque<ErrorInfo> Errors;

    bool expectToken(TokenType type, Token &token, const char *message);
    bool readToken(Token &token);
    void skipSpaces();
    bool match(Location pattern,
               int patternLength);
    bool readComment();
    bool readCStyleComment();
    bool readCppStyleComment();
    bool readString();
    void readNumber();
    bool readValue();
    bool readObject(Token &token);
    bool readArray(Token &token);
    bool decodeNumber(Token &token);
    bool decodeString(Token &token);
    bool decodeString(Token &token, std::string &decoded);
    bool decodeDouble(Token &token);
    bool decodeUnicodeCodePoint(Token &token,
                                Location &current,
                                Location end,
                                unsigned int &unicode);
    bool decodeUnicodeEscapeSequence(Token &token,
                                     Location &current,
                                     Location end,
                                     unsigned int &unicode);
    bool addError(const std::string &message,
                  Token &token,
                  Location extra=0);
    bool recoverFromError(TokenType skipUntilToken);
    bool addErrorAndRecover(const std::string &message,
                            Token &token,
                            TokenType skipUntilToken);
    void skipUntilSpace();
    JSONValue &currentValue();
    Char getNextChar();
    void getLocationLineAndColumn(Location location,
                                  int &line,
                                  int &column) const;
    std::string getLocationLineAndColumn(Location location) const;
    void addComment(Location begin,
                    Location end,
                    CommentPlacement placement);
    void skipCommentTokens(Token &token);

    typedef std::stack<JSONValue *> Nodes;
    Nodes nodes_;
    Errors errors_;
    std::string document_;
    Location begin_;
    Location end_;
    Location current_;
    Location lastValueEnd_;
    JSONValue *lastValue_;
    std::string commentsBefore_;
    JSONFeatures features_;
    bool collectComments_;
};

/** \brief Read from 'sin' into 'root'.

 Always keep comments from the input JSON.

 This can be used to read a file into a particular sub-object.
 For example:
 \code
 apolloron::JSONValue root;
 cin >> root["dir"]["file"];
 cout << root;
 \endcode
 Result:
 \verbatim
 {
"dir": {
    "file": {
 // The input stream JSON would be nested here.
    }
}
 }
 \endverbatim
 \throw std::exception on parse error.
 \see apolloron::operator<<()
*/
std::istream& operator>>(std::istream&, JSONValue&);

class JSONValue;

/** \brief Abstract class for writers.
 */
class JSON_API JSONWriter {
public:
    virtual ~JSONWriter();

    virtual String write(const JSONValue &root) = 0;
};

/** \brief Outputs a JSONValue in <a HREF="http://www.json.org">JSON</a> format without formatting (not human friendly).
 *
 * The JSON document is written in a single line. It is not intended for 'human' consumption,
 * but may be usefull to support feature such as RPC where bandwith is limited.
 * \sa JSONReader, JSONValue
 */
class JSON_API JSONFastWriter : public JSONWriter {
public:
    JSONFastWriter();
    virtual ~JSONFastWriter() {}

    void enableYAMLCompatibility();

public: // overridden from Writer
    virtual String write(const JSONValue &root);

private:
    void writeValue(const JSONValue &value);

    std::string document_;
    bool yamlCompatiblityEnabled_;
};

/** \brief Writes a JSONValue in <a HREF="http://www.json.org">JSON</a> format in a human friendly way.
 *
 * The rules for line break and indent are as follow:
 * - Object value:
 *     - if empty then print {} without indent and line break
 *     - if not empty the print '{', line break & indent, print one value per line
 *       and then unindent and line break and print '}'.
 * - Array value:
 *     - if empty then print [] without indent and line break
 *     - if the array contains no object value, empty array or some other value types,
 *       and all the values fit on one lines, then print the array on a single line.
 *     - otherwise, it the values do not fit on one line, or the array contains
 *       object or non empty array, then print one value per line.
 *
 * If the JSONValue have comments then they are outputed according to their #CommentPlacement.
 *
 * \sa JSONReader, JSONValue, JSONValue::setComment()
 */
class JSON_API JSONStyledWriter: public JSONWriter {
public:
    JSONStyledWriter();
    virtual ~JSONStyledWriter() {}

public: // overridden from JSONWriter
    /** \brief Serialize a JSONValue in <a HREF="http://www.json.org">JSON</a> format.
     * \param root JSONValue to serialize.
     * \return String containing the JSON document that represents the root value.
     */
    virtual String write(const JSONValue &root);

private:
    void writeValue(const JSONValue &value);
    void writeArrayValue(const JSONValue &value);
    bool isMultineArray(const JSONValue &value);
    void pushValue(const String &value);
    void writeIndent();
    void writeWithIndent(const String &value);
    void indent();
    void unindent();
    void writeCommentBeforeValue(const JSONValue &root);
    void writeCommentAfterValueOnSameLine(const JSONValue &root);
    bool hasCommentForValue(const JSONValue &value);
    static std::string normalizeEOL(const std::string &text);

    typedef std::vector<std::string> ChildValues;

    ChildValues childValues_;
    std::string document_;
    std::string indentString_;
    int rightMargin_;
    int indentSize_;
    bool addChildValues_;
};

/** \brief Writes a JSONValue in <a HREF="http://www.json.org">JSON</a> format in a human friendly way,
     to a stream rather than to a string.
 *
 * The rules for line break and indent are as follow:
 * - Object value:
 *     - if empty then print {} without indent and line break
 *     - if not empty the print '{', line break & indent, print one value per line
 *       and then unindent and line break and print '}'.
 * - Array value:
 *     - if empty then print [] without indent and line break
 *     - if the array contains no object value, empty array or some other value types,
 *       and all the values fit on one lines, then print the array on a single line.
 *     - otherwise, it the values do not fit on one line, or the array contains
 *       object or non empty array, then print one value per line.
 *
 * If the JSONValue have comments then they are outputed according to their #CommentPlacement.
 *
 * \param indentation Each level will be indented by this amount extra.
 * \sa JSONReader, JSONValue, JSONValue::setComment()
 */
class JSON_API JSONStyledStreamWriter {
public:
    JSONStyledStreamWriter(String indentation="\t");
    ~JSONStyledStreamWriter() {}

public:
    /** \brief Serialize a JSONValue in <a HREF="http://www.json.org">JSON</a> format.
     * \param out Stream to write to. (Can be ostringstream, e.g.)
     * \param root JSONValue to serialize.
     * \note There is no point in deriving from Writer, since write() should not return a value.
     */
    void write(std::ostream &out, const JSONValue &root);

private:
    void writeValue(const JSONValue &value);
    void writeArrayValue(const JSONValue &value);
    bool isMultineArray(const JSONValue &value);
    void pushValue(const String &value);
    void writeIndent();
    void writeWithIndent(const String &value);
    void indent();
    void unindent();
    void writeCommentBeforeValue(const JSONValue &root);
    void writeCommentAfterValueOnSameLine(const JSONValue &root);
    bool hasCommentForValue(const JSONValue &value);
    static std::string normalizeEOL(const std::string &text);

    typedef std::vector<std::string> ChildValues;

    ChildValues childValues_;
    std::ostream* document_;
    std::string indentString_;
    int rightMargin_;
    std::string indentation_;
    bool addChildValues_;
};

String JSON_API valueToString(Int value);
String JSON_API valueToString(UInt value);
String JSON_API valueToString(double value);
String JSON_API valueToString(bool value);
String JSON_API valueToQuotedString(const char *value);

/// \brief Output using the JSONStyledStreamWriter.
/// \see apolloron::operator>>()
std::ostream& operator<<(std::ostream&, const JSONValue &root);
    

#ifndef _FCGIAPP_H
#define FCGI_FAIL_ACCEPT_ON_INTR 1

typedef struct FCGX_Stream {
    unsigned char *rdNext;    /* reader: first valid byte
                               * writer: equals stop */
    unsigned char *wrNext;    /* writer: first free byte
                               * reader: equals stop */
    unsigned char *stop;      /* reader: last valid byte + 1
                               * writer: last free byte + 1 */
    unsigned char *stopUnget; /* reader: first byte of current buffer
                               * fragment, for ungetc
                               *                                * writer: undefined */
    int isReader;
    int isClosed;
    int wasFCloseCalled;
    int FCGI_errno;                /* error status */
    void (*fillBuffProc)(struct FCGX_Stream *stream);
    void (*emptyBuffProc)(struct FCGX_Stream *stream, int doClose);
    void *data;
} FCGX_Stream;

typedef struct FCGX_Request {
    int requestId;            /* valid if isBeginProcessed */
    int role;
    FCGX_Stream *in;
    FCGX_Stream *out;
    FCGX_Stream *err;
    char **envp;

    /* Don't use anything below here */

    struct Params *paramsPtr;
    int ipcFd;               /* < 0 means no connection */
    int isBeginProcessed;     /* FCGI_BEGIN_REQUEST seen */
    int keepConnection;       /* don't close ipcFd at end of request */
    int appStatus;
    int nWriters;             /* number of open writers (0..2) */
    int flags;
    int listen_sock;
} FCGX_Request;
#endif


enum OpCode {
    CONTINUATION = 0x0,
    TEXT = 0x1,
    BINARY = 0x2,
    CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xA
};
class CGI;
/*----------------------------------------------------------------------------*/
/* WebSocket class                                                            */
/*----------------------------------------------------------------------------*/
/*! @brief WebSocket Class.
 */
class WebSocket {
protected:
    CGI* parent;
    bool isCGI;
    bool isWebSocket;
    bool maskingEnabled;
    String buffer; // receive buffer
    String fragmentedMessage; // store fragmented message
    OpCode fragmentedOpcode; // store opcode of fragmented message
    bool isFragmented; // true if we are in the middle of a fragmented message

    bool doWrite(const char* data, long length);
    bool doRead(char* data, long length);
    bool processFrame(const unsigned char* data, long length, String& message, OpCode& opcode, size_t& processed);
    String createFrame(const String& message, OpCode opcode);

public:
    WebSocket();
    virtual ~WebSocket();

    virtual void setParent(CGI* parent, bool isCGI);
    virtual bool handshake(const char* key);
    virtual bool send(const String& message, OpCode opcode = TEXT);
    virtual bool receive(String& message, OpCode& opcode);
    virtual bool close();
    virtual String generateAcceptKey(const char* key);
    virtual void onWebSocketMessage(const String& message, OpCode opcode);
};


/*----------------------------------------------------------------------------*/
/* CGI class                                                                  */
/*----------------------------------------------------------------------------*/
/*! @brief CGI Class.
 */
class CGI {
protected:
    Keys requestData; // CGI request values
    Keys requestFileName; // CGI request file names
    Keys cookie; // CGI request file names
    bool putHead; // put header done?
    List header; // header strings
    char primary_lang[100]; // primary language of getenv("HTTP_ACCEPT_LANGUAGE")
    JSONValue requestJSON;
    bool isWebSocketReq;
    WebSocket webSocket;
public:
    CGI(WebSocket *customWebSocket = NULL);
    virtual ~CGI();

    // Deletion of object instance
    virtual bool clear();

    virtual String& getValue(const String &key);
    virtual String& getValue(const char *key);
    virtual const JSONValue& getRequestJSON() const;
    virtual const char *getEnv(const String &key) const;
    virtual const char *getEnv(const char *key) const;
    virtual const char *getPrimaryLanguage() const;
    virtual String& getFileName(const String &key, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");
    virtual String& getFileName(const char *key, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");
    virtual String& getCookie(const char *key, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");

    virtual bool setHeader(const String &key, const String &value);
    virtual bool clearHeader();
    virtual bool putHeader();
    virtual bool putContent(const String &content, long size=-1);
    virtual bool putContent(const char *content, long size=-1);
    virtual bool putError(const String &err_str, long size=-1);
    virtual bool putError(const char *err_str, long size=-1);

    virtual bool isWebSocketRequest();
    virtual WebSocket& getWebSocket();
};


/*----------------------------------------------------------------------------*/
/* FCGI class                                                                 */
/*----------------------------------------------------------------------------*/
/*! @brief FCGI Class.
 */

int FCGX_Init(void);
int FCGX_InitRequest(FCGX_Request *request, int sock, int flags);
int FCGX_Accept_r(FCGX_Request *request);
void FCGX_Finish_r(FCGX_Request *request);

class FCGI : public CGI {
public:
    FCGX_Request *request; // TODO: canbe protected?
    FCGI(FCGX_Request *req, WebSocket *customWebSocket = NULL);
    virtual ~FCGI();

    // Deletion of object instance
    virtual bool clear();

    virtual String& getValue(const String &key);
    virtual String& getValue(const char *key);
    virtual const JSONValue& getRequestJSON() const;
    virtual const char *getEnv(const String &key) const;
    virtual const char *getEnv(const char *key) const;
    virtual const char *getPrimaryLanguage() const;
    virtual String& getFileName(const String &key, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");
    virtual String& getFileName(const char *key, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");
    virtual String& getCookie(const char *key, const char * src_charset="UTF-8", const char * dest_charset="UTF-8");

    virtual bool setHeader(const String &key, const String &value);
    virtual bool clearHeader();
    virtual bool putHeader();
    virtual bool putContent(const String &content, long size=-1);
    virtual bool putContent(const char *content, long size=-1);
    virtual bool putError(const String &err_str, long size=-1);
    virtual bool putError(const char *err_str, long size=-1);

    virtual bool isWebSocketRequest();
    virtual WebSocket& getWebSocket();
};


/*----------------------------------------------------------------------------*/
/* HTTPClient class                                                           */
/*----------------------------------------------------------------------------*/
/*! @brief Class of HTTP client
 */
class HTTPClient {
protected:
    String content;
    String origCharset;
public:
    HTTPClient();
    virtual ~HTTPClient();

    // Deletion of String instance
    virtual bool clear();

    // get content specified by URL
    virtual String& getURL(const String &url, long timeout=5);

    // get content original charset
    virtual String& getOrigCharset();
};


/*----------------------------------------------------------------------------*/
/* FTPStream class                                                           */
/*----------------------------------------------------------------------------*/
/*! @brief Class of FTP Stream
 */

typedef enum {UNIX=1, Windows_NT=2} TFTPSystemType;
typedef enum {ASCII=1, IMAGE=3} TFTPDataType;

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

class FTPStream {
protected:
    String host;
    String port;
    String user; // FTP auth ID
    String passwd; // FTP auth password
    bool loggedIn;
    LIBOFTP ftp_obj; // LIBOFTP object
    bool pasvMode;
    int timeout;
    String tmpString;
    Sheet tmpSheet;
public:
    FTPStream();
    virtual ~FTPStream();
    bool clear();

    // Timeout setup
    virtual bool setTimeout(int sec);

    virtual bool login(const String &user, const String &passwd,
                       const String &host, const String &port="21");
    virtual bool logout(); // QUIT and kill socket stream
    virtual bool isLoggedIn();
    virtual bool passive(bool pasv_mode=true);
    virtual bool isPassive();
    virtual bool reset();
    virtual bool noop();

    virtual bool site(const String &cmdline);
    virtual bool dele(const String &filename);
    virtual bool rename(const String &from, const String &to);

    virtual String &receiveBuffer(const String &filename);
    virtual bool sendBuffer(const String &buf, const String &filename);
    virtual bool appendBuffer(const String &buf, const String &filename);
    virtual bool receiveFile(const String &filename, const String &local_filename);
    virtual bool sendFile(const String &local_filename, const String &filename);
    virtual bool appendFile(const String &local_filename, const String &filename);

    virtual Sheet &list();
    virtual Sheet &nlist();
    virtual bool mkdir(const String &dirname);
    virtual bool mkdirp(const String &dirname);
    virtual bool rmdir(const String &dirname);
    virtual String &pwd();
    virtual bool cd(const String &dirname);
};


/*----------------------------------------------------------------------------*/
/* POP3Stream class                                                           */
/*----------------------------------------------------------------------------*/
/*! @brief Class of POP3 Stream
 */

class POP3Stream {
protected:
    String host;
    String port;
    String user; // POP3 auth ID
    String passwd; // POP3 auth password
    bool loggedIn;
    Socket socket; // POP3 stream
    String tmpString;
public:
    POP3Stream();
    virtual ~POP3Stream();
    bool clear();

    // Timeout setup
    virtual bool setTimeout(int sec);

    virtual bool login(const String &user, const String &passwd,
                       const String &host, const String &port="110",
                       bool apop=false, bool ssl=false);
    virtual bool logout(); // QUIT and kill socket stream
    virtual bool isLoggedIn(); // NOOP

    virtual bool stat(long &total_num, long &total_size); // STAT
    virtual bool size(List &size); // LIST
    virtual String& fetchHeader(long n); // TOP n 0
    virtual String& fetch(long n); // RETR n
    virtual String& uidl(long n); // UIDL n
    virtual bool dele(long n); // DELE n
};


/*----------------------------------------------------------------------------*/
/* IMAPStream class                                                           */
/*----------------------------------------------------------------------------*/
/*! @brief Class of IMAP Stream
 */

class IMAPStream {
protected:
    String host;
    String port;
    String user; // IMAP auth ID
    String passwd; // IMAP auth password
    bool loggedIn;
    String mailbox; // selected modified UTF-7 mailbox name
    bool readOnly; // select(false) or examine(true)
    String capability; // list of IMAP extentions
    Socket socket; // IMAP stream
    long nextTag;
    String tmpString;
public:
    IMAPStream();
    virtual ~IMAPStream();
    bool clear();

    // Timeout setup
    virtual bool setTimeout(int sec);

    virtual bool login(const String &user, const String &passwd,
                       const String &host, const String &port="143",
                       bool ssl=false);
    virtual bool logout(); // logout and kill socket stream
    virtual bool isLoggedIn() const;

    virtual bool select(const String &mailbox);
    virtual bool examine(const String &mailbox);
    virtual bool unselect();
    virtual const String& getCapability();
    virtual const String& getSelectedMailBox();
    virtual bool isReadOnly();

    virtual bool append(const String &eml, const String &internal_date="");
    virtual bool createMailBox(const String &mailbox);
    virtual bool deleteMailBox(const String &mailbox);

    virtual long getNextTag();

    virtual bool send(const String& line);
    virtual String& receiveLine();
    virtual String& receive(long size);
};


/*----------------------------------------------------------------------------*/
/* IMAPMailBoxList class                                                      */
/*----------------------------------------------------------------------------*/
/*! @brief Class of IMAP mail boxes
 */

class IMAPMailBoxList {
protected:
    IMAPStream *imapStream;
    List mailboxListUtf7; // modified UTF-7 original mailbox list
    List mailboxList; // UTF-8 mailbox list with '/' delimiter
    List namespaceDelimList; // namespace delimiter ('.', '/' or '\\')
    virtual bool setMailBoxList();
public:
    IMAPMailBoxList(IMAPStream *imap_stream);
    virtual ~IMAPMailBoxList();

    // Deletion of object instance
    virtual bool clear();

    // sync with IMAP (usually not needed, it's automatic)
    virtual bool reload();

    virtual const List &getMailBoxListUtf7();
    virtual const List &getMailBoxList();
    virtual const List &getNamespaceDelimList();

    virtual bool loadFile(const String &filename); // load from mailbox cache
    virtual bool saveFile(const String &filename); // save to mailbox cache
};


/*----------------------------------------------------------------------------*/
/* IMAPSearch class                                                         */
/*----------------------------------------------------------------------------*/
/*! @brief Class of IMAP mail list
 */

class IMAPSearch {
protected:
    IMAPStream *imapStream;
    String mailbox; // modified UTF-7 mailbox name
    String sortCondition;
    String searchCondition;
    String searchCharSet;
    long *uids; // NULL terminated list
public:
    IMAPSearch(IMAPStream *imap_stream, const String &mailbox,
               const String &sort_cond="REVERSE ARRIVAL",
               const String &search_cond="ALL",
               const String &search_charset="UTF-8");
    virtual ~IMAPSearch();

    // Deletion of object instance
    virtual bool clear();

    virtual bool setUIDs();
    virtual const long *getUIDs();

    virtual bool loadFile(const String &filename); // load from mail list cache
    virtual bool saveFile(const String &filename); // save to mail list cache
};


/*----------------------------------------------------------------------------*/
/* IMAPMail class                                                             */
/*----------------------------------------------------------------------------*/
/*! @brief Class of IMAP mail
 */

typedef struct {
    char* id; // ex. "1.2.1"
    char* filename; // UTF-8 file name
    char* contentType; // Content-Type header
    char* contentTransferEncoding; // Content-Transfer-Encoding header
    char* contentID; // Content-ID header
    long size;
} TMailPart;

typedef struct {
    char* internalDate;
    long size;
    char* headerDate;
    char* messageID;
    char* subject;
    char* contentType;
    int priority;
    List fromList;
    List senderList;
    List toList;
    List ccList;
    List bccList;
    List replyToList;
    List referenceList;
    List flagList;
    long partsLength;
    TMailPart *parts;
} TMail;

class IMAPMail {
protected:
    IMAPStream *imapStream;
    String mailbox; // modified UTF-7 mailbox name
    long uid;
    TMail mailData;
    List AttachFilePartIDList;
    String tmpString;
    virtual bool fetchMailStructure();

    virtual void parseEnvelope(const String& envelope);
    virtual String parseEnvelopeElement(const String& envelope, long *pos);
    virtual void parseAddressList(const String& envelope, long *pos, List& addressList);
    virtual void parseBodyStructure(const String& structure);
    virtual void countBodyParts(const String& structure, long *pos, long& count);
    virtual void parseBodyPart(const String& structure, long *pos, long& currentPart);
    virtual void parseHeaderFields(const String& headers);
    virtual long findMatchingParenthesis(const String& str, long start);

public:
    IMAPMail(IMAPStream *imap_stream, const String &mailbox, long uid);
    virtual ~IMAPMail();

    // Deletion of object instance
    virtual bool clear();

    virtual String& getEML(); // raw mail source
    virtual String& getHeader(); // raw header text
    virtual String& getInteralDate(); // IMAP INTERNALDATE
    virtual long getSize(); // IMAP RFC822.SIZE
    virtual String& getHeaderDate(); // MIME-decoded date header (UTF-8)
    virtual String& getMessageID(); // Message-ID
    virtual String& getSubject(); // UTF-8 Subject
    virtual List& getFromList(); // UTF-8 From list
    virtual List& getSenderList(); // UTF-8 Sender list
    virtual List& getToList(); // UTF-8 To list
    virtual List& getCcList(); // UTF-8 Cc list
    virtual List& getBccList(); // UTF-8 Bcc list
    virtual List& getReplyToList(); // UTF-8 Reply-To list
    virtual List& getReferenceList(); // References
    virtual List& getFlagList(); // UTF-8 Bcc list
    virtual String& getPlainTextBody(); // UTF-8 plain text body
    virtual String& getHTMLBody(); // UTF-8 HTML body
    virtual List& getAttachFilePartIDList();
    virtual String& getAttachFileNane(const String &part_id); // UTF-8 file name
    virtual String& getAttachFile(const String &part_id); // file content(binary)
    virtual const TMailPart& getMailPart(const String &part_id);
};


/*----------------------------------------------------------------------------*/
/* SMTPStream class                                                           */
/*----------------------------------------------------------------------------*/
/*! @brief Class of SMTP Stream
 */
class SMTPStream {
protected:
    String host;
    String port;
    Socket socket; // SMTP stream
    List responseList;
public:
    SMTPStream();
    virtual ~SMTPStream();

    // Deletion of String instance
    virtual bool clear();

    virtual bool setTimeout(int sec);

    virtual bool connect(const String &host, const String &port="25");
    virtual bool disconnect(); // send QUIT and kill socket stream

    virtual bool sendEHLO(const String &fqdn);
    virtual bool sendMailFrom(const String &mail_from);
    virtual bool sendRcptTo(const String &rcpt_to);
    virtual bool sendData(const String &eml);

    virtual const List& getResponseList() const;
};


/*----------------------------------------------------------------------------*/
/* Utility functions                                                          */
/*----------------------------------------------------------------------------*/
/*! @brief Utility functions.
 */
bool isFileExist(const String &filename);
bool isDirExist(const String &dirname);
bool isEmailAddress(const String &email);
bool isEmailLocalPart(const String &local_part);
bool isEmailDomain(const String &domain);


/*----------------------------------------------------------------------------*/
/* ANSI compatible functions                                                  */
/*----------------------------------------------------------------------------*/
/*! @brief ANSI clone functions.
 */
int itoa(char *buf, int value, int s=1, int base=10);
int ltoa(char *buf, long value, int s=1, int base=10);
int dtoa(char *buf, double value);
int dtoa_lang(char *buf, double value, int figure, const char *lang);
int mkdirp(const char *pathname, mode_t mode);
int rmrf(const char *pathname);

#if defined(__GNUC__)
#ifdef strcasestr
#undef strcasestr
#endif
#define strcasestr(x, y) ({ \
  const char *_strcasestr_pstr; \
  int _strcasestr_i; \
  char *_strcasestr_ret; \
  const char *_strcasestr_y = (y); \
  if (!*(x) || !(_strcasestr_y)) { \
    _strcasestr_ret = NULL; \
  } else if (!*(y)) { \
    _strcasestr_ret = (char *)(x); \
  } else { \
    _strcasestr_ret = NULL; \
    for (_strcasestr_pstr = (x); *_strcasestr_pstr; _strcasestr_pstr++) { \
      for (_strcasestr_i = 0; (y)[_strcasestr_i]; _strcasestr_i++) { \
        if (toupper(_strcasestr_pstr[_strcasestr_i]) != toupper((y)[_strcasestr_i])) break; \
      } \
      if (!((y)[_strcasestr_i])) { \
        _strcasestr_ret = (char *)_strcasestr_pstr; \
        break; \
      } \
    } \
  } \
  (_strcasestr_ret); \
})
#endif

#if defined(__GNUC__)
#ifdef strlen
#undef strlen
#endif
#define strlen(x) ({ \
  const char *_strlen_str; \
  int _strlen_len; \
  _strlen_str = (const char *)(x); \
  _strlen_len = 0; \
  while (_strlen_str[_strlen_len] != '\0') _strlen_len++; \
  (_strlen_len); \
})
#endif

#if defined(__GNUC__)
#ifdef strcpy
#undef strcpy
#endif
#define strcpy(x, y) ({ \
  char *_strcpy_dest, *_strcpy_save; \
  const char *_strcpy_src; \
  _strcpy_dest = (x); \
  _strcpy_src = (y); \
  _strcpy_save = _strcpy_dest; \
  for (; (*_strcpy_dest = *_strcpy_src) != '\0'; ++_strcpy_src, ++_strcpy_dest); \
  (_strcpy_save); \
})
#endif

#if defined(__GNUC__)
#ifdef strcmp
#undef strcmp
#endif
#define strcmp(x, y) ({ \
  const char *_strcmp_string1, *_strcmp_string2; \
  int _strcmp_first, _strcmp_second; \
  _strcmp_string1 = (x); \
  _strcmp_string2 = (y); \
  do { \
    _strcmp_first  = (int)((unsigned char)*_strcmp_string1); \
    _strcmp_second = (int)((unsigned char)*_strcmp_string2); \
    _strcmp_string1++; \
    _strcmp_string2++; \
  } while (_strcmp_first && _strcmp_first == _strcmp_second); \
  (_strcmp_first - _strcmp_second); \
})
#endif

#if defined(__GNUC__)
#ifdef strncmp
#undef strncmp
#endif
#define strncmp(x, y, z) ({ \
  const char *_strncmp_string1, *_strncmp_string2; \
  int _strncmp_first = 0, _strncmp_second = 0, _strncmp_n; \
  _strncmp_string1 = (x); \
  _strncmp_string2 = (y); \
  _strncmp_n = (z); \
  while (0 < _strncmp_n) { \
    _strncmp_first  = (int)((unsigned char)*_strncmp_string1); \
    _strncmp_second = (int)((unsigned char)*_strncmp_string2); \
    _strncmp_string1++; \
    _strncmp_string2++; \
    _strncmp_n--; \
    if (!(_strncmp_first && _strncmp_first == _strncmp_second)) break; \
  } \
  (_strncmp_first - _strncmp_second); \
})
#endif

#if defined(__GNUC__)
#ifdef strcasecmp
#undef strcasecmp
#endif
#define strcasecmp(x, y) ({ \
  const char *_strcasecmp_string1, *_strcasecmp_string2; \
  int _strcasecmp_first, _strcasecmp_second; \
  _strcasecmp_string1 = (x); \
  _strcasecmp_string2 = (y); \
  do { \
    _strcasecmp_first  = tolower(*_strcasecmp_string1); \
    _strcasecmp_second = tolower(*_strcasecmp_string2); \
    _strcasecmp_string1++; \
    _strcasecmp_string2++; \
  } while (_strcasecmp_first && _strcasecmp_first == _strcasecmp_second); \
  (_strcasecmp_first - _strcasecmp_second); \
})
#endif

#if defined(__GNUC__)
#ifdef strncasecmp
#undef strncasecmp
#endif
#define strncasecmp(x, y, z) ({ \
  const char *_strncasecmp_string1, *_strncasecmp_string2; \
  int _strncasecmp_n = (z); \
  int _strncasecmp_first = 0, _strncasecmp_second = 0; \
  _strncasecmp_string1 = (x); \
  _strncasecmp_string2 = (y); \
  while (0 < _strncasecmp_n) { \
    _strncasecmp_first  = tolower(*_strncasecmp_string1); \
    _strncasecmp_second = tolower(*_strncasecmp_string2); \
    _strncasecmp_string1++; \
    _strncasecmp_string2++; \
    _strncasecmp_n--; \
    if (!(_strncasecmp_first && _strncasecmp_first == _strncasecmp_second)) break; \
  } \
  (_strncasecmp_first - _strncasecmp_second); \
})
#endif

#if defined(__GNUC__)
#ifdef strcat
#undef strcat
#endif
#define strcat(x, y) ({ \
  char *_strcat_dest, *_strcat_save; \
  const char *_strcat_src; \
  _strcat_dest = (x); \
  _strcat_src = (y); \
  _strcat_save = _strcat_dest; \
  for (; *_strcat_dest != '\0'; ++_strcat_dest); \
  for (; (*_strcat_dest = *_strcat_src) != '\0'; ++_strcat_src, ++_strcat_dest); \
  (_strcat_save); \
})
#endif

/* ctype.h macros */
#ifndef _CTYPE_H  /* for Solaris */
#define _CTYPE_H
#endif
#ifndef _CTYPE_H_ /* for MacOSX */
#define _CTYPE_H_
#endif
#ifndef __H_CTYPE /* for AIX */
#define __H_CTYPE
#endif
#ifndef _CTYPE_INCLUDED /* for HP-UX */
#define _CTYPE_INCLUDED
#endif

#ifdef isalnum
#undef isalnum
#endif
#define isalnum(c)   (isalpha(c) || isdigit(c))

#ifdef isalpha
#undef isalpha
#endif
#define isalpha(c)   (isupper(c) || islower(c))

#ifdef isascii
#undef isascii
#endif
#define isascii(c)   (0 < ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)0x7F)

#ifdef iscntrl
#undef iscntrl
#endif
#define iscntrl(c)   (((unsigned char)(c)) <= (unsigned char)0x1F || ((unsigned char)(c)) == (unsigned char)0x7F)

#ifdef isdigit
#undef isdigit
#endif
#define isdigit(c)   ((unsigned char)'0' <= ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'9')

#ifdef isgraph
#undef isgraph
#endif
#define isgraph(c)   (((unsigned char)(c)) != (unsigned char)' ' && isprint(c))

#ifdef islower
#undef islower
#endif
#define islower(c)   ((unsigned char)'a' <= ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'z')

#ifdef isprint
#undef isprint
#endif
#define isprint(c)   ((unsigned char)' ' <= ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'~')

#ifdef ispunct
#undef ispunct
#endif
#define ispunct(c)   (((unsigned char)' ' < ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'~') && !isalnum(c))

#ifdef isspace
#undef isspace
#endif
#define isspace(c)   (((unsigned char)(c)) == (unsigned char)' ' || ((unsigned char)(c)) == (unsigned char)'\f' || ((unsigned char)(c)) == (unsigned char)'\n' || ((unsigned char)(c)) == (unsigned char)'\r' || ((unsigned char)(c)) == (unsigned char)'\t' || ((unsigned char)(c)) == (unsigned char)'\v')

#ifdef isblank
#undef isblank
#endif
#define isblank(c)   ((unsigned char)(c)) == (unsigned char)' ')

#ifdef isupper
#undef isupper
#endif
#define isupper(c)   ((unsigned char)'A' <= ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'Z')

#ifdef isxdigit
#undef isxdigit
#endif
#define isxdigit(c)  (isxupper(c) || isxlower(c))

#ifdef isxlower
#undef isxlower
#endif
#define isxlower(c)  (isdigit(c) || ((unsigned char)'a' <= ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'f'))

#ifdef isxupper
#undef isxupper
#endif
#define isxupper(c)  (isdigit(c) || ((unsigned char)'A' <= ((unsigned char)(c)) && ((unsigned char)(c)) <= (unsigned char)'F'))

#ifdef tolower
#undef tolower
#endif
#define tolower(c)   (isupper(c) ? (((unsigned char)(c)) - (unsigned char)'A' + (unsigned char)'a') : (c))

#ifdef toupper
#undef toupper
#endif
#define toupper(c)   (islower(c) ? (((unsigned char)(c)) - (unsigned char)'a' + (unsigned char)'A') : (c))

#ifdef _AIX
#undef LOCK_SH
#undef LOCK_EX
#undef LOCK_NB
#undef LOCK_UN
#endif
#if !defined(LOCK_SH) || defined(_AIX)
#define LOCK_SH F_LOCK   /* shared lock (for reading) */
#define LOCK_EX F_LOCK   /* exclusive lock (for writing) */
#define LOCK_NB 0        /* don't block when locking */
#define LOCK_UN F_ULOCK  /* unlock */
#define flock(fd, operation) lockf((int)(fd), (int)(operation), 0)
#endif

} // namespace apolloron

#endif
