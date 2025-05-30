/******************************************************************************/
/*! @file Sheet.cc
    @brief Sheet class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apolloron.h"


namespace {

/* modified qsort */
/*-
 * Copyright (c) 1992, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the University of
 *    California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define min(a, b) (a) < (b) ? a : b

#define swapcode(parmi, parmj, n) { \
  long i = (n) / sizeof (long); \
  long *pi = (long *) (parmi); \
  long *pj = (long *) (parmj); \
  do { \
    long t = *pi; \
    *pi++ = *pj; \
    *pj++ = t; \
  } while (--i > 0); \
}

inline void swapfunc(const char *a, const char *b, int n) {
    swapcode(a, b, n)
}

#define swap(a, b) \
  { \
    long t = *(long *)(a); \
    *(long *)(a) = *(long *)(b); \
    *(long *)(b) = t; \
  }

#define vecswap(a, b, n) if ((n) > 0) swapfunc(a, b, n)

inline const char *med3(const char *a, const char *b, const char *c, int (*cmp)(const void *, const void *, const char *), const char *key) {
    return cmp(a, b, key) < 0 ?
           (cmp(b, c, key) < 0 ? b : (cmp(a, c, key) < 0 ? c : a ))
           :(cmp(b, c, key) > 0 ? b : (cmp(a, c, key) < 0 ? a : c ));
}

void qsort_with_key(void *a, long n, size_t es, int (*cmp)(const void *, const void *, const char *), const char *key) {
    const char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    long d, r, swap_cnt;

loop:
    swap_cnt = 0;
    if (n < 7) {
        for (pm = (const char *)a + es; pm < (const char *)a + n * es; pm += es)
            for (pl = pm; pl > (const char *)a && cmp(pl - es, pl, key) > 0; pl -= es)
                swap(pl, (const char *)(pl - es));
        return;
    }
    pm = (const char *)a + (n / 2) * es;
    if (n > 7) {
        pl = (const char *)a;
        pn = (const char *)a + (n - 1) * es;
        if (n > 40) {
            d = (n / 8) * es;
            pl = med3(pl, pl + d, pl + 2 * d, cmp, key);
            pm = med3(pm - d, pm, pm + d, cmp, key);
            pn = med3(pn - 2 * d, pn - d, pn, cmp, key);
        }
        pm = med3(pl, pm, pn, cmp, key);
    }
    swap((const char *)a, pm);
    pa = pb = (const char *)a + es;

    pc = pd = (const char *)a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = cmp(pb, a, key)) <= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap(pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (r = cmp(pc, a, key)) >= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap(pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        swap(pb, pc);
        swap_cnt = 1;
        pb += es;
        pc -= es;
    }
    if (swap_cnt == 0) {  /* Switch to insertion sort */
        for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
            for (pl = pm; pl > (char *)a && cmp(pl - es, pl, key) > 0; pl -= es)
                swap(pl, pl - es);
        return;
    }

    pn = (const char *)a + n * es;
    r = min((unsigned long long)(pa - (const char *)a), (unsigned long long)(pb - pa));
    vecswap((const char *)a, pb - r, r);
    r = min((unsigned long long)(pd - pc), (unsigned long long)(pn - pd - es));
    vecswap(pb, pn - r, r);
    if ((r = (long)(pb - pa)) > (long)es)
        qsort_with_key(a, r / es, es, cmp, key);
    if ((r = (long)(pd - pc)) > (long)es) {
        /* Iterate rather than recurse to save stack space */
        a = (void *)(pn - r);
        n = r / es;
        goto loop;
    }
}


/* compare function for sort() */
static int _sheet_cmp_asc(const void *ppkeys1, const void *ppkeys2, const char *key) {
    apolloron::Keys *pkeys1, *pkeys2;
    const char *value1, *value2;
    int cmp_result;

    pkeys1 = *((apolloron::Keys **)ppkeys1);
    pkeys2 = *((apolloron::Keys **)ppkeys2);

    value1 = (*pkeys1)[key].c_str();
    value2 = (*pkeys2)[key].c_str();

    cmp_result = strcmp(value1, value2);

    return cmp_result;
}


/* compare function for sortr() */
static int _sheet_cmp_desc(const void *ppkeys1, const void *ppkeys2, const char *key) {
    apolloron::Keys *pkeys1, *pkeys2;
    const char *value1, *value2;
    int cmp_result;

    pkeys1 = *((apolloron::Keys **)ppkeys1);
    pkeys2 = *((apolloron::Keys **)ppkeys2);

    value1 = (*pkeys1)[key].c_str();
    value2 = (*pkeys2)[key].c_str();

    cmp_result = (-(strcmp(value1, value2)));

    return cmp_result;
}


/* compare function for sortnum() */
static int _sheet_cmp_num(const void *ppkeys1, const void *ppkeys2, const char *key) {
    apolloron::Keys *pkeys1, *pkeys2;
    const char *value1, *value2;
    long num1, num2;
    int cmp_result;

    pkeys1 = *((apolloron::Keys **)ppkeys1);
    pkeys2 = *((apolloron::Keys **)ppkeys2);

    value1 = (*pkeys1)[key].c_str();
    value2 = (*pkeys2)[key].c_str();

    num1 = atol(value1);
    num2 = atol(value2);
    if (num2 < num1) {
        cmp_result = 1;
    } else if (num1 < num2) {
        cmp_result = -1;
    } else {
        cmp_result = 0;
    }

    return cmp_result;
}

/* compare function for sortnumr() */
static int _sheet_cmp_numr(const void *ppkeys1, const void *ppkeys2, const char *key) {
    apolloron::Keys *pkeys1, *pkeys2;
    const char *value1, *value2;
    long num1, num2;
    int cmp_result;

    pkeys1 = *((apolloron::Keys **)ppkeys1);
    pkeys2 = *((apolloron::Keys **)ppkeys2);

    value1 = (*pkeys1)[key].c_str();
    value2 = (*pkeys2)[key].c_str();

    num1 = atol(value1);
    num2 = atol(value2);
    if (num1 < num2) {
        cmp_result = 1;
    } else if (num2 < num1) {
        cmp_result = -1;
    } else {
        cmp_result = 0;
    }

    return cmp_result;
}


/* Get line length of CSV string */
long _searchCSVLineEnd(const apolloron::String &csv, long start_pos) {
    bool quoted, comma;
    long i, max, length;

    quoted = false;
    comma = true;
    length = 0;
    max = csv.len();
    for (i = start_pos; i < max; i++) {
        length++;
        if (quoted == false) {
            if (csv[i] == '\n') {
                break;
            } else if (csv[i] == '\r' && csv[i+1] == '\n') {
                length++;
                break;
            } else if (csv[i] == '\r') {
                break;
            } else if (csv[i] == ',') {
                comma = true;
            } else if (comma == true && csv[i] == '"') {
                quoted = true;
            } else {
                comma = false;
            }
        } else {
            if (csv[i] == '"' && csv[i+1] == '"') {
                length++;
                i++;
            } else if (csv[i] == '"') {
                quoted = false;
            }
            comma = false;
        }
    }
    return length;
}


/* Get column length of CSV string */
long _searchCSVColEnd(const apolloron::String &csv, long start_pos, apolloron::String &cell_data, const char *src_charset, const char *dest_charset, const char *return_code) {
    bool quoted;
    long i, max, length, start, l;

    quoted = false;
    length = 0;
    max = csv.len();
    i = start_pos;

    cell_data = "";
    if (max < i) {
        return 0;
    }

    start = 0;
    if (csv[i] == '"') {
        start = 1;
        length++;
        quoted = true;
        i++;
    }
    l = 0;
    for (; i < max; i++) {
        length++;
        if (quoted == false) {
            if (csv[i] == '\n') {
                break;
            } else if (csv[i] == '\r' && csv[i+1] == '\n') {
                length++;
                break;
            } else if (csv[i] == '\r') {
                break;
            } else if (csv[i] == ',') {
                break;
            } else {
                l++;
            }
        } else {
            if (csv[i] == '"' && csv[i+1] == '"') {
                l += 2;
                length++;
                i++;
            } else if (csv[i] == '"') {
                quoted = false;
            } else {
                l++;
            }
        }
    }

    if (csv[start_pos] == '"') {
        cell_data = csv.mid(start_pos + start, l).unescapeCSV(src_charset, dest_charset, return_code);
    } else {
        cell_data = csv.mid(start_pos + start, l).strconv(src_charset, dest_charset).changeReturnCode(return_code);
    }

    return length;
}

} // namespace


namespace apolloron {

/*! Constructor of Sheet.
    @param void
    @return void
 */
Sheet::Sheet() {
    (*this).tmpString = (String *)NULL;
    (*this).tmpCSVString = (String *)NULL;
    (*this).pRows = (Keys **)NULL;
    (*this).rowsMax = 0;
    (*this).rowsCapacity = 0;
    (*this).clear();
}


/*! Copy constructor of Sheet.
    @param sheet sheet object to set
    @return void
 */
Sheet::Sheet(Sheet &sheet) {
    (*this).tmpString = (String *)NULL;
    (*this).tmpCSVString = (String *)NULL;
    (*this).pRows = (Keys **)NULL;
    (*this).rowsMax = 0;
    (*this).rowsCapacity = 0;
    (*this).clear();
    (*this).set(sheet);
}


/*! Destructor of Sheet.
    @param void
    @return void
 */
Sheet::~Sheet() {
    (*this).clear();
}


/*! Clear Sheet object.
    @param void
    @retval true  success
    @retval false failure
 */
bool Sheet::clear() {
    long i;

    (*this).columns.clear();
    if ((*this).pRows != (Keys **)NULL) {
        for (i = 0; i < (*this).rowsMax; i++) {
            delete (*this).pRows[i];
        }
        delete [] (*this).pRows;
    }
    (*this).pRows = (Keys **)NULL;
    (*this).rowsMax = 0;
    (*this).rowsCapacity = 0;
    if ((*this).tmpString != (String *)NULL) {
        delete (*this).tmpString;
        (*this).tmpString = (String *)NULL;
    }
    if ((*this).tmpCSVString != (String *)NULL) {
        delete (*this).tmpCSVString;
        (*this).tmpCSVString = (String *)NULL;
    }

    return true;
}


/*! Get max number of columns.
    @param void
    @return max number of columns
 */
long Sheet::maxCol() const {
    return (*this).columns.max();
}


/*! Get max number of rows.
    @param void
    @return max number of rows
 */
long Sheet::maxRow() const {
    return (*this).rowsMax;
}


/*! Set data by copying from other Sheet object.
    @param sheet sheet data to set
    @retval true  success
    @retval false failure
 */
bool Sheet::set(Sheet &sheet) {
    long col, row, col_max, row_max;

    (*this).clear();

    col_max = sheet.maxCol();
    row_max = sheet.maxRow();

    for (col = 0; col < col_max; col++) {
        (*this).addCol(sheet.col(col));
    }

    for (row = 0; row < row_max; row++) {
        (*this).addRow();
        for (col = 0; col < col_max; col++) {
            (*(*this).pRows[row])[sheet.col(col)] = sheet.getValue(sheet.col(col), row);
        }
    }

    return true;
}


/*! Set data by copying from other Sheet object.
    @param sheet sheet data to set
    @return reference of Sheet
 */
const Sheet &Sheet::operator = (Sheet &sheet) {
    (*this).set(sheet);
    return (*this);
}


/*! Compare sheets
    @param sheet data to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int Sheet::operator == (Sheet &sheet) {
    long col, row, col_max, row_max;

    col_max = (*this).columns.max();
    if (col_max != sheet.maxCol()) {
        return 0;
    }

    row_max = (*this).rowsMax;
    if (row_max != sheet.maxRow()) {
        return 0;
    }

    for (col = 0; col < col_max; col++) {
        if ((*this).columns[col] != sheet.col(col)) {
            return 0;
        }
    }

    for (row = 0; row < row_max; row++) {
        for (col = 0; col < col_max; col++) {
            String colname = (*this).columns[col];
            if ((*(*this).pRows[row])[colname] != sheet.getValue(colname, row)) {
                return 0;
            }
            colname.clear();
        }
    }

    return 1;
}


/*! Compare sheets
    @param sheet data to compare with
    @retval 1  if matched
    @retval 0  if not matched
 */
int Sheet::operator != (Sheet &sheet) {
    long col, row, col_max, row_max;

    col_max = (*this).columns.max();
    if (col_max != sheet.maxCol()) {
        return 1;
    }

    row_max = (*this).rowsMax;
    if (row_max != sheet.maxRow()) {
        return 1;
    }

    for (col = 0; col < col_max; col++) {
        if ((*this).columns[col] != sheet.col(col)) {
            return 1;
        }
    }

    for (row = 0; row < row_max; row++) {
        for (col = 0; col < col_max; col++) {
            String colname = (*this).columns[col];
            if ((*(*this).pRows[row])[colname] != sheet.getValue(colname, row)) {
                return 1;
            }
            colname.clear();
        }
    }

    return 0;
}


/*! Set column list (overwrite).
    @param list column list to set
    @retval true  success
    @retval false failure
 */
bool Sheet::setCols(const List &list) {
    (*this).columns.set(list);
    return true;
}


/*! Get column name.
    @param col  column index
    @return column name
 */
String &Sheet::col(long col) {
    return (*this).columns[col];
}


/*! Get column index by column name.
    @param colname  column name
    @return column index (-1 if not found)
 */
long Sheet::colIndex(const String &colname) {
    long i;
    long col_index;
    long col_max;

    col_index = -1;
    col_max = (*this).columns.max();
    for (i = 0; i < col_max; i++) {
        if ((*this).columns[i] == colname) {
            col_index = i;
            break;
        }
    }

    return col_index;
}


/*! Add column.
    @param colname column name to add
    @retval true  success
    @retval false failure
 */
bool Sheet::addCol(const String &colname) {
    return (*this).columns.add(colname);
}


/*! Delete column.
    @param colname column name to delete
    @retval true  success
    @retval false failure
 */
bool Sheet::delCol(const String &colname) {
    long col_index = (*this).colIndex(colname);
    if (col_index < 0) {
        // already removed
        return true;
    }

    return (*this).columns.remove(col_index);
}


/*! Rename column.
    @param old_colname target column name to rename
    @param new_colname new column name
    @retval true  success
    @retval false failure
 */
bool Sheet::renameCol(const String &old_colname, const String &new_colname) {
    long col_index = (*this).colIndex(old_colname);
    if (col_index < 0) {
        // specified column name is invalid
        return false;
    }

    (*this).columns[col_index] = new_colname;
    return true;
}


/*! Add row.
    @param void
    @retval true  success
    @retval false failure
 */
bool Sheet::addRow() {
    long i;

    if ((*this).rowsMax == 0 && (*this).rowsCapacity == 0) {
        (*this).pRows = new Keys * [128];
        (*this).pRows[0] = new Keys;
        (*this).rowsMax = 1;
        (*this).rowsCapacity = 128;
    } else {
        if ((*this).rowsCapacity <= (*this).rowsMax + 1) {
            Keys **p = new Keys * [(*this).rowsMax + 128];
            (*this).rowsCapacity = (*this).rowsMax + 128;
            Keys **pp = (*this).pRows;
            for (i = 0; i < (*this).rowsMax; i++) {
               p[i] = (*this).pRows[i];
               pp[i] = (Keys *)NULL;
            }
            (*this).pRows = p;
            delete [] pp;
            (*this).pRows[(*this).rowsMax] = new Keys;
            (*this).rowsMax++;
	    } else {
                (*this).pRows[(*this).rowsMax] = new Keys;
                (*this).rowsMax++;
    	}
    }

    return true;
}


/*! Insert row.
    @param row row index
    @retval true  success
    @retval false failure
 */
bool Sheet::insertRow(long row) {
    long i;

    if (row < 0 || (*this).rowsMax <= row) {
        return false;
    }

    if ((*this).rowsMax == 0 && (*this).rowsCapacity == 0) {
        (*this).pRows = new Keys * [128];
        (*this).pRows[0] = new Keys;
        (*this).rowsMax = 1;
        (*this).rowsCapacity = 128;
    } else {
        if ((*this).rowsCapacity <= (*this).rowsMax + 1) {
            // expand capacity if insufficient
            Keys **p = new Keys * [(*this).rowsMax + 128];
            (*this).rowsCapacity = (*this).rowsMax + 128;

            // copy data before insertion point
            for (i = 0; i < row; i++) {
                p[i] = (*this).pRows[i];
            }

            // insert new row
            p[row] = new Keys;

            // move data after insertion point
            for (i = row; i < (*this).rowsMax; i++) {
                p[i + 1] = (*this).pRows[i];
            }

            // delete old array and replace with new array
            delete [] (*this).pRows;
            (*this).pRows = p;
        } else {
            // if capacity is sufficient
            // shift data from back to front to make space for insertion
            for (i = (*this).rowsMax; i > row; i--) {
                (*this).pRows[i] = (*this).pRows[i - 1];
            }
            // insert new row
            (*this).pRows[row] = new Keys;
        }
        (*this).rowsMax++;
    }

    return true;
}


/*! Delete row.
    @param row row index
    @retval true  success
    @retval false failure
 */
bool Sheet::delRow(long row) {
    long i;

    if (row < 0 || (*this).rowsMax <= row) {
        return false;
    }

    if ((*this).rowsMax == 1) {
        if ((*this).pRows != (Keys **)NULL) {
            for (i = 0; i < (*this).rowsMax; i++) {
                delete (*this).pRows[i];
            }
            delete [] (*this).pRows;
        }
        (*this).pRows = (Keys **)NULL;
        (*this).rowsMax = 0;
        (*this).rowsCapacity = 0;
    } else {
        delete (*this).pRows[row];
        for (i = row; i < (*this).rowsMax - 1; i++) {
            (*this).pRows[i] = (*this).pRows[i + 1];
        }
        (*this).pRows[(*this).rowsMax - 1] = (Keys *)NULL;
        (*this).rowsMax--;
    }

    return true;
}


/*! Get value of cell.
    @param colname column name
    @param row row index
    @return string value of cell
 */
String& Sheet::getValue(const String &colname, long row) {
    if (row < 0 || (*this).rowsMax <= row) {
        if ((*this).tmpString == (String *)NULL) {
            (*this).tmpString = new String;
        }
        *((*this).tmpString) = "";
        return *((*this).tmpString);
    }

    return (*(*this).pRows[row])[colname];
}


/*! Get value of cell.
    @param col column index
    @param row row index
    @return string value of cell
 */
String& Sheet::getValue(long col, long row) {
    if (col < 0 || (*this).columns.max() <= col ||
            row < 0 || (*this).rowsMax <= row) {
        if ((*this).tmpString == (String *)NULL) {
            (*this).tmpString = new String;
        }
        *((*this).tmpString) = "";
        return *((*this).tmpString);
    }

    return (*(*this).pRows[row])[(*this).columns[col]];
}


/*! Set value into cell.
    @param colname column name
    @param row row index
    @param value string value to set
    @retval true  success
    @retval false failure
 */
bool Sheet::setValue(const String &colname, long row, const String &value) {
    if (row < 0 || (*this).rowsMax <= row) {
        return false;
    }

    (*(*this).pRows[row])[colname] = value;

    return true;
}


/*! Set value into cell.
    @param col column index
    @param row row index
    @param value string value to set
    @retval true  success
    @retval false failure
 */
bool Sheet::setValue(long col, long row, const String &value) {
    if (col < 0 || (*this).columns.max() <= col ||
            row < 0 || (*this).rowsMax <= row) {
        return false;
    }

    (*(*this).pRows[row])[(*this).columns[col]] = value;

    return true;
}


/*! Search row in a column by specified value.
    @param colname column name
    @param value search target value
    @return row index (-1 if not found)
 */
long Sheet::searchRow(const String &colname, const String &value) {
    long i;

    for (i = 0; i < (*this).rowsMax; i++) {
        if ((*(*this).pRows[i])[colname] == value) {
            return i;
        }
    }

    return -1;
}


/*! Sort rows (ascending order).
    @param colname column name
    @retval true  success
    @retval false failure
 */
bool Sheet::sort(const String &colname) {
    long col_index = (*this).colIndex(colname);
    if (col_index < 0) {
        // specified column name is invalid
        return false;
    }

    if (0 < (*this).rowsMax) {
        qsort_with_key((*this).pRows, (*this).rowsMax, sizeof(Keys *), _sheet_cmp_asc, colname.c_str());
    }
    return true;
}


/*! Sort rows (descending order).
    @param colname column name
    @retval true  success
    @retval false failure
 */
bool Sheet::sortr(const String &colname) {
    long col_index = (*this).colIndex(colname);
    if (col_index < 0) {
        // specified column name is invalid
        return false;
    }

    if (0 < (*this).rowsMax) {
        qsort_with_key((*this).pRows, (*this).rowsMax, sizeof(Keys *), _sheet_cmp_desc, colname.c_str());
    }
    return true;
}


/*! Sort rows as number (ascending order).
    @param colname column name
    @retval true  success
    @retval false failure
 */
bool Sheet::sortnum(const String &colname) {
    long col_index = (*this).colIndex(colname);
    if (col_index < 0) {
        // specified column name is invalid
        return false;
    }

    if (0 < (*this).rowsMax) {
        qsort_with_key((*this).pRows, (*this).rowsMax, sizeof(Keys *), _sheet_cmp_num, colname.c_str());
    }
    return true;
}


/*! Sort rows as number (descending order).
    @param colname column name
    @retval true  success
    @retval false failure
 */
bool Sheet::sortnumr(const String &colname) {
    long col_index = (*this).colIndex(colname);
    if (col_index < 0) {
        // specified column name is invalid
        return false;
    }

    if (0 < (*this).rowsMax) {
        qsort_with_key((*this).pRows, (*this).rowsMax, sizeof(Keys *), _sheet_cmp_numr, colname.c_str());
    }
    return true;
}


/*! Convert sheet to CSV.
    @param void
    @return CSV string
 */
String& Sheet::getCSV(const char *src_charset, const char *dest_charset, const char *return_str) {
    long i, j;
    long col_max;
    String str;
    String trimstr;

    if ((*this).tmpCSVString == (String *)NULL) {
        (*this).tmpCSVString = new String;
    }
    *((*this).tmpCSVString) = "";

    col_max = (*this).columns.max();
    if (col_max <= 0) {
        return *((*this).tmpCSVString);
    }

    for (i = 0; i < col_max; i++) {
        if (0 < i) {
            *((*this).tmpCSVString) += ",";
        }
        str = (*this).columns[i];
        trimstr = str.trim();
        if (str == trimstr && str[0] != '+' && str[0] != '-' &&
                (str[0] != '0' || (str[0] == '0' && !isdigit(str[1]))) &&
                str.searchChar(',') <= 0 && str.searchChar('"') <= 0 &&
                str.searchChar('\r') <= 0 && str.searchChar('\n') <= 0) {
            *((*this).tmpCSVString) += (*this).columns[i].strconv(src_charset, dest_charset).changeReturnCode(return_str);
        } else {
            *((*this).tmpCSVString) += "\"";
            *((*this).tmpCSVString) += (*this).columns[i].escapeCSV(src_charset, dest_charset, return_str);
            *((*this).tmpCSVString) += "\"";
        }
    }
    *((*this).tmpCSVString) += return_str;

    for (i = 0; i < (*this).rowsMax; i++) {
        for (j = 0; j < col_max; j++) {
            if (0 < j) {
                *((*this).tmpCSVString) += ",";
            }
            str = (*(*this).pRows[i])[(*this).columns[j]];
            trimstr = str.trim();
            if (str == trimstr && str[0] != '+' && str[0] != '-' &&
                    (str[0] != '0' || (str[0] == '0' && !isdigit(str[1]))) &&
                    str.searchChar(',') <= 0 && str.searchChar('"') <= 0 &&
                    str.searchChar('\r') <= 0 && str.searchChar('\n') <= 0) {
                *((*this).tmpCSVString) += str.strconv(src_charset, dest_charset).changeReturnCode(return_str);
            } else {
                *((*this).tmpCSVString) += "\"";
                *((*this).tmpCSVString) += str.escapeCSV(src_charset, dest_charset, return_str);
                *((*this).tmpCSVString) += "\"";
            }
        }
        *((*this).tmpCSVString) += return_str;
    }

    str.clear();
    trimstr.clear();

    return *((*this).tmpCSVString);
}


/*! Set CSV data to sheet.
    @param CSV string
    @retval true  success
    @retval false failure
 */
bool Sheet::setCSV(const String &csv, const char *src_charset, const char *dest_charset) {
    long pos1, pos2, length1, length2, col_max, col, row;
    String csv_line;
    String cell_data;

    (*this).clear();

    if (src_charset == NULL || src_charset[0] == '\0' || !strcasecmp(src_charset, "AUTODETECT")) {
        src_charset = csv.detectCharSet();
    } else if (!strcasecmp(src_charset, "AUTODETECT_JP")) {
        src_charset = csv.detectCharSetJP();
    }

    // Set column names
    pos1 = 0;
    length1 = _searchCSVLineEnd(csv, pos1);
    if (length1 <= 0) {
        return true;
    }
    csv_line = csv.mid(pos1, length1);
    pos2 = 0;
    col_max = 0;
    while (1) {
        length2 = _searchCSVColEnd(csv_line, pos2, cell_data, src_charset, dest_charset, "\n");
        if (cell_data == "" && length2 <= 0) {
            break;
        }
        (*this).addCol(cell_data);
        pos2 += length2;
        col_max++;
    }
    pos1 += length1;

    row = 0;
    while (0 < (length1 = _searchCSVLineEnd(csv, pos1))) {
        (*this).addRow();
        csv_line = csv.mid(pos1, length1);
        pos2 = 0;
        for (col = 0; col < col_max; col++) {
            length2 = _searchCSVColEnd(csv_line, pos2, cell_data, src_charset, dest_charset, "\n");
            (*(*this).pRows[row])[(*this).columns[col]] = cell_data;
            if (cell_data == "" && length2 <= 0) {
                break;
            }
            pos2 += length2;
        }
        row++;
        pos1 += length1;
    }

    csv_line.clear();
    cell_data.clear();

    return true;
}


/*! Load CSV file into sheet.
    @param filename CSV file name
    @retval true  success
    @retval false failure
 */
bool Sheet::loadCSV(const String &filename, const char *src_charset, const char *dest_charset) {
    String csv;
    csv.loadFile(filename);
    (*this).setCSV(csv, src_charset, dest_charset);
    csv.clear();
    return true;
}


/*! Save sheet data to CSV file.
    @param filename CSV file name to save
    @retval true  success
    @retval false failure
 */
bool Sheet::saveCSV(const String &filename, const char *src_charset, const char *dest_charset, const char *return_str) {
    String csv;
    csv = (*this).getCSV(src_charset, dest_charset);
    csv.saveFile(filename);
    csv.clear();
    return true;
}

} // namespace apolloron
