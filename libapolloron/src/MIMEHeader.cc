/******************************************************************************/
/*! @file MIMEHeader.cc
    @brief MIMEHeader class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apolloron.h"


namespace apolloron {

/*! Constructor of MIMEHeader.
    @param void
    @return void
 */
MIMEHeader::MIMEHeader() {
    (*this).tmpString = (String *)NULL;
    (*this).clear();
}


/*! Constructor of MIMEHeader.
    @param mime_header mime header text to set
    @return void
 */
MIMEHeader::MIMEHeader(const String &mime_header) {
    (*this).tmpString = (String *)NULL;
    (*this).clear();
    (*this).set(mime_header);
}


/*! Copy constructor of MIMEHeader.
    @param mime_header mime header object to set
    @return void
 */
MIMEHeader::MIMEHeader(MIMEHeader &mime_header) {
    (*this).tmpString = (String *)NULL;
    (*this).clear();
    (*this).set(mime_header);
}


/*! Destructor of MIMEHeader.
    @param void
    @return void
 */
MIMEHeader::~MIMEHeader() {
    (*this).clear();
}


/*! Clear MIMEHeader object.
    @param void
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::clear() {
    (*this).names.clear();
    (*this).values.clear();
    if ((*this).tmpString != (String *)NULL) {
        delete (*this).tmpString;
        (*this).tmpString = (String *)NULL;
    }

    return true;
}


/*! Get max number of rows.
    @param void
    @return max number of rows
 */
long MIMEHeader::maxRow() const {
    return (*this).names.max();
}


/*! Set data by copying from MIME header text.
    @param mime_header mime header text to set
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::set(const String &mime_header) {
    String str;
    long length, row, start, i, j, p;
    char c;

    (*this).clear();

    length = mime_header.len();
    row = 0;
    start = 0;
    while (start < length && isspace(mime_header[start])) start++;
    while (start < length) {
        j = -1;
        for (i = 0; (c = mime_header[start + i]) != '\0'; i++) {
            if (c == '\n') {
                j = i;
                break;
            } else if (c == '\r' && mime_header[start + i + 1] == '\n') {
                j = i;
                i++;
                break;
            }
        }
        if (j < 0) {
            if (mime_header[start + i] == '\0') {
                j = i;
            } else {
                j = 0;
            }
        }

        // blank line found, header lines end
        if (j == 0) {
            break;
        }

        if (0 < row && isspace(mime_header[start])) {
            (*this).values[row - 1] += "\r\n";
            (*this).values[row - 1] += mime_header.mid(start, j);
        } else {
            str = mime_header.mid(start, j);
            p = str.searchChar(':');
            if (0 < p) {
                (*this).names.add(str.left(p).trimR());
                (*this).values.add(str.mid(p + 1).trimL());
                row++;
            }
            str.clear();
        }

        start += (i + 1);
    }

    return true;
}


/*! Set data by copying from other MIMEHeader object.
    @param mime_header mime_header data to set
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::set(MIMEHeader &mime_header) {
    long row, row_max;

    (*this).clear();

    row_max = mime_header.maxRow();

    for (row = 0; row < row_max; row++) {
        (*this).addRow(mime_header.getName(row), mime_header.getValue(row));
    }

    return true;
}


/*! Set data by copying from MIME header text.
    @param mime_header mime header text to set
    @return reference of MIMEHeader
 */
const MIMEHeader &MIMEHeader::operator = (const String &mime_header) {
    (*this).set(mime_header);
    return (*this);
}


/*! Set data by copying from other MIMEHeader object.
    @param mime_header mime header data to set
    @return reference of MIMEHeader
 */
const MIMEHeader &MIMEHeader::operator = (MIMEHeader &mime_header) {
    (*this).set(mime_header);
    return (*this);
}


/*! Get header name of row by specified row index
    @param row row index
    @return header name of row
 */
String &MIMEHeader::getName(long row) {
    return names[row];
}


/*! Search the specified header name in header
    @param name string name to search
    @retuen row index (-1 if not found)
 */
long MIMEHeader::searchRow(const String &name) {
    long row, max;
    bool found;

    max = (*this).names.max();
    found = false;
    for (row = 0; row < max; row++) {
        if (!strcasecmp((*this).names[row].c_str(), name.c_str())) {
            found = true;
            break;
        }
    }

    return (found == true)?row:-1;
}


/*! Get value of row by specified row index
    @param row row index
    @return content of row
 */
String &MIMEHeader::getValue(long row) {
    return values[row];
}


/*! Get value of row by specified name
    @param name string name to get (ignore case, first match)
    @return content of row
 */
String &MIMEHeader::getValue(const String &name) {
    long row, max;
    bool found;

    max = (*this).names.max();
    found = false;
    for (row = 0; row < max; row++) {
        if (!strcasecmp((*this).names[row].c_str(), name.c_str())) {
            found = true;
            break;
        }
    }

    if (found == true) {
        return (*this).values[row];
    }

    if ((*this).tmpString == (String *)NULL) {
        (*this).tmpString = new String;
    }
    *((*this).tmpString) = "";
    return *((*this).tmpString);
}


/*! Add row (overwrite if there is same name row).
    @param name string name to set
    @param value string value to set
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::addRow(const String &name, const String &value) {
    if (name == "") {
        return false;
    }

    (*this).names.add(name);
    (*this).values.add(value);

    return true;
}


/*! Insert row.
    @param row row index
    @param name string name to set
    @param value string value to set
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::insertRow(long row, const String &name, const String &value) {
    if (row < 0 || (*this).names.max() <= row) {
        return false;
    }

    (*this).names.insert(row, name);
    (*this).values.insert(row, value);

    return true;
}


/*! Delete row.
    @param row row index
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::delRow(long row) {
    if (row < 0 || (*this).names.max() <= row) {
        return false;
    }

    (*this).names.remove(row);
    (*this).values.remove(row);

    return true;
}


/*! Set name into row.
    @param row row index
    @param name string name to set
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::setName(long row, const String &name) {
    if (row < 0 || (*this).names.max() <= row || name == "") {
        return false;
    }

    (*this).names[row] = name;

    return true;
}


/*! Set value into row.
    @param row row index
    @param value string value to set
    @retval true  success
    @retval false failure
 */
bool MIMEHeader::setValue(long row, const String &value) {
    if (row < 0 || (*this).names.max() <= row) {
        return false;
    }

    (*this).values[row] = value;

    return true;
}


String &MIMEHeader::toString() {
    long row, max;

    if ((*this).tmpString == (String *)NULL) {
        (*this).tmpString = new String;
    }
    *((*this).tmpString) = "";

    max = (*this).names.max();
    for (row = 0; row < max; row++) {
        *((*this).tmpString) += (*this).names[row];
        *((*this).tmpString) += ": ";
        *((*this).tmpString) += (*this).values[row];
        *((*this).tmpString) += "\r\n";
    }

    return *((*this).tmpString);
}


} // namespace apolloron
