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
    const raylib::Image* targetImg = nullptr;
    raylib::Image mask;
    const raylib::Color maskColorPos = { 0, 230, 0, 180 };
    const raylib::Color maskColorNeg = { 0, 0, 0, 0 };
    float progressReport = 0.0f;
    std::vector<Nanotube> nanotubes;

    std::vector<Point> checkPixel(int x, int y, bool* checkArray);
    void setProgress(float prog);

    bool analysisCancelled = false;
public:
    Analyser();
    Analyser(const raylib::Image* targetImg);
    Analyser(const Analyser& other) = delete;

    // analysis config
    float pixelSize_nm = 0.0f;
    float extremumDeltaStep = 0.02f;
    int extremumOverflowTolerance = 5;
    int minPixelsInTube = 50;
    bool processFullRange = false;


    void setTargetImg(const raylib::Image* targetImg);
    void calculateMask(float threshold);
    void scanMaskForTubes();
    void startExtremumAnalysis();
    void startManualAnalysis(float threshold);
    const raylib::Image* getMask() const;
    const std::vector<Nanotube>* getTubes() const;
    float getProgress() const;
    float getImageArea();
    float getDensity();
    bool areTubesCalculated() const;
    void cancelAnalysis();

    void resetAll();
};

}  // namespace nano

#endif // ANALYSER_HPP