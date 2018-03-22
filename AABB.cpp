#include "AABB.h"

void AABB::Init(const Eigen::Vector3d* controlPoints)
{
    maxp = Eigen::Vector3d(-RAND_MAX, -RAND_MAX, -RAND_MAX);
    minp = Eigen::Vector3d(RAND_MAX, RAND_MAX, RAND_MAX);
    for(int i = 0; i < 16; ++i)
        for(int j = 0; j < 3; ++j)
        {
            maxp(j) = std::max(maxp(j), controlPoints[i](j));
            minp(j) = std::min(minp(j), controlPoints[i](j));
        }
}

double AABB::Distance()
{
    return (maxp - minp).lpNorm<Eigen::Infinity>();
}

bool AABB::Intersect(const Eigen::Vector3d &ray_O, const Eigen::Vector3d &ray_V)
{
    const double eps = 1e-6;
    double ox = ray_O(0), oy = ray_O(1), oz = ray_O(2);
    double dx = ray_V(0), dy = ray_V(1), dz = ray_V(2);
    double x0 = minp(0), y0 = minp(1), z0 = minp(2);
    double x1 = maxp(0), y1 = maxp(1), z1 = maxp(2);
    double min_x = -RAND_MAX,min_y = -RAND_MAX, min_z = -RAND_MAX;
    double max_x = RAND_MAX, max_y = RAND_MAX, max_z = RAND_MAX;
    if(fabs(dx) < eps) 
    {
        if(ox < x0 || ox > x1)
            return false ;
    }
    else
    {
        if(dx >= 0)
        {
            min_x = (x0-ox)/dx;
            max_x = (x1-ox)/dx;
        }
        else
        {
            min_x = (x1-ox)/dx;
            max_x = (x0-ox)/dx;
        }
    }
    if(fabs(dy) < eps) 
    {
        if(oy < y0 || oy > y1)
            return false ;
    }
    else
    {
        if(dy >= 0)
        {
            min_y = (y0-oy)/dy;
            max_y = (y1-oy)/dy;
        }
        else
        {
            min_y = (y1-oy)/dy;
            max_y = (y0-oy)/dy;
        }
    }
    if(fabs(dz) < eps) 
    {
        if(oz < z0 || oz > z1)
            return false ;
    }
    else
    {
        if(dz >= 0)
        {
            min_z = (z0-oz)/dz;
            max_z = (z1-oz)/dz;
        }
        else
        {
            min_z = (z1-oz)/dz;
            max_z = (z0-oz)/dz;
        }
    }
    double t0,t1;
    t0 = std::max(min_z,std::max(min_x,min_y));
    t1 = std::min(max_z,std::min(max_x,max_y));
    return t0 < t1;
}