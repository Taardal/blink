#include "Tag.h"

std::string blink::FormatTag(const char* typeName, const char* functionName, uint32_t lineNumber)
{
    std::stringstream ss;
    ss << typeName << ":" << functionName << ":" << lineNumber;
    return ss.str();
}