#ifndef ANALYSER_HPP
#define ANALYSER_HPP

#include <raylib-cpp.hpp>
#include <vector>
#include "nanotube.hpp"

namespace nano
{

class Analyser
{
private:
    const raylib::Image* targetImg;
    raylib::Image mask;
    const raylib::Color maskColorPos = { 0, 230, 0, 180 };
    const raylib::Color maskColorNeg = { 0, 0, 0, 0 };
    std::vector<Point> nanotubes;

    std::vector<Point> addAdjacentPixels(int x, int y, bool* checkArray);
public:
    Analyser(const raylib::Image* targetImg);
    Analyser(const Analyser& other) = delete;

    void setTargetImg(const raylib::Image* targetImg);
    void calculateMask(float threshold);
    void findNanotubes();
};

}  // namespace nano

#endif // ANALYSER_HPP