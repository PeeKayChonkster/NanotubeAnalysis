#include "analyser.hpp"
#include "prim_exception.hpp"
#include "app.hpp"
#include "ui.hpp"
#include <stack>
#include <iostream>
#include <cassert>



nano::Analyser::Analyser() { mask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1); tubeMask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1); }

nano::Analyser::Analyser(const raylib::Image* targetImg): 
        targetImg(targetImg), mask(targetImg->width, targetImg->height, maskColorNeg), tubeMask(targetImg->width, targetImg->height, maskColorNeg), nanotubes()
{
    mask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1);
    tubeMask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1);
    setTargetImg(targetImg);
}

void nano::Analyser::setTargetImg(const raylib::Image* targetImg)
{
    this->targetImg = targetImg;
}

void nano::Analyser::calculateMask(float threshold)
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to calculate mask without target image.");
    if(!mask.IsReady()) mask = raylib::Image(targetImg->width, targetImg->height, maskColorNeg);

    uint32_t numberOfPixels = targetImg->width * targetImg->height;

    for(int y = 0; y < targetImg->height; ++y)
    {
        for(int x = 0; x < targetImg->width; ++x)
        {
            const uint32_t idx = x + y * targetImg->width;
            float value = ::GetImageColor(*targetImg, x, y).r / 255.0f;
            if(value >= threshold)
            {
                mask.DrawPixel(x, y, maskColorPos);
            }
            else
            {
                mask.DrawPixel(x, y, maskColorNeg);
            }
        }
    }
}

void nano::Analyser::scanMaskForTubes()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to scan mask without target image.");
    if(!mask.IsReady()) throw PRIM_EXCEPTION("Trying to scan mask for tubes before creating mask.");
    if(!tubeMask.IsReady()) tubeMask = raylib::Image(targetImg->width, targetImg->height, maskColorNeg);
    tubeMask.ClearBackground(maskColorNeg);
    setProgress(0.0f);
    nanotubes.clear();
    uint32_t numberOfPixels = targetImg->width * targetImg->height;
    bool* checkArray = new bool[numberOfPixels] { false };

    for(int y = 0; y < mask.height; ++y)
    {
        for(int x = 0; x < mask.width; ++x)
        {
            int idx = x + y * mask.width;
            setProgress(idx / (float)numberOfPixels);
            if(!checkArray[idx])
            {
                std::vector<Point> points = checkPixel(x, y, checkArray);
                if(points.size() >= minPixelsInTube)
                {
                    for(const auto& point : points) { tubeMask.DrawPixel(point.x, point.y, tubeMaskColorPos); }
                    nanotubes.push_back(std::move(points));
                }
            }
        }
    }
    setProgress(1.0f);
    delete[] checkArray;
}

std::vector<nano::Point> nano::Analyser::checkPixel(int x, int y, bool* checkArray)
{
    std::stack<Point> pointsStack;
    std::vector<Point> resultPoints;

    Point start(x, y);
    pointsStack.push(start);
    
    while(!pointsStack.empty())
    {
        Point currPoint = pointsStack.top(); pointsStack.pop();
        int idx = currPoint.x + currPoint.y * mask.width;

        if(currPoint.x < mask.width && currPoint.x >= 0 && currPoint.y < mask.height && currPoint.y >= 0 && !checkArray[idx] && ::GetImageColor(mask, currPoint.x, currPoint.y).a )
        {
            checkArray[idx] = true;
            resultPoints.push_back(currPoint);

            Point adjPoint;

            adjPoint = currPoint;
            adjPoint.x += 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x += 1u;
            adjPoint.y -= 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.y -= 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x -= 1u;
            adjPoint.y -= 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x -= 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x -= 1u;
            adjPoint.y += 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.y += 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);

            adjPoint = currPoint;
            adjPoint.x += 1u;
            adjPoint.y += 1u;
            if(adjPoint.x < mask.width && adjPoint.x >= 0 && adjPoint.y < mask.height && adjPoint.y >= 0 && !checkArray[adjPoint.x + adjPoint.y * mask.width]) pointsStack.push(adjPoint);
        }
    }

    return std::move(resultPoints);
}

void nano::Analyser::startExtremumAnalysis()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to find nanotube extremum without target image.");
    setProgress(0.0f);
    float threshold = 1.0f;
    float extremumThreshold = 1.0f;
    uint32_t extremumNumberOfTubes = 0u;
    calculateMask(threshold);
    scanMaskForTubes();
    uint8_t stopFlag = 0u;
    uint32_t currNumberOfTubes = nanotubes.size();
    uint32_t prevNumberOfTubes = currNumberOfTubes;
    UI::inst().printLine("Threshold = " + UI::inst().floatToString(threshold, 3u) + "; Nanotubes = " + std::to_string(currNumberOfTubes));
    while(true)
    {
        if(analysisCancelled)
        {
            analysisCancelled = false;
            progressReport = 0.0f;
            mask.Unload();
            nanotubes.clear();
            UI::inst().printLine("<<<<< Analysis cancelled >>>>>");
            return;
        }

        threshold -= extremumDeltaStep;
        calculateMask(threshold);
        scanMaskForTubes();
        currNumberOfTubes = nanotubes.size();
        UI::inst().printLine("Threshold = " + UI::inst().floatToString(threshold, 3u) + "; Nanotubes = " + std::to_string(currNumberOfTubes));

        if(currNumberOfTubes <= prevNumberOfTubes && currNumberOfTubes > 0u)    // derivative is negative
        {
            if(stopFlag == 0u && extremumNumberOfTubes < currNumberOfTubes)     // found local extremum
            {
                extremumThreshold = threshold + extremumDeltaStep;
                extremumNumberOfTubes = prevNumberOfTubes;
            }
            ++stopFlag;
        }
        else
        {
            stopFlag = 0u;
        }
        if((stopFlag >= extremumOverflowTolerance && !processFullRange) || threshold - extremumDeltaStep < 0.0f)                 // end analysis
        {
            calculateMask(extremumThreshold);
            scanMaskForTubes();

            UI::inst().printLine("<<<<< Results >>>>>");
            UI::inst().printLine("Extremum threshold = " + UI::inst().floatToString(extremumThreshold, 3u));
            UI::inst().printLine("Extremum number of tubes = " + std::to_string(extremumNumberOfTubes));
            UI::inst().printLine("Min pixels in nanotube = " + std::to_string(minPixelsInTube));
            UI::inst().printLine("Pixel size = " + UI::inst().floatToString(pixelSize_nm, 3u) + " (nm)");
            UI::inst().printLine("Image area = " + UI::inst().floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
            UI::inst().printLine("Nanotube density = " + UI::inst().floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
            return;
        }
        else                                                                    // resume analysis
        {
            prevNumberOfTubes = currNumberOfTubes;
        }
    }
    
}

void nano::Analyser::startManualAnalysis(float threshold)
{
    setProgress(0.0f);
    nanotubes.clear();
    calculateMask(threshold);
    scanMaskForTubes();
    UI::inst().printLine("<<<<< Results >>>>>");
    UI::inst().printLine("Number of tubes = " + std::to_string(nanotubes.size()));
    UI::inst().printLine("Threshold = " + UI::inst().floatToString(threshold, 3u));
    UI::inst().printLine("Min pixels in nanotube = " + std::to_string(minPixelsInTube));
    UI::inst().printLine("Pixel size = " + UI::inst().floatToString(pixelSize_nm, 3u) + " (nm)");
    UI::inst().printLine("Image area = " + UI::inst().floatToString(getImageArea() * 0.000001, 3u) + " (mm2)");
    UI::inst().printLine("Nanotube density = " + UI::inst().floatToString(getDensity() * 1000000.0f, 3u) + " (1/mm2)\n");
}

const raylib::Image* nano::Analyser::getMask() const
{
    return &mask;
}

const raylib::Image* nano::Analyser::getTubeMask() const
{
    return &tubeMask;
}

const std::vector<nano::Nanotube>* nano::Analyser::getTubes() const
{ 
    return &nanotubes;
}

void nano::Analyser::setProgress(float prog)
{
    progressReport = prog;
}

float nano::Analyser::getProgress() const
{
    return progressReport;
}

float nano::Analyser::getImageArea()
{
    assert(targetImg);
    return static_cast<float>(targetImg->GetWidth()) * pixelSize_nm * static_cast<float>(targetImg->GetHeight()) * pixelSize_nm;
}

float nano::Analyser::getDensity()
{
    return nanotubes.size() / getImageArea();
}

void nano::Analyser::resetAll()
{
    targetImg = nullptr;
    mask.Unload();
    tubeMask.Unload();
    progressReport = 0.0f;
    pixelSize_nm = 0.0f;
    nanotubes.clear();
}

bool nano::Analyser::areTubesCalculated() const
{
    return !nanotubes.empty();
}

void nano::Analyser::cancelAnalysis()
{
    analysisCancelled = true;
}