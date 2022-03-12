#ifndef PRIM_EXCEPTION_HPP
#define PRIM_EXCEPTION_HPP

#include <string>

#define PRIM_EXCEPTION(what) prim::Exception(what, __FILE__, __LINE__)

namespace prim
{

class Exception
{
private:
    std::string whatString;
public:
    Exception(): whatString("prim exception.") {}
    Exception(std::string what, std::string file, int line): whatString("<PRIM EXCEPTION>. File: " + file + "; Line: " + std::to_string(line) + "; What: " + what) {}

    inline std::string what() const { return whatString; }
};

}

#endif // PRIM_EXCEPTION_HPP