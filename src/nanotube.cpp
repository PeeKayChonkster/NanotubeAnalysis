#include "nanotube.hpp"


nano::Nanotube::Nanotube(std::vector<Point>&& points) : points(points) {}

void nano::Nanotube::addPoints(std::vector<Point>&& newPoints)
{
    if(!newPoints.empty())
    {
        points.insert(points.end(), std::make_move_iterator(newPoints.begin()), std::make_move_iterator(newPoints.end()));
    }
}
