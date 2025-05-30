/******************************************************************************/
/*! @file Keys.cc
    @brief Keys class
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "apolloron.h"

namespace apolloron {

// change 1 byte value to 6bit vale (0-63)
#define b64(a) (((unsigned char)(a) & 0xfc) >> 2)

/*! Constructor of Keys.
    @param void
    @return void
 */
Keys::Keys() {
    int i;

    (*this).pElement = new Element ** [64];
    for (i = 0; i < 64; i++) {
        (*this).nElementTotal[i] = 0;
        (*this).pElement[i] = (Element **)NULL;
    }

    (*this).tmpString = (String *)NULL;
    (*this).emptyString = new String;
    *((*this).emptyString) = ""; // stay empty
}


/*! Constructor of Keys.
    @param keys: Keys for initialization.
    @return void
 */
Keys::Keys(const Keys &keys) {
    int i;

    (*this).pElement = new Element ** [64];
    for (i = 0; i < 64; i++) {
        (*this).nElementTotal[i] = 0;
        (*this).pElement[i] = (Element **)NULL;
    }

    (*this).tmpString = (String *)NULL;
    (*this).emptyString = new String;
    *((*this).emptyString) = ""; // stay empty

    (*this).set(keys);
}


/*! Destructor of Keys.
    @param void
    @return void
 */
Keys::~Keys() {
    int i;
    long l;

    if ((*this).pElement != NULL) {
        for (i = 0; i < 64; i++) {
            if ((*this).pElement[i] != NULL) {
                l = 0;
                while (l < (*this).nElementTotal[i]) {
                    if ((*this).pElement[i][l] != NULL) {
                        if ((*this).pElement[i][l]->key != NULL) {
                            delete [] (*this).pElement[i][l]->key;
                        }
                        delete (*this).pElement[i][l]->value;
                        delete [] (*this).pElement[i][l];
                    }
                    l++;
                }
                delete [] (*this).pElement[i];
                (*this).nElementTotal[i] = 0;
            }
        }
        delete [] (*this).pElement;
        (*this).pElement = NULL;
    }

    if ((*this).tmpString != (String *)NULL) {
        delete (*this).tmpString;
        (*this).tmpString = (String *)NULL;
    }

    if ((*this).emptyString != (String *)NULL) {
        delete (*this).emptyString;
        (*this).emptyString = (String *)NULL;
    }
}

/*! Delete instance of Keys.
    @param void
    @retval true   success
    @retval false  failure
 */
bool Keys::clear() {
    int i;
    long l;

    if ((*this).pElement != NULL && *((*this).pElement) != NULL) {
        for (i = 0; i < 64; i++) {
            l = 0;
            while (l < (*this).nElementTotal[i]) {
                if ((*this).pElement[i][l] != NULL) {
                    if ((*this).pElement[i][l]->key != NULL) {
                        delete [] (*this).pElement[i][l]->key;
                    }
                    delete (*this).pElement[i][l]->value;
                    delete [] (*this).pElement[i][l];
                }
                l++;
            }
            delete *((*this).pElement[i]);
            (*this).nElementTotal[i] = 0;
        }
        delete [] *((*this).pElement);
        (*this).pElement = NULL;
    }

    (*this).pElement = new Element ** [64];
    for (i = 0; i < 64; i++) {
        (*this).nElementTotal[i] = 0;
        (*this).pElement[i] = (Element **)NULL;
        (*this).nElementCapacity[i] = 0;
    }

    if ((*this).tmpString != (String *)NULL) {
        delete (*this).tmpString;
        (*this).tmpString = (String *)NULL;
    }

    return true;
}


/*! Get an element that have match key.
    @param key  key of object
    @return String object
 */
String& Keys::operator [] (const String &key) {
    if ((*this).isKeyExist(key)) {
        return (*this).getValue(key.c_str());
    } else {
        (*this).addKey(key, "");
        return (*this).getValue(key.c_str());
    }
}


/*! Get an element that have match key.
    @param key  key of object
    @return String object
 */
const String& Keys::operator [] (const String &key) const {
    return (const String&)(*this).getValue(key.c_str());
}


/*! Get an element that have match key.
    @param key  key of object
    @return String object
 */
String& Keys::operator [] (const char *key) {
    if (!(*this).isKeyExist(key)) {
        (*this).addKey(key, "");
    }
    return (*this).getValue(key);
}


/*! Get an element that have match key.
    @param key  key of object
    @return String object
 */
const String& Keys::operator [] (const char *key) const {
    return (const String&)(*this).getValue(key);
}


/*! Get value of an element that have match key.
    @param key  key of object
    @return Keys object
 */
String& Keys::getValue(const String &key) const {
    return (*this).getValue(key.c_str());
}


/*! Get value of an element that have match key.
    @param key  key of object
    @return Keys object
 */
 String& Keys::getValue(const char *key) const {
    if (key == NULL) {
        if ((*this).emptyString->c_str()[0] != '\0') {
            *(*this).emptyString = "";
        }
        return *(*this).emptyString;
    }

    int start = b64(key[0]);
    if ((*this).nElementTotal[start] <= 0 || !(*this).pElement[start]) {
        if ((*this).emptyString->c_str()[0] != '\0') {
            *(*this).emptyString = "";
        }
        return *(*this).emptyString;
    }

    // Binary search to find target element
    long left = 0;
    long right = (*this).nElementTotal[start];
    
    while (left < right) {
        long mid = (left + right) / 2;
        if (!(*this).pElement[start][mid] || !(*this).pElement[start][mid]->key) {
            break;
        }
        int cmp = strcmp(key, (*this).pElement[start][mid]->key);
        if (cmp == 0) {
            return *((*this).pElement[start][mid]->value);
        }
        if (cmp < 0) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    // Key not found
    if ((*this).emptyString->c_str()[0] != '\0') {
        (*this).emptyString->operator=("");
    }
    return *(*this).emptyString;
}


/*! Get value of an element that have match key. (for multi-threaded use)
    @param key  key of object
    @return Keys object
 */
const char* Keys::read(const String &key) const {
    return (*this).read(key.c_str());
}


/*! Get value of an element that have match key. (for multi-threaded use)
    @param key  key of object
    @return Keys object
 */
 const char* Keys::read(const char *key) const {
    if (key == NULL) {
        return NULL;
    }

    int start = b64(key[0]);

    if (!(*this).pElement[start]) {
        return NULL;
    }

    long left = 0;
    long right = (*this).nElementTotal[start];

    if (right <= 0) {
        return NULL;
    }

    while (left < right) {
        long mid = (left + right) / 2;
        if (!(*this).pElement[start][mid]) {
            return NULL;
        }
        if (!(*this).pElement[start][mid]->key) {
            return NULL;
        }
        int cmp = strcmp(key, (*this).pElement[start][mid]->key);
        if (cmp == 0) {
            if ((*this).pElement[start][mid]->value == (const String *)NULL) {
                return NULL;
            }
            const char* value = (*((*this).pElement[start][mid]->value)).c_str();
            return value;
        }
        if (cmp < 0) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    return NULL;
}


/*! Check the existance of key.
    @param key  key of object
    @retval true  key exist
    @retval false key not exist
 */
bool Keys::isKeyExist(const String &key) const {
    return isKeyExist(key.c_str());
}


/*! Check the existance of key.
    @param key  key of object
    @retval true  key exist
    @retval false key not exist
 */
bool Keys::isKeyExist(const char *key) const {
    int start = b64(key[0]);
    if (!(*this).pElement[start]) return false;

    long left = 0;
    long right = (*this).nElementTotal[start];

    while (left < right) {
        long mid = (left + right) / 2;
        if (!(*this).pElement[start][mid] || !(*this).pElement[start][mid]->key) {
            return false;
        }
        int cmp = strcmp(key, (*this).pElement[start][mid]->key);
        if (cmp == 0) {
            return true;
        }
        if (cmp < 0) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    return false;
}


/*! get the Number of keys in array.
    @param void
    @return number of keys
 */
long Keys::max() const {
    int i;
    long total;

    total = 0;

    for (i = 0; i < 64; i++) {
        total += (*this).nElementTotal[i];
    }

    return total;
}


/*! Get key of array.
    @param index  index of key
    @return key string
 */
const char *Keys::key(long index) const {
    Element *element;
    element = (*this).array(index);
    return element->key;
}


/*! Setup of keys.
    @param keys  Keys object to set
    @retval true  success
    @retval false failure
 */
bool Keys::set(const Keys & keys) {
    long i;

    (*this).clear();

    i = 0;
    while (i < keys.max()) {
        addKey(keys.array(i)->key, *(keys.array(i)->value));
        i++;
    }

    return true;
}


/*! Setup of keys.
    @param keys  Keys object to set
    @return Keys object
 */
const Keys& Keys::operator = (const Keys &keys) {
    (*this).set(keys);

    return *this;
}


/*! Get value of key in array.
    @param keys  Keys object to set
    @return Keys object
 */
String& Keys::value(long index) const {
    Element *element;
    element = (*this).array(index);
    return *(element->value);
}


/*! Get element of array
    @param index index of Keys
    @return Element object
 */
Element* Keys::array(long index) const {
    Element *element;

    element = (Element *)NULL;

    if (0 <= index) {
        int i;
        int start;
        long total;

        start = -1;
        total = 0;
        for (i = 0; i < 64; i++) {
            total += (*this).nElementTotal[i];
            if (index < total) {
                start = i;
                break;
            }
        }

        if (0 <= start) {
            long internal_index = index - (total - (*this).nElementTotal[start]);
            if (0 <= internal_index && internal_index <= (*this).nElementTotal[start]) {
                element = (*this).pElement[start][internal_index];
            }
        }
    }

    return element;
}


/*! Append a key to array
    @param key   key of array
    @param value value to append
    @retval true  success
    @retval false failure
 */
bool Keys::addKey(const String &key, const String &value) {
    return addKey(key.c_str(), value.c_str(), key.len());
}

/*! Append a key to array
    @param key   key of array
    @param value value to append
    @retval true  success
    @retval false failure
 */
bool Keys::addKey(const String &key, const char *value) {
    if (value == NULL) {
        return false;
    }
    return addKey(key.c_str(), value, key.len());
}

/*! Append a key to array
    @param key   key of array
    @param value value to append
    @retval true  success
    @retval false failure
 */
bool Keys::addKey(const char *key, const String &value) {
    if (key == NULL) {
        return false;
    }
    return addKey(key, value.c_str(), strlen(key));
}

/*! Append a key to array
    @param key   key of array
    @param value value to append
    @retval true  success
    @retval false failure
 */
bool Keys::addKey(const char *key, const char *value) {
    if (key == NULL || value == NULL) {
        return false;
    }
    return addKey(key, value, strlen(key));
}


/*! Append a key to array
    @param key   key of array
    @param value value to append
    @param keyLen length of key
    @retval true  success
    @retval false failure
 */
 bool Keys::addKey(const char *key, const char *value, long keyLen) {
    if (key == NULL || value == NULL) return false;

    int start = b64(key[0]);

    // Create new array if needed
    if ((*this).nElementTotal[start] == 0) {
        // Start with capacity of 128 elements
        (*this).pElement[start] = new Element*[128];
        (*this).nElementCapacity[start] = 128;
        (*this).pElement[start][0] = new Element;
        (*this).pElement[start][0]->key = new char[keyLen + 1];
        strncpy((*this).pElement[start][0]->key, key, keyLen);
        (*this).pElement[start][0]->key[keyLen] = '\0';
        (*this).pElement[start][0]->value = new String;
        *((*this).pElement[start][0]->value) = value;
        (*this).nElementTotal[start] = 1;
        return true;
    }

    if (!(*this).pElement[start]) return false;

    // Find insertion point using binary search on sorted array
    long left = 0;
    long right = (*this).nElementTotal[start];
    long insert_pos = 0;

    while (left < right) {
        long mid = (left + right) / 2;
        if (!(*this).pElement[start][mid] || !(*this).pElement[start][mid]->key) {
            break;
        }
        int cmp = strcmp(key, (*this).pElement[start][mid]->key);
        if (cmp == 0) {
            // Overwrite existing value
            *((*this).pElement[start][mid]->value) = value;
            return true;
        }
        if (cmp < 0) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    insert_pos = left;

    // create new array with increased capacity if needed
    if ((*this).nElementCapacity[start] <= (*this).nElementTotal[start]) {
        // increase capacity by 128 elements
        long new_capacity = (*this).nElementCapacity[start] + 128;
        Element** new_array = new Element*[new_capacity];

        // initialize all elements to NULL
        //for (long i = 0; i < new_capacity; i++) {
        //    new_array[i] = NULL;
        //}

        // copy existing elements
        //for (long i = 0; i < (*this).nElementTotal[start]; i++) {
        //    new_array[i] = (*this).pElement[start][i];
        //}
        memcpy(new_array, (*this).pElement[start], sizeof(Element*) * (*this).nElementTotal[start]);

        // store old array temporarily
        Element** old_array = (*this).pElement[start];
        (*this).pElement[start] = new_array;
        (*this).nElementCapacity[start] = new_capacity;

        // delete old array
        if (old_array != NULL) {
            delete[] old_array;
        }
    }

    // shift elements to make space for new element using memmove
    //for (long i = (*this).nElementTotal[start]; i > insert_pos; i--) {
    //    (*this).pElement[start][i] = (*this).pElement[start][i - 1];
    //}
    if (insert_pos < (*this).nElementTotal[start]) {
        memmove(&(*this).pElement[start][insert_pos + 1],
                &(*this).pElement[start][insert_pos],
                sizeof(Element*) * ((*this).nElementTotal[start] - insert_pos));
    }

    // insert new element
    (*this).pElement[start][insert_pos] = new Element;
    (*this).pElement[start][insert_pos]->key = new char[keyLen + 1];
    strncpy((*this).pElement[start][insert_pos]->key, key, keyLen);
    (*this).pElement[start][insert_pos]->key[keyLen] = '\0';
    (*this).pElement[start][insert_pos]->value = new String;
    *((*this).pElement[start][insert_pos]->value) = value;
    (*this).nElementTotal[start]++;
    return true;
}


/* Remove a key from array
    @retval true  success
    @retval false failure
 */
bool Keys::delKey(const String &key) {
    return delKey(key.c_str());
}


/* Remove a key from array
    @retval true  success
    @retval false failure
 */
 bool Keys::delKey(const char *key) {
    if (key == NULL) return false;

    int start = b64(key[0]);

    if (!(*this).pElement[start]) return false;

    // Find deletion position using binary search
    long left = 0;
    long right = (*this).nElementTotal[start];
    long deletePos = -1;

    while (left < right) {
        long mid = (left + right) / 2;
        if (!(*this).pElement[start][mid] || !(*this).pElement[start][mid]->key) {
            break;
        }
        int cmp = strcmp(key, (*this).pElement[start][mid]->key);
        if (cmp == 0) {
            deletePos = mid;
            break;
        }
        if (cmp < 0) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }

    if (deletePos == -1) return false;

    // delete element
    if ((*this).pElement[start][deletePos]->key != NULL) {
        delete [] (*this).pElement[start][deletePos]->key;
    }
    delete (*this).pElement[start][deletePos]->value;
    delete [] (*this).pElement[start][deletePos];

    // Shift elements to fill gap using memcpy
    //for (long i = deletePos; i < (*this).nElementTotal[start] - 1; i++) {
    //    (*this).pElement[start][i] = (*this).pElement[start][i + 1];
    //}
    if (deletePos < (*this).nElementTotal[start] - 1) {
        memcpy(&(*this).pElement[start][deletePos],
               &(*this).pElement[start][deletePos + 1],
               sizeof(Element*) * ((*this).nElementTotal[start] - deletePos - 1));
    }

    (*this).nElementTotal[start]--;
    return true;
}


/*! Convert Keys data to serialized string.
    @param void
    @return Serialized string
 */
String &Keys::toString() {
    long m, i;
    String tmp_str1, tmp_str2;

    if ((*this).tmpString == (String *)NULL) {
        (*this).tmpString = new String;
    }
    *((*this).tmpString) = "";

    m = (*this).max();
    (*((*this).tmpString)).sprintf("# %ld key(s)\n", m);
    for (i = 0; i < m; i++) {
        *((*this).tmpString) += (*this).key(i);
        tmp_str1 = (*this).value(i).trim();
        tmp_str2 = (*this).value(i).escapeQuote();
        if (tmp_str1 == tmp_str2) {
            *((*this).tmpString) += "=";
            *((*this).tmpString) += tmp_str1;
            *((*this).tmpString) += "\n";
        } else {
            *((*this).tmpString) += "=\"";
            *((*this).tmpString) += tmp_str2;
            *((*this).tmpString) += "\"\n";
        }
    }

    tmp_str1.clear();
    tmp_str2.clear();

    return *((*this).tmpString);
}


/*! Set Keys structure by serialized string.
    @param str    String to set.
    @retval true  success
    @retval false failure
 */
bool Keys::setString(const String &str) {
    String line, key, value, tmp_str;
    long len, i, j, k;

    (*this).clear();

    len = str.len();
    if (len < 0L) {
        return false;
    }

    for (i = 0L; i < len; i++) {
        for (j = 0L; i + j < len; j++) {
            if (str[i + j] == '\n') {
                break;
            }
        }
        if (0L < j) {
            line = str.mid(i, j).trim();
            if (0L < line.len() && line[0] != '#' && 0 < (k = line.searchChar('='))) {
                String p, p1;
                p = line;
                p1 = p.left(k).trim();
                key = p1;
                p1 = p.mid(k+1).trim();
                value = p1;
                if (2L <= value.len() && value[0] == '"' && value[value.len()-1] == '"') {
                    tmp_str = value.mid(1, value.len()-2);
                    value = tmp_str.unescapeQuote();
                    tmp_str.clear();
                }
                (*this).addKey(key, value);
            }
            line.clear();
            i += j;
        }
    }

    return true;
}


/*! Read structured text file into a Keys.
    @param filename  File name to read.
    @retval true  success
    @retval false failure
 */
bool Keys::loadFile(const String &filename) {
    return (*this).loadFile((const char *)(filename.c_str()));
}


/*! Read structured text file into a Keys.
    @param filename  File name to read.
    @retval true  success
    @retval false failure
 */
bool Keys::loadFile(const char *filename) {
    String str, line, key, value, tmp_str;
    long len, i, j, k;

    (*this).clear();

    len = str.loadFile(filename);
    if (len < 0L) {
        return false;
    }

    for (i = 0L; i < len; i++) {
        for (j = 0L; i + j < len; j++) {
            if (str[i + j] == '\n') {
                break;
            }
        }
        if (0L < j) {
            line = str.mid(i, j).trim();
            if (0L < line.len() && line[0] != '#' && 0 < (k = line.searchChar('='))) {
                String p, p1;
                p = line;
                p1 = p.left(k).trim();
                key = p1;
                p1 = p.mid(k+1).trim();
                value = p1;
                if (2L <= value.len() && value[0] == '"' && value[value.len()-1] == '"') {
                    tmp_str = value.mid(1, value.len()-2);
                    value = tmp_str.unescapeQuote();
                    tmp_str.clear();
                }
                (*this).addKey(key, value);
            }
            line.clear();
            i += j;
        }
    }

    str.clear();

    return true;
}


/*! Write structured text file of serialized Keys.
    @param filename  File name to wtite.
    @retval true  success
    @retval false failure
 */
bool Keys::saveFile(const String &filename) const {
    return (*this).saveFile((const char *)(filename.c_str()));
}


/*! Write structured text file of serialized Keys.
    @param filename  File name to wtite.
    @retval true  success
    @retval false failure
 */
bool Keys::saveFile(const char *filename) const {
    long m, i;
    String str, tmp_str1, tmp_str2;
    bool ret;

    m = (*this).max();
    str.sprintf("# %ld key(s)\n", m);
    for (i = 0; i < m; i++) {
        str += (*this).key(i);
        tmp_str1 = (*this).value(i).trim();
        tmp_str2 = (*this).value(i).escapeQuote();
        if (tmp_str1 == tmp_str2) {
            str += "=";
            str += tmp_str1;
            str += "\n";
        } else {
            str += "=\"";
            str += tmp_str2;
            str += "\"\n";
        }
    }

    ret = str.saveFile(filename);

    tmp_str1.clear();
    tmp_str2.clear();
    str.clear();

    return ret;
}

} // namespace apolloron
