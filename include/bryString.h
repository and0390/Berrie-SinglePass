#pragma once

#include <iostream>
#include <string_view>
#include <cstdint>

/// @brief Custom implementation of std::string-like class
class String
{
public:
    friend std::ostream& operator<<(std::ostream& os, const String& str);
    friend String operator+(const char* lhs, const String& rhs);
    friend String operator+(std::string_view lhs, const String& rhs);
    friend String operator+(const String& lhs, std::string_view rhs);
    friend String operator+(const String& lhs, const char* rhs);
    friend String operator+(const String& lhs, const String& rhs);
    friend String operator-(const String& lhs, const char* rhs);
    friend String operator-(const char* lhs, const String& rhs);
    friend String operator-(const String& lhs, std::string_view rhs);
    friend String operator-(std::string_view lhs, const String& rhs);
    friend String operator-(const String& lhs, const String& rhs);
    friend String operator*(std::int64_t times, const String& rhs);
    friend String operator*(const String& lhs, std::int64_t times);
    friend bool operator==(const String &, const String &) noexcept;
    friend bool operator<(const String &, const String &) noexcept;
    friend bool operator>(const String &, const String &) noexcept;
    bool operator!() const noexcept;
    String(const char* stringLit);
    String(const char* stringLit, size_t size);
    String(const String& other);
    String& operator=(const String& other);
    String(std::int64_t value);
    String(size_t size);
    String(double value);
    size_t size() const noexcept;
    char& operator[](size_t index);
    String(String&& other) noexcept;
    explicit operator std::string_view() const noexcept;
    String& operator=(String&& other) noexcept;
    const char* c_str() const noexcept;
    String(std::string_view sv);
    String();
    ~ String();
private:
    size_t len_;
    char* str_;
};



