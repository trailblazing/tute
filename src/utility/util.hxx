#ifndef UTIL_HXX
#define UTIL_HXX
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

class formatter {
public:
    formatter() {}
    ~formatter() {}

    template <typename Type>
    formatter& operator<<(const Type& value)
    {
        stream_ << value;
        return *this;
    }
    std::string str() const { return stream_.str(); }
    operator std::string() const { return stream_.str(); }

    enum ConvertToString {
        to_str
    };
    std::string operator>>(ConvertToString) { return stream_.str(); }

private:
    std::stringstream stream_;

    formatter(const formatter&);
    formatter& operator=(formatter&);
};

#endif // UTIL_HXX
