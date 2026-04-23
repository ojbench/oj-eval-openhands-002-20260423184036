#include "include/int2048.h"

namespace sjtu {

const int BASE = 100000000; // 10^8
const int WIDTH = 8;

// Private helper methods
void int2048::normalize() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.empty()) {
        digits.push_back(0);
        negative = false;
    }
    if (digits.size() == 1 && digits[0] == 0) {
        negative = false;
    }
}

int int2048::compareAbs(const int2048 &a, const int2048 &b) {
    if (a.digits.size() != b.digits.size()) {
        return a.digits.size() < b.digits.size() ? -1 : 1;
    }
    for (int i = a.digits.size() - 1; i >= 0; --i) {
        if (a.digits[i] != b.digits[i]) {
            return a.digits[i] < b.digits[i] ? -1 : 1;
        }
    }
    return 0;
}

int2048 int2048::addAbs(const int2048 &a, const int2048 &b) {
    int2048 result;
    result.digits.clear();
    int carry = 0;
    size_t maxLen = std::max(a.digits.size(), b.digits.size());
    for (size_t i = 0; i < maxLen || carry; ++i) {
        int sum = carry;
        if (i < a.digits.size()) sum += a.digits[i];
        if (i < b.digits.size()) sum += b.digits[i];
        result.digits.push_back(sum % BASE);
        carry = sum / BASE;
    }
    result.negative = false;
    return result;
}

int2048 int2048::subAbs(const int2048 &a, const int2048 &b) {
    int2048 result;
    result.digits.clear();
    int borrow = 0;
    for (size_t i = 0; i < a.digits.size(); ++i) {
        int diff = a.digits[i] - borrow;
        if (i < b.digits.size()) diff -= b.digits[i];
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits.push_back(diff);
    }
    result.normalize();
    result.negative = false;
    return result;
}

// Constructors
int2048::int2048() : negative(false) {
    digits.push_back(0);
}

int2048::int2048(long long x) : negative(x < 0) {
    if (x == 0) {
        digits.push_back(0);
        negative = false;
        return;
    }
    if (x < 0) x = -x;
    while (x > 0) {
        digits.push_back(x % BASE);
        x /= BASE;
    }
}

int2048::int2048(const std::string &s) {
    read(s);
}

int2048::int2048(const int2048 &other) : digits(other.digits), negative(other.negative) {}

// Read a big integer
void int2048::read(const std::string &s) {
    digits.clear();
    negative = false;
    
    size_t start = 0;
    if (s[0] == '-') {
        negative = true;
        start = 1;
    } else if (s[0] == '+') {
        start = 1;
    }
    
    // Read digits from right to left in groups of WIDTH
    for (int i = s.length(); i > (int)start; i -= WIDTH) {
        int left = std::max((int)start, i - WIDTH);
        std::string part = s.substr(left, i - left);
        digits.push_back(std::stoi(part));
    }
    
    normalize();
}

// Print the stored big integer
void int2048::print() {
    if (negative && !(digits.size() == 1 && digits[0] == 0)) {
        std::cout << '-';
    }
    std::cout << digits.back();
    for (int i = digits.size() - 2; i >= 0; --i) {
        std::cout.width(WIDTH);
        std::cout.fill('0');
        std::cout << digits[i];
    }
}

// Add a big integer (in-place)
int2048 &int2048::add(const int2048 &other) {
    bool thisNeg = negative;
    if (negative == other.negative) {
        *this = addAbs(*this, other);
        this->negative = thisNeg;
    } else {
        int cmp = compareAbs(*this, other);
        if (cmp >= 0) {
            *this = subAbs(*this, other);
            this->negative = thisNeg;
        } else {
            *this = subAbs(other, *this);
            this->negative = other.negative;
        }
    }
    normalize();
    return *this;
}

// Return the sum of two big integers
int2048 add(int2048 a, const int2048 &b) {
    return a.add(b);
}

// Subtract a big integer (in-place)
int2048 &int2048::minus(const int2048 &other) {
    bool thisNeg = negative;
    if (negative != other.negative) {
        *this = addAbs(*this, other);
        this->negative = thisNeg;
    } else {
        int cmp = compareAbs(*this, other);
        if (cmp >= 0) {
            *this = subAbs(*this, other);
            this->negative = thisNeg;
        } else {
            *this = subAbs(other, *this);
            this->negative = !thisNeg;
        }
    }
    normalize();
    return *this;
}

// Return the difference of two big integers
int2048 minus(int2048 a, const int2048 &b) {
    return a.minus(b);
}

// Unary operators
int2048 int2048::operator+() const {
    return *this;
}

int2048 int2048::operator-() const {
    int2048 result = *this;
    if (!(result.digits.size() == 1 && result.digits[0] == 0)) {
        result.negative = !result.negative;
    }
    return result;
}

// Assignment operator
int2048 &int2048::operator=(const int2048 &other) {
    if (this != &other) {
        digits = other.digits;
        negative = other.negative;
    }
    return *this;
}

// Arithmetic compound assignment operators
int2048 &int2048::operator+=(const int2048 &other) {
    return add(other);
}

int2048 operator+(int2048 a, const int2048 &b) {
    return a += b;
}

int2048 &int2048::operator-=(const int2048 &other) {
    return minus(other);
}

int2048 operator-(int2048 a, const int2048 &b) {
    return a -= b;
}

int2048 &int2048::operator*=(const int2048 &other) {
    bool resultNeg = negative != other.negative;
    
    // Simple multiplication
    int2048 result;
    result.digits.assign(digits.size() + other.digits.size(), 0);
    
    for (size_t i = 0; i < digits.size(); ++i) {
        long long carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry; ++j) {
            long long cur = result.digits[i + j] + carry;
            if (j < other.digits.size()) {
                cur += (long long)digits[i] * other.digits[j];
            }
            result.digits[i + j] = cur % BASE;
            carry = cur / BASE;
        }
    }
    
    result.normalize();
    result.negative = resultNeg;
    if (result.digits.size() == 1 && result.digits[0] == 0) {
        result.negative = false;
    }
    *this = result;
    return *this;
}

int2048 operator*(int2048 a, const int2048 &b) {
    return a *= b;
}

int2048 &int2048::operator/=(const int2048 &other) {
    bool resultNeg = negative != other.negative;
    
    // Division using long division
    int2048 dividend = *this;
    dividend.negative = false;
    int2048 divisor = other;
    divisor.negative = false;
    
    if (compareAbs(dividend, divisor) < 0) {
        // If dividend < divisor, result is 0 or -1 depending on signs
        if (negative != other.negative) {
            *this = int2048(-1);
        } else {
            *this = int2048(0);
        }
        return *this;
    }
    
    int2048 quotient;
    quotient.digits.assign(dividend.digits.size(), 0);
    int2048 remainder;
    remainder.digits.clear();
    remainder.negative = false;
    
    for (int i = dividend.digits.size() - 1; i >= 0; --i) {
        // Shift remainder left by one "digit" (in base BASE) and add next digit
        remainder *= int2048((long long)BASE);
        remainder += int2048((long long)dividend.digits[i]);
        
        // Binary search for quotient digit
        int left = 0, right = BASE - 1;
        while (left < right) {
            int mid = (left + right + 1) / 2;
            int2048 temp = divisor;
            temp *= int2048((long long)mid);
            if (compareAbs(temp, remainder) <= 0) {
                left = mid;
            } else {
                right = mid - 1;
            }
        }
        
        quotient.digits[i] = left;
        int2048 temp = divisor;
        temp *= int2048((long long)left);
        remainder -= temp;
    }
    
    quotient.normalize();
    
    // Apply floor division semantics
    if (resultNeg && !(remainder.digits.size() == 1 && remainder.digits[0] == 0)) {
        quotient += int2048(1);
    }
    
    quotient.negative = resultNeg;
    if (quotient.digits.size() == 1 && quotient.digits[0] == 0) {
        quotient.negative = false;
    }
    *this = quotient;
    return *this;
}

int2048 operator/(int2048 a, const int2048 &b) {
    return a /= b;
}

int2048 &int2048::operator%=(const int2048 &other) {
    int2048 quotient = *this / other;
    *this -= quotient * other;
    return *this;
}

int2048 operator%(int2048 a, const int2048 &b) {
    return a %= b;
}

// Stream operators
std::istream &operator>>(std::istream &is, int2048 &num) {
    std::string s;
    is >> s;
    num.read(s);
    return is;
}

std::ostream &operator<<(std::ostream &os, const int2048 &num) {
    if (num.negative && !(num.digits.size() == 1 && num.digits[0] == 0)) {
        os << '-';
    }
    os << num.digits.back();
    for (int i = num.digits.size() - 2; i >= 0; --i) {
        os.width(WIDTH);
        os.fill('0');
        os << num.digits[i];
    }
    return os;
}

// Comparison operators
bool operator==(const int2048 &a, const int2048 &b) {
    return a.negative == b.negative && a.digits == b.digits;
}

bool operator!=(const int2048 &a, const int2048 &b) {
    return !(a == b);
}

bool operator<(const int2048 &a, const int2048 &b) {
    if (a.negative != b.negative) return a.negative;
    int cmp = int2048::compareAbs(a, b);
    return a.negative ? cmp > 0 : cmp < 0;
}

bool operator>(const int2048 &a, const int2048 &b) {
    return b < a;
}

bool operator<=(const int2048 &a, const int2048 &b) {
    return !(a > b);
}

bool operator>=(const int2048 &a, const int2048 &b) {
    return !(a < b);
}

} // namespace sjtu
