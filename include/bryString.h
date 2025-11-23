#pragma once

#include <iostream>
#include <string_view>

class String
{
public:
    friend std::ostream& operator<<(std::ostream& os, const String& str);
    friend String operator+(const char* lhs, const String& rhs);
    friend String operator+(std::string_view lhs, const String& rhs);
    friend String operator+(const String& lhs, std::string_view rhs);
    friend String operator+(const String& lhs, const char* rhs);
    friend String operator-(const String& lhs, const char* rhs);
    friend String operator-(const char* lhs, const String& rhs);
    friend String operator-(const String& lhs, std::string_view rhs);
    friend String operator-(std::string_view lhs, const String& rhs);
    String(const char* stringLit);
    String(std::string_view sv);
    String();
    ~ String();
private:
    size_t len_;
    char* str_;
    String(size_t size);
};



