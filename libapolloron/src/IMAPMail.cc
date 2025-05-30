/******************************************************************************/
/*! @file IMAPMail.cc
    @brief IMAPMail class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;

namespace apolloron {

/*! Constructor of IMAPMail.
    @param void
    @return void
 */
IMAPMail::IMAPMail(IMAPStream *imap_stream, const String &mailbox, long uid) {
    (*this).mailData = TMail();  // use value initialization instead of memset
    (*this).clear();

    (*this).imapStream = imap_stream;
    (*this).mailbox = mailbox;
    (*this).uid = uid;
}


/*! Destructor of IMAPMail.
    @param void
    @return void
 */
IMAPMail::~IMAPMail() {
    (*this).clear();
}


/*! Delete instance of IMAPMail.
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPMail::clear() {
    long i;

    (*this).imapStream = NULL;
    (*this).mailbox.clear();
    (*this).uid = 0L;

    if ((*this).mailData.internalDate) {
        delete [] (*this).mailData.internalDate;
        (*this).mailData.internalDate = (char *)NULL;
    }
    (*this).mailData.size = 0L;
    if ((*this).mailData.headerDate) {
        delete [] (*this).mailData.headerDate;
        (*this).mailData.headerDate = (char *)NULL;
    }
    if ((*this).mailData.messageID) {
        delete [] (*this).mailData.messageID;
        (*this).mailData.messageID = (char *)NULL;
    }
    if ((*this).mailData.subject) {
        delete [] (*this).mailData.subject;
        (*this).mailData.subject = (char *)NULL;
    }
    if ((*this).mailData.contentType) {
        delete [] (*this).mailData.contentType;
        (*this).mailData.contentType = (char *)NULL;
    }
    (*this).mailData.priority = 3;
    (*this).mailData.fromList.clear();
    (*this).mailData.senderList.clear();
    (*this).mailData.toList.clear();
    (*this).mailData.ccList.clear();
    (*this).mailData.bccList.clear();
    (*this).mailData.replyToList.clear();
    (*this).mailData.referenceList.clear();
    (*this).mailData.flagList.clear();
    if ((*this).mailData.parts != NULL) {
        for (i = 0; i < (*this).mailData.partsLength; i++) {
            if ((*this).mailData.parts[i].id) {
                delete [] (*this).mailData.parts[i].id;
            }
            if ((*this).mailData.parts[i].filename) {
                delete [] (*this).mailData.parts[i].filename;
            }
            if ((*this).mailData.parts[i].contentTransferEncoding) {
                delete [] (*this).mailData.parts[i].contentTransferEncoding;
            }
            if ((*this).mailData.parts[i].contentType) {
                delete [] (*this).mailData.parts[i].contentType;
            }
            if ((*this).mailData.parts[i].contentID) {
                delete [] (*this).mailData.parts[i].contentID;
            }
        }
        delete [] (*this).mailData.parts;
        (*this).mailData.parts = (TMailPart *)NULL;
    }
    (*this).mailData.partsLength = 0L;

    (*this).AttachFilePartIDList.clear();
    (*this).tmpString.clear();

    return true;
}


/*! set mail structure.
    @param void
    @retval true   success
    @retval false  failure
 */
bool IMAPMail::fetchMailStructure() {
    String receive_line;
    String send_line;
    String expect_line;
    String ext_header;
    String s;
    bool ret;
    long i, j, tag;

    (*this).clear();

    if ((*this).imapStream == NULL || (*this).imapStream->isLoggedIn() == false) {
        return false;
    }

    if ((*this).imapStream->getSelectedMailBox() != (*this).mailbox) {
        (*this).imapStream->examine((*this).mailbox);
    }

    // fetch body structure
    tag = (*this).imapStream->getNextTag();
    send_line.sprintf("%05ld UID FETCH %ld (INTERNALDATE RFC822.SIZE FLAGS "
                      "ENVELOPE BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Cont"
                      "ent-Type X-Priority References)])\r\n", tag, (*this).uid);
    ret = (*this).imapStream->send(send_line);
    send_line.clear();
    if (ret == false) {
        return false;
    }

    ext_header = "";

    do {
        receive_line = (*this).imapStream->receiveLine();
        if (receive_line.left(2) == "* " && isdigit(receive_line[2])) {
            long col, length;

            length = receive_line.len();
            col = 2;
            while (col < length) {
                if (receive_line[col] == ' ') {
                    col++;
                    break;
                }
                col++;
            }
            if (strncasecmp(&(receive_line.c_str()[col]), "FETCH ", 6) != 0) {
                continue;
            }
            col += 6;

            s = receive_line.mid(col).trim();

            i = s.len();
            j = 0L;
            if (3 < i && s[i-1] == '}') {
                i = i - 2;
                while (0 < i && isdigit(s[i])) {
                    i--;
                }
                if (s[i] == '{') {
                    j = atol(s + i + 1);
                    ext_header = (*this).imapStream->receive(j);
                    receive_line = s.mid(0L, i);
                    receive_line += (*this).imapStream->receiveLine();
                    s = receive_line.trim();
                }
            } else {
                receive_line = s;
            }

            if (2 < receive_line.len()) {
                // parse the response line
                String response = receive_line;

                // extract and parse INTERNALDATE
                i = response.search("INTERNALDATE \"");
                if (0 <= i) {
                    i += 14; // length of "INTERNALDATE \""
                    j = response.search("\"", i);
                    if (i < j) {
                        String date = response.mid(i, j - i);
                        if ((*this).mailData.internalDate) {
                            delete [] (*this).mailData.internalDate;
                        }
                        (*this).mailData.internalDate = new char [date.len() + 1];
                        strcpy((*this).mailData.internalDate, date.c_str());
                    }
                }

                // extract and parse RFC822.SIZE
                i = response.search("RFC822.SIZE ");
                if (0 <= i) {
                    i += 12; // length of "RFC822.SIZE "
                    j = response.search(" ", i);
                    if (i < j) {
                        (*this).mailData.size = atol(response.mid(i, j - i).c_str());
                    }
                }

                // extract and parse FLAGS
                i = response.search("FLAGS (");
                if (0 <= i) {
                    i += 7; // length of "FLAGS ("
                    j = response.search(")", i);
                    if (i < j) {
                        String flags = response.mid(i, j - i);
                        (*this).mailData.flagList.clear();

                        // Split flags by space and add them
                        long start = 0;
                        long pos;
                        while (0 <= (pos = flags.search(" ", start))) {
                            (*this).mailData.flagList.add(flags.mid(start, pos - start));
                            start = pos + 1;
                        }
                        if (start < flags.len()) {
                            (*this).mailData.flagList.add(flags.mid(start));
                        }
                    }
                }

                // extract and parse ENVELOPE
                i = response.search("ENVELOPE (");
                if (0 <= i) {
                    i += 10; // length of "ENVELOPE ("
                    j = response.search(")", i);
                    if (i < j) {
                        String envelope = response.mid(i, j - i);
                        // parse envelope fields (date, subject, from, sender, reply-to, to, cc, bcc, in-reply-to, message-id)
                        (*this).parseEnvelope(envelope);
                    }
                }

                // extract and parse BODYSTRUCTURE
                i = response.search("BODYSTRUCTURE (");
                if (0 <= i) {
                    i += 14; // length of "BODYSTRUCTURE ("
                    j = (*this).findMatchingParenthesis(response, i - 1);
                    if (i < j) {
                        String structure = response.mid(i, j - i);
                        (*this).parseBodyStructure(structure);
                    }
                }

                // parse Content-Type and other header fields from ext_header if available
                if (0 < ext_header.len()) {
                    (*this).parseHeaderFields(ext_header);
                }

            }
        }
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", tag);
    if (receive_line.left(expect_line.len()) != expect_line) {
        expect_line.clear();
        receive_line.clear();
        return false;
    }

    s.clear();
    ext_header.clear();
    expect_line.clear();
    receive_line.clear();

    return true;
}


/*! Parse IMAP envelope structure.
    @param envelope Envelope string from IMAP response
    @retval void
 */
void IMAPMail::parseEnvelope(const String& envelope) {
    long pos = 0;

    // format: (date subject from sender reply-to to cc bcc in-reply-to message-id)
    // each field can be NIL or a string/list

    // parse date
    String date = (*this).parseEnvelopeElement(envelope, &pos);
    if (date != "NIL") {
        if ((*this).mailData.headerDate) {
            delete [] (*this).mailData.headerDate;
        }
        (*this).mailData.headerDate = new char [date.len() + 1];
        strcpy((*this).mailData.headerDate, date.c_str());
    }

    // parse subject
    String subject = (*this).parseEnvelopeElement(envelope, &pos);
    if (subject != "NIL") {
        if ((*this).mailData.subject) {
            delete [] (*this).mailData.subject;
        }
        // decode MIME and convert to UTF-8
        String decodedSubject = subject.decodeMIME("AUTODETECT", "UTF-8");
        (*this).mailData.subject = new char [decodedSubject.len() + 1];
        strcpy((*this).mailData.subject, decodedSubject.c_str());
    }

    // parse from addresses
    (*this).parseAddressList(envelope, &pos, (*this).mailData.fromList);

    // parse sender
    (*this).parseAddressList(envelope, &pos, (*this).mailData.senderList);

    // parse reply-to
    (*this).parseAddressList(envelope, &pos, (*this).mailData.replyToList);

    // parse to
    (*this).parseAddressList(envelope, &pos, (*this).mailData.toList);

    // parse cc
    (*this).parseAddressList(envelope, &pos, (*this).mailData.ccList);

    // parse bcc
    (*this).parseAddressList(envelope, &pos, (*this).mailData.bccList);

    // parse in-reply-to
    String inReplyTo = (*this).parseEnvelopeElement(envelope, &pos);
    if (inReplyTo != "NIL") {
        (*this).mailData.referenceList.add(inReplyTo);
    }

    // parse message-id
    String messageId = (*this).parseEnvelopeElement(envelope, &pos);
    if (messageId != "NIL") {
        if ((*this).mailData.messageID) {
            delete [] (*this).mailData.messageID;
        }
        (*this).mailData.messageID = new char [messageId.len() + 1];
        strcpy((*this).mailData.messageID, messageId.c_str());
    }
}


/*! Parse a single element from the envelope structure.
    @param envelope Full envelope string
    @param pos Current position in envelope string (updated)
    @retval String Parsed element value
 */
String IMAPMail::parseEnvelopeElement(const String& envelope, long *pos) {
    while (*pos < envelope.len() && isspace(envelope[pos])) (*pos)++;

    if (envelope.len() <= *pos) return "NIL";

    // check for NIL
    if (!strncasecmp(envelope.c_str() + *pos, "NIL", 3)) {
        (*pos) += 3;
        return "NIL";
    }

    // check for literal {size}
    if (envelope[*pos] == '{') {
        (*pos)++; // skip opening brace
        long start = *pos;

        // find the end of size number
        while (*pos < envelope.len() && isdigit(envelope[*pos])) {
            (*pos)++;
        }

        // convert size string to number
        long literalSize = 0;
        if (start < *pos) {
            literalSize = atol(envelope.mid(start, *pos - start).c_str());
        }

        if (*pos < envelope.len() && envelope[*pos] == '}') {
            (*pos)++; // skip closing brace

            // skip CRLF
            if (*pos + 1 < envelope.len() && 
                envelope[*pos] == '\r' && 
                envelope[*pos + 1] == '\n') {
                (*pos) += 2;

                // read literal data
                if (*pos + literalSize <= envelope.len()) {
                    String result = envelope.mid(*pos, literalSize);
                    *pos += literalSize;
                    return result;
                }
            }
        }
        return "NIL";
    }

    // check for quoted string
    if (envelope[*pos] == '\"') {
        (*pos)++; // skip opening quote
        String result;
        while (*pos < envelope.len() && envelope[*pos] != '\"') {
            if (envelope[*pos] == '\\') (*pos)++; // skip escape character
            if (*pos < envelope.len()) result += envelope[(*pos)++];
        }
        if (*pos < envelope.len()) (*pos)++; // skip closing quote
        return result;
    }

    // check for atom
    if (!isspace(envelope[*pos]) && envelope[*pos] != '(' && envelope[*pos] != ')') {
        String result;
        while (*pos < envelope.len() && 
               !isspace(envelope[*pos]) && 
               envelope[*pos] != '(' && 
               envelope[*pos] != ')') {
            result += envelope[(*pos)++];
        }
        return result;
    }

    return "NIL";
}


/*! Parse list of email addresses from envelope.
    @param envelope Full envelope string
    @param pos Current position in envelope string (updated)
    @param addressList List to store parsed addresses
    @retval void
 */
void IMAPMail::parseAddressList(const String& envelope, long *pos, List& addressList) {
    while (*pos < envelope.len() && isspace(envelope[*pos])) (*pos)++;

    if (envelope.len() <= *pos) return;

    // check for NIL
    if (!strncasecmp(envelope.c_str() + *pos, "NIL", 3)) {
        (*pos) += 3;
        return;
    }

    // parse address list: ((name host-name mail-box host) ...)
    if (envelope[*pos] == '(') {
        (*pos)++; // skip opening parenthesis

        while (*pos < envelope.len()) {
            while (*pos < envelope.len() && isspace(envelope[*pos])) (*pos)++;

            if (envelope[*pos] == ')') {
                (*pos)++;
                break;
            }

            if (envelope[*pos] == '(') {
                (*pos)++; // skip opening parenthesis of address

                // parse address components
                String name = (*this).parseEnvelopeElement(envelope, pos);
                String hostName = (*this).parseEnvelopeElement(envelope, pos);
                String mailBox = (*this).parseEnvelopeElement(envelope, pos);
                String host = (*this).parseEnvelopeElement(envelope, pos);

                // construct email address
                String address;
                if (name != "NIL") {
                    // decode name and convert to UTF-8
                    String decodedName = name.decodeMIME("AUTODETECT", "UTF-8");
                    address = decodedName.replace("\t", " ").replace("\r\n", " ").replace("\n", " ");
                }
                address += "\t";
                if (mailBox != "NIL" && host != "NIL") {
                    address += mailBox + "@" + host;
                } else if (mailBox != "NIL") {
                    address += mailBox;
                }

                if (address != "\t") {
                    addressList.add(address);
                }

                while (*pos < envelope.len() && envelope[*pos] != ')') (*pos)++;
                if (*pos < envelope.len()) (*pos)++; // Skip closing parenthesis of address
            }
        }
    }
}


/*! Parse IMAP body structure.
    @param structure Body structure string from IMAP response
    @retval void
 */
void IMAPMail::parseBodyStructure(const String& structure) {
    // clear existing parts
    if ((*this).mailData.parts != NULL) {
        for (long i = 0; i < (*this).mailData.partsLength; i++) {
            if ((*this).mailData.parts[i].id) delete [] (*this).mailData.parts[i].id;
            if ((*this).mailData.parts[i].filename) delete [] (*this).mailData.parts[i].filename;
            if ((*this).mailData.parts[i].contentType) delete [] (*this).mailData.parts[i].contentType;
            if ((*this).mailData.parts[i].contentTransferEncoding) delete [] (*this).mailData.parts[i].contentTransferEncoding;
            if ((*this).mailData.parts[i].contentID) delete [] (*this).mailData.parts[i].contentID;
        }
        delete [] (*this).mailData.parts;
        (*this).mailData.parts = NULL;
    }
    (*this).mailData.partsLength = 0;

    // count number of parts first
    long pos = 0;
    long numParts = 0;
    countBodyParts(structure, &pos, numParts);

    // allocate parts array
    if (0 < numParts) {
        (*this).mailData.parts = new TMailPart[numParts];
        (*this).mailData.partsLength = numParts;
        // initialize all parts to zero
        for (long i = 0; i < numParts; i++) {
            (*this).mailData.parts[i].id = NULL;
            (*this).mailData.parts[i].filename = NULL;
            (*this).mailData.parts[i].contentType = NULL;
            (*this).mailData.parts[i].contentTransferEncoding = NULL;
            (*this).mailData.parts[i].contentID = NULL;
        }
    }

    // parse the actual structure
    pos = 0;
    long currentPart = 0;
    parseBodyPart(structure, &pos, currentPart);
}


/*! Parse a body part from the body structure.
    @param structure Body structure string
    @param pos Current position in structure string (updated)
    @param currentPart Current part index (updated)
    @retval void
 */
void IMAPMail::parseBodyPart(const String& structure, long *pos, long& currentPart) {
    while (*pos < structure.len() && isspace(structure[*pos])) (*pos)++;

    if (structure.len() <= *pos || structure[*pos] != '(') return;
    (*pos)++; // skip opening parenthesis

    if ((*this).mailData.partsLength <= currentPart) return;

    // parse type
    String type = (*this).parseEnvelopeElement(structure, pos);
    if (type != "NIL") {
        String subtype = (*this).parseEnvelopeElement(structure, pos);
        if (subtype != "NIL") {
            String fullType = type + "/" + subtype;
            (*this).mailData.parts[currentPart].contentType = new char[fullType.len() + 1];
            strcpy((*this).mailData.parts[currentPart].contentType, fullType.c_str());
        }
    }

    // parse parameters
    if (structure[*pos] == '(') {
        (*pos)++; // skip opening parenthesis
        Keys paramMap; // store parameters for RFC2231 processing
        
        while (*pos < structure.len() && structure[*pos] != ')') {
            String param = (*this).parseEnvelopeElement(structure, pos);
            String value = (*this).parseEnvelopeElement(structure, pos);
            
            // handle RFC2231 encoded parameters and split parameters
            String baseParam = param;
            long asteriskPos = param.search("*");
            long partNumber = -1;
            
            if (0 <=asteriskPos) {
                // check if it's an encoded parameter (ends with *)
                if (param[param.len() - 1] == '*') {
                    baseParam = param.left(param.len() - 1);
                }
                
                // check if it's a split parameter (contains *n or *n*)
                String numStr;
                long secondAsterisk = param.search("*", asteriskPos + 1);
                if (0 <= secondAsterisk) {
                    numStr = param.mid(asteriskPos + 1, secondAsterisk - asteriskPos - 1);
                    baseParam = param.left(asteriskPos);
                } else if (asteriskPos < param.len() - 1) {
                    numStr = param.mid(asteriskPos + 1);
                    baseParam = param.left(asteriskPos);
                }
                
                if (0 < numStr.len()) {
                    partNumber = atol(numStr.c_str());
                }
            }
            
            // store in map with proper key for sorting
            String mapKey;
            if (0 <= partNumber) {
                String numBuf;
                numBuf.sprintf("%04ld", partNumber); // Zero-pad for proper sorting
                mapKey = baseParam + "*" + numBuf;
            } else {
                mapKey = baseParam;
            }
            
            // Store raw value in paramMap
            paramMap.addKey(mapKey, value);
        }
        if (*pos < structure.len()) (*pos)++; // skip closing parenthesis
        
        // Process filename parameters
        String filename;
        bool hasRFC2231 = false;
        
        // First try RFC2231 encoded filename (filename*)
        String key = "filename*";
        if (paramMap.isKeyExist(key)) {
            String value = paramMap.getValue(key);
            long singleQuote1 = value.search("'");
            if (0 <= singleQuote1) {
                String charset = value.left(singleQuote1);
                long singleQuote2 = value.search("'", singleQuote1 + 1);
                if (0 <= singleQuote2) {
                    String encodedValue = value.mid(singleQuote2 + 1);
                    String decodedValue = encodedValue.decodeURL();
                    String utf8Value = decodedValue.strconv(charset, "UTF-8");
                    if (0 < utf8Value.len()) {
                        filename = utf8Value;
                    } else {
                        filename = decodedValue;
                    }
                }
            }
            hasRFC2231 = true;
        }
        
        // If no RFC2231 filename found, try regular filename or name
        if (!hasRFC2231) {
            String value;
            if (paramMap.isKeyExist("filename")) {
                value = paramMap.getValue("filename");
            } else if (paramMap.isKeyExist("name")) {
                value = paramMap.getValue("name");
            }
            
            if (0 < value.len()) {
                filename = value.decodeMIME("AUTODETECT", "UTF-8");
            }
        }
        
        if (0 < filename.len()) {
            (*this).mailData.parts[currentPart].filename = new char[filename.len() + 1];
            strcpy((*this).mailData.parts[currentPart].filename, filename.c_str());
        }
    }

    // Parse ID
    String id = (*this).parseEnvelopeElement(structure, pos);
    if (id != "NIL") {
        const char *filename;
        (*this).mailData.parts[currentPart].id = new char[id.len() + 1];
        strcpy((*this).mailData.parts[currentPart].id, id.c_str());
        filename = (*this).mailData.parts[currentPart].filename;
        if (filename && filename != NULL) {
            (*this).AttachFilePartIDList.add(id);
        }
    }

    // Parse description (skip)
    (*this).parseEnvelopeElement(structure, pos);

    // Parse encoding
    String encoding = (*this).parseEnvelopeElement(structure, pos);
    if (encoding != "NIL") {
        (*this).mailData.parts[currentPart].contentTransferEncoding = new char[encoding.len() + 1];
        strcpy((*this).mailData.parts[currentPart].contentTransferEncoding, encoding.c_str());
    }

    // Parse size
    String sizeStr = (*this).parseEnvelopeElement(structure, pos);
    if (0 < sizeStr.len()) {
        (*this).mailData.parts[currentPart].size = atol(sizeStr.c_str());
    }

    currentPart++;
}


/*! Count the number of body parts in the structure.
    @param structure Body structure string
    @param pos Current position in structure string (updated)
    @param count Number of parts (output)
    @retval void
 */
void IMAPMail::countBodyParts(const String& structure, long *pos, long& count) {
    while (*pos < structure.len() && isspace(structure[*pos])) (*pos)++;

    if (structure.len() <= *pos || structure[*pos] != '(') return;
    (*pos)++; // skip opening parenthesis

    count++; // Count this part

    // Skip through the structure until matching closing parenthesis
    int depth = 1;
    while (*pos < structure.len() && 0 < depth) {
        if (structure[*pos] == '(') depth++;
        else if (structure[*pos] == ')') depth--;
        (*pos)++;
    }
}


/*! Parse mail header fields.
    @param headers Header fields string
    @retval void
 */
void IMAPMail::parseHeaderFields(const String& headers) {
    long pos = 0;
    long len = headers.len();

    while (pos < len) {
        // find field name
        long start = pos;
        while (pos < len && headers[pos] != ':') pos++;

        if (len <= pos) break;

        String fieldName = headers.mid(start, pos - start).trim();
        pos++; // skip colon

        // find field value
        while (pos < len && isspace(headers[pos])) pos++;
        start = pos;

        // find end of value (next field or end of headers)
        while (pos < len) {
            if (pos + 1 < len && headers[pos] == '\r' && headers[pos + 1] == '\n') {
                if (pos + 3 < len && !isspace(headers[pos + 2])) {
                    break;
                }
                pos += 2;
            } else {
                pos++;
            }
        }

        String fieldValue = headers.mid(start, pos - start).trim();

        // store specific fields we're interested in
        if (!strcasecmp(fieldName.c_str(), "Content-Type")) {
            if ((*this).mailData.contentType) {
                delete [] (*this).mailData.contentType;
            }
            (*this).mailData.contentType = new char [fieldValue.len() + 1];
            strcpy((*this).mailData.contentType, fieldValue.c_str());
        } else if (!strcasecmp(fieldName.c_str(), "X-Priority")) {
            (*this).mailData.priority = atoi(fieldValue.c_str());
        }

        if (pos < len) pos += 2; // Skip CRLF
    }
}


/*! Find matching closing parenthesis.
    @param str String to search in
    @param start Starting position (position of opening parenthesis)
    @retval long Position of matching closing parenthesis or -1 if not found
 */
long IMAPMail::findMatchingParenthesis(const String& str, long start) {
    long count = 1;
    long pos = start + 1;
    long len = str.len();

    while (pos < len && 0 < count) {
        if (str[pos] == '(') {
            count++;
        } else if (str[pos] == ')') {
            count--;
        } else if (str[pos] == '\"') {
            pos++; // skip opening quote
            while (pos < len && str[pos] != '\"') {
                if (str[pos] == '\\') pos++; // Skip escape character
                if (pos < len) pos++;
            }
        }
        pos++;
    }

    return count == 0?pos-1:-1;
}


/*! get raw mail source.
    @param void
    @retuen .eml image
 */
String& IMAPMail::getEML() {
    String receive_line;
    String send_line;
    String expect_line;
    String ext_header;
    bool ret;
    long i, j, tag;

    if ((*this).imapStream == NULL || (*this).imapStream->isLoggedIn() == false) {
        (*this).tmpString = "";
        return (*this).tmpString;
    }

    if ((*this).imapStream->getSelectedMailBox() != (*this).mailbox) {
        (*this).imapStream->examine((*this).mailbox);
    }

    // get mail source (A002 UID FETCH 123 (RFC822))
    tag = (*this).imapStream->getNextTag();
    send_line.sprintf("%05ld UID FETCH %ld (RFC822)\r\n", tag, (*this).uid);
    ret = (*this).imapStream->send(send_line);
    send_line.clear();
    if (ret == false) {
        (*this).tmpString = "";
        return (*this).tmpString;
    }

    (*this).tmpString = "";

    do {
        receive_line = (*this).imapStream->receiveLine();
        if (receive_line.left(2) == "* " && isdigit(receive_line[2])) {
            long col, length;
            String s;

            length = receive_line.len();
            col = 2;
            while (col < length) {
                if (receive_line[col] == ' ') {
                    col++;
                    break;
                }
                col++;
            }
            if (strncasecmp(&(receive_line.c_str()[col]), "FETCH ", 6) != 0) {
                continue;
            }
            col += 6;

            s = receive_line.mid(col).trim();

            i = s.len();
            j = 0L;
            if (3 < i && s[i-1] == '}') {
                i = i - 2;
                while (0 < i && isdigit(s[i])) {
                    i--;
                }
                if (s[i] == '{') {
                    j = atol(s + i + 1);
                    (*this).tmpString = (*this).imapStream->receive(j);
                    receive_line = s.mid(0L, i);
                    receive_line += (*this).imapStream->receiveLine();
                    s = receive_line.trim();
                }
            } else {
                receive_line = s;
            }
            s.clear();
        }
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", tag);
    if (receive_line.left(expect_line.len()) != expect_line) {
        expect_line.clear();
        receive_line.clear();
        (*this).tmpString = "";
        return (*this).tmpString;
    }

    expect_line.clear();
    receive_line.clear();

    return (*this).tmpString;
}


/*! get raw header text.
    @param void
    @retuen raw header text
 */
String& IMAPMail::getHeader() {
    String receive_line;
    String send_line;
    String expect_line;
    String ext_header;
    bool ret;
    long i, j, tag;

    if ((*this).imapStream == NULL || (*this).imapStream->isLoggedIn() == false) {
        (*this).tmpString = "";
        return (*this).tmpString;
    }

    if ((*this).imapStream->getSelectedMailBox() != (*this).mailbox) {
        (*this).imapStream->examine((*this).mailbox);
    }

    // get mail header (A002 UID FETCH 123 (RFC822.HEADER))
    tag = (*this).imapStream->getNextTag();
    send_line.sprintf("%05ld UID FETCH %ld (RFC822.HEADER)\r\n", tag, (*this).uid);
    ret = (*this).imapStream->send(send_line);
    send_line.clear();
    if (ret == false) {
        (*this).tmpString = "";
        return (*this).tmpString;
    }

    (*this).tmpString = "";

    do {
        receive_line = (*this).imapStream->receiveLine();
        if (receive_line.left(2) == "* " && isdigit(receive_line[2])) {
            long col, length;
            String s;

            length = receive_line.len();
            col = 2;
            while (col < length) {
                if (receive_line[col] == ' ') {
                    col++;
                    break;
                }
                col++;
            }
            if (strncasecmp(&(receive_line.c_str()[col]), "FETCH ", 6) != 0) {
                continue;
            }
            col += 6;

            s = receive_line.mid(col).trim();

            i = s.len();
            j = 0L;
            if (3 < i && s[i-1] == '}') {
                i = i - 2;
                while (0 < i && isdigit(s[i])) {
                    i--;
                }
                if (s[i] == '{') {
                    j = atol(s + i + 1);
                    (*this).tmpString = (*this).imapStream->receive(j);
                    receive_line = s.mid(0L, i);
                    receive_line += (*this).imapStream->receiveLine();
                    s = receive_line.trim();
                }
            } else {
                receive_line = s;
            }
            s.clear();
        }
    } while (receive_line.left(2) == "* ");

    expect_line.sprintf("%05ld OK", tag);
    if (receive_line.left(expect_line.len()) != expect_line) {
        expect_line.clear();
        receive_line.clear();
        (*this).tmpString = "";
        return (*this).tmpString;
    }

    expect_line.clear();
    receive_line.clear();

    return (*this).tmpString;
}


/*! get IMAP INTERNALDATE.
    @param void
    @retuen IMAP INTERNALDATE
 */
String& IMAPMail::getInteralDate() {

    if ((*this).mailData.internalDate == NULL) {
        // get IMAP INTERNALDATE
        (*this).fetchMailStructure();
    }

    (*this).tmpString = (*this).mailData.internalDate;

    return (*this).tmpString;
}


/*! get IMAP RFC822.SIZE
    @param void
    @retuen IMAP  RFC822.SIZE
 */
long IMAPMail::getSize() {

    if ((*this).mailData.internalDate == NULL) {
        // get IMAP RFC822.SIZE
        (*this).fetchMailStructure();
    }

    return (*this).mailData.size;
}


/*! get MIME-decoded date header (UTF-8)
    @param void
    @retuen MIME-decoded date header (UTF-8)
 */
String& IMAPMail::getHeaderDate() {

    if ((*this).mailData.internalDate == NULL) {
        // get date header
        (*this).fetchMailStructure();
    }

    (*this).tmpString = (*this).mailData.headerDate;

    return (*this).tmpString;
}


/*! get Message-ID
    @param void
    @retuen Message-ID
 */
String& IMAPMail::getMessageID() {

    if ((*this).mailData.internalDate == NULL) {
        // get Message-ID
        (*this).fetchMailStructure();
    }

    (*this).tmpString = (*this).mailData.messageID;

    return (*this).tmpString;
}


/*! get UTF-8 Subject
    @param void
    @retuen UTF-8 Subject
 */
String& IMAPMail::getSubject() {

    if ((*this).mailData.internalDate == NULL) {
        // get subject
        (*this).fetchMailStructure();
    }

    (*this).tmpString = (*this).mailData.subject;

    return (*this).tmpString;
}


/*! get UTF-8 From list
    @param void
    @retuen UTF-8 From list
 */
List& IMAPMail::getFromList() {

    if ((*this).mailData.internalDate == NULL) {
        // get From list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.fromList;
}


/*! get UTF-8 Sender list
    @param void
    @retuen UTF-8 Sender list
 */
List& IMAPMail::getSenderList() {

    if ((*this).mailData.internalDate == NULL) {
        // get Sender list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.senderList;
}


/*! get UTF-8 To list
    @param void
    @retuen UTF-8 To list
 */
List& IMAPMail::getToList() {

    if ((*this).mailData.internalDate == NULL) {
        // get To list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.toList;
}


/*! get UTF-8 Cc list
    @param void
    @retuen UTF-8 Cc list
 */
List& IMAPMail::getCcList() {

    if ((*this).mailData.internalDate == NULL) {
        // get Cc list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.ccList;
}


/*! get UTF-8 Bcc list
    @param void
    @retuen UTF-8 Bcc list
 */
List& IMAPMail::getBccList() {

    if ((*this).mailData.internalDate == NULL) {
        // get Bcc list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.bccList;
}


/*! get UTF-8 Reply-To list
    @param void
    @retuen UTF-8 Reply-To list
 */
List& IMAPMail::getReplyToList() {

    if ((*this).mailData.internalDate == NULL) {
        // get Reply-To list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.replyToList;
}


/*! get References
    @param void
    @retuen References
 */
List& IMAPMail::getReferenceList() {

    if ((*this).mailData.internalDate == NULL) {
        // get Reference list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.referenceList;
}


/*! get Flags
    @param void
    @retuen Flags
 */
List& IMAPMail::getFlagList() {

    if ((*this).mailData.internalDate == NULL) {
        // get Flag list
        (*this).fetchMailStructure();
    }

    return (*this).mailData.flagList;
}


/*! get plain text body
    @param void
    @retuen plain text body
 */
String& IMAPMail::getPlainTextBody() {


    // get plain text body


    return (*this).tmpString;
}


/*! get plain HTML body
    @param void
    @retuen plain HTML body
 */
String& IMAPMail::getHTMLBody() {


    // get plain HTML body


    return (*this).tmpString;
}


/*! get Part ID list of attachment files
    @param void
    @retuen Part ID list of attachment files
 */
List& IMAPMail::getAttachFilePartIDList() {


    // get Part ID list of attachment files


    return (*this).AttachFilePartIDList;
}


/*! get UTF-8 file name by Part ID
    @param part_id Part ID
    @retuen get UTF-8 file name
 */
String& IMAPMail::getAttachFileNane(const String &part_id) {


    // get UTF-8 file name by Part ID


    return (*this).tmpString;
}


/*! get file content(binary)
    @param part_id Part ID
    @retuen file content(binary)
 */
String& IMAPMail::getAttachFile(const String &part_id) {


    // get file content by Part ID


    return (*this).tmpString;
}


/*! get mail part (TMailPart structure)
    @param part_id Part ID
    @retuen mail part (TMailPart structure)
 */
const TMailPart& IMAPMail::getMailPart(const String &part_id) {
    long i;

    if ((*this).mailData.parts == (TMailPart *)NULL) {
        (*this).mailData.parts = new TMailPart [1];
        memset(&((*this).mailData.parts[0]), 0, sizeof(TMailPart));
        return (*this).mailData.parts[0];
    }

    // get mail part by Part ID
    i = 0;


    return (*this).mailData.parts[i];
}


} // namespace apolloron
