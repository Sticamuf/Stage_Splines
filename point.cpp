#include "point.h"

Point::Point(QVector<double> p): point{p}
{

}

Point::Point(): point{}
{

}

QVector<double> Point::getPoint() const
{
    return point;
}

void Point::setPoint(QVector<double> &p)
{
    point = p;
}

int Point::getNumberOfDimensions() const
{
    return point.length();
}

void Point::setNumberOfDimensions(int number)
{
    point.resize(number);
}

double Point::getValueAtDimension(int dimension) const
{
    if(dimension >= 0 && dimension < point.length())
        return point[dimension];
    return 0;
}

void Point::setValueAtDimension(int dimension, double value)
{
    if(dimension >= 0 && dimension < point.length()){
        point[dimension] = value;
    }
}

Point Point::operator*(const double &val)
{
    Point p{point};
    for(int i = 0 ; i<point.length() ; i++)
    {
        p.point[i] *= val;
    }
    return p;
}

Point Point::operator+(const Point &p)
{
    if(point.length() == p.point.length())
    {
        Point ret(point);
        for(int i = 0 ; i<point.length() ; i++)
        {
            ret.point[i] += p.point[i];
        }
        return ret;
    }
    return Point();
}

bool Point::operator==(const Point &p)
{
    return point == p.point;
}

Point Point::operator-(const Point &p)
{
    if(point.length() == p.point.length())
    {
        Point ret(point);
        for(int i = 0 ; i<point.length() ; i++)
        {
            ret.point[i] -= p.point[i];
        }
        return ret;
    }
    return Point();
}

bool Point::operator<(const double &val)
{
    for(int i = 0 ; i<point.length() ; i++)
    {
        if(point[i] >= val)
            return false;
    }
    return true;
}

Point Point::abs(const Point& p)
{
    Point ret(p);
    for(int i = 0 ; i<ret.point.length() ; i++)
    {
        if(ret.point[i] < 0)
            ret.point[i] = -ret.point[i];
    }
    return ret;
}

std::string Point::toString()
{
    std::string s = "[";
    for(int i = 0 ; i<point.length()-1 ; i++)
    {
        s += point[i];
        s += "; ";
    }
    s += point[point.length()-1];
    s += "]";
    return s;
}



