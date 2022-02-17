#include "analyser.hpp"

nano::Analyser::Analyser(const raylib::Image* targetImg): targetImg(targetImg), mask(targetImg->width, targetImg->height, maskColorNeg), nanotubes()
{
    mask.Format(PIXELFORMAT_UNCOMPRESSED_R5G5B5A1); 
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

    for(int x = 0; x < targetImg->width; ++x)
    {
        for(int y = 0; y < targetImg->height; ++y)
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
    bool* checkArray = new bool[targetImg->width * targetImg->height] { false };

    for(int x = 0; x < mask.width; ++x)
    {
        for(int y = 0; y < mask.height; ++y)
        {
            int idx = x + y * mask.width;
            if(!checkArray[idx])
            {
                checkArray[idx] = true;
                if(::GetImageColor(mask, x, y).a)
                {
                    Nanotube newTube;
                    newTube.addPoints(addAdjacentPixels(x, y, checkArray));
                    
                }
            }
        }
    }
}

std::vector<nano::Point> nano::Analyser::addAdjacentPixels(int x, int y, bool* checkArray)
{
    std::vector<Point> points;
    
    int currX = x + 1;
    int currY = y;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x + 1;
    currY = y + 1;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x;
    currY = y + 1;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;
        
        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x - 1;
    currY = y + 1;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x - 1;
    currY = y;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x - 1;
    currY = y - 1;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x;
    currY = y - 1;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    currX = x + 1;
    currY = y - 1;
    if(currX < mask.width || currX >= 0 || currY < mask.height || currY <= 0)
    {
        checkArray[currX + currY * mask.width] = true;

        if(::GetImageColor(mask, currX, currY).a)
        {
            points.emplace_back(currX, currY);
            std::vector<Point> morePoints = addAdjacentPixels(currX, currY, checkArray);
            if(!morePoints.empty())
            {
                points.insert(points.end(), std::make_move_iterator(morePoints.begin()), std::make_move_iterator(morePoints.end()));
            }
        }
    }

    return std::move(points);
}