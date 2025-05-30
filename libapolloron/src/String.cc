/******************************************************************************/
/*! @file String.cc
    @brief String class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

#include "apolloron.h"
#include "charset.h"
#include "strmidi.h"
#if __REGEX == 1
#include "regex.h"
#endif
#if __MD5 == 1
#include "md5.h"
#endif
#if __SHA1 == 1
#include "sha1.h"
#endif

#define STRING_ADD_SIZE 8192

namespace apolloron {

/*! Constructor of String.
    @param void
    @return void
 */
String::String() {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;
}


/*! Constructor of String
    @param value  Text for initialization.
    @return void
 */
String::String(const String &value) {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    (*this).set(value);
}


/*! Constructor of String
    @param value  Text for initialization.
    @return void
 */
String::String(const char *value) {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    (*this).set(value);
}


/*! Constructor of String
    @param value  Long integer for initialization.
    @return void
 */
String::String(long value) {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    (*this).set(value);
}


/*! Constructor of String
    @param value  Integer for initialization.
    @return void
 */
String::String(int value) {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    (*this).set(value);
}


/*! Constructor of String
    @param value  Integer for initialization.
    @return void
 */
String::String(char value) {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    (*this).set(value);
}


/*! Constructor of String
    @param value  Double float for initialization.
    @return void
 */
String::String(double value) {
    (*this).pText = NULL;
    (*this).pTmp = new (String **);
    *((*this).pTmp) = NULL;
    (*this).pTmpLen = new long;
    *((*this).pTmpLen) = 0;
    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    (*this).set(value);
}


/*! Destructor of String
    @param void
    @return void
 */
String::~String() {
    clear();
    if ((*this).pTmp != NULL) {
        delete [] (*this).pTmp;
        (*this).pTmp = NULL;
    }
    if ((*this).pTmpLen != NULL) {
        delete (*this).pTmpLen;
        (*this).pTmpLen = NULL;
    }
}


/*! Acquisition of a temporary String
    @param void
    @return Pointer of a temporary String.
 */
String* String::tmpStr() const {
    if (*((*this).pTmp) == NULL) {
        *((*this).pTmp) = new String * [1];
        **((*this).pTmp) = new String;
        *((*this).pTmpLen) = 1L;
    } else {
        String **ptmptmp;
        ptmptmp = new String * [(*((*this).pTmpLen)) + 1];
        memcpy(ptmptmp, *((*this).pTmp), sizeof(String *) * (*((*this).pTmpLen)));
        delete [] *((*this).pTmp);
        *((*this).pTmp) = ptmptmp;
        (*((*this).pTmp))[*((*this).pTmpLen)] = new String;
        (*((*this).pTmpLen))++;
    }
    return (*((*this).pTmp))[*((*this).pTmpLen) - 1];
}


/*! Delete temporary Strings currently held
    @param void
    @retval true  success
    @retval false failure
 */
bool String::clearTmpStr() const {
    if ((*this).pTmp != NULL) {
        if (*((*this).pTmp) != NULL) {
            if (**((*this).pTmp) != NULL) {
                long i;
                for (i = 0; i < *((*this).pTmpLen) ; i++) {
                    if ((*((*this).pTmp))[i] != NULL) {
                        delete ((*((*this).pTmp))[i]);
                    }
                }
                **pTmp = NULL;
            }
            delete [] *((*this).pTmp);
            *((*this).pTmp) = NULL;
        }
    }
    if ((*this).pTmpLen != NULL) {
        *((*this).pTmpLen) = 0;
    }
    return true;
}


/*! Check if String is empty (text mode)
    @param void
    @retval true  empty
    @retval false not empty
 */
bool String::empty() const {
    return (*this).nLength == 0;
}


/*! Delete instance of String
    @param void
    @retval true  success
    @retval false failure
 */
bool String::clear() {
    clearTmpStr();

    if ((*this).pText != NULL) {
        delete [] (*this).pText;
        (*this).pText = NULL;
    }

    (*this).nLength = 0;
    (*this).nCapacity = 0;
    (*this).nFixedLength = 0;
    (*this).nBinaryLength = -1;
    (*this).nLengthRenewRecommended = false;

    return true;
}


/*! Delete temporary Strings currently held
    @param void
    @retval true  success
    @retval false failure
 */
bool String::gc() const {
    return clearTmpStr();
}


/*! Replace pText with the pointer specified by the argument
    @param str         Pointer of text you want to replace by
    @param text_len    Length of str as text
    @param binary_len  Length of str as binary
    @param capacity    Allocated size of memory for str
    @retval true  success
    @retval false failure
 */
bool String::pTextReplace(char *str, long text_len, long binary_len, long capacity) {
    if ((*this).pText != NULL) {
        delete [] (*this).pText;
    }

    (*this).pText = str;

    if (0 <= text_len) {
        (*this).nLength = text_len;
        (*this).nLengthRenewRecommended = false;
    } else {
        (*this).nLengthRenewRecommended = true;
    }

    if (0 <= binary_len) {
        (*this).nFixedLength = binary_len;
        (*this).nBinaryLength = binary_len;
    } else {
        (*this).nFixedLength = (0 <= text_len)?text_len:-1;
        (*this).nBinaryLength = -1;
    }

    if (0 <= capacity) {
        (*this).nCapacity = capacity;
    } else if (0 <= binary_len) {
        (*this).nCapacity = binary_len + 1;
    } else {
        if ((*this).nLengthRenewRecommended == true) {
            (*this).nLength = strlen(str);
            (*this).nLengthRenewRecommended = false;
        }
        (*this).nCapacity = (*this).nLength + 1;
    }

    return true;
}


/*! Change the size of valiable.
    @param length  Length of Text.
    @retval true  success
    @retval false failure
 */
bool String::resize(long length) {
    long nCapacity_orig;
    char *tmp;

    if (length <= 0) {
        if ((*this).pText && 0 < (*this).nCapacity) {
            (*this).pText[0] = '\0';
        } else {
            if ((*this).pText) delete [] (*this).pText;
            (*this).nCapacity = 255L;
            (*this).pText = new char[(*this).nCapacity];
            (*this).pText[0] = '\0';
        }
        (*this).nLength = 0;
        (*this).nFixedLength = 0;
        (*this).nLengthRenewRecommended = false;
        return true;
    }

    nCapacity_orig = (*this).nCapacity;

    if ((*this).nCapacity < length + 1) {
        if ((*this).nCapacity) {
            (*this).nCapacity = length + 1 + (*this).nCapacity * 2;
        } else {
            (*this).nCapacity = length + 1;
        }
        if ((*this).nCapacity < 255L) (*this).nCapacity = 255L;
    }
    if ((*this).nCapacity <= (*this).nFixedLength) (*this).nCapacity = (*this).nFixedLength + 1;

    if (nCapacity_orig != (*this).nCapacity) {
        long len;
        tmp = new char[(*this).nCapacity];
        if (nCapacity_orig < (*this).nCapacity) {
            len = nCapacity_orig;
        } else {
            len = (*this).nCapacity - 1;
            if ((*this).nCapacity < (*this).nFixedLength) {
                (*this).nFixedLength = length;
            }
        }
        if ((*this).pText && 0L < len) {
            memcpy(tmp, (*this).pText, (nCapacity_orig < len)?nCapacity_orig:len);
            delete [] (*this).pText;
        }
        tmp[len] = '\0';
        (*this).pText = tmp;
    }

    if (nCapacity_orig < (*this).nFixedLength) {
        if ((*this).pText != NULL) {
            (*this).nLength = strlen((*this).pText);
        } else {
            (*this).nLength = 0;
        }
    } else {
        (*this).nLength = length;
    }

    (*this).nLengthRenewRecommended = false;

    return true;
}


/*! Renew nLength
    @param void
    @retval true  success
    @retval false failure
 */
bool String::nLengthRenew() {
    if ((*this).pText != NULL) {
        (*this).nLength = strlen((*this).pText);
    } else {
        (*this).nLength = 0;
    }
    (*this).nLengthRenewRecommended = false;
    return true;
}


/*! Set nLengthRenewRecommended (Call this when if size may become unfixed)
    @param void
    @retval true  success
    @retval false failure
 */
bool String::nLengthNeedRenew() {
    (*this).nLengthRenewRecommended = true;
    return true;
}


/*! Change the minimum size of variable
    @param size  Size for resize
    @retval true  success
    @retval false failure
 */
bool String::mresize(long size) {

    if (size < 0) {
        return false;
    }

    (*this).nFixedLength = size;
    if ((*this).nCapacity <= (*this).nFixedLength) {
        (*this).resize(size);
    }

    return true;
}


/*! Length of pText. (use pText as text mode)
    @param void
    @return Length of pText
 */
long String::len() const {
    long length;
    if ((*this).nLengthRenewRecommended || (*this).isBinary()) {
        if ((*this).pText != NULL) {
            length = strlen((*this).pText);
        } else {
            length = 0;
        }
    } else {
        length = (*this).nLength;
    }
    return length;
}


/*! Width of pText. (use pText as text mode)
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Length of pText
 */
long String::width(const char * src_charset) const {
    char *src_utf8;
    long width;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        width = utf8_width(src_utf8);
        delete [] src_utf8;
    } else {
        width = 0;
    }

    return width;
}


// Get capacity of variable
long String::capacity() const {
    return (*this).nCapacity;
}


// Get the minimum size of variable
long String::fixedLength() const {
    return (*this).nFixedLength;
}


// Change the minimum size of variable
bool String::setFixedLength(long size) {
    return (*this).mresize(size);
}


// Get the size of variable as binary
long String::binaryLength() const {
    return (*this).nBinaryLength;
}


// Set valiable mode binary (Reset binary size)
bool String::useAsBinary(long size) {

    if (size < 0) {
        return false;
    }

    if ((*this).nFixedLength <= size) {
        (*this).mresize(size + 1);
    }
    (*this).nBinaryLength = size;
    (*this).pText[size] = '\0';
    return true;
}


// Change to text mode
bool String::useAsText() {
    if (0 <= (*this).nBinaryLength) {
        (*this).pText[(*this).nBinaryLength] = '\0';
        (*this).nLengthRenewRecommended = true;
    }
    (*this).nBinaryLength = -1;
    return true;
}


// Is content text?
bool String::isText() const {
    if (0 <= (*this).nBinaryLength) {
        return false;
    } else {
        return true;
    }
}


// Is content binary?
bool String::isBinary() const {
    if (0 <= (*this).nBinaryLength) {
        return true;
    } else {
        return false;
    }
}


String::operator const char*() const {
    return (*this).c_str();
}


String::operator long() const {
    return (*this).toLong();
}


String::operator int() const {
    return (*this).toInt();
}


String::operator const char() const {
    return (*this).toChar();
}


String::operator double() const {
    return (*this).toDouble();
}


const char* String::c_str() const {
    if ((*this).pText == NULL) {
        return "";
    }

    return (*this).pText;
}


long String::toLong() const {
    if ((*this).pText == NULL) {
        return 0;
    }

    return atol((*this).pText);
}


int String::toInt() const {
    if ((*this).pText == NULL) {
        return 0;
    }

    return atoi((*this).pText);
}


char String::toChar() const {
    if ((*this).pText == NULL) {
        return (char)0;
    }

    return (*this).pText[0];
}


double String::toDouble() const {
    if ((*this).pText == NULL) {
        return 0.0;
    }

    return atof((*this).pText);
}


char& String::operator [] (long index) {
    static  char  tmp = 0;

    if (index < 0 || ((*this).nFixedLength <= index && (*this).nLength <= index)) {
        return tmp;
    }

    (*this).nLengthNeedRenew();

    return (*this).pText[index];
}


char& String::operator [] (int index) {
    static  char  tmp = 0;

    if (index < 0 || ((*this).nFixedLength <= index && (*this).nLength <= index)) {
        return tmp;
    }

    (*this).nLengthNeedRenew();

    return (*this).pText[index];
}


bool String::set(const String &value) {
    long length, fsize, max;

    length = value.len();
    fsize = value.fixedLength();
    max = (fsize < length)?length:fsize;

    // Memory allocation
    (*this).resize(max);
    (*this).setFixedLength(fsize);
    (*this).nBinaryLength = value.binaryLength();

    // Set value
    memcpy((*this).pText, value.c_str(), max + 1);
    (*this).nLength = length;
    (*this).nLengthRenewRecommended = false;

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::set(const char *value, long length) {
    long nlength;

    if (value != NULL) {
        if (length < 0L) {
            nlength = strlen(value);
        } else {
            nlength = length;
        }
    } else {
        nlength = 0L;
    }

    // Memory allocation
    (*this).resize(nlength);

    // Set value
    if (value != NULL && 0L < nlength) {
        memcpy((*this).pText, value, nlength);
        (*this).pText[nlength] = '\0';
    } else {
        (*this).pText[0] = '\0';
    }
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::set(long value) {
    char buf[32];
    int length;

    // Number --> Text
    length = ltoa(buf, value);

    // Memory allocation
    (*this).resize((long)length);

    // Set value
    strcpy((*this).pText, buf);
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::set(int value) {
    char buf[32];
    int length;

    // Number --> Text
    length = itoa(buf, value);

    // Memory allocation
    (*this).resize((long)length);

    // Set value
    strcpy((*this).pText, buf);
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::set(char value) {
    // Memory allocation
    (*this).resize(1);

    // Set value
    (*this).pText[0] = value;
    (*this).pText[1] = '\0';
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::set(double value) {
    char buf[64];
    long length;

    // Number --> Text
    length = dtoa(buf, (double)value);

    // Memory allocation
    (*this).resize(length);

    // Set value
    strcpy((*this).pText, buf);
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::setBinary(char value) {

    // Memory allocation
    (*this).useAsBinary(1);

    // Set value
    (*this).pText[0] = value;

    return true;
}


bool String::setBinary(const char *value, long length) {

    // Memory allocation
    (*this).useAsBinary(length);

    // Set value
    memcpy((*this).pText, value, length);

    return true;
}


int String::operator == (const String &value) const {
    return (*this).compare(value) ? 0 : 1;
}


int String::operator == (const char *value) const {
    return (*this).compare(value) ? 0 : 1;
}


int String::operator == (long value) const {
    return (*this).compare(value) ? 0 : 1;
}


int String::operator == (int value) const {
    return (*this).compare(value) ? 0 : 1;
}


int String::operator == (char value) const {
    return (*this).compare(value) ? 0 : 1;
}


int String::operator == (double value) const {
    return (*this).compare(value) ? 0 : 1;
}


int String::operator != (const String &value) const {
    return (*this).compare(value) ? 1 : 0;
}


int String::operator != (const char *value) const {
    return (*this).compare(value) ? 1 : 0;
}


int String::operator != (long value) const {
    return (*this).compare(value) ? 1 : 0;
}


int String::operator != (int value) const {
    return (*this).compare(value) ? 1 : 0;
}


int String::operator != (char value) const {
    return (*this).compare(value) ? 1 : 0;
}


int String::operator != (double value) const {
    return (*this).compare(value) ? 1 : 0;
}


int String::operator <= (const String &value) const {
    return (*this).compare(value) <= 0 ? 1 : 0;
}


int String::operator <= (const char *value) const {
    return (*this).compare(value) <= 0 ? 1 : 0;
}


int String::operator <= (long value) const {
    return (*this).compare(value) <= 0 ? 1 : 0;
}


int String::operator <= (int value) const {
    return (*this).compare(value) <= 0 ? 1 : 0;
}


int String::operator <= (char value) const {
    return (*this).compare(value) <= 0 ? 1 : 0;
}

int String::operator <= (double value) const {
    return (*this).compare(value) <= 0 ? 1 : 0;
}


int String::operator >= (const String &value) const {
    return (*this).compare(value) >= 0 ? 1 : 0;
}


int String::operator >= (const char *value) const {
    return (*this).compare(value) >= 0 ? 1 : 0;
}


int String::operator >= (long value) const {
    return (*this).compare(value) >= 0 ? 1 : 0;
}


int String::operator >= (int value) const {
    return (*this).compare(value) >= 0 ? 1 : 0;
}


int String::operator >= (char value) const {
    return (*this).compare(value) >= 0 ? 1 : 0;
}


int String::operator >= (double value) const {
    return (*this).compare(value) >= 0 ? 1 : 0;
}


int String::operator < (const String &value) const {
    return (*this).compare(value) < 0 ? 1 : 0;
}


int String::operator < (const char *value) const {
    return (*this).compare(value) < 0 ? 1 : 0;
}


int String::operator < (long value) const {
    return (*this).compare(value) < 0 ? 1 : 0;
}


int String::operator < (int value) const {
    return (*this).compare(value) < 0 ? 1 : 0;
}


int String::operator < (char value) const {
    return (*this).compare(value) < 0 ? 1 : 0;
}


int String::operator < (double value) const {
    return (*this).compare(value) < 0 ? 1 : 0;
}


int String::operator > (const String &value) const {
    return (*this).compare(value) > 0 ? 1 : 0;
}


int String::operator > (const char *value) const {
    return (*this).compare(value) > 0 ? 1 : 0;
}


int String::operator > (long value) const {
    return (*this).compare(value) > 0 ? 1 : 0;
}


int String::operator > (int value) const {
    return (*this).compare(value) > 0 ? 1 : 0;
}


int String::operator > (char value) const {
    return (*this).compare(value) > 0 ? 1 : 0;
}


int String::operator > (double value) const {
    return (*this).compare(value) > 0 ? 1 : 0;
}


const String& String::operator += (const String &value) {
    (*this).add(value);

    return *this;
}


const String& String::operator += (const char *value) {
    (*this).add(value);

    return *this;
}


const String& String::operator += (long value) {
    (*this).add(value);

    return *this;
}


const String& String::operator += (int value) {
    (*this).add(value);

    return *this;
}


const String& String::operator += (char value) {
    (*this).add(value);

    return *this;
}


const String& String::operator += (double value) {
    (*this).add(value);

    return *this;
}


String& String::operator + (const String &value) const {
    String *tmp = (*this).tmpStr();

    (*tmp).set(*this);
    (*tmp).add(value);

    return *tmp;
}


String& String::operator + (const char *value) const {
    String *tmp = (*this).tmpStr();

    (*tmp).set(*this);
    (*tmp).add(value);

    return *tmp;
}


String& String::operator + (long value) const {
    String *tmp = (*this).tmpStr();

    (*tmp).set(*this);
    (*tmp).add(value);

    return *tmp;
}


String& String::operator + (int value) const {
    String *tmp = (*this).tmpStr();

    (*tmp).set(*this);
    (*tmp).add(value);

    return *tmp;
}

String& String::operator + (char value) const {
    String *tmp = (*this).tmpStr();

    (*tmp).set(*this);
    (*tmp).add(value);

    return *tmp;
}


String& String::operator + (double value) const {
    String *tmp = (*this).tmpStr();

    (*tmp).set(*this);
    (*tmp).add(value);

    return *tmp;
}


const String& String::operator = (const String &value) {
    set(value);

    return *this;
}


const String& String::operator = (const char *value) {
    set(value);

    return *this;
}


const String& String::operator = (long value) {
    set(value);

    return *this;
}


const String& String::operator = (int value) {
    set(value);

    return *this;
}


const String& String::operator = (char value) {
    set(value);

    return *this;
}


const String& String::operator = (double value) {
    set(value);

    return *this;
}


int String::compare(const String &value) const {
    const char  *t1 = (*this).c_str();
    const char  *t2 = value.c_str();

    return strcmp(t1, t2);
}


int String::compare(const char *value) const {
    const char  *t1 = (*this).c_str();
    const char  *t2 = value;

    return strcmp(t1, t2);
}


int String::compare(long value) const {
    String *tmp = (*this).tmpStr();
    (*tmp) = value;
    const char  *t1 = (*this).c_str();
    const char  *t2 = (*tmp).c_str();

    return strcmp(t1, t2);
}


int String::compare(int value) const {
    String *tmp = (*this).tmpStr();
    (*tmp) = value;
    const char  *t1 = (*this).c_str();
    const char  *t2 = (*tmp).c_str();

    return strcmp(t1, t2);
}


int String::compare(char value) const {
    String *tmp = (*this).tmpStr();
    (*tmp) = value;
    const char  *t1 = (*this).c_str();
    const char  *t2 = (*tmp).c_str();

    return strcmp(t1, t2);
}


int String::compare(double value) const {
    String *tmp = (*this).tmpStr();
    (*tmp) = value;
    const char  *t1 = (*this).c_str();
    const char  *t2 = (*tmp).c_str();

    return strcmp(t1, t2);
}


bool String::add(const String &value) {
    long lenTotal;
    long lenPrev;
    bool ret = true;

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value);
    }

    if ((*this).isText() || value.isText()) {
        lenPrev = (*this).len();
        lenTotal = lenPrev + value.len();

        // Memory allocation
        if ((*this).nCapacity <= lenTotal) {
            ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
        }
        (*this).nLength = lenTotal;
        (*this).nLengthRenewRecommended = false;

        if (0 < lenTotal) {
            strcpy(&(*this).pText[lenPrev], value.c_str());
        }
        (*this).useAsText();
    } else {
        lenTotal = (*this).nBinaryLength + value.binaryLength();
        lenPrev = (*this).nBinaryLength;

        // Memory allocation
        if ((*this).nCapacity <= lenTotal) {
            ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
        }

        memcpy(&(*this).pText[lenPrev], value.c_str(), lenTotal - lenPrev);
        (*this).pText[lenTotal] = '\0';
        (*this).useAsBinary(lenTotal);
        (*this).nLengthRenew();
    }

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(const char *value) {
    long lenTotal;
    long lenPrev;
    bool ret=true;

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value);
    }

    lenPrev = (*this).len();
    lenTotal = lenPrev + strlen(value);

    // Memory allocation
    if ((*this).nCapacity <= lenTotal) {
        ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
    }
    (*this).nLength = lenTotal;
    (*this).nLengthRenewRecommended = false;

    strcpy(&((*this).pText[lenPrev]), value);
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(const String &value, long length) {
    long lenTotal;
    long lenPrev;
    bool ret = true;

    if (length < 0) {
        length = value.len();
    } else if (length == 0) {
        return true;
    }

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value.c_str(), length);
    }

    if ((*this).isText() || value.isText()) {
        if (value.len() < length) {
            length = value.len();
        }

        lenPrev = (*this).len();
        lenTotal = lenPrev + length;

        // Memory allocation
        if ((*this).nCapacity <= lenTotal) {
            ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
        }
        (*this).nLength = lenTotal;
        (*this).nLengthRenewRecommended = false;

        if (0 < lenTotal) {
            memcpy(&(*this).pText[lenPrev], value.c_str(), length);
            (*this).pText[lenPrev + length] = '\0';
        }
        (*this).useAsText();
    } else {
        if (value.binaryLength() < length) {
            length = value.binaryLength();
        }

        lenTotal = (*this).nBinaryLength + length;
        lenPrev = (*this).nBinaryLength;

        // Memory allocation
        if ((*this).nCapacity <= lenTotal) {
            ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
        }

        memcpy(&(*this).pText[lenPrev], value.c_str(), length);
        (*this).pText[lenTotal] = '\0';
        (*this).useAsBinary(lenTotal);
        (*this).nLengthRenew();
    }

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(const char *value, long length) {
    long lenTotal;
    long lenPrev;
    long len;
    bool ret = true;

    if (value == NULL) {
    } else if (length < 0) {
        length = strlen(value);
    } else if (length == 0) {
        return true;
    } else {
        len = strlen(value);
        if (len < length) {
            length = len;
        }
    }

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value, length);
    }

    lenPrev = (*this).len();
    lenTotal = lenPrev + length;

    // Memory allocation
    if ((*this).nCapacity <= lenTotal) {
        ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
    }
    (*this).nLength = lenTotal;
    (*this).nLengthRenewRecommended = false;

    memcpy(&((*this).pText[lenPrev]), value, length);
    (*this).pText[lenTotal] = '\0';
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(long value) {
    long lenTotal;
    long lenPrev;
    bool ret = true;

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value);
    }

    lenPrev = (*this).len();
    lenTotal = lenPrev + 16;

    // Memory allocation
    if ((*this).nCapacity <= lenTotal) {
        ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
    }

    ltoa(&((*this).pText[lenPrev]), value);
    (*this).nLengthRenew();
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(int value) {
    long lenTotal;
    long lenPrev;
    bool ret = true;

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value);
    }

    lenPrev = (*this).len();
    lenTotal = lenPrev + 16;

    // Memory allocation
    if ((*this).nCapacity <= lenTotal) {
        ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
    }

    itoa(&((*this).pText[lenPrev]), value);
    (*this).nLengthRenew();
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(char value) {
    long lenTotal;
    long lenPrev;
    bool ret = true;

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value);
    }

    lenPrev = (*this).len();
    lenTotal = lenPrev + 1;

    // Memory allocation
    if ((*this).nCapacity <= lenTotal) {
        ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
    }

    (*this).pText[lenPrev] = value;
    (*this).pText[lenPrev + 1] = '\0';
    (*this).nLengthRenew();
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::add(double value) {
    long lenTotal;
    long lenPrev;
    bool ret = true;

    if ((*this).nLengthRenewRecommended) {
        (*this).nLengthRenew();
    }

    if ((*this).pText == NULL) {
        return (*this).set(value);
    }

    lenPrev = (*this).len();
    lenTotal = lenPrev + 64;

    // Memory allocation
    if ((*this).nCapacity <= lenTotal) {
        ret = (*this).resize(lenTotal + STRING_ADD_SIZE);
    }

    dtoa(&((*this).pText[lenPrev]), (double)value);
    (*this).nLengthRenew();
    (*this).useAsText();

    // Deletion of temporary String
    (*this).clearTmpStr();

    return ret;
}


bool String::addBinary(char value) {
    long lenPrev;

    if ((*this).isText()) {
        lenPrev = (*this).len();
    } else {
        lenPrev = (*this).binaryLength();
    }

    // Memory allocation
    (*this).useAsBinary(lenPrev + 1);

    // Set value
    (*this).pText[lenPrev] = value;
    (*this).pText[lenPrev + 1] = '\0';

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


bool String::addBinary(const char *value, long length) {
    long lenPrev;

    if ((*this).isText()) {
        lenPrev = (*this).len();
    } else {
        lenPrev = (*this).binaryLength();
    }

    // Memory allocation
    (*this).useAsBinary(lenPrev + length);

    // Set value
    memcpy((*this).pText + lenPrev, value, length);
    (*this).pText[lenPrev + length] = '\0';

    // Deletion of temporary String
    (*this).clearTmpStr();

    return true;
}


/*! get sub String (left)
    @param length sub String length (max or -1 for all)
    @return sub String
 */
String& String::left(long length) const {
    String *tmp = (*this).tmpStr();
    long i, l;

    l = (*this).len();
    l = (l < length)?l:length;

    (*tmp).setFixedLength(l + 1);
    i = 0;
    while (i < l) {
        (*tmp).pText[i] = (*this).pText[i];
        i++;
    }
    (*tmp).pText[l] = '\0';
    (*tmp).nLength = l;
    (*tmp).setFixedLength(0);

    return *tmp;
}


/*! get sub String (right)
    @param length sub String length (max or -1 for all)
    @return sub String
 */
String& String::right(long length) const {
    String *tmp = (*this).tmpStr();
    long i, j, l;

    l = (*this).len();
    l = (l < length)?l:length;

    j = (*this).len() - l;

    (*tmp).setFixedLength(l + 1);
    i = 0;
    while (i < l) {
        (*tmp).pText[i] = (*this).pText[j++];
        i++;
    }
    (*tmp).pText[i] = '\0';
    (*tmp).nLength = l;
    (*tmp).setFixedLength(0);

    return *tmp;
}


/*! get sub String
    @param pos start position
    @param length sub String length (max or -1 for all)
    @return sub String
 */
String& String::mid(long pos, long length) const {
    String *tmp = (*this).tmpStr();
    long i, j, l, p;

    *tmp = "";
    l = (*this).len();
    p = (0 <= pos)?pos:0;
    if (length == 0 || l <= p) {
        return *tmp;
    } else if (length < 0) {
        length = l - p;
    }

    (*tmp).setFixedLength(length + 1);
    i = 0;
    j = p;
    while (i < length && (*this).pText[j] != '\0') {
        (*tmp).pText[i] = (*this).pText[j];
        j++;
        i++;
    }
    (*tmp).pText[i] = '\0';
    (*tmp).nLength = i;
    (*tmp).nLengthRenewRecommended = false;
    (*tmp).setFixedLength(0);

    return *tmp;
}


/*! get trimmed String (right)
    @param void
    @return trimmed String
 */
String& String::trimR() const {
    String *tmp = (*this).tmpStr();
    long i, length;

    (*tmp).useAsText();
    length = (*this).len();
    if (length <= 0) {
        *tmp = "";
        return *tmp;
    }

    for (i = length - 1; 0 <= i; i--) {
        if (!isspace((*this).pText[i])) {
            *tmp = (*this).left(i + 1);
            return *tmp;
        }
    }

    return *tmp;
}


/*! get trimmed String (left)
    @param void
    @return trimmed String
 */
String& String::trimL() const {
    String *tmp = (*this).tmpStr();
    long i, length;

    (*tmp).useAsText();
    length = (*this).len();
    for (i = 0; i < length; i++) {
        if (!isspace((*this).pText[i])) {
            *tmp = (*this).right(length - i);
            return *tmp;
        }
    }

    return *tmp;
}


/*! get trimmed String
    @param void
    @return trimmed String
 */
String& String::trim() const {
    return (*this).trimR().trimL();
}


/*! get upper conversion
    @param void
    @return upper String
 */
String& String::upper() const {
    String *tmp = (*this).tmpStr();
    long  i, c, length;

    *tmp = "";
    length = (*this).len();
    (*tmp).setFixedLength(length + 1);
    for (i = 0; i < length; i++) {
        c = (*this).pText[i];
        if ('a' <= c && c <= 'z') {
            (*tmp).pText[i] = 'A' + (c - 'a');
        } else {
            (*tmp).pText[i] = c;
        }
    }
    (*tmp).pText[i] = '\0';
    (*tmp).resize(i);
    (*tmp).setFixedLength(0);
    return *tmp;
}


/*! get lower String
    @param void
    @return lower String
 */
String& String::lower() const {
    String *tmp = (*this).tmpStr();
    int  i, c, length;

    *tmp = "";
    length = (*this).len();
    (*tmp).setFixedLength(length + 1);
    for (i = 0; i < length; i++) {
        c = (*this).pText[i];
        if ('A' <= c && c <= 'Z') {
            (*tmp).pText[i] = 'a' + (c - 'A');
        } else {
            (*tmp).pText[i] = c;
        }
    }
    (*tmp).pText[i] = '\0';
    (*tmp).resize(i);
    (*tmp).setFixedLength(0);
    return *tmp;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::search(const String &key, long pos) const {
    long count, i, length;

    length = key.len();
    count = (*this).len() - length + 1;
    if (count < 0) return -1;

    for (i = pos; i < count; i++) {
        if (memcmp(&((*this).pText[i]), key.pText, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::search(const char *key, long pos) const {
    long count, i, length;

    length = strlen(key);
    count = (*this).len() - length + 1;
    if (count < 0) return -1;

    for (i = pos; i < count; i++) {
        if (memcmp(&((*this).pText[i]), key, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchCase(const String &key, long pos) const {
    long count, i, length;

    length = key.len();
    count = (*this).len() - length + 1;
    if (count < 0) return -1;

    for (i = pos; i < count; i++) {
        if (strncasecmp(&((*this).pText[i]), key.pText, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchCase(const char *key, long pos) const {
    long count, i, length;

    length = strlen(key);
    count = (*this).len() - length + 1;
    if (count < 0) return -1;

    for (i = pos; i < count; i++) {
        if (strncasecmp(&((*this).pText[i]), key, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of character
    @param key search key (char)
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchChar(const char c, long pos) const {
    long i, length;

    length = (*this).len();
    for (i = pos; i < length; i++) {
        if ((*this).pText[i] == c) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchR(const String &key, long pos) const {
    long count, i, length;

    length = key.len();
    count = (*this).len() - length;
    if (count < 0) return -1;

    for (i = (pos < 0)?count:pos; 0 <= i; i--) {
        if (memcmp(&((*this).pText[i]), key.pText, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchR(const char *key, long pos) const {
    long count, i, length;

    length = strlen(key);
    count = (*this).len() - length;
    if (count < 0) return -1;

    for (i = (pos < 0)?count:pos; 0 <= i; i--) {
        if (memcmp(&((*this).pText[i]), key, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchCaseR(const String &key, long pos) const {
    long count, i, length;

    length = key.len();
    count = (*this).len() - length;
    if (count < 0) return -1;

    for (i = (pos < 0)?count:pos; 0 <= i; i--) {
        if (strncasecmp(&((*this).pText[i]), key.pText, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of word
    @param key search key
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchCaseR(const char *key, long pos) const {
    long count, i, length;

    length = strlen(key);
    count = (*this).len() - length;
    if (count < 0) return -1;

    for (i = (pos < 0)?count:pos; 0 <= i; i--) {
        if (strncasecmp(&((*this).pText[i]), key, length) == 0) {
            return i;
        }
    }

    return -1;
}


/*! Search of character
    @param key search key (char)
    @param pos find start position
    @return finded position (-1 if error)
 */
long String::searchCharR(const char c, long pos) const {
    long i, length;

    length = (*this).len();
    for (i = (pos < 0)?length:pos; 0 <= i; i--) {
        if ((*this).pText[i] == c) {
            return i;
        }
    }

    return -1;
}


/*! Replace of word
    @param key search key
    @param replace_key replace word
    @return replaced String
 */
String& String::replace(const String &key, const String &replace_str, long pos) const {
    String *tmp = (*this).tmpStr();
    String s;
    long i, j, length, key_length;

    s = (*this);
    length = s.len();
    key_length = key.len();
    *tmp = s.left(pos);
    for (i = pos; i <= length; i++) {
        j = s.search(key, i);
        if (0 <= j) {
            (*tmp) += s.mid(i, j-i);
            (*tmp) += replace_str;
            i = j + key_length - 1;
        } else {
            (*tmp) += s.mid(i);
            break;
        }
    }
    s.clear();

    return *tmp;
}


/*! Replace of word (key case ignore)
    @param key search key
    @param replace_key replace word
    @return replaced String
 */
String& String::replaceCase(const String &key, const String &replace_str, long pos) const {
    String *tmp = (*this).tmpStr();
    String s;
    long i, j, length, key_length;

    s = (*this);
    length = s.len();
    key_length = key.len();
    *tmp = s.left(pos);
    for (i = pos; i <= length; i++) {
        j = s.searchCase(key, i);
        if (0 <= j) {
            (*tmp) += s.mid(i, j-i);
            (*tmp) += replace_str;
            i = j + key_length - 1;
        } else {
            (*tmp) += s.mid(i);
            break;
        }
    }
    s.clear();

    return *tmp;
}


String& String::insert(long pos, const String &value) const {
    String *tmp = (*this).tmpStr();
    long length, value_length;

    length = (*this).len();
    value_length = value.len();

    if (length <= pos) {
        *tmp = (*this);
        (*tmp).add(value);
    } else if (pos <= 0) {
        *tmp = value;
        (*tmp).add(*this);
    } else {
        char *str;
        str = new char[length + value_length + 1];
        memcpy(str, (*this).pText, pos);
        if (0 < value_length) {
            memcpy(&(str[pos]), value.c_str(), value_length);
        }
        memcpy(&(str[pos + value_length]), &((*this).pText[pos]), length - pos);
        str[length + value_length] = '\0';
        (*tmp).pTextReplace(str, length + value_length, length + value_length + 1, length + value_length + 1);
    }

    return *tmp;
}


String& String::insert(long pos, const char *value) const {
    String *tmp = (*this).tmpStr();
    long length, value_length;

    length = (*this).len();
    if (value != NULL) {
        value_length = strlen(value);
    } else {
        value_length = 0;
    }

    if (length <= pos) {
        *tmp = (*this);
        if (0 < value_length) {
            (*tmp).add(value);
        }
    } else if (pos <= 0) {
        *tmp = value;
        (*tmp).add(*this);
    } else {
        char *str;
        str = new char[length + value_length + 1];
        memcpy(str, (*this).pText, pos);
        if (0 < value_length) {
            memcpy(&(str[pos]), value, value_length);
        }
        memcpy(&(str[pos + length]), &((*this).pText[pos]), length - pos);
        str[length + value_length] = '\0';
        (*tmp).pTextReplace(str, length + value_length, length + value_length + 1, length + value_length + 1);
    }

    return *tmp;
}


String& String::insert(long pos, long value) const {
    String *tmp = (*this).tmpStr();
    char value_txt[32];
    long length, value_length;

    value_length = ltoa(value_txt, value);
    length = (*this).len();

    if (length <= pos) {
        *tmp = (*this);
        (*tmp).add(value_txt);
    } else if (pos <= 0) {
        *tmp = value_txt;
        (*tmp).add(*this);
    } else {
        char *str;
        str = new char [length + value_length + 1];
        memcpy(str, (*this).pText, pos);
        if (0 < value_length) {
            memcpy(&(str[pos]), value_txt, value_length);
        }
        memcpy(&(str[pos + length]), &((*this).pText[pos]), length - pos);
        str[length + value_length] = '\0';
        (*tmp).pTextReplace(str, length + value_length, length + value_length + 1, length + value_length + 1);
    }

    return *tmp;
}


String& String::insert(long pos, int value) const {
    String *tmp = (*this).tmpStr();
    char value_txt[32];
    long length, value_length;

    value_length = itoa(value_txt, value);
    length = (*this).len();

    if (length <= pos) {
        *tmp = (*this);
        (*tmp).add(value_txt);
    } else if (pos <= 0) {
        *tmp = value_txt;
        (*tmp).add(*this);
    } else {
        char *str;
        str = new char [length + value_length + 1];
        memcpy(str, (*this).pText, pos);
        if (0 < value_length) {
            memcpy(&(str[pos]), value_txt, value_length);
        }
        memcpy(&(str[pos + length]), &((*this).pText[pos]), length - pos);
        str[length + value_length] = '\0';
        (*tmp).pTextReplace(str, length + value_length, length + value_length + 1, length + value_length + 1);
    }

    return *tmp;
}


String& String::insert(long pos, char value) const {
    String *tmp = (*this).tmpStr();
    long length;

    length = (*this).len();

    if (length <= pos) {
        *tmp = (*this);
        (*tmp).add(value);
    } else if (pos <= 0) {
        *tmp = value;
        (*tmp).add(*this);
    } else {
        char *str;
        str = new char [length + 2];
        memcpy(str, (*this).pText, pos);
        str[pos] = value;
        memcpy(&(str[pos + length]), &((*this).pText[pos]), length - pos);
        str[length + 1] = '\0';
        (*tmp).pTextReplace(str, length + 1, length + 2, length + 2);
    }

    return *tmp;
}


String& String::insert(long pos, double value) const {
    String *tmp = (*this).tmpStr();
    char value_txt[64];
    long length, value_length;

    value_length = dtoa(value_txt, (double)value);
    length = (*this).len();

    if (length <= pos) {
        *tmp = (*this);
        (*tmp).add(value_txt);
    } else if (pos <= 0) {
        *tmp = value_txt;
        (*tmp).add(*this);
    } else {
        char *str;
        str = new char [length + value_length + 1];
        memcpy(str, (*this).pText, pos);
        if (0 < value_length) {
            memcpy(&(str[pos]), value_txt, value_length);
        }
        memcpy(&(str[pos + length]), &((*this).pText[pos]), length - pos);
        str[length + value_length] = '\0';
        (*tmp).pTextReplace(str, length + value_length, length + value_length + 1, length + value_length + 1);
    }

    return *tmp;
}


int String::sprintf(const char *format, ...) {
    char *buf;
    long i, format_len;
    const char *p;
    const String *ps;
    va_list ap;
    long size;

    (*this).set("");

    if (format) {
        // Memory allocation
        format_len = strlen(format);
        buf = new char[format_len + 65];

        // Format
        va_start(ap, format);
        i = 0;
        while (i < format_len) {
            size = strcspn(&format[i], "%%");
            if (0 < size) {
                memcpy(buf, &format[i], size);
                buf[size] = '\0';
                (*this).add(buf);
                i += size-1;
            } else if (format[i] == '%') {
                if (format_len <= i+1) {
                    (*this).add("%%");
                } else switch (format[i+1]) {
                        case '%':
                            (*this).add("%%");
                            i++;
                            break;
                        case 'S':
                            ps = (const String *)va_arg(ap, const String *);
                            size = (*ps).len();
                            if (0 < size) {
                                (*this).add(*ps);
                            }
                            i++;
                            break;
                        case 's':
                            p = (const char *)va_arg(ap, const char *);
                            size = strlen((char *)p);
                            if (0 < size) {
                                (*this).add(p);
                            }
                            i++;
                            break;
                        case '*':
                            if (format[i+2] == 's') {
                                int l;
                                l = (int)va_arg(ap, int);
                                p = (const char *)va_arg(ap, const char *);
                                size = (p != NULL)?strlen(p):0;
                                if (0 < size) {
                                    int space;
                                    delete [] buf;
                                    space = l - size;
                                    if (0 < space) {
                                        buf = new char[l + 1];
                                        strcpy(buf + space, p);
                                        while (0 < space) {
                                            buf[--space] = ' ';
                                        }
                                    } else {
                                        buf = new char[size + 1];
                                        strcpy(buf, p);
                                    }
                                    (*this).add(buf);
                                    delete [] buf;
                                    buf = new char[format_len + 65];
                                }
                                i += 2;
                                break;
                            }
                        default:
                            size = strcspn(&format[i], "abcdefgijkmnopqrstuvwxyzABCDEFGHIJKMNOPQRSTUVWXYZ")+1;
                            if (1 < size) {
                                char *buf1;
                                long buf1_len;
                                buf = new char[65];
                                memcpy(buf, &format[i], size);
                                buf[size] = '\0';
                                buf1_len = 65 + (size * 3) + 2048; // ok?
                                buf1 = new char[buf1_len + 1];
                                ::snprintf(buf1, buf1_len + 1, buf, va_arg(ap, void *));
                                (*this).add(buf1);
                                delete [] buf1;
                                i += size-1;
                            }
                    }
            }
            i++;
        }

        va_end(ap);
        delete [] buf;
    }

    return (*this).len();
}


/*! Escape URL-encoded text
    @param void
    @return Temporary string object (URL-encoded text)
 */
String& String::encodeURL() const {
    String *tmp = (*this).tmpStr();
    char *buf;
    long i, j, length;
    unsigned char a0, a1;

    *tmp = "";

    length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();

    if ((*this).pText) {
        // Memory allocation
        buf = new char[length * 3 + 1];

        // URL encoding
        j = 0;
        for (i = 0; i < length; i++) {
            if (((*this).pText[i] & 0x80) || !(('0' <= (*this).pText[i] && (*this).pText[i] <= '9') ||
                                               ('A' <= (*this).pText[i] && (*this).pText[i] <= 'Z') || ('a' <= (*this).pText[i] && (*this).pText[i] <= 'z') ||
                                               (*this).pText[i] == '-' || (*this).pText[i] == '.' || (*this).pText[i] == '@' || (*this).pText[i] == '_')) {
                a0 = (((*this).pText[i] & 0xF0) >> 4);
                a0 = (a0 < 10)?'0'+a0:'A'-10+a0;
                a1 = ((*this).pText[i] & 0x0F);
                a1 = (a1 < 10)?'0'+a1:'A'-10+a1;
                buf[j] = '%';
                buf[j+1] = a0;
                buf[j+2] = a1;
                j += 3;
            } else {
                buf[j] = (*this).pText[i];
                j++;
            }
        }
        buf[j] = '\0';
        (*tmp).useAsText();
        (*tmp).pTextReplace(buf, j, -1, length * 3 + 1);
    }

    return *tmp;
}


/*! Unescape URL-encoded text.
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Unescaped text)
 */
String& String::decodeURL(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    unsigned char *buf;
    long i, j, length;
    unsigned char a0, a1, a2, a3, b0, b1, c0, c1;
    int convert_flag;

    *tmp = "";

    if (src_charset == NULL || dest_charset == NULL || !strcasecmp(src_charset, dest_charset)) {
        convert_flag = 0;
    } else {
        convert_flag = 1;
    }

    if ((*this).pText) {
        // Memory allocation
        length = (*this).len();
        buf = new unsigned char[(length * 4) + 1];

        // URL decode
        j = 0;
        for (i = 0; i < length; i++) {
            if (i + 2 < length && (*this).pText[i] == '%' &&
                    (('0' <= (a0 = (*this).pText[i + 1]) && a0 <= '9') ||
                     ('A' <= a0 && a0 <= 'F') ||
                     ('a' <= a0 && a0 <= 'f')) &&
                    (('0' <= (a1 = (*this).pText[i + 2]) && a1 <= '9') ||
                     ('A' <= a1 && a1 <= 'F') ||
                     ('a' <= a1 && a1 <= 'f')) ) {
                switch (a0) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        b0 = (a0 - '0') << 4;
                        break;
                    case 'A':
                    case 'a':
                        b0 = 0xA0;
                        break;
                    case 'B':
                    case 'b':
                        b0 = 0xB0;
                        break;
                    case 'C':
                    case 'c':
                        b0 = 0xC0;
                        break;
                    case 'D':
                    case 'd':
                        b0 = 0xD0;
                        break;
                    case 'E':
                    case 'e':
                        b0 = 0xE0;
                        break;
                    case 'F':
                    case 'f':
                        b0 = 0xF0;
                        break;
                    default:
                        b0 = 0x00;
                        break;
                }
                switch (a1) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        b1 = a1 - '0';
                        break;
                    case 'A':
                    case 'a':
                        b1 = 0x0A;
                        break;
                    case 'B':
                    case 'b':
                        b1 = 0x0B;
                        break;
                    case 'C':
                    case 'c':
                        b1 = 0x0C;
                        break;
                    case 'D':
                    case 'd':
                        b1 = 0x0D;
                        break;
                    case 'E':
                    case 'e':
                        b1 = 0x0E;
                        break;
                    case 'F':
                    case 'f':
                        b1 = 0x0F;
                        break;
                    default:
                        b1 = 0x00;
                        break;
                }
                buf[j] = b0 | b1;
                i += 2;
                j++;
            } else if (i + 5 < length && (*this).pText[i] == '%' &&
                       ((*this).pText[i + 1] == 'U' || (*this).pText[i + 1] == 'u') &&
                       (('0' <= (a0 = (*this).pText[i + 2]) && a0 <= '9') ||
                        ('A' <= a0 && a0 <= 'F') ||
                        ('a' <= a0 && a0 <= 'f')) &&
                       (('0' <= (a1 = (*this).pText[i + 3]) && a1 <= '9') ||
                        ('A' <= a1 && a1 <= 'F') ||
                        ('a' <= a1 && a1 <= 'f')) &&
                       (('0' <= (a2 = (*this).pText[i + 4]) && a2 <= '9') ||
                        ('A' <= a2 && a2 <= 'F') ||
                        ('a' <= a2 && a2 <= 'f')) &&
                       (('0' <= (a3 = (*this).pText[i + 5]) && a3 <= '9') ||
                        ('A' <= a3 && a3 <= 'F') ||
                        ('a' <= a3 && a3 <= 'f')) ) {
                // unicode UCS2 to UTF-8
                switch (a0) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        b0 = (a0 - '0') << 4;
                        break;
                    case 'A':
                    case 'a':
                        b0 = 0xA0;
                        break;
                    case 'B':
                    case 'b':
                        b0 = 0xB0;
                        break;
                    case 'C':
                    case 'c':
                        b0 = 0xC0;
                        break;
                    case 'D':
                    case 'd':
                        b0 = 0xD0;
                        break;
                    case 'E':
                    case 'e':
                        b0 = 0xE0;
                        break;
                    case 'F':
                    case 'f':
                        b0 = 0xF0;
                        break;
                    default:
                        b0 = 0x00;
                        break;
                }
                switch (a1) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        b1 = a1 - '0';
                        break;
                    case 'A':
                    case 'a':
                        b1 = 0x0A;
                        break;
                    case 'B':
                    case 'b':
                        b1 = 0x0B;
                        break;
                    case 'C':
                    case 'c':
                        b1 = 0x0C;
                        break;
                    case 'D':
                    case 'd':
                        b1 = 0x0D;
                        break;
                    case 'E':
                    case 'e':
                        b1 = 0x0E;
                        break;
                    case 'F':
                    case 'f':
                        b1 = 0x0F;
                        break;
                    default:
                        b1 = 0x00;
                        break;
                }
                c0 = b0 | b1;

                switch (a2) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        b0 = (a2 - '0') << 4;
                        break;
                    case 'A':
                    case 'a':
                        b0 = 0xA0;
                        break;
                    case 'B':
                    case 'b':
                        b0 = 0xB0;
                        break;
                    case 'C':
                    case 'c':
                        b0 = 0xC0;
                        break;
                    case 'D':
                    case 'd':
                        b0 = 0xD0;
                        break;
                    case 'E':
                    case 'e':
                        b0 = 0xE0;
                        break;
                    case 'F':
                    case 'f':
                        b0 = 0xF0;
                        break;
                    default:
                        b0 = 0x00;
                        break;
                }
                switch (a3) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        b1 = a3 - '0';
                        break;
                    case 'A':
                    case 'a':
                        b1 = 0x0A;
                        break;
                    case 'B':
                    case 'b':
                        b1 = 0x0B;
                        break;
                    case 'C':
                    case 'c':
                        b1 = 0x0C;
                        break;
                    case 'D':
                    case 'd':
                        b1 = 0x0D;
                        break;
                    case 'E':
                    case 'e':
                        b1 = 0x0E;
                        break;
                    case 'F':
                    case 'f':
                        b1 = 0x0F;
                        break;
                    default:
                        b1 = 0x00;
                        break;
                }
                c1 = b0 | b1;

                if (c0 <= (unsigned char)0x07) {
                    buf[j++] = 0xC0 | ((c0 & 7) << 2) | (c1 >> 6);
                    buf[j++] = 0x80 | (0x3F & c1);
                    if (convert_flag == 1) {
                        int l;
                        char *buf1;
                        buf[j] = '\0';
                        buf1 = charset_convert((const char *)(buf + j - 2), STR_UTF8, src_charset);
                        if (buf1 != NULL) {
                            l = strlen(buf1);
                            memcpy((char *)(buf + j - 2), buf1, l);
                            delete [] buf1;
                            j = (j - 2) + l;
                        }
                    }
                } else {
                    buf[j++] = 0xE0 | (c0 >> 4);
                    buf[j++] = 0x80 | ((0x3F & (c0 << 2)) | (c1 >> 6));
                    buf[j++] = 0x80 | (0x3F & c1);
                    if (convert_flag == 1) {
                        int l;
                        char *buf1;
                        buf[j] = '\0';
                        buf1 = charset_convert((const char *)(buf + j - 3), STR_UTF8, src_charset);
                        if (buf1 != NULL) {
                            l = strlen(buf1);
                            memcpy((char *)(buf + j - 3), buf1, l);
                            delete [] buf1;
                            j = (j - 3) + l;
                        }
                    }
                }
                i += 5;
            } else {
                buf[j] = (*this).pText[i];
                j++;
            }
        }
        buf[j] = '\0';
        if (convert_flag == 1) {
            unsigned char *buf1;
            if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
                int bom = strcasestr(dest_charset, "BOM")?1:0;
                if (strcasestr(dest_charset, "LE") != NULL) {
                    buf1 = (unsigned char *)utf8_to_utf16((const char *)buf, &j, bom?'L':'l');
                } else if (strcasestr(dest_charset, "BE") != NULL) {
                    buf1 = (unsigned char *)utf8_to_utf16((const char *)buf, &j, bom?'B':'b');
                } else {
                    buf1 = (unsigned char *)utf8_to_utf16((const char *)buf, &j, 'B');
                }
                if (buf1 != (unsigned char *)NULL) {
                    delete [] buf;
                    buf = buf1;
                }
            } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
                int bom = strcasestr(dest_charset, "BOM")?1:0;
                if (strcasestr(dest_charset, "LE") != NULL) {
                    buf1 = (unsigned char *)utf8_to_utf32((const char *)buf, &j, bom?'L':'l');
                } else if (strcasestr(dest_charset, "BE") != NULL) {
                    buf1 = (unsigned char *)utf8_to_utf32((const char *)buf, &j, bom?'B':'b');
                } else {
                    buf1 = (unsigned char *)utf8_to_utf32((const char *)buf, &j, 'B');
                }
                if (buf1 != (unsigned char *)NULL) {
                    delete [] buf;
                    buf = buf1;
                }
            } else {
                buf1 = (unsigned char *)charset_convert((const char *)buf, src_charset, dest_charset);
                if (buf1 != (unsigned char *)NULL) {
                    delete [] buf;
                    buf = buf1;
                    j = strlen(buf);
                }
            }
        }
        (*tmp).pTextReplace((char *)buf, -1, j, (convert_flag == 1)?j:(length + 1));
        (*tmp).useAsBinary(j);
    }

    return *tmp;
}


/*! Escape HTML tag (ex. ">" --> "&gt;")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param options       Encoding options
                         's': replace a space to "&nbsp;"
                         'b': replace a return-code to "<br>"
                         'u': use "&#xx;" encodings
    @return Temporary string object (Escaped text)
 */
String& String::escapeHTML(const char *src_charset, const char *dest_charset, const char *options) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len, ucs2;
    int option_s, option_b, option_u;
    int space_count;

    if (options != NULL && options[0] != '\0') {
        option_s = (strchr(options, 's'))?1:0;
        option_b = (strchr(options, 'b'))?1:0;
        option_u = (strchr(options, 'u'))?1:0;
    } else {
        option_s = 0;
        option_b = 0;
        option_u = 0;
    }

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        space_count = 0;
        length = strlen(src_utf8);
        buf = new char[(length * 8) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            if (space_count != 0 && src_utf8[i] != ' ') {
                space_count = 0;
            }
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '<') {
                memcpy(buf + j, "&lt;", 4);
                j += 4;
            } else if (src_utf8[i] == '>') {
                memcpy(buf + j, "&gt;", 4);
                j += 4;
            } else if (src_utf8[i] == '&') {
                memcpy(buf + j, "&amp;", 5);
                j += 5;
            } else if (src_utf8[i] == '"') {
                memcpy(buf + j, "&quot;", 6);
                j += 6;
            } else if (src_utf8[i] == '\'') {
                memcpy(buf + j, "&#39;", 5);
                j += 5;
            } else if (option_s && src_utf8[i] == ' ') {
                if ((space_count%2) == 0) {
                    buf[j++] = ' ';
                } else {
                    memcpy(buf + j, "&nbsp;", 6);
                    j += 6;
                }
                space_count++;
            } else if (option_b && src_utf8[i] == '\r' && src_utf8[i+1] == '\n') {
                memcpy(buf + j, "<br>\r\n", 6);
                j += 6;
                i++;
            } else if (option_b && src_utf8[i] == '\n') {
                memcpy(buf + j, "<br>\n", 5);
                j += 5;
            } else if (option_u) {
                if ((src_utf8[i] & (char)0x80) == 0 && !isalnum(src_utf8[i]) &&
                        !(src_utf8[i] == '-' || src_utf8[i] == '.' || src_utf8[i] == '@' || src_utf8[i] == '_' || src_utf8[i] == '/' || src_utf8[i] == ' ')) {
                    buf[j++] = '&';
                    buf[j++] = '#';
                    itoa(buf + j, (int)(src_utf8[i]));
                    j += strlen(buf + j);
                    buf[j++] = ';';
                } else if (1 < u8len) {
                    if (u8len == 2) {
                        unsigned char a0, a1;
                        a0 = 0x07 & ((unsigned char)(src_utf8[i]) >> 2);
                        a1 = ((unsigned char)(src_utf8[i]) << 6) | (0x3F & (unsigned char)(src_utf8[i+1]));
                        ucs2 = (((int)a0) << 8) | (int)a1;
                        buf[j++] = '&';
                        buf[j++] = '#';
                        j += itoa(buf + j, ucs2);
                        buf[j++] = ';';
                    } else {
                        k = 0;
                        while (i + k < length && k < u8len) {
                            if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                                buf[j++] = src_utf8[i + k];
                            } else {
                                break;
                            }
                            k++;
                        }
                        i += k;
                        continue;
                    }
                } else {
                    buf[j++] = src_utf8[i];
                }
            } else {
                for (k = 0; k < u8len; k++) {
                    buf[j] = src_utf8[i + k];
                    j++;
                }
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Unescape HTML tag (ex. "&#62;" --> ">")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Unescaped text)
 */
String& String::unescapeHTML(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k, l;
    long length;
    int u8len, ucs2;

    typedef struct {
        const char *tag;
        unsigned char latin1;
    } T_HTMLTAG;
    const int replace_htmltag_max = 95;
    const T_HTMLTAG replace_htmltag[replace_htmltag_max] = {
        {"iexcl", 161}, {"cent", 162},
        {"pound", 163}, {"curren", 164},
        {"yen", 165}, {"brvbar", 166},
        {"sect", 167}, {"uml", 168},
        {"copy", 169}, {"ordf", 170},
        {"laquo", 171}, {"not", 172},
        {"shy", 173}, {"reg", 174},
        {"macr", 175}, {"deg", 176},
        {"plusmn", 177}, {"sup2", 178},
        {"sup3", 179}, {"acute", 180},
        {"micro", 181}, {"para", 182},
        {"middot", 183}, {"cedil", 184},
        {"sup1", 185}, {"ordm", 186},
        {"raquo", 187}, {"frac14", 188},
        {"frac12", 189}, {"frac34", 190},
        {"iquest", 191}, {"agrave", 192},
        {"aacute", 193}, {"acirc", 194},
        {"atilde", 195}, {"auml", 196},
        {"aring", 197}, {"aelig", 198},
        {"ccedil", 199}, {"egrave", 200},
        {"eacute", 201}, {"ecirc", 202},
        {"euml", 203}, {"igrave", 204},
        {"iacute", 205}, {"icirc", 206},
        {"iuml", 207}, {"eth", 208},
        {"ntilde", 209}, {"ograve", 210},
        {"oacute", 211}, {"ocirc", 212},
        {"otilde", 213}, {"ouml", 214},
        {"times", 215}, {"oslash", 216},
        {"ugrave", 217}, {"uacute", 218},
        {"ucirc", 219}, {"uuml", 220},
        {"yacute", 221}, {"thorn", 222},
        {"szlig", 223}, {"agrave", 224},
        {"aacute", 225}, {"acirc", 226},
        {"atilde", 227}, {"auml", 228},
        {"aring", 229}, {"aelig", 230},
        {"ccedil", 231}, {"egrave", 232},
        {"eacute", 233}, {"ecirc", 234},
        {"euml", 235}, {"igrave", 236},
        {"iacute", 237}, {"icirc", 238},
        {"iuml", 239}, {"eth", 240},
        {"ntilde", 241}, {"ograve", 242},
        {"oacute", 243}, {"ocirc", 244},
        {"otilde", 245}, {"ouml", 246},
        {"divide", 247}, {"oslash", 248},
        {"ugrave", 249}, {"uacute", 250},
        {"ucirc", 251}, {"uuml", 252},
        {"yacute", 253}, {"thorn", 254},
        {"yuml", 255}
    };

    typedef struct {
        const char *tag;
        const char *utf8;
    } T_GREEK_TAG;
    const int replace_g_htmltag_max = 144;
    static const T_GREEK_TAG replace_g_htmltag[replace_g_htmltag_max] = {
        {"Alpha", "\xCE\x91"},
        {"Beta", "\xCE\x92"},
        {"Gamma", "\xCE\x93"},
        {"Delta", "\xCE\x94"},
        {"Epsilon", "\xCE\x95"},
        {"Zeta", "\xCE\x96"},
        {"Eta", "\xCE\x97"},
        {"Theta", "\xCE\x98"},
        {"Iota", "\xCE\x99"},
        {"Kappa", "\xCE\x9A"},
        {"Lambda", "\xCE\x9B"},
        {"Mu", "\xCE\x9C"},
        {"Nu", "\xCE\x9D"},
        {"Xi", "\xCE\x9E"},
        {"Omicron", "\xCE\x9F"},
        {"Pi", "\xCE\xA0"},
        {"Rho", "\xCE\xA1"},
        {"Sigma", "\xCE\xA3"},
        {"Tau", "\xCE\xA4"},
        {"Upsilon", "\xCE\xA5"},
        {"Phi", "\xCE\xA6"},
        {"Chi", "\xCE\xA7"},
        {"Psi", "\xCE\xA8"},
        {"Omega", "\xCE\xA9"},
        {"alpha", "\xCE\xB1"},
        {"beta", "\xCE\xB2"},
        {"gamma", "\xCE\xB3"},
        {"delta", "\xCE\xB4"},
        {"epsilon", "\xCE\xB5"},
        {"zeta", "\xCE\xB6"},
        {"eta", "\xCE\xB7"},
        {"theta", "\xCE\xB8"},
        {"iota", "\xCE\xB9"},
        {"kappa", "\xCE\xBA"},
        {"lambda", "\xCE\xBB"},
        {"mu", "\xCE\xBC"},
        {"nu", "\xCE\xBD"},
        {"xi", "\xCE\xBE"},
        {"omicron", "\xCE\xBF"},
        {"pi", "\xCF\x80"},
        {"rho", "\xCF\x81"},
        {"sigmaf", "\xCF\x82"},
        {"sigma", "\xCF\x83"},
        {"tau", "\xCF\x84"},
        {"upsilon", "\xCF\x85"},
        {"phi", "\xCF\x86"},
        {"chi", "\xCF\x87"},
        {"psi", "\xCF\x88"},
        {"omega", "\xCF\x89"},
        {"thetasym", "\xCF\x91"},
        {"upsih", "\xCF\x92"},
        {"piv", "\xCF\x96"},
        {"ensp", "\xE2\x80\x82"},
        {"emsp", "\xE2\x80\x83"},
        {"thinsp", "\xE2\x80\x89"},
        {"zwnj", "\xE2\x80\x8C"},
        {"zwj", "\xE2\x80\x8D"},
        {"lrm", "\xE2\x80\x8E"},
        {"rlm", "\xE2\x80\x8F"},
        {"ndash", "\xE2\x80\x93"},
        {"mdash", "\xE2\x80\x94"},
        {"lsquo", "\xE2\x80\x98"},
        {"rsquo", "\xE2\x80\x99"},
        {"sbquo", "\xE2\x80\x9A"},
        {"ldquo", "\xE2\x80\x9C"},
        {"rdquo", "\xE2\x80\x9D"},
        {"bdquo", "\xE2\x80\x9E"},
        {"dagger", "\xE2\x80\xA0"},
        {"Dagger", "\xE2\x80\xA1"},
        {"bull", "\xE2\x80\xA2"},
        {"hellip", "\xE2\x80\xA6"},
        {"permil", "\xE2\x80\xB0"},
        {"prime", "\xE2\x80\xB2"},
        {"Prime", "\xE2\x80\xB3"},
        {"lsaquo", "\xE2\x80\xB9"},
        {"rsaquo", "\xE2\x80\xBA"},
        {"oline", "\xE2\x80\xBE"},
        {"frasl", "\xE2\x81\x84"},
        {"euro", "\xE2\x82\xAC"},
        {"image", "\xE2\x84\x91"},
        {"weierp", "\xE2\x84\x98"},
        {"real", "\xE2\x84\x9C"},
        {"trade", "\xE2\x84\xA2"},
        {"alefsym", "\xE2\x84\xB5"},
        {"larr", "\xE2\x86\x90"},
        {"uarr", "\xE2\x86\x91"},
        {"rarr", "\xE2\x86\x92"},
        {"darr", "\xE2\x86\x93"},
        {"harr", "\xE2\x86\x94"},
        {"crarr", "\xE2\x86\xB5"},
        {"lArr", "\xE2\x87\x90"},
        {"uArr", "\xE2\x87\x91"},
        {"rArr", "\xE2\x87\x92"},
        {"dArr", "\xE2\x87\x93"},
        {"hArr", "\xE2\x87\x94"},
        {"forall", "\xE2\x88\x80"},
        {"part", "\xE2\x88\x82"},
        {"exist", "\xE2\x88\x83"},
        {"empty", "\xE2\x88\x85"},
        {"nabla", "\xE2\x88\x87"},
        {"isin", "\xE2\x88\x88"},
        {"notin", "\xE2\x88\x89"},
        {"ni", "\xE2\x88\x8B"},
        {"prod", "\xE2\x88\x8F"},
        {"sum", "\xE2\x88\x91"},
        {"minus", "\xE2\x88\x92"},
        {"lowast", "\xE2\x88\x97"},
        {"radic", "\xE2\x88\x9A"},
        {"prop", "\xE2\x88\x9D"},
        {"infin", "\xE2\x88\x9E"},
        {"ang", "\xE2\x88\xA0"},
        {"and", "\xE2\x88\xA7"},
        {"or", "\xE2\x88\xA8"},
        {"cap", "\xE2\x88\xA9"},
        {"cup", "\xE2\x88\xAA"},
        {"int", "\xE2\x88\xAB"},
        {"there4", "\xE2\x88\xB4"},
        {"sim", "\xE2\x88\xBC"},
        {"cong", "\xE2\x89\x85"},
        {"asymp", "\xE2\x89\x88"},
        {"ne", "\xE2\x89\xA0"},
        {"equiv", "\xE2\x89\xA1"},
        {"le", "\xE2\x89\xA4"},
        {"ge", "\xE2\x89\xA5"},
        {"sub", "\xE2\x8A\x82"},
        {"sup", "\xE2\x8A\x83"},
        {"nsub", "\xE2\x8A\x84"},
        {"sube", "\xE2\x8A\x86"},
        {"supe", "\xE2\x8A\x87"},
        {"oplus", "\xE2\x8A\x95"},
        {"otimes", "\xE2\x8A\x97"},
        {"perp", "\xE2\x8A\xA5"},
        {"sdot", "\xE2\x8B\x85"},
        {"lceil", "\xE2\x8C\x88"},
        {"rceil", "\xE2\x8C\x89"},
        {"lfloor", "\xE2\x8C\x8A"},
        {"rfloor", "\xE2\x8C\x8B"},
        {"lang", "\xE2\x8C\xA9"},
        {"rang", "\xE2\x8C\xAA"},
        {"loz", "\xE2\x97\x8A"},
        {"spades", "\xE2\x99\xA0"},
        {"clubs", "\xE2\x99\xA3"},
        {"hearts", "\xE2\x99\xA5"},
        {"diams", "\xE2\x99\xA6"},
    };

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '&' && src_utf8[i + 1] == '#' &&
                    ('0' <= src_utf8[i + 2] && src_utf8[i + 2] <= '9')) {
                char tmp_str[10];
                unsigned char a0, a1;

                k = 0;
                while ('0' <= src_utf8[i + 2 + k] && src_utf8[i + 2 + k] <= '9' && k < 9) {
                    tmp_str[k] = src_utf8[i + 2 + k];
                    k++;
                }
                tmp_str[k] = '\0';
                ucs2 = atoi(tmp_str);

                if (ucs2 == 160) {
                    ucs2 = 0x20;
                }

                if (ucs2 < 0x80) {
                    buf[j++] = (char)ucs2;
                } else {
                    a0 = (unsigned char)((ucs2 & 0xFF00) >> 8);
                    a1 = (unsigned char)(ucs2 & 0xFF);
                    if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                }
                i += (k + 2);
                if (src_utf8[i] == ';') i++;
                continue;
            } else if (src_utf8[i] == '&' && src_utf8[i + 1] == '#' &&
                       (src_utf8[i + 2] == 'x' || src_utf8[i + 2] == 'X') &&
                       (('0' <= src_utf8[i + 3] && src_utf8[i + 3] <= '9') ||
                        ('A' <= src_utf8[i + 3] && src_utf8[i + 3] <= 'F') ||
                        ('a' <= src_utf8[i + 3] && src_utf8[i + 3] <= 'f'))) {
                char tmp_str[10];
                unsigned char a0, a1;

                k = 0;
                while ((('0' <= src_utf8[i + 3 + k] && src_utf8[i + 3 + k] <= '9') ||
                        ('A' <= src_utf8[i + 3 + k] && src_utf8[i + 3 + k] <= 'F') ||
                        ('a' <= src_utf8[i + 3 + k] && src_utf8[i + 3 + k] <= 'f')) && k < 9) {
                    tmp_str[k] = src_utf8[i + 3 + k];
                    k++;
                }
                tmp_str[k] = '\0';
                ucs2 = (int)strtol(tmp_str, NULL, 16);

                if (ucs2 == 160) {
                    ucs2 = 0x20;
                }

                if (ucs2 < 0x80) {
                    buf[j++] = (char)ucs2;
                } else {
                    a0 = (unsigned char)((ucs2 & 0xFF00) >> 8);
                    a1 = (unsigned char)(ucs2 & 0xFF);
                    if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                }
                i += (k + 3);
                if (src_utf8[i] == ';') i++;
                continue;
            } else if (src_utf8[i] == '&') {

                if (!strncasecmp(src_utf8 + i + 1, "nbsp", 4)) {
                    buf[j++] = ' ';
                    i += (1 + 4);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "lt", 2)) {
                    buf[j++] = '<';
                    i += (1 + 2);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "gt", 2)) {
                    buf[j++] = '>';
                    i += (1 + 2);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "amp", 3)) {
                    buf[j++] = '&';
                    i += (1 + 3);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "quot", 4)) {
                    buf[j++] = '"';
                    i += (1 + 4);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "apos", 4)) {
                    buf[j++] = '\'';
                    i += (1 + 5);
                    if (src_utf8[i] == ';') i++;
                } else {
                    int taglen;
                    unsigned char a1;
                    k = 0;
                    taglen = -1;
                    while (k < replace_htmltag_max) {
                        taglen = strlen(replace_htmltag[k].tag);
                        if (!strncasecmp(src_utf8 + i + 1, replace_htmltag[k].tag, taglen)) {
                            break;
                        }
                        k++;
                    }
                    if (k < replace_htmltag_max) {
                        a1 = replace_htmltag[k].latin1;
                        buf[j++] = 0xC0 | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                        i += (1 + taglen);
                        if (src_utf8[i] == ';') i++;
                    } else {
                        k = 0;
                        taglen = -1;
                        while (k < replace_g_htmltag_max) {
                            taglen = strlen(replace_g_htmltag[k].tag);
                            if (!strncmp(src_utf8 + i + 1, replace_g_htmltag[k].tag, taglen)) {
                                break;
                            }
                            k++;
                        }
                        if (k < replace_g_htmltag_max) {
                            l = strlen(replace_g_htmltag[k].utf8);
                            strncpy(buf+j, replace_g_htmltag[k].utf8, l);
                            j += l;
                            i += (1 + taglen);
                            if (src_utf8[i] == ';') i++;
                        } else {
                            buf[j++] = src_utf8[i];
                            i++;
                        }
                    }
                }
                continue;
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape Back-quote (ex. ` --> \` , ${ --> \${)
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Escaped text)
 */
String& String::escapeHTMLBackQuote(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 3) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                switch (src_utf8[i]) {
                    case '\\':
                        buf[j++] = '\\';
                        buf[j++] = '\\';
                        break;
                    case '`':
                        buf[j++] = '\\';
                        buf[j++] = '`';
                        break;
                    case '$':
                        if (src_utf8[i+1] == '{') {
                            buf[j++] = '\\';
                            buf[j++] = '$';
                            buf[j++] = '{';
                            i++;
                        }
                        break;
                    default:
                        buf[j++] = src_utf8[i];
                        break;
                }
            } else {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape XML tag (ex. ">" --> "&gt;")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param options       Encoding options
                         's': replace a space to "&#20;"
                         'b': replace a return-code to "&#10;"
                         'u': use "&#xx;" encodings
    @return Temporary string object (Escaped text)
 */
String& String::escapeXML(const char *src_charset, const char *dest_charset, const char *options) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len, ucs2;
    int option_s, option_b, option_u;
    int space_count;

    if (options != NULL && options[0] != '\0') {
        option_s = (strchr(options, 's'))?1:0;
        option_b = (strchr(options, 'b'))?1:0;
        option_u = (strchr(options, 'u'))?1:0;
    } else {
        option_s = 0;
        option_b = 0;
        option_u = 0;
    }

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        space_count = 0;
        length = strlen(src_utf8);
        buf = new char[(length * 8) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            if (space_count != 0 && src_utf8[i] != ' ') {
                space_count = 0;
            }
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '<') {
                memcpy(buf + j, "&lt;", 4);
                j += 4;
            } else if (src_utf8[i] == '>') {
                memcpy(buf + j, "&gt;", 4);
                j += 4;
            } else if (src_utf8[i] == '&') {
                memcpy(buf + j, "&amp;", 5);
                j += 5;
            } else if (src_utf8[i] == '"') {
                memcpy(buf + j, "&quot;", 6);
                j += 6;
            } else if (src_utf8[i] == '\'') {
                memcpy(buf + j, "&apos;", 6);
                j += 6;
            } else if (option_s && src_utf8[i] == ' ') {
                if ((space_count%2) == 0) {
                    buf[j++] = ' ';
                } else {
                    memcpy(buf + j, "&#20;", 5);
                    j += 5;
                }
                space_count++;
            } else if (option_b && src_utf8[i] == '\r' && src_utf8[i+1] == '\n') {
                memcpy(buf + j, "&#10;", 5);
                j += 5;
                i++;
            } else if (option_b && src_utf8[i] == '\n') {
                memcpy(buf + j, "&#10;", 5);
                j += 5;
            } else if (option_u) {
                if ((src_utf8[i] & (char)0x80) == 0 && !isalnum(src_utf8[i]) &&
                        !(src_utf8[i] == '-' || src_utf8[i] == '.' || src_utf8[i] == '@' || src_utf8[i] == '_' || src_utf8[i] == '/' || src_utf8[i] == ' ')) {
                    buf[j++] = '&';
                    buf[j++] = '#';
                    itoa(buf + j, (int)(src_utf8[i]));
                    j += strlen(buf + j);
                    buf[j++] = ';';
                } else if (1 < u8len) {
                    if (u8len == 2) {
                        unsigned char a0, a1;
                        a0 = 0x07 & ((unsigned char)(src_utf8[i]) >> 2);
                        a1 = ((unsigned char)(src_utf8[i]) << 6) | (0x3F & (unsigned char)(src_utf8[i+1]));
                        ucs2 = (((int)a0) << 8) | (int)a1;
                        buf[j++] = '&';
                        buf[j++] = '#';
                        j += itoa(buf + j, ucs2);
                        buf[j++] = ';';
                    } else if (u8len == 3 && src_utf8[i] == '\xEF' &&
                            src_utf8[i + 1] == '\xBF' &&
                            (src_utf8[i + 2] == '\xB0' || src_utf8[i + 2] == '\xB1' ||
                            src_utf8[i + 2] == '\xB2' || src_utf8[i + 2] == '\xB3' ||
                            src_utf8[i + 2] == '\xB4' || src_utf8[i + 2] == '\xB5' ||
                            src_utf8[i + 2] == '\xB6' || src_utf8[i + 2] == '\xB7' ||
                            src_utf8[i + 2] == '\xB8' || src_utf8[i + 2] == '\xB9' ||
                            src_utf8[i + 2] == '\xBA' || src_utf8[i + 2] == '\xBB' ||
                            src_utf8[i + 2] == '\xBC' || src_utf8[i + 2] == '\xBD' ||
                            src_utf8[i + 2] == '\xBE' || src_utf8[i + 2] == '\xBF')) {
                        i += 3;
                        continue;
                    } else {
                        k = 0;
                        while (i + k < length && k < u8len) {
                            if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                                buf[j++] = src_utf8[i + k];
                            } else {
                                break;
                            }
                            k++;
                        }
                        i += k;
                        continue;
                    }
                } else {
                    buf[j++] = src_utf8[i];
                }
            } else {
                for (k = 0; k < u8len; k++) {
                    buf[j] = src_utf8[i + k];
                    j++;
                }
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Unescape XML tag (ex. "&#62;" --> ">")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Unescaped text)
 */
String& String::unescapeXML(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k, l;
    long length;
    int u8len, ucs2;

    typedef struct {
        const char *tag;
        unsigned char latin1;
    } T_HTMLTAG;
    const int replace_htmltag_max = 95;
    const T_HTMLTAG replace_htmltag[replace_htmltag_max] = {
        {"iexcl", 161}, {"cent", 162},
        {"pound", 163}, {"curren", 164},
        {"yen", 165}, {"brvbar", 166},
        {"sect", 167}, {"uml", 168},
        {"copy", 169}, {"ordf", 170},
        {"laquo", 171}, {"not", 172},
        {"shy", 173}, {"reg", 174},
        {"macr", 175}, {"deg", 176},
        {"plusmn", 177}, {"sup2", 178},
        {"sup3", 179}, {"acute", 180},
        {"micro", 181}, {"para", 182},
        {"middot", 183}, {"cedil", 184},
        {"sup1", 185}, {"ordm", 186},
        {"raquo", 187}, {"frac14", 188},
        {"frac12", 189}, {"frac34", 190},
        {"iquest", 191}, {"agrave", 192},
        {"aacute", 193}, {"acirc", 194},
        {"atilde", 195}, {"auml", 196},
        {"aring", 197}, {"aelig", 198},
        {"ccedil", 199}, {"egrave", 200},
        {"eacute", 201}, {"ecirc", 202},
        {"euml", 203}, {"igrave", 204},
        {"iacute", 205}, {"icirc", 206},
        {"iuml", 207}, {"eth", 208},
        {"ntilde", 209}, {"ograve", 210},
        {"oacute", 211}, {"ocirc", 212},
        {"otilde", 213}, {"ouml", 214},
        {"times", 215}, {"oslash", 216},
        {"ugrave", 217}, {"uacute", 218},
        {"ucirc", 219}, {"uuml", 220},
        {"yacute", 221}, {"thorn", 222},
        {"szlig", 223}, {"agrave", 224},
        {"aacute", 225}, {"acirc", 226},
        {"atilde", 227}, {"auml", 228},
        {"aring", 229}, {"aelig", 230},
        {"ccedil", 231}, {"egrave", 232},
        {"eacute", 233}, {"ecirc", 234},
        {"euml", 235}, {"igrave", 236},
        {"iacute", 237}, {"icirc", 238},
        {"iuml", 239}, {"eth", 240},
        {"ntilde", 241}, {"ograve", 242},
        {"oacute", 243}, {"ocirc", 244},
        {"otilde", 245}, {"ouml", 246},
        {"divide", 247}, {"oslash", 248},
        {"ugrave", 249}, {"uacute", 250},
        {"ucirc", 251}, {"uuml", 252},
        {"yacute", 253}, {"thorn", 254},
        {"yuml", 255}
    };

    typedef struct {
        const char *tag;
        const char *utf8;
    } T_GREEK_TAG;
    const int replace_g_htmltag_max = 14;
    static const T_GREEK_TAG replace_g_htmltag[replace_g_htmltag_max] = {
        {"euro", "\xE2\x82\xAC"},
        {"dagger", "\xE2\x80\xA0"},
        {"Dagger", "\xE2\x80\xA1"},
        {"bull", "\xE2\x80\xA2"},
        {"trade", "\xE2\x84\xA2"},
        {"permil", "\xE2\x80\xB0"},
        {"lsquo", "\xE2\x80\x98"},
        {"rsquo", "\xE2\x80\x99"},
        {"sbquo", "\xE2\x80\x9A"},
        {"ldquo", "\xE2\x80\x9C"},
        {"rdquo", "\xE2\x80\x9D"},
        {"bdquo", "\xE2\x80\x9E"},
        {"ndash", "\xE2\x80\x93"},
        {"mdash", "\xE2\x80\x94"}
    };

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '&' && src_utf8[i + 1] == '#' &&
                    ('0' <= src_utf8[i + 2] && src_utf8[i + 2] <= '9')) {
                char tmp_str[10];
                unsigned char a0, a1;

                k = 0;
                while ('0' <= src_utf8[i + 2 + k] && src_utf8[i + 2 + k] <= '9' && k < 9) {
                    tmp_str[k] = src_utf8[i + 2 + k];
                    k++;
                }
                tmp_str[k] = '\0';
                ucs2 = atoi(tmp_str);

                if (ucs2 == 160) {
                    ucs2 = 0x20;
                }

                if (ucs2 < 0x80) {
                    buf[j++] = (char)ucs2;
                } else {
                    a0 = (unsigned char)((ucs2 & 0xFF00) >> 8);
                    a1 = (unsigned char)(ucs2 & 0xFF);
                    if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                }
                i += (k + 2);
                if (src_utf8[i] == ';') i++;
                continue;
            } else if (src_utf8[i] == '&' && src_utf8[i + 1] == '#' &&
                       (src_utf8[i + 2] == 'x' || src_utf8[i + 2] == 'X') &&
                       (('0' <= src_utf8[i + 3] && src_utf8[i + 3] <= '9') ||
                        ('A' <= src_utf8[i + 3] && src_utf8[i + 3] <= 'F') ||
                        ('a' <= src_utf8[i + 3] && src_utf8[i + 3] <= 'f'))) {
                char tmp_str[10];
                unsigned char a0, a1;

                k = 0;
                while ((('0' <= src_utf8[i + 3 + k] && src_utf8[i + 3 + k] <= '9') ||
                        ('A' <= src_utf8[i + 3 + k] && src_utf8[i + 3 + k] <= 'F') ||
                        ('a' <= src_utf8[i + 3 + k] && src_utf8[i + 3 + k] <= 'f')) && k < 9) {
                    tmp_str[k] = src_utf8[i + 3 + k];
                    k++;
                }
                tmp_str[k] = '\0';
                ucs2 = (int)strtol(tmp_str, NULL, 16);

                if (ucs2 == 160) {
                    ucs2 = 0x20;
                }

                if (ucs2 < 0x80) {
                    buf[j++] = (char)ucs2;
                } else {
                    a0 = (unsigned char)((ucs2 & 0xFF00) >> 8);
                    a1 = (unsigned char)(ucs2 & 0xFF);
                    if (a0 <= (unsigned char)0x07) {
                        buf[j++] = 0xC0 | ((a0 & 7) << 2) | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                    } else {
                        buf[j++] = 0xE0 | (a0 >> 4);
                        buf[j++] = 0x80 | ((0x3F & (a0 << 2)) | (a1 >> 6));
                        buf[j++] = 0x80 | (0x3F & a1);
                    }
                }
                i += (k + 3);
                if (src_utf8[i] == ';') i++;
                continue;
            } else if (src_utf8[i] == '&') {

                if (!strncasecmp(src_utf8 + i + 1, "nbsp", 4)) {
                    buf[j++] = ' ';
                    i += (1 + 4);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "lt", 2)) {
                    buf[j++] = '<';
                    i += (1 + 2);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "gt", 2)) {
                    buf[j++] = '>';
                    i += (1 + 2);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "amp", 3)) {
                    buf[j++] = '&';
                    i += (1 + 3);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "quot", 4)) {
                    buf[j++] = '"';
                    i += (1 + 4);
                    if (src_utf8[i] == ';') i++;
                } else if (!strncasecmp(src_utf8 + i + 1, "apos", 4)) {
                    buf[j++] = '\'';
                    i += (1 + 5);
                    if (src_utf8[i] == ';') i++;
                } else {
                    int taglen;
                    unsigned char a1;
                    k = 0;
                    taglen = -1;
                    while (k < replace_htmltag_max) {
                        taglen = strlen(replace_htmltag[k].tag);
                        if (!strncasecmp(src_utf8 + i + 1, replace_htmltag[k].tag, taglen)) {
                            break;
                        }
                        k++;
                    }
                    if (k < replace_htmltag_max) {
                        a1 = replace_htmltag[k].latin1;
                        buf[j++] = 0xC0 | (a1 >> 6);
                        buf[j++] = 0x80 | (0x3F & a1);
                        i += (1 + taglen);
                        if (src_utf8[i] == ';') i++;
                    } else {
                        k = 0;
                        taglen = -1;
                        while (k < replace_g_htmltag_max) {
                            taglen = strlen(replace_g_htmltag[k].tag);
                            if (!strncmp(src_utf8 + i + 1, replace_g_htmltag[k].tag, taglen)) {
                                break;
                            }
                            k++;
                        }
                        if (k < replace_g_htmltag_max) {
                            l = strlen(replace_g_htmltag[k].utf8);
                            strncpy(buf+j, replace_g_htmltag[k].utf8, l);
                            j += l;
                            i += (1 + taglen);
                            if (src_utf8[i] == ';') i++;
                        } else {
                            buf[j++] = src_utf8[i];
                            i++;
                        }
                    }
                }
                continue;
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape string for SQLite3 (ex. ' --> '')
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Escaped text)
 */
String& String::escapeSQLite3(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                switch (src_utf8[i]) {
                    case '\'':
                        buf[j++] = '\'';
                        buf[j++] = '\'';
                        break;
                    default:
                        buf[j++] = src_utf8[i];
                        break;
                }
            } else {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape string for MySQL (ex. ` --> \`)
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Escaped text)
 */
String& String::escapeMySQL(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                switch (src_utf8[i]) {
                    case '\\':
                        buf[j++] = '\\';
                        buf[j++] = '\\';
                        break;
                    case '\0':
                        buf[j++] = '\\';
                        buf[j++] = '0';
                        break;
                    case '\'':
                        buf[j++] = '\\';
                        buf[j++] = '\'';
                        break;
                    case '"':
                        buf[j++] = '\\';
                        buf[j++] = '"';
                        break;
                    case '\b':
                        buf[j++] = '\\';
                        buf[j++] = 'b';
                        break;
                    case '\n':
                        buf[j++] = '\\';
                        buf[j++] = 'n';
                        break;
                    case '\r':
                        buf[j++] = '\\';
                        buf[j++] = 'r';
                        break;
                    case '\t':
                        buf[j++] = '\\';
                        buf[j++] = 't';
                        break;
                    case 26:
                        buf[j++] = '\\';
                        buf[j++] = 'Z';
                        break;
                    case '%':
                        buf[j++] = '\\';
                        buf[j++] = '%';
                        break;
                    case '_':
                        buf[j++] = '\\';
                        buf[j++] = '_';
                        break;
                    default:
                        buf[j++] = src_utf8[i];
                        break;
                }
            } else {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape for JSON (ex. " --> \")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Escaped text)
 */
String& String::escapeJSON(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                switch (src_utf8[i]) {
                    case '\\':
                        buf[j++] = '\\';
                        buf[j++] = '\\';
                        break;
                    case '\r':
                        buf[j++] = '\\';
                        buf[j++] = 'r';
                        break;
                    case '\n':
                        buf[j++] = '\\';
                        buf[j++] = 'n';
                        break;
                    case '\t':
                        buf[j++] = '\\';
                        buf[j++] = 't';
                        break;
                    case '\b':
                        buf[j++] = '\\';
                        buf[j++] = 'b';
                        break;
                    case '\f':
                        buf[j++] = '\\';
                        buf[j++] = 'f';
                        break;
                    case '"':
                        buf[j++] = '\\';
                        buf[j++] = '"';
                        break;
                    case '/':
                        buf[j++] = '\\';
                        buf[j++] = '/';
                        break;
                    default:
                        buf[j++] = src_utf8[i];
                        break;
                }
            } else {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Unescape JSON (ex. \" --> ")
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Unescaped text)
 */
String& String::unescapeJSON(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '\\') {
                if (i + 1 < length) {
                    i++;
                    switch (src_utf8[i]) {
                        case 'r':
                            buf[j++] = '\r';
                            break;
                        case 'n':
                            buf[j++] = '\n';
                            break;
                        case 't':
                            buf[j++] = '\t';
                            break;
                        case 'b':
                            buf[j++] = '\b';
                            break;
                        case 'f':
                            buf[j++] = '\f';
                            break;
                        default:
                            buf[j++] = src_utf8[i];
                            break;
                    }
                } else {
                    buf[j++] = '\\';
                }
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape Back-slash, Double-quote and others for inside Double-quote (ex. " --> \")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Escaped text)
 */
String& String::escapeCommandParam(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                switch (src_utf8[i]) {
                    case '\\':
                        buf[j++] = '\\';
                        buf[j++] = '\\';
                        break;
                    case '"':
                        buf[j++] = '\\';
                        buf[j++] = '"';
                        break;
                    case '`':
                        buf[j++] = '\\';
                        buf[j++] = '`';
                        break;
                    case '$':
                        buf[j++] = '\\';
                        buf[j++] = '$';
                        break;
                    default:
                        buf[j++] = src_utf8[i];
                        break;
                }
            } else {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Escape Back-slash, Double-quote, Single-quote (ex. " --> \")
    @param src_charset   Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset  Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Escaped text)
 */
String& String::escapeQuote(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                switch (src_utf8[i]) {
                    case '\\':
                        buf[j++] = '\\';
                        buf[j++] = '\\';
                        break;
                    case '\r':
                        buf[j++] = '\\';
                        buf[j++] = 'r';
                        break;
                    case '\n':
                        buf[j++] = '\\';
                        buf[j++] = 'n';
                        break;
                    case '\t':
                        buf[j++] = '\\';
                        buf[j++] = 't';
                        break;
                    case '\b':
                        buf[j++] = '\\';
                        buf[j++] = 'b';
                        break;
                    case '\f':
                        buf[j++] = '\\';
                        buf[j++] = 'f';
                        break;
                    case '"':
                        buf[j++] = '\\';
                        buf[j++] = '"';
                        break;
                    case '\'':
                        buf[j++] = '\\';
                        buf[j++] = '\'';
                        break;
                    default:
                        buf[j++] = src_utf8[i];
                        break;
                }
            } else {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Unescape Back-slash, Double-quote, Single-quote (ex. \" --> ")
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Unescaped text)
 */
String& String::unescapeQuote(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '\\') {
                if (i + 1 < length) {
                    i++;
                    switch (src_utf8[i]) {
                        case 'r':
                            buf[j++] = '\r';
                            break;
                        case 'n':
                            buf[j++] = '\n';
                            break;
                        case 't':
                            buf[j++] = '\t';
                            break;
                        case 'b':
                            buf[j++] = '\b';
                            break;
                        case 'f':
                            buf[j++] = '\f';
                            break;
                        case 'v':
                            buf[j++] = '\v';
                            break;
                        default:
                            buf[j++] = src_utf8[i];
                            break;
                    }
                } else {
                    buf[j++] = '\\';
                }
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}

/*! Escape CSV (ex. " --> "")
    @param src_charset  Character set of input. (ex. "UTF-8", "SHIFT_JIS", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "SHIFT_JIS", etc..)
    @param return_str  String of return code (ex. "\r\n")
    @return Temporary string object (Escaped text)
 */
String& String::escapeCSV(const char *src_charset, const char *dest_charset, const char *return_str) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '"') {
                buf[j++] = '"';
                buf[j++] = src_utf8[i];
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return (*tmp).changeReturnCode(return_str);
}


/*! Unescape CSV (ex. "" --> ")
    @param src_charset  Character set of input. (ex. "UTF-8", "SHIFT_JIS", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "SHIFT_JIS", etc..)
    @param return_str  String of return code (ex. "\r\n")
    @return Temporary string object (Unescaped text)
 */
String& String::unescapeCSV(const char *src_charset, const char *dest_charset, const char *return_str) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[(length * 2) + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (src_utf8[i] == '"') {
                if (i + 1 < length) {
                    i++;
                    switch (src_utf8[i]) {
                        case '"':
                            buf[j++] = '"';
                            break;
                        default:
                            buf[j++] = src_utf8[i];
                            break;
                    }
                } else {
                    buf[j++] = '"';
                }
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return (*tmp).changeReturnCode(return_str);
}


/*! Escape file name (ex. " --> "")
 *     @param src_charset  Character set of input. (ex. "UTF-8", "SHIFT_JIS", etc..)
 *         @param dest_charset Character set of output. (ex. "UTF-8", "SHIFT_JIS", etc..)
 *             @return Temporary string object (Escaped text)
 *              */
String& String::escapeFileName(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long i, j, k;
    long length;
    int u8len;
    char c;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        length = strlen(src_utf8);
        buf = new char[length + 1];

        i = 0;
        j = 0;
        while (i < length) {
            u8len = utf8_len(src_utf8[i]);
            if (u8len == 1) {
                c = src_utf8[i];
                if ((c & 0x1F) == 0 || (j == 0 && c == '.')) {
                    buf[j++] = '_';
                } else {
                    switch (c) {
                        case '\\':
                        case '/':
                        case ':':
                        case ';':
                        case '*':
                        case '?':
                        case '"':
                        case '<':
                        case '>':
                        case '|':
                        case '\t':
                            buf[j++] = '_';
                            break;
                        default:
                            buf[j++] = src_utf8[i];
                            break;
                    }
                }
            } else if (1 < u8len) {
                k = 0;
                while (i + k < length && k < u8len) {
                    if (k == 0 || (0x80 & src_utf8[i + k]) != 0) {
                        buf[j++] = src_utf8[i + k];
                    } else {
                        break;
                    }
                    k++;
                }
                i += k;
                continue;
            } else {
                buf[j++] = src_utf8[i];
            }
            i += u8len;
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Encode data to HEX
    @param void
    @return Temporary string object (Encoded text)
 */
String& String::encodeHEX() const {
    String *tmp = (*this).tmpStr();
    const unsigned char hex[] = "0123456789ABCDEF";
    long i, length;

    length = (*this).len();

    *tmp = "";
    (*tmp).setFixedLength(length * 2 + 1);
    (*tmp).useAsText();

    for (i = 0; i < length; i++) {
        (*tmp)[i * 2] += hex[((unsigned char)(*this).pText[i]) >> 4];
        (*tmp)[i * 2 + 1] += hex[((unsigned char)(*this).pText[i]) & 0x0F];
    }
    (*tmp)[i * 2] = '\0';

    return *tmp;
}


/*! Decode HEX encoded text
    @param void
    @return Temporary string object (Decoded data)
 */
String& String::decodeHEX() const {
    String *tmp = (*this).tmpStr();
    long i, length;
    const int hextable[128] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,
        0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    };

    (*tmp).useAsText();
    *tmp = "";

    length = (*this).len();

    for (i = 0; i < length; i++) {
        if (i + 1 < length && (isalnum((*this).pText[i]) && isalnum((*this).pText[i+1]))) {
            (*tmp) += (unsigned char)((hextable[(unsigned char)(*this).pText[i]] << 4) &
                                      hextable[(unsigned char)(*this).pText[i+1]]);
            i++;
        } else {
            (*tmp) += (*this).pText[i];
        }
    }

    return *tmp;
}


/*! Decode UUencoded text
    @param filename  Detected filename
    @return Temporary string object (Decoded data)
 */
String& String::decodeUU(String * filename) const {
    String *tmp = (*this).tmpStr();
    long begin_pos, end_pos, content_len;
    String line_str;
    long i, j, length;
    int n;

    *tmp = "";
    if (filename != (String *)NULL) {
        *filename = "";
    }

    // format check ("begin nnn FILENAME\n" ... "\nend")
    begin_pos = 0;
    end_pos = -1;
    while (0 <= (begin_pos = (*this).search("begin ", begin_pos))) {
        if ((int)(*this).len() < begin_pos + 15) {
            // "begin nnn FILENAME" is not found
            begin_pos = -1;
            break;
        }
        if (!isdigit((*this).pText[begin_pos + 6]) || !isdigit((*this).pText[begin_pos + 7]) ||
                !isdigit((*this).pText[begin_pos + 8]) || (*this).pText[begin_pos + 9] != ' ' ||
                (*this).pText[begin_pos + 10] == '\0' || (*this).pText[begin_pos + 10] == '\r' ||
                (*this).pText[begin_pos + 10] == '\n') {
            // this is not "begin nnn FILENAME" line
            continue;
        }
        if (0 <= (end_pos = (*this).search("\r\nend", begin_pos + 11)) ||
                0 <= (end_pos = (*this).search("\nend", begin_pos + 11))) {
            // found "begin nnn FILENAME" ... "\nend" pattern!
            if (filename != (String *)NULL) {
                int filename_len = (*this).search("\r\n", begin_pos + 10);
                if (0 <= filename_len) {
                    filename_len -= (begin_pos + 10);
                    *filename = (*this).mid(begin_pos + 10, filename_len);
                }
            }
            break;
        }
    }
    if (begin_pos < 0 || end_pos < 0) {
        // pattern is not found
        return *tmp;
    }
    begin_pos = (*this).searchChar('\n', begin_pos + 10) + 1;
    content_len = end_pos - begin_pos;

    // decode uuencoded text
    (*tmp).useAsBinary(0);
    line_str = "";
    for (i = 0; i < content_len; i++) {
        if ((*this).pText[begin_pos + i] == '\r' || (*this).pText[begin_pos + i] == '\n') {
            continue;
        }
        length = (*this).search("\r\n", begin_pos + i);
        if (0 <= length) {
            length -= (begin_pos + i); // line_str length
            line_str = (*this).mid(begin_pos + i, length);
        } else {
            line_str = (*this).mid(begin_pos + i, end_pos - (begin_pos + i));
            length = line_str.len(); // line_str length
        }

        n = (((line_str[0]) - ' ') & 077);

        // output a group of 3 bytes (4 input characters).
        for (j = 1; 0 < n && j < length; j += 4) {
            unsigned char c1, c2, c3;

            c1 = ((((line_str[j]) - ' ') & 077) << 2) | ((((line_str[j + 1]) - ' ') & 077) >> 4);
            c2 = ((((line_str[j + 1]) - ' ') & 077) << 4) | ((((line_str[j + 2]) - ' ') & 077) >> 2);
            c3 = ((((line_str[j + 2]) - ' ') & 077) << 6) | (((line_str[j + 3]) - ' ') & 077);
            if (1 <= n) {
                (*tmp).addBinary((char)c1);
            }
            if (2 <= n) {
                (*tmp).addBinary((char)c2);
            }
            if (3 <= n) {
                (*tmp).addBinary((char)c3);
            }

            n -= 3;
        }

        if (0 < length) {
            i += (length - 1);
        }
    }

    line_str.clear();
    return *tmp;
}


/*! Encode text to MIME text.
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param max_width    Max width of line
    @param return_str   String of return code (ex. "\r\n")
    @param enctype      Encode type ('B' or 'Q')
    @return Temporary string object (Encoded text)
 */
String& String::encodeMIME(const char * src_charset, const char * dest_charset, long max_width, const char * return_str, const char enctype) const {
    String *tmp = (*this).tmpStr();
    String *str = new String;
    char prefix_str[37];
    long i, j, length, sub_len;
    char *p;

    if (src_charset == NULL || src_charset[0] == '\0') {
        src_charset = (*this).detectCharSet();
    }
    if (dest_charset == NULL || dest_charset[0] == '\0') {
        dest_charset = src_charset;
    }

    if (enctype == 'q' || enctype == 'Q') {
        prefix_str[0] = '=';
        prefix_str[1] = '?';
        memcpy(prefix_str+2, dest_charset, 32);
        prefix_str[34] = '\0';
        p = strcasestr(prefix_str, "_BOM");
        if (p) *p = '\0';
        p = strcasestr(prefix_str, "-BOM");
        if (p) *p = '\0';
        i = strlen(prefix_str);
        prefix_str[i++] = '?';
        prefix_str[i++] = 'Q';
        prefix_str[i++] = '?';
        prefix_str[i] = '\0';
    } else {
        prefix_str[0] = '=';
        prefix_str[1] = '?';
        memcpy(prefix_str+2, dest_charset, 32);
        prefix_str[34] = '\0';
        p = strcasestr(prefix_str, "_BOM");
        if (p) *p = '\0';
        p = strcasestr(prefix_str, "-BOM");
        if (p) *p = '\0';
        i = strlen(prefix_str);
        prefix_str[i++] = '?';
        prefix_str[i++] = 'B';
        prefix_str[i++] = '?';
        prefix_str[i] = '\0';
    }

    if (max_width <= 0 || return_str == NULL || return_str[0] == '\0') {
        *str = (*this).strconv(src_charset, dest_charset);
        if (enctype == 'q' || enctype == 'Q') {
            *tmp = (*str).encodeQuotedPrintable(0, "", prefix_str, "?=");
        } else {
            *tmp = (*str).encodeBASE64(0, "", prefix_str, "?=");
        }
    } else {
        String *utf8_str = new String;
        long _max;
        int u8len;
        if (enctype == 'q' || enctype == 'Q') {
            _max = max_width / 3 - 3;
            if (_max <= 0) _max = max_width / 3 + 1;
        } else {
            _max = max_width * 2 / 3 + 1;
        }
        *utf8_str = (*this).strconv(src_charset, STR_UTF8);
        length = (*utf8_str).len();
        *tmp = "";
        i = 0;
        while (i < length) {
            sub_len = 0;
            j = i;
            while (sub_len < _max && j < length) {
                u8len = utf8_len((*utf8_str)[j]);
                sub_len += u8len;
                j += u8len;
            }
            *str = (*utf8_str).mid(i, sub_len).strconv(STR_UTF8, dest_charset);
            if (enctype == 'q' || enctype == 'Q') {
                *tmp += (*str).encodeQuotedPrintable(0, "", prefix_str, "?=");
            } else {
                *tmp += (*str).encodeBASE64(0, "", prefix_str, "?=");
            }
            if (i + sub_len < length) {
                *tmp += return_str;
            }
            i += sub_len;
        }

        delete *utf8_str;
    }

    delete *str;

    return *tmp;
}


/*! Decode MIME text.
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Decoded text)
 */
String& String::decodeMIME(const char * src_charset, const char * dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf;
    String str;
    long i, j, col, length;
    long charset_s, charset_len;
    unsigned char *charset;
    unsigned char a, a0, a1, b0, b1;
    bool isMimed;
    enum {B, Q} mime_type;

    *tmp = "";

    if ((*this).pText) {
        // Memory allocation
        length = (*this).len();
        buf = new char[((length * 3) / 4) + 5];

        // decoding BASE64 or Quoted-Printable
        col = 0;
        i = j = 0;
        isMimed = false;
        mime_type = B;
        charset_s = 0;
        charset_len = 0;
        while (i < length) {
            a = (*this).pText[i];
            if (isMimed == true && a == '?' && i + 1 < length && (*this).pText[i + 1] == '=') {
                long ii;

                i += 2;
                a = (*this).pText[i];

                // ignore all return code, white space and tab code appears after '='
                while (a == '\n' || a == '\r') {
                    i++;
                    a = (*this).pText[i];
                    if (a == '\t') {
                        i++;
                        a = (*this).pText[i];
                    }
                }
                ii = i;
                while (ii < length && isspace((*this).pText[ii])) ii++;
                if ((*this).pText[ii] == '=') {
                    i = ii;
                    a = (*this).pText[i];
                }
                isMimed = false;
                if (0 < col) {
                    buf[col] = '\0';
                    str = buf;
                    if (0 < charset_len) {
                        charset = new unsigned char[charset_len + 1];
                        for (ii = 0; ii < charset_len; ii++) {
                            charset[ii] = (*this).pText[charset_s + ii];
                        }
                        charset[ii] = '\0';
                        (*tmp).add(str.strconv((const char *)charset, dest_charset));
                        delete [] charset;
                    } else {
                        (*tmp).add(str.strconv(src_charset, dest_charset));
                    }
                }
                charset_len = 0;
                col = 0;
            }
            if (isMimed == false && a == '=' && i + 1 < length && (*this).pText[i + 1] == '?') {
                long ii, question_count;
                if (0 < col) {
                    buf[col] = '\0';
                    str = buf;
                    (*tmp).add(str.strconv(src_charset, dest_charset));
                    col = 0;
                }
                ii = i + 2;
                charset_s = ii;
                charset_len = 0;
                question_count = 1;
                while (ii < length && question_count < 3) {
                    if ((*this).pText[ii] == '?') {
                        question_count++;
                        if (question_count == 2) {
                            charset_len = ii - charset_s;
                            if ((*this).pText[ii + 1] == 'Q' || (*this).pText[ii + 1] == 'q') {
                                mime_type = Q;
                            } else {
                                mime_type = B;
                            }
                        }
                    }
                    ii++;
                }
                if (question_count == 3) {
                    i = ii;
                    a = (*this).pText[i];
                }
                isMimed = true;
            }
            if (isMimed == false) {
                while (a == '\n' || a == '\r') {
                    i++;
                    a = (*this).pText[i];
                    if (a == '\t') {
                        i++;
                        a = (*this).pText[i];
                    }
                }

                buf[col++] = a;
                j = 0;
                i++;
                continue;
            }
            i++;

            if (mime_type == B) {
                if ('A' <= a && a <= 'Z') {
                    a -= 'A';
                } else if ('a' <= a && a <= 'z') {
                    a -= 'a' - 26;
                } else if ('0' <= a && a <= '9') {
                    a -= '0' - 52;
                } else if (a == '+') {
                    a = 62;
                } else if (a == '/') {
                    a = 63;
                } else if (a == '=') {
                    if ((j++) == 3) {
                        j = 0;
                        col--;
                    }
                    continue;
                } else {
                    continue;
                }

                switch (j++) {
                    case 0:
                        buf[col++] = a << 2;
                        break;
                    case 1:
                        buf[col-1] |= a >> 4;
                        buf[col++] = a << 4;
                        break;
                    case 2:
                        buf[col-1] |= a >> 2;
                        buf[col++] = a << 6;
                        break;
                    case 3:
                        buf[col-1] |= a;
                        j = 0;
                        break;
                }
            } else { // mime_type == Q
                if (i + 1 < length && a == '=') {
                    a0 = (*this).pText[i];
                    a1 = (*this).pText[i + 1];
                    if ((('0' <= a0 && a0 <= '9') ||
                            ('A' <= a0 && a0 <= 'F') ||
                            ('a' <= a0 && a0 <= 'f')) &&
                            (('0' <= a1 && a1 <= '9') ||
                             ('A' <= a1 && a1 <= 'F') ||
                             ('a' <= a1 && a1 <= 'f')) ) {
                        switch (a0) {
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                b0 = (a0 - '0') << 4;
                                break;
                            case 'A':
                            case 'a':
                                b0 = 0xA0;
                                break;
                            case 'B':
                            case 'b':
                                b0 = 0xB0;
                                break;
                            case 'C':
                            case 'c':
                                b0 = 0xC0;
                                break;
                            case 'D':
                            case 'd':
                                b0 = 0xD0;
                                break;
                            case 'E':
                            case 'e':
                                b0 = 0xE0;
                                break;
                            case 'F':
                            case 'f':
                                b0 = 0xF0;
                                break;
                            default:
                                b0 = 0x00;
                                break;
                        }
                        switch (a1) {
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                b1 = a1 - '0';
                                break;
                            case 'A':
                            case 'a':
                                b1 = 0x0A;
                                break;
                            case 'B':
                            case 'b':
                                b1 = 0x0B;
                                break;
                            case 'C':
                            case 'c':
                                b1 = 0x0C;
                                break;
                            case 'D':
                            case 'd':
                                b1 = 0x0D;
                                break;
                            case 'E':
                            case 'e':
                                b1 = 0x0E;
                                break;
                            case 'F':
                            case 'f':
                                b1 = 0x0F;
                                break;
                            default:
                                b1 = 0x00;
                                break;
                        }
                        buf[col++] = b0 | b1;
                        i += 2;
                    } else {
                        buf[col++] = a;
                    }
                } else if (a == '_') {
                    buf[col++] = ' ';
                } else {
                    buf[col++] = a;
                }
            }
        }

        if (0 < col) {
            buf[col] = '\0';
            str = buf;
            (*tmp).add(str.strconv(src_charset, dest_charset));
            col = 0;
        }

        delete [] buf;
    }

    str.clear();
    return *tmp;
}


/*! Encode text to BASE64 text.
    @param max_width   Output size of a line
    @param return_str  String of return code (ex. "\r\n\t")
    @param prefix_str  Prefix of MIME encoding (ex. "=?ISO-2022-JP?B?")
    @param suffix_str  Suffix of MIME encoding (ex. "?=")
    @return Temporary string object (Encoded text)
 */
String& String::encodeBASE64(long max_width, const char * return_str, const char * prefix_str, const char * suffix_str) const {
    String *tmp = (*this).tmpStr();
    char *buf;
    long i, j, width, length;
    long return_len, prefix_len, suffix_len;
    long str_pos, row_ext;
    unsigned char a0, a1, a2;
    const char s[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    *tmp = "";

    return_len = (return_str != NULL)?strlen(return_str):0;
    prefix_len = (prefix_str != NULL)?strlen(prefix_str):0;
    suffix_len = (suffix_str != NULL)?strlen(suffix_str):0;

    length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();

    if (0 <= max_width && 0 < length) {
        long linesize, ls, bufsize;

        // Memory allocation
        if (max_width == 0) {
            width = length;
        } else {
            width = max_width;
        }
        linesize = (0 < (ls = width-(return_len+prefix_len+suffix_len)))?ls:1;
        bufsize = length*5/3 + ((length*2)/linesize + 1)*(return_len+prefix_len+suffix_len) + 10;
        buf = new char[bufsize];

        // encoding BASE64
        j = 0;
        row_ext = - suffix_len;
        str_pos = 0;
        while (str_pos < prefix_len) {
            buf[j] = prefix_str[str_pos];
            j++;
            str_pos++;
        }
        for (i = 0; i < length; i += 3) {
            a0 = (i < length)?(*this).pText[i]:'\0';
            a1 = (i+1 < length)?(*this).pText[i+1]:'\0';
            a2 = (i+2 < length)?(*this).pText[i+2]:'\0';
            if (0 < max_width && 0 < j && i < length - 1 && (j-row_ext) % max_width == 0) {
                str_pos = 0;
                while (str_pos < suffix_len) {
                    buf[j] = suffix_str[str_pos];
                    j++;
                    str_pos++;
                }
                str_pos = 0;
                while (str_pos < return_len) {
                    buf[j] = return_str[str_pos];
                    j++;
                    str_pos++;
                }
                row_ext += return_len;
                str_pos = 0;
                while (str_pos < prefix_len) {
                    buf[j] = prefix_str[str_pos];
                    j++;
                    str_pos++;
                }
            }
            buf[j++] = s[(a0 & 0xfc) >> 2];
            if (0 < max_width && 0 < j && i < length - 1 && (j-row_ext) % max_width == 0) {
                str_pos = 0;
                while (str_pos < suffix_len) {
                    buf[j] = suffix_str[str_pos];
                    j++;
                    str_pos++;
                }
                str_pos = 0;
                while (str_pos < return_len) {
                    buf[j] = return_str[str_pos];
                    j++;
                    str_pos++;
                }
                row_ext += return_len;
                str_pos = 0;
                while (str_pos < prefix_len) {
                    buf[j] = prefix_str[str_pos];
                    j++;
                    str_pos++;
                }
            }
            buf[j++] = s[((a0 & 0x03) << 4) | ((a1 & 0xF0) >> 4)];
            if (0 < max_width && 0 < j && i < length - 1 && (j-row_ext) % max_width == 0) {
                str_pos = 0;
                while (str_pos < suffix_len) {
                    buf[j] = suffix_str[str_pos];
                    j++;
                    str_pos++;
                }
                str_pos = 0;
                while (str_pos < return_len) {
                    buf[j] = return_str[str_pos];
                    j++;
                    str_pos++;
                }
                row_ext += return_len;
                str_pos = 0;
                while (str_pos < prefix_len) {
                    buf[j] = prefix_str[str_pos];
                    j++;
                    str_pos++;
                }
            }
            buf[j++] = (i+1 < length)?s[((a1 & 0x0F) << 2) | ((a2 & 0xc0) >> 6)]:'=';
            if (0 < max_width && 0 < j && i < length - 1 && (j-row_ext) % max_width == 0) {
                str_pos = 0;
                while (str_pos < suffix_len) {
                    buf[j] = suffix_str[str_pos];
                    j++;
                    str_pos++;
                }
                str_pos = 0;
                while (str_pos < return_len) {
                    buf[j] = return_str[str_pos];
                    j++;
                    str_pos++;
                }
                row_ext += return_len;
                str_pos = 0;
                while (str_pos < prefix_len) {
                    buf[j] = prefix_str[str_pos];
                    j++;
                    str_pos++;
                }
            }
            buf[j++] = (i+2 < length)?s[a2 & 0x3f]:'=';
        }
        str_pos = 0;
        while (str_pos < suffix_len) {
            buf[j] = suffix_str[str_pos];
            j++;
            str_pos++;
        }
        buf[j] = '\0';
        (*tmp).useAsText();
        (*tmp).pTextReplace(buf, j, -1, bufsize);
    }

    return *tmp;
}


/*! Decode BASE64 text.
    @param void
    @return Temporary string object (Decoded text)
 */
String& String::decodeBASE64() const {
    String *tmp = (*this).tmpStr();
    char *buf;
    long i, j, col, length;
    unsigned char a0;

    *tmp = "";

    if ((*this).pText) {
        // Memory allocation
        length = (*this).len();
        buf = new char[((length * 3) / 4) + 5];

        // decoding BASE64
        col = 0;
        i = j = 0;
        while (i < length) {
            a0 = (i < length)?(*this).pText[i]:'\0';
            if (a0 == '?' && i + 1 < length && (*this).pText[i + 1] == '=') {
                i += 2;
                a0 = (*this).pText[i];

                // ignore all return code, white space and tab code appears after '='
                while (a0 == '\n' || a0 == '\r' || a0 == ' ' || a0 == '\t') {
                    i++;
                    a0 = (*this).pText[i];
                }
            }
            if (a0 == '=' && i + 1 < length && (*this).pText[i + 1] == '?') {
                long ii, question_count;
                ii = i + 2;
                question_count = 1;
                while (ii < length && question_count < 3) {
                    if ((*this).pText[ii] == '?') question_count++;
                    ii++;
                }
                if (question_count == 3) {
                    i = ii;
                    a0 = (*this).pText[i];
                }
            }
            i++;
            if ('A' <= a0 && a0 <= 'Z') {
                a0 -= 'A';
            } else if ('a' <= a0 && a0 <= 'z') {
                a0 -= 'a' - 26;
            } else if ('0' <= a0 && a0 <= '9') {
                a0 -= '0' - 52;
            } else if (a0 == '+') {
                a0 = 62;
            } else if (a0 == '/') {
                a0 = 63;
            } else if (a0 == '=') {
                if ((j++) == 3) j = 0;
                continue;
            } else {
                continue;
            }

            switch (j++) {
                case 0:
                    buf[col++] = a0 << 2;
                    break;
                case 1:
                    buf[col-1] |= a0 >> 4;
                    buf[col++] = a0 << 4;
                    break;
                case 2:
                    buf[col-1] |= a0 >> 2;
                    buf[col++] = a0 << 6;
                    break;
                case 3:
                    buf[col-1] |= a0;
                    j = 0;
                    break;
            }
        }
        buf[col] = '\0';
        (*tmp).pTextReplace((char *)buf, -1, col, ((length * 3) / 4) + 5);
        (*tmp).useAsBinary(col);
    }

    return *tmp;
}


/*! Encode text to Quoted-printable text.
    @param max_width   Output size of a line
    @param return_str  String of return code (ex. "\r\n\t")
    @param prefix_str  Prefix of MIME encoding (ex. "=?ISO-2022-JP?Q?")
    @param suffix_str  Suffix of MIME encoding (ex. "?=")
    @return Temporary string object (Encoded text)
 */
String& String::encodeQuotedPrintable(long max_width, const char * return_str, const char * prefix_str, const char * suffix_str) const {
    String *tmp = (*this).tmpStr();
    char *buf;
    long i, j, width, length;
    long return_len, prefix_len, suffix_len;
    long str_pos, row_ext;
    unsigned char a, a0, a1;

    *tmp = "";

    return_len = (return_str != NULL)?strlen(return_str):0;
    prefix_len = (prefix_str != NULL)?strlen(prefix_str):0;
    suffix_len = (suffix_str != NULL)?strlen(suffix_str):0;

    length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();

    if (0 <= max_width && 0 < length) {
        int linesize, ls, bufsize;

        // Memory allocation
        if (max_width == 0) {
            width = length;
        } else {
            width = max_width;
        }
        linesize = (0 < (ls = width-(return_len+prefix_len+suffix_len)))?ls:1;
        bufsize = length*3 + (length/linesize + 2)*(return_len+prefix_len+suffix_len + 1)*(linesize+1) + 10;
        buf = new char[bufsize];

        // encoding Quoted-Printable
        j = 0;
        row_ext = -suffix_len;
        str_pos = 0;
        while (str_pos < prefix_len) {
            buf[j] = prefix_str[str_pos];
            j++;
            str_pos++;
        }
        for (i = 0; i < length; i++) {
            a = (*this).pText[i];
            if (a < 32 || a == '=' || a == '?' || a == '_' || 126 < a) {
                a0 = ((a & 0xF0) >> 4);
                a0 = (a0 < 10)?'0'+a0:'A'-10+a0;
                a1 = (a & 0x0F);
                a1 = (a1 < 10)?'0'+a1:'A'-10+a1;
                if (0 < max_width-(suffix_len+1) && 0 < j && ((j-row_ext) % (max_width-(suffix_len+1)) == 0 || (j-row_ext+1) % (max_width-(suffix_len+1)) == 0 || (j-row_ext+2) % (max_width-(suffix_len+1)) == 0)) {
                    buf[j] = '=';
                    j++;
                    str_pos = 0;
                    while (str_pos < suffix_len) {
                        buf[j] = suffix_str[str_pos];
                        j++;
                        str_pos++;
                    }
                    str_pos = 0;
                    while (str_pos < return_len) {
                        buf[j] = return_str[str_pos];
                        j++;
                        str_pos++;
                    }
                    row_ext += return_len;
                    str_pos = 0;
                    while (str_pos < prefix_len) {
                        buf[j] = prefix_str[str_pos];
                        j++;
                        str_pos++;
                    }
                }
                buf[j] = '=';
                buf[j + 1] = a0;
                buf[j + 2] = a1;
                j += 3;
            } else {
                if (0 < max_width-(suffix_len+1) && 0 < j && (((j-row_ext) % (max_width-(suffix_len+1))) == 0 || ((j-row_ext+1) % (max_width-(suffix_len+1))) == 0)) {
                    buf[j] = '=';
                    j++;
                    str_pos = 0;
                    while (str_pos < suffix_len) {
                        buf[j] = suffix_str[str_pos];
                        j++;
                        str_pos++;
                    }
                    str_pos = 0;
                    while (str_pos < return_len) {
                        buf[j] = return_str[str_pos];
                        j++;
                        str_pos++;
                    }
                    row_ext += return_len;
                    str_pos = 0;
                    while (str_pos < prefix_len) {
                        buf[j] = prefix_str[str_pos];
                        j++;
                        str_pos++;
                    }
                }
                buf[j] = (*this).pText[i];
                j++;
            }
        }
        str_pos = 0;
        while (str_pos < suffix_len) {
            buf[j] = suffix_str[str_pos];
            j++;
            str_pos++;
        }
        buf[j] = '\0';
        (*tmp).useAsText();
        (*tmp).pTextReplace(buf, j, -1, bufsize);
    }

    return *tmp;
}


/*! Decode Quoted-printable text
    @param void
    @return Temporary string object (Decoded text)
 */
String& String::decodeQuotedPrintable() const {
    String *tmp = (*this).tmpStr();
    char *buf;
    long i, j, length;
    unsigned char a, a0, a1, b0, b1;
    bool isMimed;

    *tmp = "";

    if ((*this).pText) {
        // Memory allocation
        length = (*this).len();
        buf = new char[length + 1];

        // decoding Quoted-Printable
        i = j = 0;
        isMimed = false;
        while (i < length) {
            a = (*this).pText[i];
            if (a == '=' && i + 2 < length && (*this).pText[i + 1] == '?') {
                long ii, question_count;
                ii = i + 2;
                question_count = 1;
                if ((*this).pText[i + 2] == '=') {
                    i += 3;
                } else {
                    while (ii < length && question_count < 3) {
                        if ((*this).pText[ii] == '\n' || (*this).pText[ii] == '\r' || (*this).pText[ii] == ' ' || (*this).pText[ii] == '\t') break;
                        if ((*this).pText[ii] == '?') question_count++;
                        ii++;
                    }
                    if (question_count == 3) {
                        i = ii;
                        a = (*this).pText[i];
                        isMimed = true;
                    }
                }
            }
            if (i + 2 < length && a == '=') {
                a0 = (*this).pText[i + 1];
                a1 = (*this).pText[i + 2];
                if ((('0' <= a0 && a0 <= '9') ||
                        ('A' <= a0 && a0 <= 'F') ||
                        ('a' <= a0 && a0 <= 'f')) &&
                        (('0' <= a1 && a1 <= '9') ||
                         ('A' <= a1 && a1 <= 'F') ||
                         ('a' <= a1 && a1 <= 'f')) ) {
                    switch (a0) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            b0 = (a0 - '0') << 4;
                            break;
                        case 'A':
                        case 'a':
                            b0 = 0xA0;
                            break;
                        case 'B':
                        case 'b':
                            b0 = 0xB0;
                            break;
                        case 'C':
                        case 'c':
                            b0 = 0xC0;
                            break;
                        case 'D':
                        case 'd':
                            b0 = 0xD0;
                            break;
                        case 'E':
                        case 'e':
                            b0 = 0xE0;
                            break;
                        case 'F':
                        case 'f':
                            b0 = 0xF0;
                            break;
                        default:
                            b0 = 0x00;
                            break;
                    }
                    switch (a1) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            b1 = a1 - '0';
                            break;
                        case 'A':
                        case 'a':
                            b1 = 0x0A;
                            break;
                        case 'B':
                        case 'b':
                            b1 = 0x0B;
                            break;
                        case 'C':
                        case 'c':
                            b1 = 0x0C;
                            break;
                        case 'D':
                        case 'd':
                            b1 = 0x0D;
                            break;
                        case 'E':
                        case 'e':
                            b1 = 0x0E;
                            break;
                        case 'F':
                        case 'f':
                            b1 = 0x0F;
                            break;
                        default:
                            b1 = 0x00;
                            break;
                    }
                    buf[j] = b0 | b1;
                    i += 2;
                    j++;
                } else {
                    // ignore all return code, white space and tab code appears after '='
                    while (i + 1 < length && (a0 == '\n' || a0 == '\r' || a0 == ' ' || a0 == '\t')) {
                        i++;
                        a0 = (*this).pText[i + 1];
                    }
                }
            } else {
                if (i + 1 < length && a == '=') {
                    a0 = (*this).pText[i + 1];

                    // ignore all return code, white space and tab code appears after '='
                    while (a0 == '\n' || a0 == '\r' || a0 == ' ' || a0 == '\t') {
                        i++;
                        a0 = (*this).pText[i + 1];
                    }
                } else if (isMimed == true && a == '_') {
                    buf[j] = ' ';
                    j++;
                } else if (a != '?') {
                    buf[j] = (*this).pText[i];
                    j++;
                } else if (i + 1 < length && (*this).pText[i + 1] == '=') {
                    isMimed = false;
                    i++;
                }
            }
            i++;
        }
        buf[j] = '\0';
        (*tmp).pTextReplace((char *)buf, -1, j, length + 1);
        (*tmp).useAsBinary(j);
    }

    return *tmp;
}


/*!
    Parse To, From style email address like "name <user@domain>".
    See List::setEmails().
    @param  name   name of "name <user@domain>"
    @param  user   user of "name <user@domain>"
    @param  domain domain of "name <user@domain>"
    @retval true  success
    @retval false failure
 */
bool String::parseEmail(String& name, String& user, String& domain, const char * dest_charset) const {
    String email_str = (*this).pText;
    String address;
    long i, j, k, length;
    bool mime_decoding = false;

    name = "";
    user = "";
    domain = "";

    email_str = email_str.trim();
    if (email_str[0] == '\0') {
        return false;
    }

    if (dest_charset != NULL && dest_charset[0] != '\0') {
        mime_decoding = true;
    }

    // Set name and address
    length = email_str.len();
    if (email_str[0] == '"') {
        // "Sasaki, Tarou" <tarou@example.com>
        // "Sasaki, Tarou" tarou@example.com
        char quote = 1;
        j = 1;
        for (i = 1; i < length; i++) {
            if (email_str[i] == '"') {
                quote = 1 - quote;
                if (quote == 0) {
                    name += email_str.mid(j, i - 1);
                } else {
                    j = i + 1;
                }
                continue;
            }
            if (quote == 0 && !isspace(email_str[i])) {
                if (email_str[i] == '<') {
                    k = email_str.search('>', i + 1);
                    if (0 < k) {
                        address = email_str.mid(i + 1, k - (i + 1));
                    } else {
                        address = email_str.mid(i + 1);
                    }
                } else {
                    address = email_str.mid(i);
                }
                break;
            }
        }
    } else if (email_str[0] != '<' && 0 < (i = email_str.search('<', 1))) {
        // Tarou Sasaki <tarou@example.com>
        name = email_str.left(i).trim();
        j = email_str.search('>', i + 1);
        if (i < j) {
            address = email_str.mid(i + 1, j - (i + 1));
        } else {
            address = email_str.mid(i + 1);
        }
    } else if (email_str[0] == '<') {
        // <tarou@example.com>
        i = email_str.search('>', 1);
        if (0 < i) {
            address = email_str.mid(1, i - 1);
        } else {
            address = email_str.mid(1);
        }
    } else {
        // Tarou Sasaki tarou@example.com
        // tarou@example.com
        for (i = email_str.len()-1; 0 <= i; i--) {
            if (isspace(email_str[i])) {
                break;
            }
        }
        if (0 <= i) {
            name = email_str.left(i).trim();
            address = email_str.mid(i + 1);
        } else {
            address = email_str;
        }
    }
    name = name.replace("\r", "").replace("\n", "").replace("\t", " ");
    address = address.trim();
    if (address[0] != '\0') {
        address = address.replace("\r", "").replace("\n", "").replace(" ", "").replace("\t", "");
    }

    // Set user and domain
    if (address[0] != '\0') {
        i = address.search('@');
        if (i == 0) {
            user = "";
            domain = address.mid(1);
        } else if (0 < i) {
            user = address.left(i);
            domain = address.mid(i + 1);
        } else {
            user = address;
            domain = "";
        }
    }

    // MIME decoding
    if (mime_decoding == true) {
        name = name.decodeMIME("AUTODETECT", dest_charset);
    }

    if (user[0] == '\0') {
        return false;
    }

    if (!isEmailLocalPart(user)) {
        return false;
    }
    if (domain[0] != '\0' && !isEmailDomain(domain)) {
        return false;
    }

    return true;
}


/*! Crypt String
    @param void
    @return Crypted String
 */
String& String::crypt() const {
    String *tmp = (*this).tmpStr();
    long  i, j, length;
    int c;
    DateTime dt;
    String md5_str;

    dt.setNow();
    *tmp =  dt.toString("%02M%02S");
    md5_str = (*tmp).md5();
    length = (*this).len();
    (*tmp).setFixedLength((length * 2) + 10);
    j = 4;
    for (i = 0; i < length; i++) {
        c = (*this).pText[i];
        (*tmp).pText[j] = ((c >> 4) ^ (md5_str[(j-4)%32]));
        j++;
        (*tmp).pText[j] = ((c & 0x0F) ^ (md5_str[(j-4)%32]));
        j++;
    }
    j = 0;
    for (i = 0; i < 4; i++) {
        (*tmp).pText[i] ^= 4;
        j++;
    }
    for (i = 0; i < length; i++) {
        (*tmp).pText[j] ^= 4;
        j++;
        (*tmp).pText[j] ^= 4;
        j++;
    }
    (*tmp).pText[j] = '\0';
    md5_str.clear();
    (*tmp).resize(j);
    (*tmp).setFixedLength(0);
    return *tmp;
}


/*! Decrypt String
    @param void
    @return Decrypted String
 */
String& String::decrypt() const {
    String *tmp = (*this).tmpStr();
    long  i, length;
    int c, d;
    String md5_str, f4_str;

    *tmp = "";
    length = (*this).len();
    if (length <= 4 || length %2 != 0) {
        return *tmp;
    }
    f4_str = (*this);
    for (i = 0; i < length; i++) {
        f4_str[i] ^= 4;
    }
    md5_str = f4_str.left(4).md5();
    (*tmp).setFixedLength(length + 1);
    for (i = 4; i < length; i += 2) {
        c = ((f4_str.pText[i]) ^ (md5_str[(i-4)%32]));
        d = ((f4_str.pText[i+1]) ^ (md5_str[(i-4+1)%32]));
        (*tmp).pText[(i-4)/2] = (c << 4) + d;
    }
    (*tmp).pText[(i-4)/2] = '\0';
    md5_str.clear();
    f4_str.clear();
    (*tmp).resize((i-4)/2);
    (*tmp).setFixedLength(0);
    return *tmp;
}


/*! Culculate MD5 Digest key of String
    @param void
    @return Temporary string object (MD5 digest key = text of 32 byte HEX)
 */
String& String::md5() const {
    String *tmp = (*this).tmpStr();
#if __MD5 == 1
    long length;
    md5_state_t state;
    md5_byte_t digest[16];
    char hex_str[(16 * 2) + 1];
    int i;

    if ((*this).pText) {
        length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();
    } else {
        length = 0;
    }

    md5_init(&state);
    md5_append(&state, (const md5_byte_t *)((*this).pText?(*this).pText:""), length);
    md5_finish(&state, digest);
    for (i = 0; i < 16; i++) {
        unsigned char a0, a1;
        a0 = ((digest[i] & 0xF0) >> 4);
        a0 = (a0 < 10)?'0'+a0:'a'-10+a0;
        a1 = (digest[i] & 0x0F);
        a1 = (a1 < 10)?'0'+a1:'a'-10+a1;
        hex_str[i * 2] = a0;
        hex_str[(i * 2) + 1] = a1;
    }
    hex_str[16 * 2] = '\0';

    *tmp = hex_str;

#endif
    return *tmp;
}


/*! Culculate HMAC-MD5 key
    @param key HMAC-MD5 key
    @return Temporary string object (MD5 digest key = text of 32 byte HEX)
 */
String& String::hmacMd5(const String &key) const {
    String *tmp = (*this).tmpStr();
#if __MD5 == 1
    md5_byte_t digest[16];
    char hex_str[(16 * 2) + 1];
    long length;
    md5_state_t state;
    unsigned char k_ipad[65]; // inner padding
    unsigned char k_opad[65]; // outer padding
    int i;

    if ((*this).pText) {
        length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();
    } else {
        length = 0;
    }

    // MD5(K XOR opad, MD5(K XOR ipad, (*this).pText))
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    if (64 < key.len()) {
        md5_byte_t tk[16];
        md5_state_t tstate;

        md5_init(&tstate);
        md5_append(&tstate, (const md5_byte_t *)key.c_str(), key.len());
        md5_finish(&tstate, tk);

        memcpy(k_ipad, tk, 16);
        memcpy(k_opad, tk, 16);
    } else {
        memcpy(k_ipad, key.c_str(), key.len());
        memcpy(k_opad, key.c_str(), key.len());
    }
    for (i = 0; i < 64; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    // perform inner MD5
    md5_init(&state);
    md5_append(&state, (const md5_byte_t *)k_ipad, 64);
    md5_append(&state, (const md5_byte_t *)((*this).pText?(*this).pText:""), length);
    md5_finish(&state, (md5_byte_t *)digest);

    // perform outer MD5
    md5_init(&state);
    md5_append(&state, (const md5_byte_t *)k_opad, 64);
    md5_append(&state, (const md5_byte_t *)digest, 16);
    md5_finish(&state, (md5_byte_t *)digest);

    for (i = 0; i < 16; i++) {
        unsigned char a0, a1;
        a0 = ((digest[i] & (unsigned char)0xF0) >> 4);
        a0 = (a0 < 10)?'0'+a0:'a'-10+a0;
        a1 = (digest[i] & (unsigned char)0x0F);
        a1 = (a1 < 10)?'0'+a1:'a'-10+a1;
        hex_str[i * 2] = a0;
        hex_str[(i * 2) + 1] = a1;
    }
    hex_str[16 * 2] = '\0';

    *tmp = hex_str;

#endif
    return *tmp;
}


/*! Culculate CRAM-MD5 key by password
    @param user user id
    @param challenge challenge string
    @return Temporary string object (MD5 digest key = text of 32 byte HEX)
 */
String& String::cramMd5(const String & user, const String & challenge) const {
    String *tmp = (*this).tmpStr();
    String cram_tmp1, cram_tmp2, cram_tmp3, cram_tmp4;;

    cram_tmp1 = ((*this).pText)?(*this).pText:""; // password
    cram_tmp2 = challenge.decodeBASE64();
    cram_tmp2.useAsText();
    cram_tmp3 = cram_tmp2.hmacMd5(cram_tmp1);
    cram_tmp1.clear();
    cram_tmp2.clear();
    cram_tmp4 = user + " "  + cram_tmp3;
    cram_tmp3.clear();
    *tmp = cram_tmp4.encodeBASE64(0);
    cram_tmp4.clear();

    return *tmp;
}


/*! calculate SHA1 the NIST Secure Hash Algorithm of String
    @param void
    @return Temporary string object (SHA1 hash = text of 40 byte HEX)
 */
String& String::sha1() const {
    String *tmp = (*this).tmpStr();
#if __SHA1 == 1
    long length;
    SHA1_CTX sha;
    char hash[20];
    char hex_str[(20 * 2) + 1];
    int i;

    if ((*this).pText) {
        length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();
    } else {
        length = 0;
    }

    SHA1Init(&sha);
    SHA1Update(&sha, (u_char *)((*this).pText?(*this).pText:""), length);
    SHA1Final((u_char *)hash, &sha);
    for (i = 0; i < 20; i++) {
        unsigned char a0, a1;
        a0 = ((hash[i] & 0xF0) >> 4);
        a0 = (a0 < 10)?'0'+a0:'a'-10+a0;
        a1 = (hash[i] & 0x0F);
        a1 = (a1 < 10)?'0'+a1:'a'-10+a1;
        hex_str[i * 2] = a0;
        hex_str[(i * 2) + 1] = a1;
    }
    hex_str[20 * 2] = '\0';

    *tmp = hex_str;

#endif
    return *tmp;
}


/*! Culculate HMAC-SHA1 key
    @param key HMAC-SHA1 key
    @return Temporary string object (SHA1 hash = text of 40 byte HEX)
 */
String& String::hmacSha1(const String &key) const {
    String *tmp = (*this).tmpStr();
#if __SHA1 == 1
    char hash[20];
    char hex_str[(20 * 2) + 1];
    long length;
    SHA1_CTX sha;
    unsigned char k_ipad[65]; // inner padding
    unsigned char k_opad[65]; // outer padding
    int i;

    if ((*this).pText) {
        length = (0 < (*this).nBinaryLength)?(*this).nBinaryLength:(*this).len();
    } else {
        length = 0;
    }

    // SHA1(K XOR opad, SHA1(K XOR ipad, (*this).pText))
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    if (64 < key.len()) {
        char tk[20];
        SHA1_CTX tstate;

        SHA1Init(&tstate);
        SHA1Update(&tstate, (const u_char *)key.c_str(), key.len());
        SHA1Final((u_char *)tk, &tstate);

        memcpy(k_ipad, tk, 20);
        memcpy(k_opad, tk, 20);
    } else {
        memcpy(k_ipad, key.c_str(), key.len());
        memcpy(k_opad, key.c_str(), key.len());
    }
    for (i = 0; i < 64; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    // perform inner SHA1
    SHA1Init(&sha);
    SHA1Update(&sha, (const u_char *)k_ipad, 64);
    SHA1Update(&sha, (const u_char *)((*this).pText?(*this).pText:""), length);
    SHA1Final((u_char *)hash, &sha);

    // perform outer SHA1
    SHA1Init(&sha);
    SHA1Update(&sha, (const u_char *)k_opad, 64);
    SHA1Update(&sha, (const u_char *)hash, 20);
    SHA1Final((u_char *)hash, &sha);

    for (i = 0; i < 20; i++) {
        unsigned char a0, a1;
        a0 = ((hash[i] & (unsigned char)0xF0) >> 4);
        a0 = (a0 < 10)?'0'+a0:'a'-10+a0;
        a1 = (hash[i] & (unsigned char)0x0F);
        a1 = (a1 < 10)?'0'+a1:'a'-10+a1;
        hex_str[i * 2] = a0;
        hex_str[(i * 2) + 1] = a1;
    }
    hex_str[20 * 2] = '\0';

    *tmp = hex_str;

#endif
    return *tmp;
}


/*! Search of word in tag (for evalText)
    @param str search target string
    @param key search key
    @return finded position (-1 if error)
 */
static inline long _search_in_tag(const String& str, const char *key) {
    long count, i, length;
    const char *cpstr;
    bool in_quot;

    length = strlen(key);
    count = str.len() - length + 1;
    if (count < 0) return -1;

    cpstr = str.c_str();

    in_quot = false;
    for (i = 0; i < count; i++) {
        if (in_quot == false && memcmp(&(cpstr[i]), key, length) == 0) {
            return i;
        }
        if (in_quot == false && cpstr[i] == '"') {
           in_quot = true;
        } else if (in_quot == true && cpstr[i] == '"') {
           in_quot = false;
        } else if (in_quot == true && cpstr[i] == '\\' && cpstr[i+1] == '"') {
           i++; 
        }
    }

    return -1;
}


/*! Convert plain text or HTML
    @param replace_keys  replacement text
           replaement string for key "ABC" is as follows:
           &($ABC);
           &($ABC::html);
           &($ABC::htmlbr);
           &($ABC::htmlsp);
           &($ABC::htmlspbr);
           &($ABC::htmlbq);
           &($ABC::js);
           &($ABC::xml);
           &($ABC::sqlite3);
           &($ABC::mysql);
           &($ABC?$ABC:$DEF);
           &($ABC?$ABC:"Value is NOT set."::html);
           &($ABC=="on"?$ABC:"Value is NOT on."::html);
           "if" conditions are as follows:
           {{#if $ABC}} ... {{#elif $DEF}} ... {{#else}} ... {{#endif}}
           {{#if $ABC=="on"}} ... {{#elif $DEF=="on"}} ... {{#else}} ... {{#endif}}
    @return Temporary string object (replaced text)
 */
String& String::evalText(const Keys replace_keys) const {
    String *tmp = (*this).tmpStr();
    long i, max, tag_len, if_depth, max_depth;
    bool tag_found;
    String str, strtmp;
    String tag_content, tag_key, tag_escape;
    String tag_tmp, tag_tmp2, tag_tmp3;
    long esc_col, t1, t2, t3, t4;
    char buf[2];
    char *if_match, *if_inside, *if_matched_content, *p;
    const char *cpstr;

    *tmp = "";
    buf[0] = '\0';
    buf[1] = '\0';

    str = (*this).pText;

    max_depth = 100;
    if_match = new char [max_depth];
    if_match[0] = 1;
    if_inside = new char [max_depth];
    if_inside[0] = 1;
    if_matched_content = new char [max_depth];
    if_matched_content[0] = 1;
    strtmp = "";
    if_depth = 0;

    max = str.len();
    for (i = 0; i < max; i++) {
        tag_len = 0;
        tag_found = false;
        if (str.c_str()[i] == '{' && str.c_str()[i+1] == '{' && str.c_str()[i+2] == '#') {
            tag_len = 2;
            while (str.c_str()[i+tag_len] != '\0') {
                if (str.c_str()[i+tag_len] == '}' && str.c_str()[i+tag_len+1] == '}') {
                    tag_len += 2;
                    tag_content.set(str.c_str()+i+2, tag_len-4);
                    tag_found = true;
                    break;
                }
                tag_len++;
            }
        }

        if (tag_found && tag_content.left(3) == "#if" && isspace(tag_content[3])) {
            // &(#if $ABC);
            if_depth++;
            if (if_depth == max_depth) {
                p = new char [max_depth + 100];
                memcpy(p, if_match, max_depth);
                delete [] if_match;
                if_match = p;
                if_match[if_depth] = 0;
                p = new char [max_depth + 100];
                memcpy(p, if_inside, max_depth);
                delete [] if_inside;
                if_inside = p;
                if_inside[if_depth] = 0;
                p = new char [max_depth + 100];
                memcpy(p, if_matched_content, max_depth);
                delete [] if_matched_content;
                if_matched_content = p;
                if_matched_content[if_depth] = 0;
                max_depth += 100;
            }
            if_inside[if_depth] = 1;
            t1 = 4;
            while (isspace(tag_content[t1])) {
                t1++;
            }
            tag_key = tag_content.mid(t1);
            t2 = -1;
            t3 = _search_in_tag(tag_key, "==");
            if (t3 < 0) t2 = _search_in_tag(tag_key, "!=");
            if (0 <= t3 || 0 <= t2) {
                tag_tmp2 = tag_key.left((0 <= t3)?t3:t2).trim();
                tag_tmp3 = tag_key.mid(((0 <= t3)?t3:t2)+2).trim();
                if (tag_tmp2[0] == '$') {
                    cpstr = replace_keys.read(tag_tmp2.mid(1));
                    tag_tmp2 = cpstr?cpstr:"";
                } else if (1 < tag_tmp2.len() && ((tag_tmp2[0] == '"' && tag_tmp2[tag_tmp2.len()-1] == '"') || (tag_tmp2[0] == '\'' && tag_tmp2[tag_tmp2.len()-1] == '\''))) {
                    tag_tmp2 = tag_tmp2.mid(1, tag_tmp2.len()-2).unescapeQuote();
                }
                if (tag_tmp3[0] == '$') {
                    cpstr = replace_keys.read(tag_tmp3.mid(1));
                    tag_tmp3 = cpstr?cpstr:"";
                } else if (1 < tag_tmp3.len() && ((tag_tmp3[0] == '"' && tag_tmp3[tag_tmp3.len()-1] == '"') || (tag_tmp3[0] == '\'' && tag_tmp3[tag_tmp3.len()-1] == '\''))) {
                    tag_tmp3 = tag_tmp3.mid(1, tag_tmp3.len()-2).unescapeQuote();
                }
            } else {
                tag_key = tag_key.trim();
                if (tag_key[0] == '$') {
                    cpstr = replace_keys.read(tag_key.mid(1));
                    tag_tmp = cpstr?cpstr:"";
                } else if (1 < tag_key.len() && ((tag_key[0] == '"' && tag_key[tag_key.len()-1] == '"') || (tag_key[0] == '\'' && tag_key[tag_key.len()-1] == '\''))) {
                    tag_tmp = tag_key.mid(1, tag_key.len()-2).unescapeQuote();
                }
            }
            i += (tag_len-1);
            if (if_matched_content[if_depth - 1] == 0 || (t3 < 0 && t2 < 0 && tag_tmp == "") || (0 <= t3 && tag_tmp2 != tag_tmp3) || (0 <= t2 && tag_tmp2 == tag_tmp3)) {
                if_match[if_depth] = 0;
                if_matched_content[if_depth] = 0;
            } else {
                if_match[if_depth] = 1;
                if_matched_content[if_depth] = 1;
            }
        } else if (tag_found && 0 < if_depth && tag_content.left(5) == "#elif" && isspace(tag_content[5])) {
            // &(#elif $DEF);
            t1 = 6;
            while (isspace(tag_content[t1])) {
                t1++;
            }
            tag_key = tag_content.mid(t1);
            t2 = -1;
            t3 = -1;
            if (if_match[if_depth] == 0) {
                t2 = -1;
                t3 = _search_in_tag(tag_key, "==");
                if (t3 < 0) t2 = _search_in_tag(tag_key, "!=");
                if (0 <= t3 || 0 <= t2) {
                    tag_tmp2 = tag_key.left((0 <= t3)?t3:t2).trim();
                    tag_tmp3 = tag_key.mid(((0 <= t3)?t3:t2)+2).trim();
                    if (tag_tmp2[0] == '$') {
                        cpstr = replace_keys.read(tag_tmp2.mid(1));
                        tag_tmp2 = cpstr?cpstr:"";
                    } else if (1 < tag_tmp2.len() && ((tag_tmp2[0] == '"' && tag_tmp2[tag_tmp2.len()-1] == '"') || (tag_tmp2[0] == '\'' && tag_tmp2[tag_tmp2.len()-1] == '\''))) {
                        tag_tmp2 = tag_tmp2.mid(1, tag_tmp2.len()-2).unescapeQuote();
                    }
                    if (tag_tmp3[0] == '$') {
                        cpstr = replace_keys.read(tag_tmp3.mid(1));
                        tag_tmp3 = cpstr?cpstr:"";
                    } else if (1 < tag_tmp3.len() && ((tag_tmp3[0] == '"' && tag_tmp3[tag_tmp3.len()-1] == '"') || (tag_tmp3[0] == '\'' && tag_tmp3[tag_tmp3.len()-1] == '\''))) {
                        tag_tmp3 = tag_tmp3.mid(1, tag_tmp3.len()-2).unescapeQuote();
                    }
                } else {
                    tag_key = tag_key.trim();
                    if (tag_key[0] == '$') {
                        cpstr = replace_keys.read(tag_key.mid(1));
                        tag_tmp = cpstr?cpstr:"";
                    } else if (1 < tag_key.len() && ((tag_key[0] == '"' && tag_key[tag_key.len()-1] == '"') || (tag_key[0] == '\'' && tag_key[tag_key.len()-1] == '\''))) {
                        tag_tmp = tag_key.mid(1, tag_key.len()-2).unescapeQuote();
                    }
                }
            }
            i += (tag_len-1);
            if (if_matched_content[if_depth - 1] == 0 || if_match[if_depth] == 1 || (t3 < 0 && t2 < 0 && tag_tmp == "") || (0 <= t3 && tag_tmp2 != tag_tmp3) || (0 <= t2 && tag_tmp2 == tag_tmp3)) {
                if_matched_content[if_depth] = 0;
            } else {
                if_match[if_depth] = 1;
                if_matched_content[if_depth] = 1;
            }
        } else if (tag_found && 0 < if_depth && tag_content.left(5) == "#else") {
            // &(#else);
            i += (tag_len-1);
            if (if_match[if_depth] == 1) {
                if_matched_content[if_depth] = 0;
            } else {
                if_match[if_depth] = 1;
                if_matched_content[if_depth] = 1;
            }
        } else if (tag_found && 0 < if_depth && tag_content.left(6) == "#endif") {
            // $(#endif);
            i += (tag_len-1);
            if_match[if_depth] = 0;
            if_inside[if_depth] = 0;
            if_matched_content[if_depth] = 0;
            if_depth--;
        } else if (if_inside[if_depth] == 0 || if_matched_content[if_depth] == 1) {
            buf[0] = str.c_str()[i];
            strtmp += buf;
        }
    }
    delete [] if_match;
    delete [] if_inside;
    delete [] if_matched_content;

    str = strtmp;
    strtmp.clear();

    max = str.len();
    for (i = 0; i < max; i++) {
        tag_len = 0;
        tag_found = false;
        if (str.c_str()[i] == '&' && str.c_str()[i+1] == '(') {
            tag_len = 2;
            while (str.c_str()[i+tag_len] != '\0') {
                if (str.c_str()[i+tag_len] == ')' && str.c_str()[i+tag_len+1] == ';') {
                    tag_len += 2;
                    tag_content.set(str.c_str()+i+2, tag_len-4);
                    tag_found = true;
                    break;
                }
                tag_len++;
            }
        }

        if (tag_found) {
            esc_col = _search_in_tag(tag_content, "::");
            if (0 <= esc_col) {
                tag_key = tag_content.left(esc_col);
                tag_escape = tag_content.mid(esc_col + 2).trim();
            } else {
                tag_key = tag_content;
                tag_escape = "";
            }

            // x?y:z
            t1 = _search_in_tag(tag_key, "?");
            if (0L <= t1) {
                t2 = _search_in_tag(tag_key, ":");
                if (t1 < t2) {
                    tag_tmp = tag_key.left(t1);
                    t4 = -1;
                    t3 = _search_in_tag(tag_tmp, "==");
                    if (t3 < 0) t4 = _search_in_tag(tag_tmp, "!=");
                    if (0 <= t3 || 0 <= t4) {
                        tag_tmp2 = tag_tmp.left((0 <= t3)?t3:t4).trim();
                        tag_tmp3 = tag_tmp.mid(((0 <= t3)?t3:t4)+2).trim();
                        if (tag_tmp2[0] == '$') {
                            cpstr = replace_keys.read(tag_tmp2.mid(1));
                            tag_tmp2 = cpstr?cpstr:"";
                        } else if (1 < tag_tmp2.len() && ((tag_tmp2[0] == '"' && tag_tmp2[tag_tmp2.len()-1] == '"') || (tag_tmp2[0] == '\'' && tag_tmp2[tag_tmp2.len()-1] == '\''))) {
                            tag_tmp2 = tag_tmp2.mid(1, tag_tmp2.len()-2).unescapeQuote();
                        }
                        if (tag_tmp3[0] == '$') {
                            cpstr = replace_keys.read(tag_tmp3.mid(1));
                            tag_tmp3 = cpstr?cpstr:"";
                        } else if (1 < tag_tmp3.len() && ((tag_tmp3[0] == '"' && tag_tmp3[tag_tmp3.len()-1] == '"') || (tag_tmp3[0] == '\'' && tag_tmp3[tag_tmp3.len()-1] == '\''))) {
                            tag_tmp3 = tag_tmp3.mid(1, tag_tmp3.len()-2).unescapeQuote();
                        }
                        if ((0 <= t3 && tag_tmp2 == tag_tmp3) || (0 <= t4 && tag_tmp2 != tag_tmp3)) {
                            tag_key = tag_key.mid(t1+1, t2-t1-1);
                        } else {
                            tag_key = tag_key.mid(t2+1);
                        }
                    } else {
                        tag_tmp = tag_tmp.trim();
                        if (tag_tmp[0] == '$') {
                            cpstr = replace_keys.read(tag_tmp.mid(1));
                            tag_tmp = cpstr?cpstr:"";
                        } else if (1 < tag_tmp.len() && ((tag_tmp[0] == '"' && tag_tmp[tag_tmp.len()-1] == '"') || (tag_tmp[0] == '\'' && tag_tmp[tag_tmp.len()-1] == '\''))) {
                            tag_tmp = tag_tmp.mid(1, tag_tmp.len()-2).unescapeQuote();
                        }
                        if (tag_tmp != "") {
                            tag_key = tag_key.mid(t1+1, t2-t1-1);
                        } else {
                            tag_key = tag_key.mid(t2+1);
                        }
                    }
                    tag_tmp.clear();
                }
            }

            if (tag_key[0] == '$') {
                cpstr = replace_keys.read(tag_key.mid(1));
                tag_key = cpstr?cpstr:"";
            } else if (1 < tag_key.len() && ((tag_key[0] == '"' && tag_key[tag_key.len()-1] == '"') || (tag_key[0] == '\'' && tag_key[tag_key.len()-1] == '\''))) {
                tag_key = tag_key.mid(1, tag_key.len()-2).unescapeQuote();
            }

            if (tag_escape == "html") {
                *tmp += tag_key.escapeHTML();
            } else if (tag_escape == "htmlbr") {
                *tmp += tag_key.escapeHTML("UTF-8", "UTF-8", "b");
            } else if (tag_escape == "htmlsp") {
                *tmp += tag_key.escapeHTML("UTF-8", "UTF-8", "s");
            } else if (tag_escape == "htmlspbr") {
                *tmp += tag_key.escapeHTML("UTF-8", "UTF-8", "sb");
            } else if (tag_escape == "htmlbq") {
                *tmp += tag_key.escapeHTMLBackQuote();
            } else if (tag_escape == "js") {
                *tmp += tag_key.escapeQuote();
            } else if (tag_escape == "xml") {
                *tmp += tag_key.escapeXML();
            } else if (tag_escape == "sqlite3") {
                *tmp += tag_key.escapeSQLite3();
            } else if (tag_escape == "mysql") {
                *tmp += tag_key.escapeMySQL();
            } else {
                *tmp += tag_key;
            }
            i += (tag_len-1);
        } else {
            buf[0] = str.c_str()[i];
            *tmp += buf;
        }

    }

    tag_content.clear();
    tag_key.clear();
    tag_escape.clear();

    return *tmp;
}


/*! Convert HTML to plain text
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Plaind text)
 */
String& String::convertHTMLToPlain(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *src_utf8;
    char *buf, *buf1;
    enum {
        TAG_UNKNOWN,
        TAG_BR,
        TAG_TABLE_S, TAG_TABLE_E, TAG_TR_E, TAG_TH_E, TAG_TD_E,
        TAG_P_S, TAG_P_E,
        TAG_LI,
        TAG_HR,
        TAG_IMG,
        TAG_STYLE_S, TAG_STYLE_E,
        TAG_SCRIPT_S, TAG_SCRIPT_E
    } tag = TAG_UNKNOWN;
    bool instyle, inscript;
    long i, j, length;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        String html;
        String s;

        length = strlen(src_utf8);
        buf = new char [length + 3];

        instyle = false;
        inscript = false;
        j = 0;
        for (i = 0; i < length; i++) {
            if (src_utf8[i] == '<' && src_utf8[i+1] == '!' && src_utf8[i+2] == '-' && src_utf8[i+3] == '-') {
                i += 4;
                while (i < length) {
                    if (src_utf8[i] == '-' && src_utf8[i+1] == '-' && src_utf8[i+2] == '>') {
                        i += 2;
                        break;
                    }
                    i++;
                }
            } else if (src_utf8[i] == '<') {
                tag = TAG_UNKNOWN;
                if (!strncasecmp(src_utf8+i, "<br", 3) && (src_utf8[i+3] == '>' || isspace(src_utf8[i+3]))) {
                    tag = TAG_BR;
                } else if (!strncasecmp(src_utf8+i, "<table", 6) && (src_utf8[i+6] == '>' || isspace(src_utf8[i+6]))) {
                    tag = TAG_TABLE_S;
                } else if (!strncasecmp(src_utf8+i, "</table", 7) && (src_utf8[i+7] == '>' || isspace(src_utf8[i+7]))) {
                    tag = TAG_TABLE_E;
                } else if (!strncasecmp(src_utf8+i, "</tr", 4) && (src_utf8[i+4] == '>' || isspace(src_utf8[i+4]))) {
                    tag = TAG_TR_E;
                } else if (!strncasecmp(src_utf8+i, "</th", 4) && (src_utf8[i+4] == '>' || isspace(src_utf8[i+4]))) {
                    tag = TAG_TH_E;
                } else if (!strncasecmp(src_utf8+i, "</td", 4) && (src_utf8[i+4] == '>' || isspace(src_utf8[i+4]))) {
                    tag = TAG_TD_E;
                } else if (!strncasecmp(src_utf8+i, "<p", 2) && (src_utf8[i+2] == '>' || isspace(src_utf8[i+2]))) {
                    tag = TAG_P_S;
                } else if (!strncasecmp(src_utf8+i, "</p", 3) && (src_utf8[i+3] == '>' || isspace(src_utf8[i+3]))) {
                    tag = TAG_P_E;
                } else if (!strncasecmp(src_utf8+i, "<li", 3) && (src_utf8[i+3] == '>' || isspace(src_utf8[i+3]))) {
                    tag = TAG_LI;
                } else if (!strncasecmp(src_utf8+i, "<hr", 3) && (src_utf8[i+3] == '>' || isspace(src_utf8[i+3]))) {
                    tag = TAG_HR;
                } else if (!strncasecmp(src_utf8+i, "<img", 4) && (src_utf8[i+4] == '>' || isspace(src_utf8[i+4]))) {
                    tag = TAG_IMG;
                } else if (!strncasecmp(src_utf8+i, "<style", 6) && (src_utf8[i+6] == '>' || isspace(src_utf8[i+6]))) {
                    tag = TAG_STYLE_S;
                    instyle = true;
                } else if (!strncasecmp(src_utf8+i, "</style", 7) && (src_utf8[i+7] == '>' || isspace(src_utf8[i+7]))) {
                    tag = TAG_STYLE_E;
                    instyle = false;
                } else if (!strncasecmp(src_utf8+i, "<script", 7) && (src_utf8[i+7] == '>' || isspace(src_utf8[i+7]))) {
                    tag = TAG_SCRIPT_S;
                    inscript = true;
                } else if (!strncasecmp(src_utf8+i, "</script", 8) && (src_utf8[i+8] == '>' || isspace(src_utf8[i+8]))) {
                    tag = TAG_SCRIPT_E;
                    inscript = false;
                }

                switch (tag) {
                    case TAG_BR:
                    case TAG_TABLE_S:
                    case TAG_TABLE_E:
                    case TAG_TR_E:
                    case TAG_P_S:
                        if (0 < j && buf[j-1] != '\n') buf[j++] = '\n';
                        break;
                    case TAG_TH_E:
                    case TAG_TD_E:
                        if (0 < j && !isspace(buf[j-1])) buf[j++] = ' ';
                        break;
                    case TAG_P_E:
                        if (0 < j && buf[j-1] != '\n') buf[j++] = '\n';
                        buf[j++] = '\n';
                        break;
                    case TAG_LI:
                        if (0 < j && buf[j-1] != '\n') buf[j++] = '\n';
                        buf[j++] = '*';
                        buf[j++] = ' ';
                        break;
                    case TAG_HR:
                        if (0 < j && buf[j-1] != '\n') buf[j++] = '\n';
                        buf[j++] = '-';
                        buf[j++] = '-';
                        buf[j++] = '\n';
                        break;
                    case TAG_IMG:
                        //buf[j++] = '[';
                        //buf[j++] = ' ';
                        //buf[j++] = ']';
                        break;
                    default:
                        break;
                }

                i++;
                while (i < length) {
                    if (src_utf8[i] == '>') {
                        break;
                    }
                    i++;
                }
            } else if (!instyle && !inscript) {
                if ((j == 0 && !isspace(src_utf8[i])) || (0 < j && !(buf[j-1] == '\n' && isspace(src_utf8[i])) && !(isspace(buf[j-1]) && isspace(src_utf8[i])))) {
                    long k;
                    char *p = NULL;
                    if (src_utf8[i] == '&') {
                        for (k = i + 1; src_utf8[k] != '\0'; k++) {
                            if (isalnum(src_utf8[k]) || src_utf8[k] == '#') {
                                continue;
                            } else if (i + 1 < k && src_utf8[k] == ';') {
                                p = src_utf8 + k;
                            }
                            break;
                        }
                    }
                    if (p != NULL) {
                        long amp_len_d;
                        long amp_len_o = (long)(p - (src_utf8 + i) + 1);
                        char *amp = new char [amp_len_o + 1];
                        memcpy(amp, src_utf8 + i, amp_len_o);
                        amp[amp_len_o] = '\0';
                        html = amp;
                        delete [] amp;
                        s = html.unescapeHTML();
                        amp_len_d = s.len();
                        k = 0;
                        while (k < amp_len_d) {
                            buf[j++] = s.c_str()[k];
                            k++;
                        }
                        i += (amp_len_o - 1);
                    } else {
                        buf[j++] = src_utf8[i];
                    }
                }
            }
        }
        if (0 < j && buf[j-1] != '\n') {
            buf[j++] = '\n';
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;

        html.clear();
        s.clear();
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Convert Plain text to HTML
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (HTML)
 */
String& String::convertPlainToHTML(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *src_utf8;
    char *buf, *buf1;
    long i, j, k, l, length, bufsize;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        char linkstr[2001];
        char emailstr[260];
        char c;
        long width, start_width, addr_scanned;

        length = strlen(src_utf8);

        bufsize = length*3 + 5000;
        buf = new char [bufsize];

        width = 0;
        addr_scanned = 0;
        j = 0;
        for (i = 0; i < length; i++) {
            if (bufsize - 5000 < j) {
                bufsize += j*3 + 5000;
                buf1 = new char [bufsize];
                memcpy(buf1, buf, j);
                delete [] buf;
                buf = buf1;
                buf1 = NULL;
            }
            c = src_utf8[i];
            if (c == '&') {
                buf[j++] = '&';
                buf[j++] = 'a';
                buf[j++] = 'm';
                buf[j++] = 'p';
                buf[j++] = ';';
                width++;
            } else if (c == '<') {
                buf[j++] = '&';
                buf[j++] = 'l';
                buf[j++] = 't';
                buf[j++] = ';';
                width++;
            } else if (c == '>') {
                buf[j++] = '&';
                buf[j++] = 'g';
                buf[j++] = 't';
                buf[j++] = ';';
                width++;
            } else if (i+1 < length && c == '\xC2' && src_utf8[i+1] == '\xA0') {
                buf[j++] = '&';
                buf[j++] = 'n';
                buf[j++] = 'b';
                buf[j++] = 's';
                buf[j++] = 'p';
                buf[j++] = ';';
                width++;
                i++;
            } else if (i+1 < length && c == ' ' && src_utf8[i+1] == ' ') {
                buf[j++] = ' ';
                buf[j++] = '&';
                buf[j++] = 'n';
                buf[j++] = 'b';
                buf[j++] = 's';
                buf[j++] = 'p';
                buf[j++] = ';';
                width += 2;
                i++;
            } else if (c == '\n') {
                buf[j++] = '<';
                buf[j++] = 'b';
                buf[j++] = 'r';
                buf[j++] = ' ';
                buf[j++] = '/';
                buf[j++] = '>';
                width = 0;
            } else if (i+1 < length && c == '\r' && src_utf8[i+1] == '\n') {
                buf[j++] = '<';
                buf[j++] = 'b';
                buf[j++] = 'r';
                buf[j++] = ' ';
                buf[j++] = '/';
                buf[j++] = '>';
                width = 0;
                i++;
            } else if (c == '\r') {
                buf[j++] = '<';
                buf[j++] = 'b';
                buf[j++] = 'r';
                buf[j++] = ' ';
                buf[j++] = '/';
                buf[j++] = '>';
                width = 0;
            } else if (c == '\t') {
                start_width = width;
                k = 0;
                for (l = start_width; !(start_width < l && l%8 == 0); l++) {
                    if (k%2 == 0) {
                        buf[j++] = '&';
                        buf[j++] = 'n';
                        buf[j++] = 'b';
                        buf[j++] = 's';
                        buf[j++] = 'p';
                        buf[j++] = ';';
                    } else {
                        buf[j++] = ' ';
                    }
                    width++;
                    k++;
                }
            } else if (!strcmp(src_utf8+i, "http://") ||
                    !strcmp(src_utf8+i, "https://") ||
                    !strcmp(src_utf8+i, "ftp://")) {
                linkstr[0] = '\0';
                k = 0;
                l = 0;
                for (; i < length && k < 2001; i++) {
                    c = src_utf8[i];
                    if (c <= 0x20 || 0x7E < c || c == '<' || c == '>' ||
                            c == '\\' || c == '"') {
                        i--;
                        break;
                    }
                    linkstr[l++] = c;
                    linkstr[l] = '\0';
                    k++;
                }
                if (k == 2001) {
                    i--;
                }
                strcpy(buf+j, "<a href=\""); j += 9;
                strcpy(buf+j, linkstr); j += l;
                strcpy(buf+j, "\" target=\"_blank\">"); j += 18;
                strcpy(buf+j, linkstr); j += l;
                strcpy(buf+j, "</a>"); j += 4;
                width += (9+l+2+l+4);
            } else if (addr_scanned <= i && 0x20 < c && c <= 0x7E && c != ' ' && c != '.' &&
                    c != '\\' && c != '"' && c != '@' && c != ':' && c != '/' && c != ',') {
                bool at_found = false;
                long orig_i;

                orig_i = i;
                k = 0;
                for (; i < length && k < 257; i++) {
                    c = src_utf8[i];
                    if (c <= 0x20 || 0x7E < c || c == '<' || c == '>' ||
                            c == '\\' || c == '"' || c == ':' || (at_found && c == '@')) {
                        i--;
                        break;
                    }
                    if (at_found && !(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') ||
                            ('0' <= c && c <= '9') ||
                            (0 < i && src_utf8[i-1] != '.' && c == '.') ||
                            (0 < i && src_utf8[i-1] != '-' && c == '-'))) {
                        i--;
                        break;
                    }
                    if (at_found == false && c == '@') {
                        at_found = true;
                    }
                    emailstr[k] = c;
                    emailstr[k+1] = '\0';
                    k++;
                }
                if (k == 257) {
                    i--;
                }
                if (at_found && (c == '.' || c == '-')) {
                    k--;
                    emailstr[k] = '\0';
                    i--;
                }
                if (at_found && c != '@') {
                    strcpy(buf+j, "<a href=\"mailto:"); j += 16;
                    strcpy(buf+j, emailstr); j += k;
                    strcpy(buf+j, "\">"); j += 2;
                    strcpy(buf+j, emailstr); j += k;
                    strcpy(buf+j, "</a>"); j += 4;
                    width += (16+k+2+k+4);
                } else {
                    addr_scanned = i;
                    i = orig_i;
                    buf[j++] = src_utf8[i];
                    width++;
                }
            } else {
                int u8len;
                char u8[8];
                memset(u8, 0, sizeof(u8));
                u8len = utf8_len(c);
                strncpy(u8, src_utf8+i, u8len);
                buf[j++] = c;
                width += utf8_width(u8);
                if (1 < u8len && strlen(u8) == u8len) {
                    strncpy(buf+j, src_utf8+i+1, u8len-1);
                    i += (u8len-1);
                    j += (u8len-1);
                }
            }
        }
        buf[j] = '\0';

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Convert character set of string
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object
 */
String& String::strconv(const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf2, *utf8buf;
    const char *dest_ch;
    long length;

    if (dest_charset == NULL) {
        return *tmp;
    }

    utf8buf = NULL;
    if (src_charset != NULL && (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5))) {
        if (strcasestr(src_charset, "BE") != NULL) {
            utf8buf = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            utf8buf = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            utf8buf = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (src_charset != NULL && (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5))) {
        if (strcasestr(src_charset, "BE") != NULL) {
            utf8buf = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            utf8buf = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            utf8buf = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    }

    if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5) ||
            !strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
        dest_ch = STR_UTF8;
    } else {
        dest_ch = dest_charset;
    }

    if (utf8buf != NULL) {
        buf = charset_convert(utf8buf, STR_UTF8, dest_ch);
        delete [] utf8buf;
    } else {
        buf = charset_convert((*this).pText, src_charset, dest_ch);
    }

    if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
        int bom = strcasestr(dest_charset, "BOM")?1:0;
        if (strcasestr(dest_charset, "LE") != NULL) {
            buf2 = utf8_to_utf16(buf, &length, bom?'L':'l');
        } else if (strcasestr(dest_charset, "BE") != NULL) {
            buf2 = utf8_to_utf16(buf, &length, bom?'B':'b');
        } else {
            buf2 = utf8_to_utf16(buf, &length, 'B');
        }
        delete [] buf;
        buf = buf2;
        if (buf != NULL) {
            (*tmp).useAsBinary(length);
            (*tmp).pTextReplace((char *)buf, 0, length, length);
        } else {
            *tmp = "";
        }
    } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
        int bom = strcasestr(dest_charset, "BOM")?1:0;
        if (strcasestr(dest_charset, "LE") != NULL) {
            buf2 = utf8_to_utf32(buf, &length, bom?'L':'l');
        } else if (strcasestr(dest_charset, "BE") != NULL) {
            buf2 = utf8_to_utf32(buf, &length, bom?'B':'b');
        } else {
            buf2 = utf8_to_utf32(buf, &length, 'B');
        }
        delete [] buf;
        buf = buf2;
        if (buf != NULL) {
            (*tmp).useAsBinary(length);
            (*tmp).pTextReplace((char *)buf, 0, length, length);
        } else {
            *tmp = "";
        }
    } else {
        if (buf != NULL) {
            long len;
            len = strlen(buf);
            (*tmp).useAsText();
            (*tmp).pTextReplace((char *)buf, len, -1, len + 1);
        } else {
            *tmp = "";
        }
    }

    return *tmp;
}


const char* String::detectCharSet() const {
    const char *str;
    str = (*this).pText;

    if (4 <= (*this).binaryLength()) {
        if ((const unsigned char)str[0] == (const unsigned char)'\xFF' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFE' &&
                ((const unsigned char)str[2] != (const unsigned char)'\x00' ||
                (const unsigned char)str[3] != (const unsigned char)'\x00')) {
            return STR_UTF16LE;
        } else if ((const unsigned char)str[0] == (const unsigned char)'\xFE' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFF' &&
                ((const unsigned char)str[2] != (const unsigned char)'\x00' ||
                (const unsigned char)str[3] != (const unsigned char)'\x00')) {
            return STR_UTF16BE;
        }
        if ((const unsigned char)str[0] == (const unsigned char)'\xFF' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFE' &&
                (const unsigned char)str[2] == (const unsigned char)'\x00' &&
                (const unsigned char)str[3] == (const unsigned char)'\x00') {
            return STR_UTF32LE;
        } else if ((const unsigned char)str[0] == (const unsigned char)'\x00' &&
                (const unsigned char)str[1] == (const unsigned char)'\x00' &&
                (const unsigned char)str[2] == (const unsigned char)'\xFE' &&
                (const unsigned char)str[3] == (const unsigned char)'\xFF') {
            return STR_UTF32BE;
        }
    }

    return auto_detect(str);
}


const char* String::detectCharSetJP() const {
    const char *str;
    str = (*this).pText;

    if (4 <= (*this).binaryLength()) {
        if ((const unsigned char)str[0] == (const unsigned char)'\xFF' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFE' &&
                ((const unsigned char)str[2] != (const unsigned char)'\x00' ||
                (const unsigned char)str[3] != (const unsigned char)'\x00')) {
            return STR_UTF16LE;
        } else if ((const unsigned char)str[0] == (const unsigned char)'\xFE' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFF' &&
                ((const unsigned char)str[2] != (const unsigned char)'\x00' ||
                (const unsigned char)str[3] != (const unsigned char)'\x00')) {
            return STR_UTF16BE;
        }
        if ((const unsigned char)str[0] == (const unsigned char)'\xFF' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFE' &&
                (const unsigned char)str[2] == (const unsigned char)'\x00' &&
                (const unsigned char)str[3] == (const unsigned char)'\x00') {
            return STR_UTF32LE;
        } else if ((const unsigned char)str[0] == (const unsigned char)'\x00' &&
                (const unsigned char)str[2] == (const unsigned char)'\x00' &&
                (const unsigned char)str[2] == (const unsigned char)'\xFE' &&
                (const unsigned char)str[1] == (const unsigned char)'\xFF') {
            return STR_UTF32BE;
        }
    }

    return jis_auto_detect(str);
}


/*! Change return code
    @param return_str  String of return code (ex. "\r\n")
    @return Temporary string object
 */
String& String::changeReturnCode(const char * return_str) const {
    String *tmp = (*this).tmpStr();
    char *buf, ch;
    long i, j, ret_len, length;

    (*tmp).useAsText();

    if (return_str != NULL) {
        length = (*this).len();
        ret_len = strlen(return_str);
        buf = new char[(length * ret_len) + 1];
        j = 0;
        for (i = 0; i < length; i++) {
            ch = (*this).pText[i];
            if (ch == '\r' && (*this).pText[i+1] == '\n') {
                strcpy(buf + j, return_str);
                j += ret_len;
                i++;
            } else if (ch == '\r' || ch == '\n') {
                strcpy(buf + j, return_str);
                j += ret_len;
            } else {
                buf[j] = ch;
                j++;
            }
        }
        buf[j] = '\0';
        *tmp = buf;
        delete [] buf;
    } else {
        *tmp = (*this).pText;
    }

    return *tmp;
}


/*! Change string width (zenkaku-hankaku)
    @param option converting option (ex. "AHI")
                  'A': ASCII to full width
                  'a': ASCII to half width
                  'H': half width katakana to full width hiragana
                  'K': half width katakana to full width katakana
                  'J': full width hiragana to full width katakana
                  'j': full width hiragana to half width katakana
                  'I': full width katakana to full width hiragana
                  'k': full width katakana to half width katakana
    @param src_charset  Character set of input. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @param dest_charset Character set of output. (ex. "UTF-8", "ISO-2022-JP", etc..)
    @return Temporary string object (Unescaped text)
 */
String& String::changeWidth(const char *option, const char *src_charset, const char *dest_charset) const {
    String *tmp = (*this).tmpStr();
    char *buf, *buf1;
    char *src_utf8;
    long j;

    if (!strncasecmp(src_charset, "UTF-16", 6) || !strncasecmp(src_charset, "UTF16", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf16_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else if (!strncasecmp(src_charset, "UTF-32", 6) || !strncasecmp(src_charset, "UTF32", 5)) {
        if (strcasestr(src_charset, "BE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'b');
        } else if (strcasestr(src_charset, "LE") != NULL) {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'l');
        } else {
            src_utf8 = utf32_to_utf8((*this).pText, (*this).binaryLength(), 'B');
        }
    } else {
        src_utf8 = charset_convert((*this).pText, src_charset, STR_UTF8);
    }
    if (src_utf8) {
        buf = utf8_change_width(src_utf8, option);

        if (!strncasecmp(dest_charset, "UTF-16", 6) || !strncasecmp(dest_charset, "UTF16", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf16(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf16(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else if (!strncasecmp(dest_charset, "UTF-32", 6) || !strncasecmp(dest_charset, "UTF32", 5)) {
            int bom = strcasestr(dest_charset, "BOM")?1:0;
            if (strcasestr(dest_charset, "LE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'L':'l');
            } else if (strcasestr(dest_charset, "BE") != NULL) {
                buf1 = utf8_to_utf32(buf, &j, bom?'B':'b');
            } else {
                buf1 = utf8_to_utf32(buf, &j, 'B');
            }
            if (buf1 != NULL) {
                delete [] buf;
                buf = buf1;
                (*tmp).pTextReplace((char *)buf, -1, j, j);
                (*tmp).useAsBinary(j);
            }
        } else {
            buf1 = charset_convert(buf, STR_UTF8, dest_charset);
            delete [] buf;

            (*tmp).useAsText();
            (*tmp).pTextReplace(buf1, -1, -1, -1);
        }

        delete [] src_utf8;
    } else {
        (*tmp).useAsText();
        *tmp = "";
    }

    return *tmp;
}


/*! Regular Expression Match
    @param regex Regular Expression pattern
    @return matched String
 */
String& String::reMatch(const char *regex) const {
    String *tmp = (*this).tmpStr();
#if __REGEX == 1
    long regex_length, length;
    regex_t *reg;

    *tmp = "";

    if (regex && regex[0] != '\0') {
        regex_length = strlen(regex);
    } else {
        regex_length = 0;
    }

    re_alloc_pattern(&reg);
    REG_OPTION_ON(reg->options, REG_OPTION_MULTILINE);
    if (0 < regex_length && (*this).pText && re_compile_pattern(regex, regex_length, reg) == 0) {
        struct re_registers *regs;

        // Memory allocation
        regs = regex_region_new();
        length = (*this).len();

        if (0 < re_match(reg, (*this).pText, length, 0, regs)) {
            long i, j, match_col, match_len = 0L;
            char *match_buf = NULL;

            i = (1 < regs->num_regs)?1:0; // Excluding the earliest match string, because it is the whole of string
            while (i < regs->num_regs) {
                match_col = regs->beg[i];
                match_len = regs->end[i] - match_col;
                match_buf = new char[match_len + 1];

                j = 0;
                while (j < match_len) {
                    match_buf[j] = (*this).pText[match_col];
                    match_col++;
                    j++;
                }
                match_buf[j] = '\0';

                break; // Do not loop, return the earliest match string
                i++;
            }
            if (match_buf != NULL) {
                (*tmp).useAsText();
                (*tmp).pTextReplace(match_buf, match_len, -1, match_len + 1);
            }
        }
        re_free_registers(regs);
    }
    re_free_pattern(reg);

#endif
    return *tmp;
}


/*! Regular Expression Sub-string
    @param regex1 Regular Expression pattern1
    @param regex2 Regular Expression pattern2
    @return replaced String
 */
String& String::reSubst(const char *regex1, const char *regex2) const {
    String *tmp = (*this).tmpStr();
#if __REGEX == 1
    char *buf;
    long regex1_length, regex2_length, length, buf_size;
    regex_t *reg;

    *tmp = "";
    buf_size = 0;

    if (regex1 && regex1[0] != '\0') {
        regex1_length = strlen(regex1);
    } else {
        regex1_length = 0;
    }

    re_alloc_pattern(&reg);
    REG_OPTION_ON(reg->options, REG_OPTION_MULTILINE);
    if (0 < regex1_length && regex2 != NULL && (*this).pText && re_compile_pattern(regex1, regex1_length, reg) == 0) {
        struct re_registers *regs;

        // Memory allocation
        regs = regex_region_new();
        length = (*this).len();

        if (0 < re_search(reg, (*this).pText, length, 0, length, regs)) {
            long buf_col, col, i, j, k, num;

            regex2_length = strlen(regex2);
            buf_size = length + (regex2_length * regs->num_regs) + 1;
            buf = new char[buf_size];

            buf_col = col = 0;
            i = 0;
            while (i < regs->num_regs) {

                while (col < regs->beg[i]) {
                    buf[buf_col] = (*this).pText[col];
                    buf_col++;
                    col++;
                }

                j = 0;
                while (j < regex2_length) {
                    if (regex2[j] == '$' && 0 <= regex2[j+1] && regex2[j+1] <= '9' &&
                            0 <= (num = atol(&(regex2[j+1])))) {
                        // Extended regular expression $xx
                        long match_col, match_len;
                        char *new_buf;

                        if (num == 0) {
                            match_col = 0;
                            match_len = length;
                        } else {
                            match_col = regs->beg[num-1];
                            match_len = regs->end[num-1] - match_col;
                        }

                        new_buf = new char[buf_size + match_len];
                        if (0 < buf_col) {
                            memcpy(new_buf, buf, buf_col);
                        }
                        delete [] buf;
                        buf = new_buf;

                        k = 0;
                        while (k < match_len) {
                            buf[buf_col] = (*this).pText[match_col+k];
                            buf_col++;
                            k++;
                        }

                        j++;
                        while (0 <=  regex2[j] &&  regex2[j] <= '9') j++;
                    } else {
                        if (regex2[j] == '\\') {
                            j++;
                        }
                        buf[buf_col] = regex2[j];
                        buf_col++;
                        j++;
                    }
                }
                col += (regs->end[i] - regs->beg[i]);
                i++;
            }
            while (col < length) {
                buf[buf_col] = (*this).pText[col];
                buf_col++;
                col++;
            }
            buf[buf_col] = '\0';

            (*tmp).useAsText();
            (*tmp).pTextReplace((char *)buf, buf_col, -1, buf_size);
        }
        re_free_registers(regs);
    }
    re_free_pattern(reg);

#endif
    return *tmp;
}


/*! Regular Expression Translating
    @param regex1 Regular Expression pattern1
    @param regex2 Regular Expression pattern2
    @return translated String
 */
String& String::reTrans(const char *regex1, const char *regex2) const {
    String *tmp = (*this).tmpStr();
#if __REGEX == 1
    char *buf;
    long regex1_length, regex2_length, length;

    if (regex1 && regex1[0] != '\0') {
        regex1_length = strlen(regex1);
    } else {
        regex1_length = 0;
    }
    if (regex2 && regex2[0] != '\0') {
        regex2_length = strlen(regex2);
    } else {
        regex2_length = 0;
    }

    if (0 < regex1_length && regex1_length == regex2_length && (*this).pText) {
        long i;

        // Memory allocation
        length = (*this).len();
        buf = new char[length + 1];

        i = 0;
        while (i < length) {
            long j;

            j = 0;
            buf[i] = (*this).pText[i];
            while (j < regex1_length) {
                if (regex1[j] == (*this).pText[i]) {
                    buf[i] = regex2[j];
                    break;
                }
                j++;
            }
            i++;
        }
        buf[i] = '\0';

        (*tmp).useAsText();
        (*tmp).pTextReplace(buf, i, -1, length + 1);
    } else {
        *tmp = (*this).pText;
    }

#endif
    return *tmp;
}


/*! Wild Card Match
    @param wildcard Wild Card pattern
    @param sub_str remaining str
    @return matched String
 */
String& String::wcMatch(const char *wildcard, const char **sub_str) const {
    String *tmp = (*this).tmpStr();
    char *buf;
    long wildcard_length, wlen, length;

    if (sub_str != (const char **)NULL) {
        *sub_str = NULL;
    }

    if (wildcard && wildcard[0] != '\0') {
        wildcard_length = strlen(wildcard);
    } else {
        wildcard_length = 0;
    }

    if (0 < wildcard_length) {
        long i, j, k;

        // Memory allocation
        length = (*this).len();
        buf = new char[length + 1];

        i = j = k = 0;
        while (i < length) {
            while (i < length && j < wildcard_length) {
                if (wildcard[j] == '*') {
                    wlen = 0;
                    while (wildcard[j + 1 + wlen] != '*' && wildcard[j + 1 + wlen] != '\0') {
                        wlen++;
                    }
                    if (0 < wlen && !strncmp((*this).pText + i, wildcard + j + 1, wlen)) {
                        memcpy(buf + k, (*this).pText + i, wlen);
                        k += wlen;
                        i += wlen - 1;
                        j += wlen;
                        break;
                    } else {
                        buf[k] = (*this).pText[i];
                        k++;
                        i++;
                        continue;
                    }
                } else if (wildcard[j] == (*this).pText[i]) {
                    buf[k] = (*this).pText[i];
                    k++;
                } else {
                    i = length;
                    break;
                }
                break;
            }
            j++;
            i++;
        }
        buf[k] = '\0';

        if (sub_str != (const char **)NULL) {
            if (!strcmp(wildcard, "*")) {
                *sub_str = wildcard + 1;
            } else {
                *sub_str = wildcard + j;
            }
        }

        (*tmp).useAsText();
        (*tmp).pTextReplace(buf, k, -1, length + 1);
    } else {
        *tmp = "";
    }

    return *tmp;
}


/*! Check if match Wild Card pattern
    @param wildcard Wild Card pattern
    @retval true  matched
    @retval false not matched
 */
bool String::isWcMatchAll(const char *wildcard) const {
    String match_str;
    const char *substr;
    bool matched;

    matched = false;

    if (wildcard) {
        match_str = (*this).wcMatch(wildcard, &substr);
        if (!strcmp((*this).pText, match_str.c_str()) &&
                (substr == (const char *)NULL || substr[0] == '\0')) {
            matched = true;
        }
    }

    return matched;
}


/*! Transform each lines of (*this).pText to unique.
    @param void
    @return Temporary string object (uniqed text)
 */
String& String::uniq() const {
    String *tmp = (*this).tmpStr();
    long length;

    length = (*this).len();

    if (0 < length) {
        char *buf;
        long i, j, k;
        long token_start, token_len;

        // Memory allocation
        buf = new char[length + 1];
        strcpy(buf, (*this).pText);

        // Conversion
        i = 0;
        token_start = 0;
        while (buf[i] != '\0') {
            if (buf[i] == '\n') {
                token_len = i - token_start;

                // Delete all same lines
                j = i;
                while (buf[j] != '\0') {
                    if (buf[j] == '\n' && !strncmp(&(buf[j + 1]), &(buf[token_start]), token_len) && (buf[j + token_len + 1] == '\n' || buf[j + token_len + 1] == '\0')) {
                        k = j + 1;
                        while (buf[k + token_len + 1] != '\0') {
                            buf[k] = buf[k + token_len + 1];
                            k++;
                        }
                        buf[k] = '\0';
                    } else {
                        j++;
                    }
                }

                token_start = i + 1;
            }
            i++;
        }
        buf[i] = '\0';

        (*tmp).useAsText();
        (*tmp).pTextReplace(buf, i, -1, length + 1);
    } else {
        *tmp = "";
    }

    return *tmp;
}


/*! Read file content into (*this).pText.
    @param filename  File name to read.
    @return Length of read content, -1 if failed
 */
long String::loadFile(const String& filename) {
    return (*this).loadFile(filename.c_str());
}


/*! Read file content into (*this).pText.
    @param filename  File name to read.
    @return Length of read content, -1 if failed
 */
long String::loadFile(const char *filename) {
    FILE *fp;
    char *buf;
    long l, length;
    int retry;

    if (filename == NULL) {
        return -1;
    }

    length = 0;

    if ((*this).isText()) {
        // If text mode

        *this = "";

        if ((fp = fopen(filename, "r")) == NULL) {
            return -1;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_SH | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        // Memory allocation
        buf = new char[4096 + 1];

        while (0 < (l = fread(buf, 1, 4096, fp))) {
            buf[l] = '\0';
            (*this).add(buf);
            length += l;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        delete [] buf;
    } else {
        // If binary mode

        (*this).useAsBinary(0);

        if ((fp = fopen(filename, "rb")) == NULL) {
            return -1;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_SH | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        // Memory allocation
        buf = new char[4096 + 1];

        while (0 < (l = fread(buf, 1, 4096, fp))) {
            (*this).addBinary(buf, l);
            length += l;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        delete [] buf;
    }

    return length;
}


/*! Read file content from assigned position, then set into (*this).pText
    @param filename   File name to read
    @param start_pos  Starting position of file to write (from SEEK_SET)
    @param size: Length of to read
    @return Length of read content, -1 if failed
 */
long String::loadFilePos(const String& filename, long start_pos, long size) {
    return (*this).loadFilePos(filename.c_str(), start_pos, size);
}


/*! Read file content from assigned position, then set into (*this).pText
    @param filename   File name to read
    @param start_pos  Starting position of file to write (from SEEK_SET)
    @param size: Length of to read
    @return Length of read content, -1 if failed
 */
long String::loadFilePos(const char *filename, long start_pos, long size) {
    FILE *fp;
    char *buf;
    long l, length;
    int retry;

    *this = "";

    if (filename == NULL || start_pos < 0 || size < 0) {
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    length = 0;

    if ((*this).isText()) {
        // If text mode

        if ((fp = fopen(filename, "r")) == NULL) {
            return -1;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_SH | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        fseek(fp, start_pos, SEEK_SET);

        // Memory allocation
        buf = new char[size + 1];

        if (start_pos == ftell(fp) && 0 < (l = fread(buf, 1, size, fp))) {
            buf[l] = '\0';
            (*this).add(buf);
            length += l;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        delete [] buf;
    } else {
        // If binary mode

        (*this).useAsBinary(0);

        if ((fp = fopen(filename, "rb")) == NULL) {
            return -1;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_SH | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        fseek(fp, start_pos, SEEK_SET);

        // Memory allocation
        buf = new char[size + 1];

        if (start_pos == ftell(fp) && 0 < (l = fread(buf, 1, size, fp))) {
            (*this).addBinary(buf, l);
            length += l;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        delete [] buf;
    }

    return length;
}


/*! Write (*this).pText content to file.
    @param void
    @retval true  success
    @retval false failure
 */
bool String::saveFile(const String& filename) const {
    return (*this).saveFile(filename.c_str());
}


/*! Write (*this).pText content to file.
    @param void
    @retval true  success
    @retval false failure
 */
bool String::saveFile(const char *filename) const {
    FILE *fp;
    int retry;
    char *filename_tmp;

    if (filename == NULL) {
        return false;
    }

    filename_tmp = new char[strlen(filename) + 5];
    strcpy(filename_tmp, filename);
    strcat(filename_tmp, ".tmp");

    if ((*this).isText()) {
        if ((fp = fopen(filename_tmp, "w")) == NULL) {
            fclose(fp);
            delete [] filename_tmp;
            return false;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_EX | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        fwrite((*this).pText, (*this).len(), 1, fp);
        if (ferror(fp)) {
            clearerr(fp);
            fclose(fp);
            delete [] filename_tmp;
            return false;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        rename(filename_tmp, filename);
    } else {
        if ((fp = fopen(filename_tmp, "wb")) == NULL) {
            fclose(fp);
            delete [] filename_tmp;
            return false;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_EX | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        fwrite((*this).pText, (*this).binaryLength(), 1, fp);
        if (ferror(fp)) {
            clearerr(fp);
            fclose(fp);
            delete [] filename_tmp;
            return false;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        rename(filename_tmp, filename);
    }

    delete [] filename_tmp;

    return true;
}


/*! Write (*this).pText content to file from assigned position
    @param filename   File name to save
    @param start_pos  Starting position of file to write (from SEEK_SET)
    @param size       Length of to write
    @retval true  success
    @retval false failure
 */
bool String::saveFilePos(const String& filename, long start_pos, long size) const {
    return (*this).saveFilePos(filename.c_str(), start_pos, size);
}


/*! Write (*this).pText content to file from assigned position
    @param filename   File name to save
    @param start_pos  Starting position of file to write (from SEEK_SET)
    @param size       Length of to write
    @retval true  success
    @retval false failure
 */
bool String::saveFilePos(const char *filename, long start_pos, long size) const {
    FILE *fp;
    int retry;
    long s;

    if (filename == NULL || start_pos < 0 || size < 0) {
        return false;
    }

    if ((*this).isText()) {
        if ((fp = fopen(filename, "r+")) == NULL) {
            fclose(fp);
            return false;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_EX | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        fseek(fp, start_pos, SEEK_SET);

        if (start_pos != ftell(fp)) {
            fseek(fp, 0, SEEK_END);
            fclose(fp);
            return false;
        }
        s = (size < (*this).len())?size:(*this).len();
        fwrite((*this).pText, s, 1, fp);
        if (ferror(fp)) {
            clearerr(fp);
            fseek(fp, 0, SEEK_END);
            fclose(fp);
            return false;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
    } else {
        if ((fp = fopen(filename, "w+b")) == NULL) {
            fclose(fp);
            return false;
        }

        // Lock
        retry = 10;
        while(flock(fileno(fp), LOCK_EX | LOCK_NB) < 0) {
            if (retry <= 1) {
                fclose(fp);
                return -1;
            }
            retry--;
            usleep(1000);
        }

        fseek(fp, start_pos, SEEK_SET);

        if (start_pos != ftell(fp)) {
            fseek(fp, 0, SEEK_END);
            fclose(fp);
            return false;
        }
        s = (size < (*this).binaryLength())?size:(*this).binaryLength();
        fwrite((*this).pText, s, 1, fp);
        if (ferror(fp)) {
            clearerr(fp);
            fseek(fp, 0, SEEK_END);
            fclose(fp);
            return false;
        }

        flock(fileno(fp), LOCK_UN);
        fclose(fp);
    }

    return true;
}


/*! convert MML like Music sequencial text to MIDI format
    @param void
    @return Temporary string object (MIDI file image)
 */
String& String::toMIDI() const {
    String *tmp = (*this).tmpStr();

    (*tmp).useAsBinary(0);

    if ((*this).pText) {
        String *MIDI;
        long blen;

        MIDI = strmidi((*this).pText);

        blen = (*MIDI).nBinaryLength;
        (*tmp).pTextReplace((*MIDI).pText, 0, blen, blen);
        (*MIDI).pText = NULL;
        (*MIDI).nLength = 0;
        (*MIDI).nCapacity = 0;
        (*MIDI).nFixedLength = 0;
        (*MIDI).nBinaryLength = -1;
        (*MIDI).nLengthRenewRecommended = false;

        delete MIDI;
    }

    return *tmp;
}


/*! Concatenate string literal with String object
    @param value1  Left side string literal
    @param value2  Right side String object
    @return Temporary string object containing concatenated result
 */
String operator + (const char* value1, const String& value2) {
    String tmp(value1);

    tmp.add(value2);

    return tmp;
}


} // namespace apolloron
