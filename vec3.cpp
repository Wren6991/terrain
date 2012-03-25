#include "vec3.h"
#include <cmath>

vec3::vec3()
{
    x = 0;
    y = 0;
    z = 0;
}

vec3::vec3(double x_, double y_, double z_)
{
    x = x_;
    y = y_;
    z = z_;
}

vec3 vec3::operator+(vec3 v)
{
    return vec3(x + v.x, y + v.y, z + v.z);
}
vec3 vec3::operator-(vec3 v)
{
    return vec3(x - v.x, y - v.y, z - v.z);
}
vec3 vec3::operator*(double s)
{
    return vec3(x*s, y*s, z*s);
}

vec3 vec3::normalise()
{
    double dist = sqrt(x*x + y*y + z*z);
    x /= dist;
    y /= dist;
    z /= dist;
    return *this;
}

double vec3::length()
{
    return sqrt(x*x + y*y + z*z);
}
