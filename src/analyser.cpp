#include "analyser.hpp"
#include "prim_exception.hpp"
#include "app.hpp"
#include <raygui.h>
#include <iostream>
#include <cassert>



nano::Analyser::Analyser(App* parentApp): parentApp(parentApp), targetImg(nullptr) { mask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1); }

nano::Analyser::Analyser(App* parentApp, const raylib::Image* targetImg): parentApp(parentApp), targetImg(targetImg), mask(targetImg->width, targetImg->height, maskColorNeg), nanotubes()
{
    mask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1);
    setTargetImg(targetImg);
}

void nano::Analyser::setTargetImg(const raylib::Image* targetImg)
{
    this->targetImg = targetImg;
    mask = raylib::Image(targetImg->width, targetImg->height, maskColorNeg);
}

void nano::Analyser::calculateMask(float threshold)
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to calculate mask without target image.");
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

void nano::Analyser::scanMask()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to scan mask without target image.");
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
                checkArray[idx] = true;
                if(::GetImageColor(mask, x, y).a)
                {
                    std::vector<Point> adjacentPixels = addAdjacentPixels(x, y, checkArray, 0u);
                    if(adjacentPixels.size() > minPixelsInTube)
                    {
                        nanotubes.emplace_back();
                    }
                }
            }
        }
    }
    setProgress(1.0f);
    delete[] checkArray;
}

std::vector<nano::Point> nano::Analyser::addAdjacentPixels(int x, int y, bool* checkArray, uint32_t stackDepth)
{
    if(stackDepth > maxAdjacentPixels) throw PRIM_EXCEPTION("Stack depth got too big trying to find adjacent pixels. Try to adjust threshold.");

    std::vector<Point> points;
    
    int currX = x + 1;
    int currY = y;
    int idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x + 1;
    currY = y + 1;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x;
    currY = y + 1;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;
        
        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x - 1;
    currY = y + 1;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x - 1;
    currY = y;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x - 1;
    currY = y - 1;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x;
    currY = y - 1;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x + 1;
    currY = y - 1;
    idx = currX + currY * mask.width;
    if(currX < mask.width && currX >= 0 && currY < mask.height && currY >= 0 && !checkArray[idx])
    {
        checkArray[idx] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray, ++stackDepth);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    return std::move(points);
}

void nano::Analyser::findExtremum()
{
    if(!targetImg) throw PRIM_EXCEPTION("Trying to find nanotube extremum without target image.");
    float threshold = 1.0f;
    float extremumThreshold = 1.0f;
    uint32_t extremumNumberOfTubes = 0u;
    calculateMask(threshold);
    scanMask();
    uint8_t stopFlag = 0u;
    uint32_t currNumberOfTubes = nanotubes.size();
    uint32_t prevNumberOfTubes = currNumberOfTubes;
    while(true)
    {
        threshold -= extremumDelta;
        calculateMask(threshold);
        scanMask();
        currNumberOfTubes = nanotubes.size();
        parentApp->printLine("Nanotubes = " + std::to_string(currNumberOfTubes));

        if(currNumberOfTubes <= prevNumberOfTubes && currNumberOfTubes > 0u)
        {
            if(stopFlag == 0u && extremumNumberOfTubes < currNumberOfTubes)
            {
                extremumThreshold = threshold + extremumDelta;
                extremumNumberOfTubes = prevNumberOfTubes;
            }
            ++stopFlag;
        }
        else
        {
            stopFlag = 0u;
        }
        if(stopFlag >= extremumOverflowMax)
        {
            parentApp->printLine("Extremum threshold = " + App::floatToString(extremumThreshold, 3u));
            parentApp->printLine("Extremum number of tubes = " + std::to_string(extremumNumberOfTubes));
            calculateMask(extremumThreshold);
            scanMask();
            return;
        }
        else
        {
            prevNumberOfTubes = currNumberOfTubes;
        }
    }
    
}

const raylib::Image* nano::Analyser::getMask() const
{
    return &mask;
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

float& nano::Analyser::getPixelSize()
{
    return pixelSize_nm;
}

float nano::Analyser::getImageArea()
{
    assert(targetImg);
    return static_cast<float>(targetImg->GetWidth()) * pixelSize_nm * static_cast<float>(targetImg->GetHeight()) * (int)pixelSize_nm;
}

float nano::Analyser::getDensity()
{
    return nanotubes.size() / getImageArea();
}

void nano::Analyser::resetAll()
{
    targetImg = nullptr;
    mask.Unload();
    progressReport = 0.0f;
    pixelSize_nm = 0.0f;
    nanotubes.clear();
}
