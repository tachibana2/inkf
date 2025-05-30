#include <stdio.h>
#include <utility>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include "apolloron.h"

#if _MSC_VER >= 1400 // VC++ 8.0
#pragma warning( disable : 4996 )   // disable warning about strdup being deprecated.
#endif

namespace apolloron {

static bool isControlCharacter(char ch) {
    return ch > 0 && ch <= 0x1F;
}

static bool containsControlCharacter(const char* str) {
    while(*str) {
        if(isControlCharacter(*(str++)))
            return true;
    }
    return false;
}
static void uintToString(unsigned int value,
                         char *&current) {
    *--current = 0;
    do {
        *--current = (value % 10) + '0';
        value /= 10;
    } while(value != 0);
}

String valueToString(Int value) {
    char buffer[32];
    char *current = buffer + sizeof(buffer);
    bool isNegative = value < 0;
    if(isNegative)
        value = -value;
    uintToString(UInt(value), current);
    if(isNegative)
        *--current = '-';
    assert(current >= buffer);
    return current;
}


String valueToString(UInt value) {
    char buffer[32];
    char *current = buffer + sizeof(buffer);
    uintToString(value, current);
    assert(current >= buffer);
    return current;
}

String valueToString(double value) {
    char buffer[32];
#if defined(_MSC_VER) && defined(__STDC_SECURE_LIB__) // Use secure version with visual studio 2005 to avoid warning. 
    sprintf_s(buffer, sizeof(buffer), "%#.16g", value);
#else
    ::snprintf(buffer, sizeof(buffer), "%#.16g", value);
#endif
    char* ch = buffer + strlen(buffer) - 1;
    if(*ch != '0') return buffer;  // nothing to truncate, so save time
    while(ch > buffer && *ch == '0') {
        --ch;
    }
    char* last_nonzero = ch;
    while(ch >= buffer) {
        switch(*ch) {
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
                --ch;
                continue;
            case '.':
                // Truncate zeroes to save bytes in output, but keep one.
                *(last_nonzero + 2) = '\0';
                return buffer;
            default:
                return buffer;
        }
    }
    return buffer;
}


String valueToString(bool value) {
    return value ? "true" : "false";
}

String valueToQuotedString(const char *value) {
    // Not sure how to handle unicode...
    if(strpbrk(value, "\"\\\b\f\n\r\t") == NULL && !containsControlCharacter(value))
        return String("\"") + value + "\"";
    // We have to walk value and escape any special characters.
    // Appending to std::string is not efficient, but this should be rare.
    // (Note: forward slashes are *not* rare, but I am not escaping them.)
    unsigned maxsize = strlen(value) * 2 + 3; // allescaped+quotes+NULL
    String result;
    result.setFixedLength(maxsize); // to avoid lots of mallocs
    result += "\"";
    for(const char* c = value; *c != 0; ++c) {
        switch(*c) {
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            //case '/':
            // Even though \/ is considered a legal escape in JSON, a bare
            // slash is also legal, so I see no reason to escape it.
            // (I hope I am not misunderstanding something.
            // blep notes: actually escaping \/ may be useful in javascript to avoid </
            // sequence.
            // Should add a flag to allow this compatibility mode and prevent this
            // sequence from occurring.
            default:
                if(isControlCharacter(*c)) {
                    char oss[10];
                    ::snprintf(oss, sizeof(oss), "\\u%04x", (int)(*c));
                    result += oss;
                } else {
                    result += *c;
                }
                break;
        }
    }
    result += "\"";
    return result;
}

// Class JSONWriter
// //////////////////////////////////////////////////////////////////
JSONWriter::~JSONWriter() {
}


// Class JSONFastWriter
// //////////////////////////////////////////////////////////////////

JSONFastWriter::JSONFastWriter()
    : yamlCompatiblityEnabled_(false) {
}


void JSONFastWriter::enableYAMLCompatibility() {
    yamlCompatiblityEnabled_ = true;
}


String JSONFastWriter::write(const JSONValue &root) {
    document_ = "";
    writeValue(root);
    document_ += "\n";
    return document_.c_str();
}


void JSONFastWriter::writeValue(const JSONValue &value) {
    switch(value.type()) {
        case nullValue:
            document_ += "null";
            break;
        case intValue:
            document_ += valueToString(value.asInt()).c_str();
            break;
        case uintValue:
            document_ += valueToString(value.asUInt()).c_str();
            break;
        case realValue:
            document_ += valueToString(value.asDouble()).c_str();
            break;
        case stringValue:
            document_ += valueToQuotedString(value.c_str()).c_str();
            break;
        case booleanValue:
            document_ += valueToString(value.asBool()).c_str();
            break;
        case arrayValue: {
            document_ += "[";
            int size = value.size();
            for(int index = 0; index < size; ++index) {
                if(index > 0)
                    document_ += ",";
                writeValue(value[index]);
            }
            document_ += "]";
        }
        break;
        case objectValue: {
            JSONValue::Members members(value.getMemberNames());
            document_ += "{";
            for(JSONValue::Members::iterator it = members.begin();
                    it != members.end();
                    ++it) {
                const std::string &name = *it;
                if(it != members.begin())
                    document_ += ",";
                document_ += valueToQuotedString(name.c_str()).c_str();
                document_ += yamlCompatiblityEnabled_ ? ": "
                             : ":";
                writeValue(value[name]);
            }
            document_ += "}";
        }
        break;
    }
}


// Class JSONStyledWriter
// //////////////////////////////////////////////////////////////////

JSONStyledWriter::JSONStyledWriter()
    : rightMargin_(74)
    , indentSize_(3) {
}


String JSONStyledWriter::write(const JSONValue &root) {
    document_ = "";
    addChildValues_ = false;
    indentString_ = "";
    writeCommentBeforeValue(root);
    writeValue(root);
    writeCommentAfterValueOnSameLine(root);
    document_ += "\n";
    return document_.c_str();
}


void JSONStyledWriter::writeValue(const JSONValue &value) {
    switch(value.type()) {
        case nullValue:
            pushValue("null");
            break;
        case intValue:
            pushValue(valueToString(value.asInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble()));
            break;
        case stringValue:
            pushValue(valueToQuotedString(value.c_str()));
            break;
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            JSONValue::Members members(value.getMemberNames());
            if(members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                JSONValue::Members::iterator it = members.begin();
                while(true) {
                    const std::string &name = *it;
                    const JSONValue &childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedString(name.c_str()));
                    document_ += " : ";
                    writeValue(childValue);
                    if(++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    document_ += ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        }
        break;
    }
}


void JSONStyledWriter::writeArrayValue(const JSONValue &value) {
    unsigned size = value.size();
    if(size == 0)
        pushValue("[]");
    else {
        bool isArrayMultiLine = isMultineArray(value);
        if(isArrayMultiLine) {
            writeWithIndent("[");
            indent();
            bool hasChildValue = !childValues_.empty();
            unsigned index = 0;
            while(true) {
                const JSONValue &childValue = value[index];
                writeCommentBeforeValue(childValue);
                if(hasChildValue)
                    writeWithIndent(childValues_[index].c_str());
                else {
                    writeIndent();
                    writeValue(childValue);
                }
                if(++index == size) {
                    writeCommentAfterValueOnSameLine(childValue);
                    break;
                }
                document_ += ",";
                writeCommentAfterValueOnSameLine(childValue);
            }
            unindent();
            writeWithIndent("]");
        } else { // output on a single line
            assert(childValues_.size() == size);
            document_ += "[ ";
            for(unsigned index = 0; index < size; ++index) {
                if(index > 0)
                    document_ += ", ";
                document_ += childValues_[index];
            }
            document_ += " ]";
        }
    }
}


bool JSONStyledWriter::isMultineArray(const JSONValue &value) {
    int size = value.size();
    bool isMultiLine = size * 3 >= rightMargin_ ;
    childValues_.clear();
    for(int index = 0; index < size  &&  !isMultiLine; ++index) {
        const JSONValue &childValue = value[index];
        isMultiLine = isMultiLine  ||
                      ((childValue.isArray()  ||  childValue.isObject())  &&
                       childValue.size() > 0);
    }
    if(!isMultiLine) {    // check if line length > max line length
        childValues_.reserve(size);
        addChildValues_ = true;
        int lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
        for(int index = 0; index < size  &&  !isMultiLine; ++index) {
            writeValue(value[index]);
            lineLength += int(childValues_[index].length());
            isMultiLine = isMultiLine  &&  hasCommentForValue(value[index]);
        }
        addChildValues_ = false;
        isMultiLine = isMultiLine  ||  lineLength >= rightMargin_;
    }
    return isMultiLine;
}


void JSONStyledWriter::pushValue(const String &value) {
    if(addChildValues_)
        childValues_.push_back(value.c_str());
    else
        document_ += value.c_str();
}


void JSONStyledWriter::writeIndent() {
    if(!document_.empty()) {
        char last = document_[document_.length() - 1];
        if(last == ' ')        // already indented
            return;
        if(last != '\n')       // Comments may add new-line
            document_ += '\n';
    }
    document_ += indentString_;
}


void JSONStyledWriter::writeWithIndent(const String &value) {
    writeIndent();
    document_ += value.c_str();
}


void JSONStyledWriter::indent() {
    indentString_ += std::string(indentSize_, ' ');
}


void JSONStyledWriter::unindent() {
    assert(int(indentString_.size()) >= indentSize_);
    indentString_.resize(indentString_.size() - indentSize_);
}


void JSONStyledWriter::writeCommentBeforeValue(const JSONValue &root) {
    if(!root.hasComment(commentBefore))
        return;
    document_ += normalizeEOL(root.getComment(commentBefore).c_str());
    document_ += "\n";
}


void JSONStyledWriter::writeCommentAfterValueOnSameLine(const JSONValue &root) {
    if(root.hasComment(commentAfterOnSameLine))
        document_ += " " + normalizeEOL(root.getComment(commentAfterOnSameLine).c_str());

    if(root.hasComment(commentAfter)) {
        document_ += "\n";
        document_ += normalizeEOL(root.getComment(commentAfter).c_str());
        document_ += "\n";
    }
}


bool JSONStyledWriter::hasCommentForValue(const JSONValue &value) {
    return value.hasComment(commentBefore)
           ||  value.hasComment(commentAfterOnSameLine)
           ||  value.hasComment(commentAfter);
}


std::string JSONStyledWriter::normalizeEOL(const std::string &text) {
    std::string normalized;
    normalized.reserve(text.length());
    const char *begin = text.c_str();
    const char *end = begin + text.length();
    const char *current = begin;
    while(current != end) {
        char c = *current++;
        if(c == '\r') {    // mac or dos EOL
            if(*current == '\n')    // convert dos EOL
                ++current;
            normalized += '\n';
        } else // handle unix EOL & other char
            normalized += c;
    }
    return normalized;
}


// Class JSONStyledStreamWriter
// //////////////////////////////////////////////////////////////////

JSONStyledStreamWriter::JSONStyledStreamWriter(String indentation)
    : document_(NULL)
    , rightMargin_(74)
    , indentation_(indentation.c_str()) {
}


void JSONStyledStreamWriter::write(std::ostream &out, const JSONValue &root) {
    document_ = &out;
    addChildValues_ = false;
    indentString_ = "";
    writeCommentBeforeValue(root);
    writeValue(root);
    writeCommentAfterValueOnSameLine(root);
    *document_ << "\n";
    document_ = NULL; // Forget the stream, for safety.
}


void JSONStyledStreamWriter::writeValue(const JSONValue &value) {
    switch(value.type()) {
        case nullValue:
            pushValue("null");
            break;
        case intValue:
            pushValue(valueToString(value.asInt()));
            break;
        case uintValue:
            pushValue(valueToString(value.asUInt()));
            break;
        case realValue:
            pushValue(valueToString(value.asDouble()));
            break;
        case stringValue:
            pushValue(valueToQuotedString(value.c_str()));
            break;
        case booleanValue:
            pushValue(valueToString(value.asBool()));
            break;
        case arrayValue:
            writeArrayValue(value);
            break;
        case objectValue: {
            JSONValue::Members members(value.getMemberNames());
            if(members.empty())
                pushValue("{}");
            else {
                writeWithIndent("{");
                indent();
                JSONValue::Members::iterator it = members.begin();
                while(true) {
                    const std::string &name = *it;
                    const JSONValue &childValue = value[name];
                    writeCommentBeforeValue(childValue);
                    writeWithIndent(valueToQuotedString(name.c_str()));
                    *document_ << " : ";
                    writeValue(childValue);
                    if(++it == members.end()) {
                        writeCommentAfterValueOnSameLine(childValue);
                        break;
                    }
                    *document_ << ",";
                    writeCommentAfterValueOnSameLine(childValue);
                }
                unindent();
                writeWithIndent("}");
            }
        }
        break;
    }
}


void JSONStyledStreamWriter::writeArrayValue(const JSONValue &value) {
    unsigned size = value.size();
    if(size == 0)
        pushValue("[]");
    else {
        bool isArrayMultiLine = isMultineArray(value);
        if(isArrayMultiLine) {
            writeWithIndent("[");
            indent();
            bool hasChildValue = !childValues_.empty();
            unsigned index = 0;
            while(true) {
                const JSONValue &childValue = value[index];
                writeCommentBeforeValue(childValue);
                if(hasChildValue)
                    writeWithIndent(childValues_[index].c_str());
                else {
                    writeIndent();
                    writeValue(childValue);
                }
                if(++index == size) {
                    writeCommentAfterValueOnSameLine(childValue);
                    break;
                }
                *document_ << ",";
                writeCommentAfterValueOnSameLine(childValue);
            }
            unindent();
            writeWithIndent("]");
        } else { // output on a single line
            assert(childValues_.size() == size);
            *document_ << "[ ";
            for(unsigned index = 0; index < size; ++index) {
                if(index > 0)
                    *document_ << ", ";
                *document_ << childValues_[index];
            }
            *document_ << " ]";
        }
    }
}


bool JSONStyledStreamWriter::isMultineArray(const JSONValue &value) {
    int size = value.size();
    bool isMultiLine = size * 3 >= rightMargin_ ;
    childValues_.clear();
    for(int index = 0; index < size  &&  !isMultiLine; ++index) {
        const JSONValue &childValue = value[index];
        isMultiLine = isMultiLine  ||
                      ((childValue.isArray()  ||  childValue.isObject())  &&
                       childValue.size() > 0);
    }
    if(!isMultiLine) {    // check if line length > max line length
        childValues_.reserve(size);
        addChildValues_ = true;
        int lineLength = 4 + (size - 1) * 2; // '[ ' + ', '*n + ' ]'
        for(int index = 0; index < size  &&  !isMultiLine; ++index) {
            writeValue(value[index]);
            lineLength += int(childValues_[index].length());
            isMultiLine = isMultiLine  &&  hasCommentForValue(value[index]);
        }
        addChildValues_ = false;
        isMultiLine = isMultiLine  ||  lineLength >= rightMargin_;
    }
    return isMultiLine;
}


void JSONStyledStreamWriter::pushValue(const String &value) {
    if(addChildValues_)
        childValues_.push_back(value.c_str());
    else
        *document_ << value.c_str();
}


void JSONStyledStreamWriter::writeIndent() {
    /*
      Some comments in this method would have been nice. ;-)

     if ( !document_.empty() )
     {
        char last = document_[document_.length()-1];
        if ( last == ' ' )     // already indented
           return;
        if ( last != '\n' )    // Comments may add new-line
           *document_ << '\n';
     }
    */
    *document_ << '\n' << indentString_;
}


void JSONStyledStreamWriter::writeWithIndent(const String &value) {
    writeIndent();
    *document_ << value.c_str();
}


void JSONStyledStreamWriter::indent() {
    indentString_ += indentation_;
}


void JSONStyledStreamWriter::unindent() {
    assert(indentString_.size() >= indentation_.size());
    indentString_.resize(indentString_.size() - indentation_.size());
}


void JSONStyledStreamWriter::writeCommentBeforeValue(const JSONValue &root) {
    if(!root.hasComment(commentBefore))
        return;
    *document_ << normalizeEOL(root.getComment(commentBefore).c_str());
    *document_ << "\n";
}


void JSONStyledStreamWriter::writeCommentAfterValueOnSameLine(const JSONValue &root) {
    if(root.hasComment(commentAfterOnSameLine))
        *document_ << " " + normalizeEOL(root.getComment(commentAfterOnSameLine).c_str());

    if(root.hasComment(commentAfter)) {
        *document_ << "\n";
        *document_ << normalizeEOL(root.getComment(commentAfter).c_str());
        *document_ << "\n";
    }
}


bool JSONStyledStreamWriter::hasCommentForValue(const JSONValue &value) {
    return value.hasComment(commentBefore)
           ||  value.hasComment(commentAfterOnSameLine)
           ||  value.hasComment(commentAfter);
}


std::string JSONStyledStreamWriter::normalizeEOL(const std::string &text) {
    std::string normalized;
    normalized.reserve(text.length());
    const char *begin = text.c_str();
    const char *end = begin + text.length();
    const char *current = begin;
    while(current != end) {
        char c = *current++;
        if(c == '\r') {    // mac or dos EOL
            if(*current == '\n')    // convert dos EOL
                ++current;
            normalized += '\n';
        } else // handle unix EOL & other char
            normalized += c;
    }
    return normalized;
}


std::ostream& operator<<(std::ostream &sout, const JSONValue &root) {
    apolloron::JSONStyledStreamWriter writer;

    writer.write(sout, root);
    return sout;
}


} // namespace apolloron
