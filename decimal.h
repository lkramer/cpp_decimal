#ifndef LKRAMER_DECIMAL_H
#define LKRAMER_DECIMAL_H

#include <cmath>
#include <ostream>
#include <cstring>

class DecimalException: public std::exception {
public:
    DecimalException(const std::string& desc)
        : mDesc(desc)
    {}

    virtual const char* what() const throw() {
        return mDesc.c_str();
    }

private:
    std::string mDesc;
};

class Decimal {
public:
    Decimal()
        : m_value(0)
        , m_exp(0)
    {}

    Decimal(const Decimal& d)
        : m_value(d.m_value)
        , m_exp(d.m_exp)
    {}

    Decimal(int64_t value, int32_t exp = 0)
        : m_value(value)
        , m_exp(exp)
    {
        if (value == std::numeric_limits<int64_t>::min()) {
            throw DecimalException("Create Decimal with int64_t minimum");
        }
    }

    Decimal(const char* value, size_t len) {
        from_c_str(value, len);
    }

    Decimal(const std::string& value) {
        from_c_str(value.c_str(), value.length());
    }

    Decimal(double value) {
        if (std::isnan(value)) {
            throw DecimalException("Create Decimal with NaN double");
        }

        std::string v = std::to_string(value);
        from_c_str(v.c_str(), v.length());
        // TODO break down the double exponent and mantisa and use that.
    }

    std::string string() const {
        char buf[32];
        c_str(buf, 32);
        return std::string(buf);
    }

    ssize_t c_str(char * buf, ssize_t length) const {
        int64_t num = m_value;
        int32_t i = 0;
        bool isNegative = false;

        if (length < 2) {
            throw DecimalException("Insufficient buffer size.");
        }

        if (num == 0) {
            buf[i++] = '0';
            buf[i] = '\0';
            return 2;
        }

        if (num < 0) {
            isNegative = true;
            num = -num;
        }


        // An elaborate attempt to write the decimal number (backwards)
        // - If the mantissa is 0, it will write out the value as a whole
        //   number. i.e 3.0 is written as "3".
        // - While the mantissa can theoretically have as many digits as
        //   the exponent, (i.e a number with an exponent of 3 can look like this X.XXX)
        //   Any insignificant 0s will be truncated. so 1.300 becomes "1.3".
        int32_t exp = m_exp;
        if (exp > 0) {
            while ((num != 0 || exp >= 0) && i < length) {
                if (exp == 0 && i != 0) {
                    buf[i++] = '.';
                    if (num == 0) { // If there no more digits, then we add a leading 0 in front of the '.'.
                        buf[i++] = '0';
                    }
                } else {
                    buf[i] = (num % 10) + '0';
                    num /= 10;
                    if (i != 0 || exp <= 0 || buf[i] != '0') {
                        ++i;
                    }
                }
                --exp;
            }
        } else {
            while (exp < 0 && i < length) {
                buf[i] = '0';
                ++exp;
                ++i;
            }

            while (num != 0 && i < length) {
                buf[i] = (num % 10) + '0';
                num /= 10;
                if (i != 0 || buf[i] != '0') {
                    ++i;
                }
            }
        }


        if (isNegative && i < length) {
            buf[i++] = '-';
        }

        if (i == length) {
            throw DecimalException("Insufficient buffer size.");
        }

        reverse(buf, i);

        buf[i] = '\0';

        return i;
    }

    // Should be a cast instead
    int64_t asInt() {
        auto scale = pow(10, m_exp);
        return m_value / scale;
    }

    inline bool isNegative() const {
        return m_value < 0;
    }

    inline bool isZero() const {
        return m_value == 0;
    }

    Decimal diff(const Decimal& other) const {
        if (m_exp == other.m_exp) {
            if (m_value > other.m_value) {
                return Decimal(static_cast<uint64_t>(m_value) - static_cast<uint64_t>(other.m_value), m_exp);
            } else {
                return Decimal(static_cast<uint64_t>(other.m_value) - static_cast<uint64_t>(m_value), m_exp);
            }
        }

        int32_t baseExp = std::max(m_exp, other.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = other.scaledCopy(baseExp);

        if (rd.m_value > rd2.m_value) {
            return Decimal(static_cast<uint64_t>(rd.m_value) - static_cast<uint64_t>(rd2.m_value), baseExp);
        } else {
            return Decimal(static_cast<uint64_t>(rd2.m_value) - static_cast<uint64_t>(rd.m_value), baseExp);
        }
    }

    void alignScale(const Decimal& other) {
        alignScale(other.m_exp);
    }

    void alignScale(int32_t newExp) {
        if (newExp == m_exp) {
            return;
        }

        int32_t diff = m_exp - newExp;
        if (diff < 0) diff = -diff;

        auto scale = pow(10, diff);
        if (newExp > m_exp) {
            m_value *= scale;
        } else {
            m_value /= scale;
        }
        m_exp = newExp;
    }

    uint64_t getValue() const {
        return m_value;
    }

    // *************** Operators **************
    bool operator>(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp) {
            return m_value > lhs.m_value;
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);

        return rd.m_value > rd2.m_value;
    }

    bool operator>(int lhs) const {
        return *this > Decimal(lhs, 0);
    }

    bool operator<(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp) {
            return m_value < lhs.m_value;
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);

        return rd.m_value < rd2.m_value;
    }

    bool operator<(int lhs) const {
        return *this < Decimal(lhs, 0);
    }

    Decimal operator-(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp){
            return Decimal(m_value-lhs.m_value, m_exp);
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);

        return Decimal(rd.m_value-rd2.m_value, baseExp);
    }

    Decimal operator-=(const Decimal& lhs) {
        *this = *this - lhs;
        return *this;
    }

    Decimal operator+(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp){
            return Decimal(m_value+lhs.m_value, m_exp);
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);

        return Decimal(rd.m_value+rd2.m_value, baseExp);
    }

    Decimal operator+=(const Decimal& lhs) {
        *this = *this + lhs;
        return *this;
    }

    Decimal operator/(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp) {
            return Decimal(static_cast<double>(m_value)/static_cast<double>(lhs.m_value));
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);
        double v = static_cast<double>(rd.m_value)/static_cast<double>(rd2.m_value);
        return Decimal(v);
    }

    Decimal operator*(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp) {
            return Decimal(m_value * lhs.m_value, m_exp+lhs.m_exp);
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);
        return Decimal(rd.m_value * rd2.m_value, rd.m_exp+rd2.m_exp);
    }

    bool operator==(const Decimal& lhs) const {
        if (m_exp == lhs.m_exp) {
            return (m_value == lhs.m_value);
        }

        int32_t baseExp = std::max(m_exp, lhs.m_exp);
        auto rd = scaledCopy(baseExp);
        auto rd2 = lhs.scaledCopy(baseExp);
        return (rd.m_value == rd2.m_value);
    }

    friend std::ostream& operator<<(std::ostream& lhs, const Decimal& rhs);
private:
    /** optimistically assumes \0 terminated **/
    inline void from_c_str(const char* value, size_t len) {
        const char* d = strchr(value, '.');
        if (d == nullptr) {
            m_value = atol(value);
            m_exp = 0;
        } else {
            const char* v = strchr(d+1, '.');
            if (v != nullptr) {
                throw DecimalException("Too many dots");
            }
            size_t intLen = d-value;
            const char* decimalStart = d+1;
            const char* decimalEnd = value+len;
            // trim trailing 0s
            while(*(decimalEnd-1) == '0') {
                --decimalEnd;
            }
            m_exp = decimalEnd-decimalStart;
            if (m_exp == 0) {
                m_value = atol(value);
            } else {
                if (len > 63) {
                    throw DecimalException("Insufficient buffer.");
                }
                char buf[64];
                strncpy(buf, value, intLen);
                strncpy(buf+intLen, d+1, m_exp);
                buf[intLen+m_exp+1]='\0';
                m_value = atol(buf);
            }
        }
    }

    Decimal scaledCopy(int32_t newExp) const {
        if (newExp == m_exp) {
            return *this;
        }

        int32_t diff = m_exp - newExp;
        if (diff < 0) diff = -diff;

        auto scale = pow(10, diff);
        if (newExp > m_exp) {
            return Decimal(m_value*scale, newExp);
        } else {
            return Decimal(m_value/scale, newExp);
        }
    }

    inline void reverse(char* buf, int length) const {
        int start = 0;
        int end = length -1;
        while (start < end) {
            std::swap(*(buf+start), *(buf+end));
            start++;
            end--;
        }
    }

    int64_t     m_value;
    int32_t     m_exp;
};

inline std::ostream& operator<<(std::ostream& lhs, const Decimal& rhs) {
    char buf[32];
    rhs.c_str(buf, 32);
    lhs << buf;
    return lhs;
}

#endif
