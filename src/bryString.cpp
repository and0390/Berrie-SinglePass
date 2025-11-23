#include "bryString.h"
#include <cstring>

String:: String(const char* stringLit)
    : len_{ stringLit ? std::strlen(stringLit) : 0 }, 
      str_{ new char[len_ + 1] }
{
    if(stringLit) {
        std::strcpy(str_, stringLit);
    }
    else {
        *str_ = '\0';
    }
}

String::String()
    : len_(0), str_(new char[1])
{
    *str_ = '\0';
}

String::String(std::string_view sv)
    : String(sv.data())
{}

String::String(size_t size)
    : len_(size), str_(new char[len_ + 1])
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

String operator-(const char* lhs, const String& rhs)
{
    //assosiative
    return rhs - lhs;
}

String::~ String()
{
    delete[] str_;
}