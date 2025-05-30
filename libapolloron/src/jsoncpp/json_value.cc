#include <iostream>
#include <utility>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include "apolloron.h"
#ifdef JSON_USE_CPPTL
# include <cpptl/conststring.h>
#endif
#include <cstddef>    // size_t
#ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
# include "json_batchallocator.h"
#endif // #ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR

#define JSON_ASSERT_UNREACHABLE assert( false )
#define JSON_ASSERT( condition ) assert( condition );  // @todo <= change this into an exception throw
#define JSON_ASSERT_MESSAGE( condition, message ) if (!( condition )) throw std::runtime_error( message );

namespace apolloron {

const JSONValue JSONValue::null;
const Int JSONValue::minInt = Int(~(UInt(-1) / 2));
const Int JSONValue::maxInt = Int(UInt(-1) / 2);
const UInt JSONValue::maxUInt = UInt(-1);

// A "safe" implementation of strdup. Allow null pointer to be passed.
// Also avoid warning on msvc80.
//
//inline char *safeStringDup( const char *czstring )
//{
//   if ( czstring )
//   {
//      const size_t length = (unsigned int)( strlen(czstring) + 1 );
//      char *newString = static_cast<char *>( malloc( length ) );
//      memcpy( newString, czstring, length );
//      return newString;
//   }
//   return 0;
//}
//
//inline char *safeStringDup( const std::string &str )
//{
//   if ( !str.empty() )
//   {
//      const size_t length = str.length();
//      char *newString = static_cast<char *>( malloc( length + 1 ) );
//      memcpy( newString, str.c_str(), length );
//      newString[length] = 0;
//      return newString;
//   }
//   return 0;
//}

JSONValueAllocator::~JSONValueAllocator() {
}

class DefaultValueAllocator : public JSONValueAllocator {
public:
    virtual ~DefaultValueAllocator() {
    }

    virtual char *makeMemberName(const char *memberName) {
        return duplicateStringValue(memberName);
    }

    virtual void releaseMemberName(char *memberName) {
        releaseStringValue(memberName);
    }

    virtual char *duplicateStringValue(const char *value,
                                       unsigned int length = unknown) {
        //@todo invesgate this old optimization
        //if ( !value  ||  value[0] == 0 )
        //   return 0;

        if(length == unknown)
            length = (unsigned int)strlen(value);
        char *newString = static_cast<char *>(malloc(length + 1));
        memcpy(newString, value, length);
        newString[length] = 0;
        return newString;
    }

    virtual void releaseStringValue(char *value) {
        if(value)
            free(value);
    }
};

static JSONValueAllocator *&valueAllocator() {
    static DefaultValueAllocator defaultAllocator;
    static JSONValueAllocator *valueAllocator = &defaultAllocator;
    return valueAllocator;
}

static struct DummyValueAllocatorInitializer {
    DummyValueAllocatorInitializer() {
        valueAllocator();      // ensure valueAllocator() statics are initialized before main().
    }
} dummyValueAllocatorInitializer;



// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// JSONValueInternals...
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
#ifdef JSON_VALUE_USE_INTERNAL_MAP
# include "json_internalarray.inl"
# include "json_internalmap.inl"
#endif // JSON_VALUE_USE_INTERNAL_MAP

# include "json_valueiterator.inl"


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class JSONValue::CommentInfo
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////


JSONValue::CommentInfo::CommentInfo()
    : comment_(0) {
}

JSONValue::CommentInfo::~CommentInfo() {
    if(comment_)
        valueAllocator()->releaseStringValue(comment_);
}


void JSONValue::CommentInfo::setComment(const char *text) {
    if(comment_)
        valueAllocator()->releaseStringValue(comment_);
    JSON_ASSERT(text);
    JSON_ASSERT_MESSAGE(text[0] == '\0' || text[0] == '/', "Comments must start with /");
    // It seems that /**/ style comments are acceptable as well.
    comment_ = valueAllocator()->duplicateStringValue(text);
}


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class JSONValue::CZString
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
# ifndef JSON_VALUE_USE_INTERNAL_MAP

// Notes: index_ indicates if the string was allocated when
// a string is stored.

JSONValue::CZString::CZString(int index)
    : cstr_(0)
    , index_(index) {
}

JSONValue::CZString::CZString(const char *cstr, DuplicationPolicy allocate)
    : cstr_(allocate == duplicate ? valueAllocator()->makeMemberName(cstr)
            : cstr)
    , index_(allocate) {
}

JSONValue::CZString::CZString(const CZString &other)
    : cstr_(other.index_ != noDuplication &&  other.cstr_ != 0
            ?  valueAllocator()->makeMemberName(other.cstr_)
            : other.cstr_)
    , index_(other.cstr_ ? (other.index_ == noDuplication ? noDuplication : duplicate)
             : other.index_) {
}

JSONValue::CZString::~CZString() {
    if(cstr_  &&  index_ == duplicate)
        valueAllocator()->releaseMemberName(const_cast<char *>(cstr_));
}

void JSONValue::CZString::swap(CZString &other) {
    std::swap(cstr_, other.cstr_);
    std::swap(index_, other.index_);
}

JSONValue::CZString & JSONValue::CZString::operator =(const CZString &other) {
    CZString temp(other);
    swap(temp);
    return *this;
}

bool JSONValue::CZString::operator<(const CZString &other) const {
    if(cstr_)
        return strcmp(cstr_, other.cstr_) < 0;
    return index_ < other.index_;
}

bool JSONValue::CZString::operator==(const CZString &other) const {
    if(cstr_)
        return strcmp(cstr_, other.cstr_) == 0;
    return index_ == other.index_;
}


int JSONValue::CZString::index() const {
    return index_;
}


const char * JSONValue::CZString::c_str() const {
    return cstr_;
}

bool JSONValue::CZString::isStaticString() const {
    return index_ == noDuplication;
}

#endif // ifndef JSON_VALUE_USE_INTERNAL_MAP


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class JSONValue::JSONValue
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/*! \internal Default constructor initialization must be equivalent to:
 * memset( this, 0, sizeof(JSONValue) )
 * This optimization is used in JSONValueInternalMap fast allocator.
 */
JSONValue::JSONValue(JSONValueType type)
    : type_(type)
    , allocated_(0)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    switch(type) {
        case nullValue:
            break;
        case intValue:
        case uintValue:
            value_.int_ = 0;
            break;
        case realValue:
            value_.real_ = 0.0;
            break;
        case stringValue:
            value_.string_ = 0;
            break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            value_.map_ = new ObjectValues();
            break;
#else
        case arrayValue:
            value_.array_ = arrayAllocator()->newArray();
            break;
        case objectValue:
            value_.map_ = mapAllocator()->newMap();
            break;
#endif
        case booleanValue:
            value_.bool_ = false;
            break;
        default:
            JSON_ASSERT_UNREACHABLE;
    }
}


JSONValue::JSONValue(Int value)
    : type_(intValue)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.int_ = value;
}


JSONValue::JSONValue(UInt value)
    : type_(uintValue)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.uint_ = value;
}

JSONValue::JSONValue(double value)
    : type_(realValue)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.real_ = value;
}

JSONValue::JSONValue(const char *value)
    : type_(stringValue)
    , allocated_(true)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.string_ = valueAllocator()->duplicateStringValue(value);
}


JSONValue::JSONValue(const char *beginValue,
                     const char *endValue)
    : type_(stringValue)
    , allocated_(true)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.string_ = valueAllocator()->duplicateStringValue(beginValue,
                     UInt(endValue - beginValue));
}


JSONValue::JSONValue(const String &value)
    : type_(stringValue)
    , allocated_(true)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.string_ = valueAllocator()->duplicateStringValue(value.c_str(),
                     (unsigned int)value.len());

}

JSONValue::JSONValue(const std::string &value)
    : type_(stringValue)
    , allocated_(true)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.string_ = valueAllocator()->duplicateStringValue(value.c_str(),
                     (unsigned int)value.length());

}

JSONValue::JSONValue(const StaticString &value)
    : type_(stringValue)
    , allocated_(false)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.string_ = const_cast<char *>(value.c_str());
}


# ifdef JSON_USE_CPPTL
JSONValue::JSONValue(const CppTL::ConstString &value)
    : type_(stringValue)
    , allocated_(true)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.string_ = valueAllocator()->duplicateStringValue(value, value.length());
}
# endif

JSONValue::JSONValue(bool value)
    : type_(booleanValue)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    value_.bool_ = value;
}


JSONValue::JSONValue(const JSONValue &other)
    : type_(other.type_)
    , comments_(0)
# ifdef JSON_VALUE_USE_INTERNAL_MAP
    , itemIsUsed_(0)
#endif
{
    switch(type_) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
            value_ = other.value_;
            break;
        case stringValue:
            if(other.value_.string_) {
                value_.string_ = valueAllocator()->duplicateStringValue(other.value_.string_);
                allocated_ = true;
            } else
                value_.string_ = 0;
            break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            value_.map_ = new ObjectValues(*other.value_.map_);
            break;
#else
        case arrayValue:
            value_.array_ = arrayAllocator()->newArrayCopy(*other.value_.array_);
            break;
        case objectValue:
            value_.map_ = mapAllocator()->newMapCopy(*other.value_.map_);
            break;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    if(other.comments_) {
        comments_ = new CommentInfo[numberOfCommentPlacement];
        for(int comment = 0; comment < numberOfCommentPlacement; ++comment) {
            const CommentInfo &otherComment = other.comments_[comment];
            if(otherComment.comment_)
                comments_[comment].setComment(otherComment.comment_);
        }
    }
}


JSONValue::~JSONValue() {
    switch(type_) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
            break;
        case stringValue:
            if(allocated_)
                valueAllocator()->releaseStringValue(value_.string_);
            break;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            delete value_.map_;
            break;
#else
        case arrayValue:
            arrayAllocator()->destructArray(value_.array_);
            break;
        case objectValue:
            mapAllocator()->destructMap(value_.map_);
            break;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }

    if(comments_)
        delete[] comments_;
}

JSONValue & JSONValue::operator=(const JSONValue &other) {
    JSONValue temp(other);
    swap(temp);
    return *this;
}

void JSONValue::swap(JSONValue &other) {
    JSONValueType temp = type_;
    type_ = other.type_;
    other.type_ = temp;
    std::swap(value_, other.value_);
    int temp2 = allocated_;
    allocated_ = other.allocated_;
    other.allocated_ = temp2;
}

JSONValueType JSONValue::type() const {
    return type_;
}


int JSONValue::compare(const JSONValue &other) {
    /*
    int typeDelta = other.type_ - type_;
    switch ( type_ )
    {
    case nullValue:

       return other.type_ == type_;
    case intValue:
       if ( other.type_.isNumeric()
    case uintValue:
    case realValue:
    case booleanValue:
       break;
    case stringValue,
       break;
    case arrayValue:
       delete value_.array_;
       break;
    case objectValue:
       delete value_.map_;
    default:
       JSON_ASSERT_UNREACHABLE;
    }
    */
    return 0;  // unreachable
}

bool JSONValue::operator <(const JSONValue &other) const {
    int typeDelta = type_ - other.type_;
    if(typeDelta)
        return typeDelta < 0 ? true : false;
    switch(type_) {
        case nullValue:
            return false;
        case intValue:
            return value_.int_ < other.value_.int_;
        case uintValue:
            return value_.uint_ < other.value_.uint_;
        case realValue:
            return value_.real_ < other.value_.real_;
        case booleanValue:
            return value_.bool_ < other.value_.bool_;
        case stringValue:
            return (value_.string_ == 0  &&  other.value_.string_)
                   || (other.value_.string_
                       &&  value_.string_
                       && strcmp(value_.string_, other.value_.string_) < 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue: {
            int delta = int(value_.map_->size() - other.value_.map_->size());
            if(delta)
                return delta < 0;
            return (*value_.map_) < (*other.value_.map_);
        }
#else
        case arrayValue:
            return value_.array_->compare(*(other.value_.array_)) < 0;
        case objectValue:
            return value_.map_->compare(*(other.value_.map_)) < 0;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return 0;  // unreachable
}

bool JSONValue::operator <=(const JSONValue &other) const {
    return !(other > *this);
}

bool JSONValue::operator >=(const JSONValue &other) const {
    return !(*this < other);
}

bool JSONValue::operator >(const JSONValue &other) const {
    return other < *this;
}

bool JSONValue::operator ==(const JSONValue &other) const {
    //if ( type_ != other.type_ )
    // GCC 2.95.3 says:
    // attempt to take address of bit-field structure member `apolloron::JSONValue::type_'
    // Beats me, but a temp solves the problem.
    int temp = other.type_;
    if(type_ != temp)
        return false;
    switch(type_) {
        case nullValue:
            return true;
        case intValue:
            return value_.int_ == other.value_.int_;
        case uintValue:
            return value_.uint_ == other.value_.uint_;
        case realValue:
            return value_.real_ == other.value_.real_;
        case booleanValue:
            return value_.bool_ == other.value_.bool_;
        case stringValue:
            return (value_.string_ == other.value_.string_)
                   || (other.value_.string_
                       &&  value_.string_
                       && strcmp(value_.string_, other.value_.string_) == 0);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            return value_.map_->size() == other.value_.map_->size()
                   && (*value_.map_) == (*other.value_.map_);
#else
        case arrayValue:
            return value_.array_->compare(*(other.value_.array_)) == 0;
        case objectValue:
            return value_.map_->compare(*(other.value_.map_)) == 0;
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return 0;  // unreachable
}

bool JSONValue::operator !=(const JSONValue &other) const {
    return !(*this == other);
}

const char * JSONValue::c_str() const {
    JSON_ASSERT(type_ == stringValue);
    return value_.string_;
}


String JSONValue::asString() const {
    switch(type_) {
        case nullValue:
            return "";
        case stringValue:
            return value_.string_ ? value_.string_ : "";
        case booleanValue:
            return value_.bool_ ? "true" : "false";
        case intValue:
        case uintValue:
        case realValue:
        case arrayValue:
        case objectValue:
            JSON_ASSERT_MESSAGE(false, "Type is not convertible to string");
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return ""; // unreachable
}

# ifdef JSON_USE_CPPTL
CppTL::ConstString JSONValue::asConstString() const {
    return CppTL::ConstString(asString().c_str());
}
# endif

JSONValue::Int JSONValue::asInt() const {
    switch(type_) {
        case nullValue:
            return 0;
        case intValue:
            return value_.int_;
        case uintValue:
            JSON_ASSERT_MESSAGE(value_.uint_ < (unsigned)maxInt, "integer out of signed integer range");
            return value_.uint_;
        case realValue:
            JSON_ASSERT_MESSAGE(value_.real_ >= minInt  &&  value_.real_ <= maxInt, "Real out of signed integer range");
            return Int(value_.real_);
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        case stringValue:
        case arrayValue:
        case objectValue:
            JSON_ASSERT_MESSAGE(false, "Type is not convertible to int");
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}

JSONValue::UInt JSONValue::asUInt() const {
    switch(type_) {
        case nullValue:
            return 0;
        case intValue:
            JSON_ASSERT_MESSAGE(value_.int_ >= 0, "Negative integer can not be converted to unsigned integer");
            return value_.int_;
        case uintValue:
            return value_.uint_;
        case realValue:
            JSON_ASSERT_MESSAGE(value_.real_ >= 0  &&  value_.real_ <= maxUInt,  "Real out of unsigned integer range");
            return UInt(value_.real_);
        case booleanValue:
            return value_.bool_ ? 1 : 0;
        case stringValue:
        case arrayValue:
        case objectValue:
            JSON_ASSERT_MESSAGE(false, "Type is not convertible to uint");
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}

double JSONValue::asDouble() const {
    switch(type_) {
        case nullValue:
            return 0.0;
        case intValue:
            return value_.int_;
        case uintValue:
            return value_.uint_;
        case realValue:
            return value_.real_;
        case booleanValue:
            return value_.bool_ ? 1.0 : 0.0;
        case stringValue:
        case arrayValue:
        case objectValue:
            JSON_ASSERT_MESSAGE(false, "Type is not convertible to double");
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}

bool JSONValue::asBool() const {
    switch(type_) {
        case nullValue:
            return false;
        case intValue:
        case uintValue:
            return value_.int_ != 0;
        case realValue:
            return value_.real_ != 0.0;
        case booleanValue:
            return value_.bool_;
        case stringValue:
            return value_.string_  &&  value_.string_[0] != 0;
        case arrayValue:
        case objectValue:
            return value_.map_->size() != 0;
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return false; // unreachable;
}


bool JSONValue::isConvertibleTo(JSONValueType other) const {
    switch(type_) {
        case nullValue:
            return true;
        case intValue:
            return (other == nullValue  &&  value_.int_ == 0)
                   || other == intValue
                   || (other == uintValue  && value_.int_ >= 0)
                   || other == realValue
                   || other == stringValue
                   || other == booleanValue;
        case uintValue:
            return (other == nullValue  &&  value_.uint_ == 0)
                   || (other == intValue  && value_.uint_ <= (unsigned)maxInt)
                   || other == uintValue
                   || other == realValue
                   || other == stringValue
                   || other == booleanValue;
        case realValue:
            return (other == nullValue  &&  value_.real_ == 0.0)
                   || (other == intValue  &&  value_.real_ >= minInt  &&  value_.real_ <= maxInt)
                   || (other == uintValue  &&  value_.real_ >= 0  &&  value_.real_ <= maxUInt)
                   || other == realValue
                   || other == stringValue
                   || other == booleanValue;
        case booleanValue:
            return (other == nullValue  &&  value_.bool_ == false)
                   || other == intValue
                   || other == uintValue
                   || other == realValue
                   || other == stringValue
                   || other == booleanValue;
        case stringValue:
            return other == stringValue
                   || (other == nullValue  && (!value_.string_  ||  value_.string_[0] == 0));
        case arrayValue:
            return other == arrayValue
                   || (other == nullValue  &&  value_.map_->size() == 0);
        case objectValue:
            return other == objectValue
                   || (other == nullValue  &&  value_.map_->size() == 0);
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return false; // unreachable;
}


/// Number of values in array or object
JSONValue::UInt JSONValue::size() const {
    switch(type_) {
        case nullValue:
        case intValue:
        case uintValue:
        case realValue:
        case booleanValue:
        case stringValue:
            return 0;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:  // size of the array is highest index + 1
            if(!value_.map_->empty()) {
                ObjectValues::const_iterator itLast = value_.map_->end();
                --itLast;
                return (*itLast).first.index() + 1;
            }
            return 0;
        case objectValue:
            return Int(value_.map_->size());
#else
        case arrayValue:
            return Int(value_.array_->size());
        case objectValue:
            return Int(value_.map_->size());
#endif
        default:
            JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}


bool JSONValue::empty() const {
    if(isNull() || isArray() || isObject())
        return size() == 0u;
    else
        return false;
}


bool JSONValue::operator!() const {
    return isNull();
}


void JSONValue::clear() {
    JSON_ASSERT(type_ == nullValue  ||  type_ == arrayValue  || type_ == objectValue);

    switch(type_) {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
        case objectValue:
            value_.map_->clear();
            break;
#else
        case arrayValue:
            value_.array_->clear();
            break;
        case objectValue:
            value_.map_->clear();
            break;
#endif
        default:
            break;
    }
}

void JSONValue::resize(UInt newSize) {
    JSON_ASSERT(type_ == nullValue  ||  type_ == arrayValue);
    if(type_ == nullValue)
        *this = JSONValue(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    UInt oldSize = size();
    if(newSize == 0)
        clear();
    else if(newSize > oldSize)
        (*this)[ newSize - 1 ];
    else {
        for(UInt index = newSize; index < oldSize; ++index)
            value_.map_->erase(index);
        assert(size() == newSize);
    }
#else
    value_.array_->resize(newSize);
#endif
}


JSONValue & JSONValue::operator[](UInt index) {
    JSON_ASSERT(type_ == nullValue  ||  type_ == arrayValue);
    if(type_ == nullValue)
        *this = JSONValue(arrayValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString key(index);
    ObjectValues::iterator it = value_.map_->lower_bound(key);
    if(it != value_.map_->end()  && (*it).first == key)
        return (*it).second;

    ObjectValues::value_type defaultValue(key, null);
    it = value_.map_->insert(it, defaultValue);
    return (*it).second;
#else
    return value_.array_->resolveReference(index);
#endif
}


const JSONValue & JSONValue::operator[](UInt index) const {
    JSON_ASSERT(type_ == nullValue  ||  type_ == arrayValue);
    if(type_ == nullValue)
        return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString key(index);
    ObjectValues::const_iterator it = value_.map_->find(key);
    if(it == value_.map_->end())
        return null;
    return (*it).second;
#else
    JSONValue *value = value_.array_->find(index);
    return value ? *value : null;
#endif
}


JSONValue & JSONValue::operator[](const String &key) {
    return resolveReference(key.c_str(), false);
}


JSONValue & JSONValue::operator[](const char *key) {
    return resolveReference(key, false);
}


JSONValue & JSONValue::resolveReference(const char *key,
                                        bool isStatic) {
    JSON_ASSERT(type_ == nullValue  ||  type_ == objectValue);
    if(type_ == nullValue)
        *this = JSONValue(objectValue);
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key, isStatic ? CZString::noDuplication
                       : CZString::duplicateOnCopy);
    ObjectValues::iterator it = value_.map_->lower_bound(actualKey);
    if(it != value_.map_->end()  && (*it).first == actualKey)
        return (*it).second;

    ObjectValues::value_type defaultValue(actualKey, null);
    it = value_.map_->insert(it, defaultValue);
    JSONValue &value = (*it).second;
    return value;
#else
    return value_.map_->resolveReference(key, isStatic);
#endif
}


JSONValue JSONValue::get(UInt index,
                         const JSONValue &defaultValue) const {
    const JSONValue *value = &((*this)[index]);
    return value == &null ? defaultValue : *value;
}


bool JSONValue::isValidIndex(UInt index) const {
    return index < size();
}



const JSONValue & JSONValue::operator[](const String &key) const {
    JSON_ASSERT(type_ == nullValue  ||  type_ == objectValue);
    if(type_ == nullValue)
        return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key.c_str(), CZString::noDuplication);
    ObjectValues::const_iterator it = value_.map_->find(actualKey);
    if(it == value_.map_->end())
        return null;
    return (*it).second;
#else
    const JSONValue *value = value_.map_->find(key.c_str());
    return value ? *value : null;
#endif
}


const JSONValue & JSONValue::operator[](const char *key) const {
    JSON_ASSERT(type_ == nullValue  ||  type_ == objectValue);
    if(type_ == nullValue)
        return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key, CZString::noDuplication);
    ObjectValues::const_iterator it = value_.map_->find(actualKey);
    if(it == value_.map_->end())
        return null;
    return (*it).second;
#else
    const JSONValue *value = value_.map_->find(key);
    return value ? *value : null;
#endif
}


JSONValue & JSONValue::operator[](const std::string &key) {
    return (*this)[ key.c_str() ];
}


const JSONValue & JSONValue::operator[](const std::string &key) const {
    return (*this)[ key.c_str() ];
}

JSONValue & JSONValue::operator[](const StaticString &key) {
    return resolveReference(key, true);
}


# ifdef JSON_USE_CPPTL
JSONValue & JSONValue::operator[](const CppTL::ConstString &key) {
    return (*this)[ key.c_str() ];
}


const JSONValue & JSONValue::operator[](const CppTL::ConstString &key) const {
    return (*this)[ key.c_str() ];
}
# endif


JSONValue & JSONValue::append(const JSONValue &value) {
    return (*this)[size()] = value;
}


JSONValue JSONValue::get(const String &key,
                         const JSONValue &defaultValue) const {
    const JSONValue *value = &((*this)[key]);
    return value == &null ? defaultValue : *value;
}


JSONValue JSONValue::get(const char *key,
                         const JSONValue &defaultValue) const {
    const JSONValue *value = &((*this)[key]);
    return value == &null ? defaultValue : *value;
}


JSONValue JSONValue::get(const std::string &key,
                         const JSONValue &defaultValue) const {
    return get(key.c_str(), defaultValue);
}

JSONValue JSONValue::removeMember(const char* key) {
    JSON_ASSERT(type_ == nullValue  ||  type_ == objectValue);
    if(type_ == nullValue)
        return null;
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    CZString actualKey(key, CZString::noDuplication);
    ObjectValues::iterator it = value_.map_->find(actualKey);
    if(it == value_.map_->end())
        return null;
    JSONValue old(it->second);
    value_.map_->erase(it);
    return old;
#else
    JSONValue *value = value_.map_->find(key);
    if(value) {
        JSONValue old(*value);
        value_.map_.remove(key);
        return old;
    } else {
        return null;
    }
#endif
}

JSONValue JSONValue::removeMember(const std::string &key) {
    return removeMember(key.c_str());
}

JSONValue JSONValue::removeMember(const String &key) {
    return removeMember(key.c_str());
}

# ifdef JSON_USE_CPPTL
JSONValue JSONValue::get(const CppTL::ConstString &key,
                         const JSONValue &defaultValue) const {
    return get(key.c_str(), defaultValue);
}
# endif

bool JSONValue::isMember(const char *key) const {
    const JSONValue *value = &((*this)[key]);
    return value != &null;
}


bool JSONValue::isMember(const std::string &key) const {
    return isMember(key.c_str());
}


bool JSONValue::isMember(const String &key) const {
    return isMember(key.c_str());
}


# ifdef JSON_USE_CPPTL
bool JSONValue::isMember(const CppTL::ConstString &key) const {
    return isMember(key.c_str());
}
#endif

JSONValue::Members JSONValue::getMemberNames() const {
    JSON_ASSERT(type_ == nullValue  ||  type_ == objectValue);
    if(type_ == nullValue)
        return JSONValue::Members();
    Members members;
    members.reserve(value_.map_->size());
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ObjectValues::const_iterator it = value_.map_->begin();
    ObjectValues::const_iterator itEnd = value_.map_->end();
    for(; it != itEnd; ++it)
        members.push_back(std::string((*it).first.c_str()));
#else
    JSONValueInternalMap::IteratorState it;
    JSONValueInternalMap::IteratorState itEnd;
    value_.map_->makeBeginIterator(it);
    value_.map_->makeEndIterator(itEnd);
    for(; !ValueInternalMap::equals(it, itEnd); JSONValueInternalMap::increment(it))
        members.push_back(std::string(JSONValueInternalMap::key(it)));
#endif
    return members;
}
//
//# ifdef JSON_USE_CPPTL
//EnumMemberNames
//Value::enumMemberNames() const
//{
//   if ( type_ == objectValue )
//   {
//      return CppTL::Enum::any(  CppTL::Enum::transform(
//         CppTL::Enum::keys( *(value_.map_), CppTL::Type<const CZString &>() ),
//         MemberNamesTransform() ) );
//   }
//   return EnumMemberNames();
//}
//
//
//EnumValues
//Value::enumValues() const
//{
//   if ( type_ == objectValue  ||  type_ == arrayValue )
//      return CppTL::Enum::anyValues( *(value_.map_),
//                                     CppTL::Type<const JSONValue &>() );
//   return EnumValues();
//}
//
//# endif


bool JSONValue::isNull() const {
    return type_ == nullValue;
}


bool JSONValue::isBool() const {
    return type_ == booleanValue;
}


bool JSONValue::isInt() const {
    return type_ == intValue;
}


bool JSONValue::isUInt() const {
    return type_ == uintValue;
}


bool JSONValue::isIntegral() const {
    return type_ == intValue
           ||  type_ == uintValue
           ||  type_ == booleanValue;
}


bool JSONValue::isDouble() const {
    return type_ == realValue;
}


bool JSONValue::isNumeric() const {
    return isIntegral() || isDouble();
}


bool JSONValue::isString() const {
    return type_ == stringValue;
}


bool JSONValue::isArray() const {
    return type_ == nullValue  ||  type_ == arrayValue;
}


bool JSONValue::isObject() const {
    return type_ == nullValue  ||  type_ == objectValue;
}


void JSONValue::setComment(const String &comment,
                           CommentPlacement placement) {
    setComment(comment.c_str(), placement);
}


void JSONValue::setComment(const char *comment,
                           CommentPlacement placement) {
    if(!comments_)
        comments_ = new CommentInfo[numberOfCommentPlacement];
    comments_[placement].setComment(comment);
}


void JSONValue::setComment(const std::string &comment,
                           CommentPlacement placement) {
    setComment(comment.c_str(), placement);
}


bool JSONValue::hasComment(CommentPlacement placement) const {
    return comments_ != 0  &&  comments_[placement].comment_ != 0;
}

String JSONValue::getComment(CommentPlacement placement) const {
    if(hasComment(placement))
        return comments_[placement].comment_;
    return "";
}


String JSONValue::toStyledString() const {
    JSONStyledWriter writer;
    return writer.write(*this);
}


JSONValue::const_iterator JSONValue::begin() const {
    switch(type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if(value_.array_) {
                JSONValueInternalArray::IteratorState it;
                value_.array_->makeBeginIterator(it);
                return const_iterator(it);
            }
            break;
        case objectValue:
            if(value_.map_) {
                JSONValueInternalMap::IteratorState it;
                value_.map_->makeBeginIterator(it);
                return const_iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if(value_.map_)
                return const_iterator(value_.map_->begin());
            break;
#endif
        default:
            break;
    }
    return const_iterator();
}

JSONValue::const_iterator JSONValue::end() const {
    switch(type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if(value_.array_) {
                JSONValueInternalArray::IteratorState it;
                value_.array_->makeEndIterator(it);
                return const_iterator(it);
            }
            break;
        case objectValue:
            if(value_.map_) {
                JSONValueInternalMap::IteratorState it;
                value_.map_->makeEndIterator(it);
                return const_iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if(value_.map_)
                return const_iterator(value_.map_->end());
            break;
#endif
        default:
            break;
    }
    return const_iterator();
}


JSONValue::iterator JSONValue::begin() {
    switch(type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if(value_.array_) {
                JSONValueInternalArray::IteratorState it;
                value_.array_->makeBeginIterator(it);
                return iterator(it);
            }
            break;
        case objectValue:
            if(value_.map_) {
                JSONValueInternalMap::IteratorState it;
                value_.map_->makeBeginIterator(it);
                return iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if(value_.map_)
                return iterator(value_.map_->begin());
            break;
#endif
        default:
            break;
    }
    return iterator();
}

JSONValue::iterator JSONValue::end() {
    switch(type_) {
#ifdef JSON_VALUE_USE_INTERNAL_MAP
        case arrayValue:
            if(value_.array_) {
                JSONValueInternalArray::IteratorState it;
                value_.array_->makeEndIterator(it);
                return iterator(it);
            }
            break;
        case objectValue:
            if(value_.map_) {
                JSONValueInternalMap::IteratorState it;
                value_.map_->makeEndIterator(it);
                return iterator(it);
            }
            break;
#else
        case arrayValue:
        case objectValue:
            if(value_.map_)
                return iterator(value_.map_->end());
            break;
#endif
        default:
            break;
    }
    return iterator();
}


// class PathArgument
// //////////////////////////////////////////////////////////////////

PathArgument::PathArgument()
    : kind_(kindNone) {
}


PathArgument::PathArgument(JSONValue::UInt index)
    : index_(index)
    , kind_(kindIndex) {
}


PathArgument::PathArgument(const String &key)
    : key_(key.c_str())
    , kind_(kindKey) {
}


PathArgument::PathArgument(const char *key)
    : key_(key)
    , kind_(kindKey) {
}


PathArgument::PathArgument(const std::string &key)
    : key_(key.c_str())
    , kind_(kindKey) {
}

// class Path
// //////////////////////////////////////////////////////////////////

Path::Path(const String &path,
           const PathArgument &a1,
           const PathArgument &a2,
           const PathArgument &a3,
           const PathArgument &a4,
           const PathArgument &a5) {
    InArgs in;
    in.push_back(&a1);
    in.push_back(&a2);
    in.push_back(&a3);
    in.push_back(&a4);
    in.push_back(&a5);
    makePath(path.c_str(), in);
}


Path::Path(const std::string &path,
           const PathArgument &a1,
           const PathArgument &a2,
           const PathArgument &a3,
           const PathArgument &a4,
           const PathArgument &a5) {
    InArgs in;
    in.push_back(&a1);
    in.push_back(&a2);
    in.push_back(&a3);
    in.push_back(&a4);
    in.push_back(&a5);
    makePath(path, in);
}


Path::Path(const char *path,
           const PathArgument &a1,
           const PathArgument &a2,
           const PathArgument &a3,
           const PathArgument &a4,
           const PathArgument &a5) {
    InArgs in;
    in.push_back(&a1);
    in.push_back(&a2);
    in.push_back(&a3);
    in.push_back(&a4);
    in.push_back(&a5);
    makePath(path, in);
}


void Path::makePath(const std::string &path,
                    const InArgs &in) {
    const char *current = path.c_str();
    const char *end = current + path.length();
    InArgs::const_iterator itInArg = in.begin();
    while(current != end) {
        if(*current == '[') {
            ++current;
            if(*current == '%')
                addPathInArg(path, in, itInArg, PathArgument::kindIndex);
            else {
                JSONValue::UInt index = 0;
                for(; current != end && *current >= '0'  &&  *current <= '9'; ++current)
                    index = index * 10 + JSONValue::UInt(*current - '0');
                args_.push_back(index);
            }
            if(current == end  ||  *current++ != ']')
                invalidPath(path, int(current - path.c_str()));
        } else if(*current == '%') {
            addPathInArg(path, in, itInArg, PathArgument::kindKey);
            ++current;
        } else if(*current == '.') {
            ++current;
        } else {
            const char *beginName = current;
            while(current != end  &&  !strchr("[.", *current))
                ++current;
            args_.push_back(std::string(beginName, current));
        }
    }
}


void Path::addPathInArg(const std::string &path,
                        const InArgs &in,
                        InArgs::const_iterator &itInArg,
                        PathArgument::Kind kind) {
    if(itInArg == in.end()) {
        // Error: missing argument %d
    } else if((*itInArg)->kind_ != kind) {
        // Error: bad argument type
    } else {
        args_.push_back(**itInArg);
    }
}


void Path::invalidPath(const std::string &path,
                       int location) {
    // Error: invalid path.
}


const JSONValue & Path::resolve(const JSONValue &root) const {
    const JSONValue *node = &root;
    for(Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        const PathArgument &arg = *it;
        if(arg.kind_ == PathArgument::kindIndex) {
            if(!node->isArray()  ||  node->isValidIndex(arg.index_)) {
                // Error: unable to resolve path (array value expected at position...
            }
            node = &((*node)[arg.index_]);
        } else if(arg.kind_ == PathArgument::kindKey) {
            if(!node->isObject()) {
                // Error: unable to resolve path (object value expected at position...)
            }
            node = &((*node)[arg.key_]);
            if(node == &JSONValue::null) {
                // Error: unable to resolve path (object has no member named '' at position...)
            }
        }
    }
    return *node;
}


JSONValue Path::resolve(const JSONValue &root,
                        const JSONValue &defaultValue) const {
    const JSONValue *node = &root;
    for(Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        const PathArgument &arg = *it;
        if(arg.kind_ == PathArgument::kindIndex) {
            if(!node->isArray()  ||  node->isValidIndex(arg.index_))
                return defaultValue;
            node = &((*node)[arg.index_]);
        } else if(arg.kind_ == PathArgument::kindKey) {
            if(!node->isObject())
                return defaultValue;
            node = &((*node)[arg.key_]);
            if(node == &JSONValue::null)
                return defaultValue;
        }
    }
    return *node;
}


JSONValue & Path::make(JSONValue &root) const {
    JSONValue *node = &root;
    for(Args::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        const PathArgument &arg = *it;
        if(arg.kind_ == PathArgument::kindIndex) {
            if(!node->isArray()) {
                // Error: node is not an array at position ...
            }
            node = &((*node)[arg.index_]);
        } else if(arg.kind_ == PathArgument::kindKey) {
            if(!node->isObject()) {
                // Error: node is not an object at position...
            }
            node = &((*node)[arg.key_]);
        }
    }
    return *node;
}


} // namespace apolloron
