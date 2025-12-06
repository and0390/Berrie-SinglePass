#include "bryString.h"
#include <cstring>
#include <exception>

String::String(size_t size)
    : len_(size), str_(new char[len_])
{
    for (size_t i = 0; i < len_; i++) {
        str_[i] = '\0';
    }
}

bool String::operator!() const noexcept {
    return true;
}

String::String(const char* stringLit)
    : len_{ stringLit ? std::strlen(stringLit) : 0 }, 
      str_{ new char[len_ + 1] }
{
    if(stringLit) {
        std::strcpy(str_, stringLit);
    }
    str_[len_] = '\0';
}

String::String(const char *stringLit, size_t size)
    : len_(size), str_{ new char[len_ + 1] }
{
    if(stringLit) {
        std::memcpy(str_, stringLit, size);
    }
    str_[len_] = '\0';
}

String::String(const String& other)
    : len_(other.len_), str_(new char[other.len_ + 1])
{
    std::memcpy(str_, other.str_, len_ + 1); // copy including '\0'
}

String& String::operator=(const String& other) {
    if (this != &other) {
        char* new_str = new char[other.len_ + 1];
        std::memcpy(new_str, other.str_, other.len_ + 1);
        delete[] str_;
        str_ = new_str;
        len_ = other.len_;
    }
    return *this;
}

String::String(std::int64_t value)
    : len_(0), str_(nullptr)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%lld", static_cast<long long>(value));
    len_ = std::strlen(buffer);
    str_ = new char[len_ + 1];
    std::memcpy(str_, buffer, len_ + 1);
}

String::String(double value) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.17g", static_cast<double>(value));
    len_ = std::strlen(buffer);
    str_ = new char[len_ + 1];
    std::memcpy(str_, buffer, len_ + 1);
}

size_t String::size() const noexcept {
    return len_;
}

char & String::operator[](size_t index) {
    if (index >= len_) throw std::out_of_range("out of range");
    return str_[index];
}

String::String(String&& other) noexcept
    : len_(other.len_), str_(other.str_)
{
    other.len_ = 0;
    other.str_ = nullptr;
}

String::operator std::string_view() const noexcept {
    return std::string_view(c_str(), len_);
}

String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        delete[] str_;       // free existing buffer
        str_ = other.str_;   // steal pointer
        len_ = other.len_;
        other.str_ = nullptr;
        other.len_ = 0;
    }
    return *this;
}

const char * String::c_str() const noexcept {
    return str_;
}

String::String()
    : len_(0), str_(new char[1])
{
    *str_ = '\0';
}

String::String(std::string_view sv)
    : String(sv.data(), sv.size())
{}

std::ostream& operator<<(std::ostream& os, const String& str)
{
    return os << str.str_;
}

String operator+(const char* lhs, const String& rhs)
{
    std::size_t lhsLen = std::strlen(lhs);

    String result;
    result.len_ = lhsLen + rhs.len_;
    result.str_ = new char[result.len_ + 1];

    std::memcpy(result.str_, lhs, lhsLen);
    std::memcpy(result.str_ + lhsLen, rhs.str_, rhs.len_);
    result.str_[result.len_] = '\0';

    return result;
}

String operator+(const String& lhs, const char* rhs)
{
    std::size_t rhsLen = std::strlen(rhs);

    String result;
    result.len_ = lhs.len_ + rhsLen;
    result.str_ = new char[result.len_ + 1];

    std::memcpy(result.str_, lhs.str_, lhs.len_);
    std::memcpy(result.str_ + lhs.len_, rhs, rhsLen);
    result.str_[result.len_] = '\0';

    return result;
}

String operator+(const String &lhs, const String &rhs) {
    return lhs.c_str() + rhs;
}

String operator+(std::string_view lhs, const String& rhs)
{
    return lhs.data() + rhs;
}

String operator+(const String& lhs, std::string_view rhs)
{
    return lhs + rhs.data();
}

String operator-(const String& lhs, const char* rhs)
{
    const char* a = lhs.str_;
    const char* b = rhs;

    const size_t a_len = lhs.len_;
    const size_t b_len = std::strlen(rhs);

    //b inside a
    if(a_len >= b_len) {
        const char* pos = std::strstr(a, b);
        if(pos) {
            const size_t idx = pos - a;

            String result(a_len - b_len);

            // Copy before match
            std::memcpy(result.str_, a, idx);

            // Copy after match
            std::memcpy(result.str_ + idx, a + idx + b_len, a_len - idx - b_len);

            result.str_[result.len_] = '\0';

             // Trim trailing space if needed
            while (result.len_ > 0 && result.str_[result.len_ - 1] == ' ') {
                result.str_[--result.len_] = '\0';
            }

            return result;
        }
    }


    // a inside b
    if(b_len >= a_len) {
        const char* pos = std::strstr(b, a);
        if(pos) {
            const size_t idx = pos - b;

            String result(b_len - a_len);

            std::memcpy(result.str_, b, idx);

            std::memcpy(result.str_ + idx, b + idx + a_len, b_len - idx - a_len);

            result.str_[result.len_] = '\0';

            while (result.len_ > 0 && result.str_[result.len_ - 1] == ' ') {
                result.str_[--result.len_] = '\0';
            }

            return result;
        }
    }

    return String("");
}

String operator-(const String& lhs, std::string_view rhs)
{
    return lhs - rhs.data();
}

String operator-(std::string_view lhs, const String& rhs)
{
    return lhs.data() - rhs;    
}

String operator-(const String &lhs, const String &rhs) {
    return lhs.c_str() - rhs;
}

String operator-(const char* lhs, const String& rhs)
{
    //assosiative
    return rhs - lhs;
}

String operator*(std::int64_t times, const String& rhs) {
    if (times <= 0) return String(static_cast<size_t>(0));

    const size_t new_len = rhs.len_ * times;
    String new_string(new_len);
    for (size_t i = 0; i < new_len; i++) {
        new_string[i] = rhs.str_[i % rhs.len_];
    }

    return  new_string;
}

String operator*(const String &lhs, std::int64_t times) {
    return times * lhs;
}

bool operator==(const String & lhs, const String & rhs) noexcept {
    return std::strcmp(lhs.str_, rhs.str_) == 0;
}

bool operator<(const String & lhs, const String & rhs) noexcept {
    return std::strcmp(lhs.str_, rhs.str_) < 0;
}

bool operator>(const String &lhs, const String &rhs) noexcept {
    return rhs < lhs;
}

String::~ String()
{
    delete[] str_;
}
