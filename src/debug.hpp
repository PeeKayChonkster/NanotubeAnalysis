#ifndef prim_DEBUG_HPP
#define prim_DEBUG_HPP

#include <raylib-cpp.hpp>
#include <vector>
#include <string>
#include <iostream>

namespace prim
{
    typedef unsigned int uint;

    class Debug
    {
    private:
        inline static std::pair<int, int> textPosition = { 5, 30 };
        inline static std::vector<std::string> lines = {};
        inline static std::vector<std::string> persistentLines = {};
    public:
        inline static int fontSize = 22;

        inline static void printLine(std::string line)
        {
            //std::cout << "prim: pushed line: " + line << std::endl;
            lines.push_back(std::move(line));
        }

        inline static void printPersistentLine(std::string line)
        {
            //std::cout << "prim: pushed persistent line: " + line << std::endl;
            persistentLines.push_back(std::move(line));
        }

        inline static void draw(Color textColor = WHITE)
        {
            uint i;
            for(i = 0u; i < persistentLines.size(); ++i)
                DrawText(persistentLines[i].c_str(), textPosition.first, textPosition.second + (i + 1) * fontSize, fontSize, textColor);

            for(uint j = 0u; j < lines.size(); ++i, ++j)
                DrawText(lines[j].c_str(), textPosition.first, textPosition.second + (i + 1) * fontSize, fontSize, textColor);
            
            lines.clear();
        }
    };
}

#endif // prim_DEBUG_HPP