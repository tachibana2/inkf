// included by json_value.cc
// everything is within apolloron namespace


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class JSONValueIteratorBase
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

JSONValueIteratorBase::JSONValueIteratorBase()
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    : current_()
    , isNull_(true) {
}
#else
    :
    isArray_(true)
    , isNull_(true) {
    iterator_.array_ = JSONValueInternalArray::IteratorState();
}
#endif


#ifndef JSON_VALUE_USE_INTERNAL_MAP
JSONValueIteratorBase::JSONValueIteratorBase(const JSONValue::ObjectValues::iterator &current)
    : current_(current)
    , isNull_(false) {
}
#else
JSONValueIteratorBase::JSONValueIteratorBase(const JSONValueInternalArray::IteratorState &state)
    : isArray_(true) {
    iterator_.array_ = state;
}


JSONValueIteratorBase::JSONValueIteratorBase(const JSONValueInternalMap::IteratorState &state)
    : isArray_(false) {
    iterator_.map_ = state;
}
#endif

JSONValue & JSONValueIteratorBase::deref() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    return current_->second;
#else
    if(isArray_)
        return JSONValueInternalArray::dereference(iterator_.array_);
    return JSONValueInternalMap::value(iterator_.map_);
#endif
}


void JSONValueIteratorBase::increment() {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    ++current_;
#else
    if(isArray_)
        JSONValueInternalArray::increment(iterator_.array_);
    JSONValueInternalMap::increment(iterator_.map_);
#endif
}


void JSONValueIteratorBase::decrement() {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    --current_;
#else
    if(isArray_)
        JSONValueInternalArray::decrement(iterator_.array_);
    JSONValueInternalMap::decrement(iterator_.map_);
#endif
}


JSONValueIteratorBase::difference_type JSONValueIteratorBase::computeDistance(const SelfType &other) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
# ifdef JSON_USE_CPPTL_SMALLMAP
    return current_ - other.current_;
# else
    // Iterator for null value are initialized using the default
    // constructor, which initialize current_ to the default
    // std::map::iterator. As begin() and end() are two instance
    // of the default std::map::iterator, they can not be compared.
    // To allow this, we handle this comparison specifically.
    if(isNull_  &&  other.isNull_) {
        return 0;
    }


    // Usage of std::distance is not portable (does not compile with Sun Studio 12 RogueWave STL,
    // which is the one used by default).
    // Using a portable hand-made version for non random iterator instead:
    //   return difference_type( std::distance( current_, other.current_ ) );
    difference_type myDistance = 0;
    for(JSONValue::ObjectValues::iterator it = current_; it != other.current_; ++it) {
        ++myDistance;
    }
    return myDistance;
# endif
#else
    if(isArray_)
        return JSONValueInternalArray::distance(iterator_.array_, other.iterator_.array_);
    return JSONValueInternalMap::distance(iterator_.map_, other.iterator_.map_);
#endif
}


bool JSONValueIteratorBase::isEqual(const SelfType &other) const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    if(isNull_) {
        return other.isNull_;
    }
    return current_ == other.current_;
#else
    if(isArray_)
        return JSONValueInternalArray::equals(iterator_.array_, other.iterator_.array_);
    return JSONValueInternalMap::equals(iterator_.map_, other.iterator_.map_);
#endif
}


void JSONValueIteratorBase::copy(const SelfType &other) {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    current_ = other.current_;
#else
    if(isArray_)
        iterator_.array_ = other.iterator_.array_;
    iterator_.map_ = other.iterator_.map_;
#endif
}


JSONValue JSONValueIteratorBase::key() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    const JSONValue::CZString czstring = (*current_).first;
    if(czstring.c_str()) {
        if(czstring.isStaticString())
            return JSONValue(StaticString(czstring.c_str()));
        return JSONValue(czstring.c_str());
    }
    return JSONValue(czstring.index());
#else
    if(isArray_)
        return JSONValue(JSONValueInternalArray::indexOf(iterator_.array_));
    bool isStatic;
    const char *memberName = JSONValueInternalMap::key(iterator_.map_, isStatic);
    if(isStatic)
        return JSONValue(StaticString(memberName));
    return JSONValue(memberName);
#endif
}


UInt JSONValueIteratorBase::index() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    const JSONValue::CZString czstring = (*current_).first;
    if(!czstring.c_str())
        return czstring.index();
    return JSONValue::UInt(-1);
#else
    if(isArray_)
        return JSONValue::UInt(JSONValueInternalArray::indexOf(iterator_.array_));
    return JSONValue::UInt(-1);
#endif
}


const char * JSONValueIteratorBase::memberName() const {
#ifndef JSON_VALUE_USE_INTERNAL_MAP
    const char *name = (*current_).first.c_str();
    return name ? name : "";
#else
    if(!isArray_)
        return JSONValueInternalMap::key(iterator_.map_);
    return "";
#endif
}


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class JSONValueConstIterator
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

JSONValueConstIterator::JSONValueConstIterator() {
}


#ifndef JSON_VALUE_USE_INTERNAL_MAP
JSONValueConstIterator::JSONValueConstIterator(const JSONValue::ObjectValues::iterator &current)
    : JSONValueIteratorBase(current) {
}
#else
JSONValueConstIterator::JSONValueConstIterator(const JSONValueInternalArray::IteratorState &state)
    : JSONValueIteratorBase(state) {
}

JSONValueConstIterator::JSONValueConstIterator(const JSONValueInternalMap::IteratorState &state)
    : JSONValueIteratorBase(state) {
}
#endif

JSONValueConstIterator & JSONValueConstIterator::operator =(const JSONValueIteratorBase &other) {
    copy(other);
    return *this;
}


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// class JSONValueIterator
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

JSONValueIterator::JSONValueIterator() {
}


#ifndef JSON_VALUE_USE_INTERNAL_MAP
JSONValueIterator::JSONValueIterator(const JSONValue::ObjectValues::iterator &current)
    : JSONValueIteratorBase(current) {
}
#else
JSONValueIterator::JSONValueIterator(const JSONValueInternalArray::IteratorState &state)
    : JSONValueIteratorBase(state) {
}

JSONValueIterator::JSONValueIterator(const JSONValueInternalMap::IteratorState &state)
    : JSONValueIteratorBase(state) {
}
#endif

JSONValueIterator::JSONValueIterator(const JSONValueConstIterator &other)
    : JSONValueIteratorBase(other) {
}

JSONValueIterator::JSONValueIterator(const JSONValueIterator &other)
    : JSONValueIteratorBase(other) {
}

JSONValueIterator & JSONValueIterator::operator =(const SelfType &other) {
    copy(other);
    return *this;
}
