#pragma once

#include <sstream>
#include <typeinfo>

#define ST_TAG_TYPE(T) ::blink::Tag<T>(__func__, __LINE__)
#define ST_TAG ::blink::Tag(*this, __func__, __LINE__)

namespace blink
{
    std::string FormatTag(const char* typeName, const char* functionName, uint32_t lineNumber);

    template<class T>
    std::string Tag(const char* functionName, uint32_t lineNumber)
    {
        return FormatTag(typeid(T).name(), functionName, lineNumber);
    }

    template<class T>
    std::string Tag(const T& t, const char* functionName, uint32_t lineNumber)
    {
        return FormatTag(typeid(t).name(), functionName, lineNumber);
    }
}