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
    class App* parentApp = nullptr;
    const raylib::Image* targetImg = nullptr;
    raylib::Image mask;
    const raylib::Color maskColorPos = { 0, 230, 0, 180 };
    const raylib::Color maskColorNeg = { 0, 0, 0, 0 };
    const uint16_t minPixelsInTube = 50u;
    const uint32_t maxAdjacentPixels = 20000u;
    const float extremumDelta = 0.02f;
    const uint8_t extremumOverflowMax = 3u;
    float progressReport = 0.0f;
    float pixelSize_nm = 0.0f;
    std::vector<Nanotube> nanotubes;

    std::vector<Point> addAdjacentPixels(int x, int y, bool* checkArray, uint32_t stackDepth);
    void setProgress(float prog);
public:
    Analyser(App* parentApp);
    Analyser(class App* parentApp, const raylib::Image* targetImg);
    Analyser(const Analyser& other) = delete;

    void setTargetImg(const raylib::Image* targetImg);
    void calculateMask(float threshold);
    void scanMask();
    void findExtremum();
    const raylib::Image* getMask() const;
    const std::vector<Nanotube>* getTubes() const;
    float getProgress() const;
    float &getPixelSize();
    float getImageArea();
    float getDensity();

    void resetAll();
};

}  // namespace nano

#endif // ANALYSER_HPP