#include "analyser.hpp"
#include "prim_exception.hpp"
#include <raygui.h>

nano::Analyser::Analyser(const raylib::Image* targetImg): targetImg(targetImg), mask(targetImg->width, targetImg->height, maskColorNeg), nanotubes()
{
    mask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1); 
    setTargetImg(targetImg);
}

void nano::Analyser::setTargetImg(const raylib::Image* targetImg)
{
    this->targetImg = targetImg;
    mask.SetWidth(targetImg->width);
    mask.SetHeight(targetImg->height);
}

void nano::Analyser::calculateMask(float threshold)
{
    bool* checkArray = new bool[targetImg->width * targetImg->height] { false };

    for(int y = 0; y < targetImg->height; ++y)
    {
        for(int x = 0; x < targetImg->width; ++x)
        {
            const int idx = x + y * targetImg->width;
            if(!checkArray[idx])
            {
                checkArray[idx] = true;
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
    delete[] checkArray;
}

void nano::Analyser::findNanotubes()
{
    nanotubes.clear();
    bool* checkArray = new bool[mask.width * mask.height] { false };

    for(int y = 0; y < mask.height; ++y)
    {
        for(int x = 0; x < mask.width; ++x)
        {
            int idx = x + y * mask.width;
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

const raylib::Image* nano::Analyser::getMask() const
{
    return &mask;
}

const std::vector<nano::Nanotube>* nano::Analyser::getTubes() const
{
    return &nanotubes;
}